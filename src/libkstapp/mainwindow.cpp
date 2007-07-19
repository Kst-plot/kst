/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mainwindow.h"
#include "boxitem.h"
#include "datamanager.h"
#include "debugdialog.h"
#include "debugnotifier.h"
#include "document.h"
#include "ellipseitem.h"
#include "exportgraphicsdialog.h"
#include "kstapplication.h"
#include "kstdebug.h"
#include "labelitem.h"
#include "lineitem.h"
#include "memorywidget.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "svgitem.h"
#include "tabwidget.h"
#include "vectoreditordialog.h"
#include "view.h"

#include <QtGui>

// Temporaries
#include "kstavector.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstequation.h"


namespace Kst {

MainWindow::MainWindow() {
  _dataManager = 0;
  _exportGraphics = 0;
  _vectorEditor = 0;
  _doc = new Document(this);
  _tabWidget = new TabWidget(this);
  _undoGroup = new QUndoGroup(this);
  _debugDialog = new DebugDialog(this); // need this early for hookups
  KstDebug::self()->setHandler(_debugDialog);

  connect(_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentViewChanged()));

  _tabWidget->createView();

  setCentralWidget(_tabWidget);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  readSettings();
  QTimer::singleShot(0, this, SLOT(performHeavyStartupActions()));
}


MainWindow::~MainWindow() {
  delete _vectorEditor;
  _vectorEditor = 0;
  delete _dataManager;
  _dataManager = 0;
  delete _doc;
  _doc = 0;
}


void MainWindow::performHeavyStartupActions() {
}


void MainWindow::cleanup() {
  KST::dataObjectList.lock().writeLock();
  KstDataObjectList dol = KST::dataObjectList; //FIXME What is going on here?
  KST::dataObjectList.clear();
  KST::dataObjectList.lock().unlock();
  dol.clear(); //and here?

  KST::dataSourceList.lock().writeLock();
  KST::dataSourceList.clear();
  KST::dataSourceList.lock().unlock();
  KST::matrixList.lock().writeLock();
  KST::matrixList.clear();
  KST::matrixList.lock().unlock();
  KST::scalarList.lock().writeLock();
  KST::scalarList.clear();
  KST::scalarList.lock().unlock();
  KST::stringList.lock().writeLock();
  KST::stringList.clear();
  KST::stringList.lock().unlock();
  KST::vectorList.lock().writeLock();
  KST::vectorList.clear();
  KST::vectorList.lock().unlock();
}


bool MainWindow::promptSave() {
    int rc = QMessageBox::warning(this, tr("Kst"), tr("Your document has been modified.\nSave changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (rc == QMessageBox::Save) {
      save();
    } else if (rc == QMessageBox::Cancel) {
      return false;
    }
    return true;
}


void MainWindow::closeEvent(QCloseEvent *e) {
  if (_doc->isChanged() && !promptSave()) {
    return;
  }
  cleanup();
  QMainWindow::closeEvent(e);
}


Document *MainWindow::document() const {
  return _doc;
}


QUndoGroup *MainWindow::undoGroup() const {
  return _undoGroup;
}


TabWidget *MainWindow::tabWidget() const {
  return _tabWidget;
}


void MainWindow::save() {
  if (_doc->isOpen()) {
    _doc->save();
  } else {
    saveAs();
  }
}


void MainWindow::saveAs() {
  QString fn = QFileDialog::getSaveFileName(this, tr("Kst: Save File"), _doc->fileName(), tr("Kst Sessions (*.kst)"));
  if (fn.isEmpty()) {
    return;
  }
  _doc->save(fn);
}


void MainWindow::open() {
  if (_doc->isChanged() && !promptSave()) {
    return;
  }
  QString fn = QFileDialog::getOpenFileName(this, tr("Kst: Open File"), _doc->fileName(), tr("Kst Sessions (*.kst)"));
  if (fn.isEmpty()) {
    return;
  }

  openFile(fn);
}


void MainWindow::openFile(const QString &file) {
  delete _doc;
  _doc = new Document(this);
  bool ok = _doc->open(file);
  if (!ok) {
    QMessageBox::critical(this, tr("Kst"), tr("Error opening document '%1':\n%2").arg(file, _doc->lastError()));
    delete _doc;
    _doc = new Document(this);
  }
}


void MainWindow::print() {
  if (!_doc->isOpen()) {
    return;
  }

  QPrinter printer(QPrinter::HighResolution);

  QPrintDialog pd(&printer, this);
  pd.addEnabledOption(QPrintDialog::PrintToFile);
  pd.addEnabledOption(QPrintDialog::PrintPageRange);

  if (pd.exec() == QDialog::Accepted) {
    QPainter painter(&printer);
    QList<QGraphicsView*> pages;
    switch (printer.printRange()) {
      case QPrinter::PageRange:
        break;
      case QPrinter::AllPages:
        foreach (QGraphicsView *view, _tabWidget->views()) {
          pages.append(view);
        }
        break;
      case QPrinter::Selection:
      default:
        pages.append(_tabWidget->currentView());
        break;
    }

    for (int i = 0; i < printer.numCopies(); ++i) {
      foreach (QGraphicsView *view, pages) {
        view->render(&painter, QRectF(), QRect(), Qt::KeepAspectRatio /* IgnoreAspectRatio */);
        printer.newPage();
      }
    }
  }
}


void MainWindow::currentViewChanged() {
  _undoGroup->setActiveStack(_tabWidget->currentView()->undoStack());
}


void MainWindow::aboutToQuit() {
  writeSettings();
}


void MainWindow::about() {
  //FIXME Build a proper about box...
  QMessageBox::about(this, tr("About Kst"), tr("FIXME."));
}


void MainWindow::createPicture() {
  CreatePictureCommand *cmd = new CreatePictureCommand;
  cmd->createItem();
}


void MainWindow::createSvg() {
  CreateSvgCommand *cmd = new CreateSvgCommand;
  cmd->createItem();
}


void MainWindow::createBox() {
  CreateBoxCommand *cmd = new CreateBoxCommand;
  cmd->createItem();
}


void MainWindow::createEllipse() {
  CreateEllipseCommand *cmd = new CreateEllipseCommand;
  cmd->createItem();
}


void MainWindow::createLabel() {
  CreateLabelCommand *cmd = new CreateLabelCommand;
  cmd->createItem();
}


void MainWindow::createLine() {
  CreateLineCommand *cmd = new CreateLineCommand;
  cmd->createItem();
}


void MainWindow::createPlot() {
  CreatePlotCommand *cmd = new CreatePlotCommand;
  cmd->createItem();
}


void MainWindow::demoModel() {
  KstVectorPtr v = new KstVector;
  v->resize(999999);
  KstVectorPtr v2 = new KstVector;
  v2->resize(999999);
  KstVectorPtr v3 = new KstAVector(25, KstObjectTag::fromString("Editable V"));
  double *d = const_cast<double *>(v->value()); // yay :)
  double *d2 = const_cast<double *>(v2->value()); // yay :)
  d[0] = 1;
  d2[0] = 1;
  for (int i = 1; i < v->length(); ++i) {
    d[i] = d[i-1] + 0.002;
    d2[i] = d2[i-1] + 0.003;
  }
  KstEquationPtr ep = new KstEquation("My Equation", "x^2", 0, 100, 1000);
  ep->writeLock();
  ep->update(0);
  ep->unlock();
  KST::addDataObjectToList(ep.data());
}


void MainWindow::createActions() {
  _undoAct = _undoGroup->createUndoAction(this);
  _undoAct->setShortcut(tr("Ctrl+Z"));
  _redoAct = _undoGroup->createRedoAction(this);
  _redoAct->setShortcut(tr("Ctrl+Shift+Z"));

  _createLabelAct = new QAction(tr("&Create label"), this);
  _createLabelAct->setStatusTip(tr("Create a label for the current view"));
  _createLabelAct->setIcon(QPixmap(":kst_gfx_label.png"));
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createBoxAct = new QAction(tr("&Create box"), this);
  _createBoxAct->setStatusTip(tr("Create a box for the current view"));
  _createBoxAct->setIcon(QPixmap(":kst_gfx_rectangle.png"));
  connect(_createBoxAct, SIGNAL(triggered()), this, SLOT(createBox()));

  _createPictureAct = new QAction(tr("&Create picture"), this);
  _createPictureAct->setStatusTip(tr("Create a picture for the current view"));
  _createPictureAct->setIcon(QPixmap(":kst_gfx_picture.png"));
  connect(_createPictureAct, SIGNAL(triggered()), this, SLOT(createPicture()));

  _createSvgAct = new QAction(tr("&Create svg"), this);
  _createSvgAct->setStatusTip(tr("Create a svg for the current view"));
  connect(_createSvgAct, SIGNAL(triggered()), this, SLOT(createSvg()));

  _createEllipseAct = new QAction(tr("&Create ellipse"), this);
  _createEllipseAct->setStatusTip(tr("Create an ellipse for the current view"));
  _createEllipseAct->setIcon(QPixmap(":kst_gfx_ellipse.png"));
  connect(_createEllipseAct, SIGNAL(triggered()), this, SLOT(createEllipse()));

  _createLineAct = new QAction(tr("&Create line"), this);
  _createLineAct->setStatusTip(tr("Create a line for the current view"));
  _createLineAct->setIcon(QPixmap(":kst_gfx_line.png"));
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _createPlotAct = new QAction(tr("&Create plot"), this);
  _createPlotAct->setStatusTip(tr("Create a plot for the current view"));
  _createPlotAct->setIcon(QPixmap(":kst_newplot.png"));
  connect(_createPlotAct, SIGNAL(triggered()), this, SLOT(createPlot()));

  _newTabAct = new QAction(tr("&New tab"), this);
  _newTabAct->setStatusTip(tr("Create a new tab"));
  _newTabAct->setIcon(QPixmap(":kst_newtab.png"));
  connect(_newTabAct, SIGNAL(triggered()), tabWidget(), SLOT(createView()));

  _closeTabAct = new QAction(tr("&Close tab"), this);
  _closeTabAct->setStatusTip(tr("Close the current tab"));
  _closeTabAct->setIcon(QPixmap(":kst_closetab.png"));
  connect(_closeTabAct, SIGNAL(triggered()), tabWidget(), SLOT(closeCurrentView()));

  _saveAct = new QAction(tr("&Save"), this);
  _saveAct->setStatusTip(tr("Save the current session"));
  _saveAct->setShortcut(tr("Ctrl+S"));
  connect(_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  _saveAsAct = new QAction(tr("Save &as..."), this);
  _saveAsAct->setStatusTip(tr("Save the current session"));
  connect(_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  _openAct = new QAction(tr("&Open..."), this);
  _openAct->setStatusTip(tr("Open a new session"));
  _openAct->setShortcut(tr("Ctrl+O"));
  connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));

  _printAct = new QAction(tr("&Print..."), this);
  _printAct->setStatusTip(tr("Print the current view"));
  connect(_printAct, SIGNAL(triggered()), this, SLOT(print()));

  _exitAct = new QAction(tr("E&xit"), this);
  _exitAct->setShortcut(tr("Ctrl+Q"));
  _exitAct->setStatusTip(tr("Exit the application"));
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _dataManagerAct = new QAction(tr("Data &Manager..."), this);
  _dataManagerAct->setStatusTip(tr("Show Kst's data manager window"));
  _dataManagerAct->setIcon(QPixmap(":kst_datamanager.png"));
  connect(_dataManagerAct, SIGNAL(triggered()), this, SLOT(showDataManager()));

  _vectorEditorAct = new QAction(tr("&Vectors..."), this);
  _vectorEditorAct->setStatusTip(tr("Show all vectors in a spreadsheet"));
  connect(_vectorEditorAct, SIGNAL(triggered()), this, SLOT(showVectorEditor()));

  _exportGraphicsAct = new QAction(tr("&Export..."), this);
  _exportGraphicsAct->setStatusTip(tr("Export graphics to disk"));
  connect(_exportGraphicsAct, SIGNAL(triggered()), this, SLOT(showExportGraphicsDialog()));

  _debugDialogAct = new QAction(tr("&Debug Dialog..."), this);
  _debugDialogAct->setStatusTip(tr("Show the Kst debugging dialog"));
  connect(_debugDialogAct, SIGNAL(triggered()), this, SLOT(showDebugDialog()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}


void MainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_newTabAct);
  _fileMenu->addAction(_openAct);
  _fileMenu->addAction(_saveAct);
  _fileMenu->addAction(_saveAsAct);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_printAct);
  _fileMenu->addAction(_exportGraphicsAct);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_closeTabAct);
  _fileMenu->addAction(_exitAct);

  _editMenu = menuBar()->addMenu(tr("&Edit"));
  _editMenu->addAction(_undoAct);
  _editMenu->addAction(_redoAct);

  _dataMenu = menuBar()->addMenu(tr("&Data"));
  _dataMenu->addAction(_dataManagerAct);
  _dataMenu->addAction(_vectorEditorAct);

  _plotMenu = menuBar()->addMenu(tr("&Plot"));
  _plotMenu->addAction(_createLabelAct);
  _plotMenu->addAction(_createBoxAct);
  _plotMenu->addAction(_createEllipseAct);
  _plotMenu->addAction(_createLineAct);
  _plotMenu->addAction(_createPictureAct);
  _plotMenu->addAction(_createPlotAct);
  _plotMenu->addAction(_createSvgAct);

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_debugDialogAct);
  _helpMenu->addAction(_aboutAct);

  // FIXME: remove this later.
  QMenu *demoMenu = menuBar()->addMenu("&Demo");
  QAction *demo = new QAction("Vector model", this);
  connect(demo, SIGNAL(triggered()), this, SLOT(demoModel()));
  demoMenu->addAction(demo);
}


void MainWindow::createToolBars() {
  _fileToolBar = addToolBar(tr("File"));
  _editToolBar = addToolBar(tr("Edit"));
  _kstToolBar = addToolBar(tr("Kst"));

  // Hook up the kst toolbar
  _kstToolBar->addAction(_dataManagerAct);
}


void MainWindow::createStatusBar() {
  _progressBar = new QProgressBar(statusBar());
  _progressBar->hide();
  statusBar()->addPermanentWidget(_progressBar);
  MemoryWidget *mw = new MemoryWidget(statusBar());
  statusBar()->addPermanentWidget(mw);
  DebugNotifier *dn = new DebugNotifier(statusBar());
  connect(dn, SIGNAL(showDebugLog()), this, SLOT(showDebugDialog()));
  connect(_debugDialog, SIGNAL(notifyOfError()), dn, SLOT(reanimate()));
  connect(_debugDialog, SIGNAL(notifyAllClear()), dn, SLOT(close()));
  statusBar()->addPermanentWidget(dn);
  statusBar()->showMessage(tr("Ready"));
}


QProgressBar *MainWindow::progressBar() const {
  return _progressBar;
}


void MainWindow::showDataManager() {
  if (!_dataManager) {
    _dataManager = new DataManager(this, _doc);
  }
  _dataManager->show();
}


void MainWindow::showVectorEditor() {
  if (!_vectorEditor) {
    _vectorEditor = new VectorEditorDialog(this, _doc);
  }
  _vectorEditor->show();
}


void MainWindow::showDebugDialog() {
  if (!_debugDialog) {
    _debugDialog = new DebugDialog(this);
  }
  _debugDialog->show();
}


void MainWindow::showExportGraphicsDialog() {
  if (!_exportGraphics) {
    _exportGraphics = new ExportGraphicsDialog(this);
  }
  _exportGraphics->show();
}


void MainWindow::readSettings() {
  QSettings settings;
  QPoint pos = settings.value("pos", QPoint(20, 20)).toPoint();
  QSize size = settings.value("size", QSize(800, 600)).toSize();
  resize(size);
  move(pos);
}


void MainWindow::writeSettings() {
  QSettings settings;
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

}

// vim: ts=2 sw=2 et
