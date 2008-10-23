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
#include "application.h"
#include "debug.h"
#include "labelitem.h"
#include "lineitem.h"
#include "circleitem.h"
#include "arrowitem.h"
#include "memorywidget.h"
#include "objectstore.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "plotitemmanager.h"
#include "svgitem.h"
#include "tabwidget.h"
#include "ui_aboutdialog.h"
#include "viewvectordialog.h"
#include "matrixeditordialog.h"
#include "viewprimitivedialog.h"
#include "view.h"
#include "viewmanager.h"
#include "updatemanager.h"
#include "applicationsettings.h"

#include "applicationsettingsdialog.h"
#include "differentiatecurvesdialog.h"
#include "choosecolordialog.h"
#include "changedatasampledialog.h"
#include "changefiledialog.h"
#include "datawizard.h"
#include "datavector.h"

#include <QtGui>

// Enable Demo Vector Model 0 Disabled 1 Enabled.
#define DEMO_VECTOR_MODEL 0

#if DEMO_VECTOR_MODEL
#include "equation.h"
#include "generatedvector.h"
#endif


namespace Kst {

MainWindow::MainWindow() {
  _dataManager = 0;
  _exportGraphics = 0;
  _doc = new Document(this);
  _tabWidget = new TabWidget(this);
  _undoGroup = new QUndoGroup(this);
  _debugDialog = new DebugDialog(this);
  Debug::self()->setHandler(_debugDialog);

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  _tabWidget->createView();
  setCentralWidget(_tabWidget);
  connect(_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentViewChanged()));

  readSettings();
  QTimer::singleShot(0, this, SLOT(performHeavyStartupActions()));
}


MainWindow::~MainWindow() {
  delete _dataManager;
  _dataManager = 0;
  delete _doc;
  _doc = 0;
}


void MainWindow::performHeavyStartupActions() {
  // Set the timer for the UpdateManager.
  UpdateManager::self()->setMinimumUpdatePeriod(ApplicationSettings::self()->minimumUpdatePeriod());
}


void MainWindow::cleanup() {
  if (document() && document()->objectStore()) {
    document()->objectStore()->clear();
  }
}


void MainWindow::setLayoutMode(bool layoutMode) {
  View *v = tabWidget()->currentView();
  Q_ASSERT(v);

  if (layoutMode)
    v->setViewMode(View::Layout);
  else
    v->setViewMode(View::Data);

  //disable all layout actions
  _createLabelAct->setEnabled(layoutMode);
  _createBoxAct->setEnabled(layoutMode);
  _createCircleAct->setEnabled(layoutMode);
  _createEllipseAct->setEnabled(layoutMode);
  _createLineAct->setEnabled(layoutMode);
  _createArrowAct->setEnabled(layoutMode);
  _createPictureAct->setEnabled(layoutMode);
  _createPlotAct->setEnabled(layoutMode);
  _createSvgAct->setEnabled(layoutMode);
  _createLayoutAct->setEnabled(layoutMode);

  _layoutToolBar->setVisible(layoutMode);
  _zoomToolBar->setVisible(!layoutMode);
}


void MainWindow::setTiedZoom(bool tiedZoom) {
  View *v = tabWidget()->currentView();
  QList<PlotItem*> plots = PlotItemManager::plotsForView(v);
  foreach (PlotItem *plotItem, plots) {
    plotItem->setTiedZoom(tiedZoom);
  }
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

void MainWindow::initFromCommandLine() {
  delete _doc;
  _doc = new Document(this);
  bool ok = _doc->initFromCommandLine();
  if (!ok) {
    //FIXME: should now exit cleanly
    exit(0);
  }
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


void MainWindow::exportGraphicsFile(const QString &filename, const QString &format, int width, int height, int display) {
  int viewCount = 0;
  foreach (QGraphicsView *view, _tabWidget->views()) {
    QSize size;
    if (display == 0) {
      size.setWidth(width);
      size.setHeight(height);
    } else if (display == 1) {
      size.setWidth(width);
      size.setHeight(width);
    } else if (display == 2) {
      QSize sizeWindow(view->geometry().size());

      size.setWidth(width);
      size.setHeight((int)((double)width * (double)sizeWindow.height() / (double)sizeWindow.width()));
    } else {
      QSize sizeWindow(view->geometry().size());

      size.setHeight(height);
      size.setWidth((int)((double)height * (double)sizeWindow.width() / (double)sizeWindow.height()));
    }

    QImage image(size, QImage::Format_ARGB32);

    QPainter painter(&image);
    view->render(&painter, QRectF(0, 0, size.width(), size.height()), QRect(), Qt::KeepAspectRatio);

    QString file = filename;
    if (viewCount != 0) {
      file += "_";
      file += QString::number(viewCount);
    }

    QImageWriter imageWriter(file, format.toLatin1());
    imageWriter.write(image);
    viewCount++;
  }
}


void MainWindow::print() {
  QPrinter printer(QPrinter::HighResolution);

  QPrintDialog pd(&printer, this);
  pd.addEnabledOption(QPrintDialog::PrintToFile);
  pd.addEnabledOption(QPrintDialog::PrintPageRange);

  if (pd.exec() == QDialog::Accepted) {

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QPainter painter(&printer);
    QList<View*> pages;
    switch (printer.printRange()) {
      case QPrinter::PageRange:
        break;
      case QPrinter::AllPages:
        foreach (View *view, _tabWidget->views()) {
          pages.append(view);
        }
        break;
      case QPrinter::Selection:
      default:
        pages.append(_tabWidget->currentView());
        break;
    }

    for (int i = 0; i < printer.numCopies(); ++i) {
      foreach (View *view, pages) {
        view->setPrinting(true);
        view->render(&painter);
        view->setPrinting(false);
        printer.newPage();
      }
    }

    QApplication::restoreOverrideCursor();
  }
}


void MainWindow::currentViewChanged() {
  _undoGroup->setActiveStack(_tabWidget->currentView()->undoStack());
  _layoutModeAct->setChecked(_tabWidget->currentView()->viewMode() == View::Layout);
}


void MainWindow::aboutToQuit() {
  writeSettings();
}


void MainWindow::about() {
  QDialog dlg;
  Ui::AboutDialog ui;
  ui.setupUi(&dlg);
  // Sorted alphabetically, first group is 2.0 contributors
  const QString msg = tr(
  "<qt><h2>Kst 2.0 - A data viewing program.</h2>\n<hr>\n"
  "Copyright &copy; 2000-2007 Barth Netterfield<br>"
  "<a href=\"http://kst.kde.org/\">http://kst.kde.org/</a><br>"
  "Please report bugs to: <a href=\"http://bugs.kde.org/\">http://bugs.kde.org/</a><br>"
  "Authors:<ul>"
  "<li>Barth Netterfield</li>"
  "<li><a href=\"http://www.staikos.net/\">Staikos Computing Services Inc.</a></li>"
  "<li>Ted Kisner</li>"
  "<li>The University of Toronto</li>"
  "</ul><ul>"
  "<li>Matthew Truch</li>"
  "<li>Nicolas Brisset</li>"
  "<li>Rick Chern</li>"
  "<li>Sumus Technology Limited</li>"
  "<li>The University of British Columbia</li>"
  "</ul>"
  );
  ui.text->setText(msg);
  dlg.exec();
}


void MainWindow::createBox() {
  CreateBoxCommand *cmd = new CreateBoxCommand;
  cmd->createItem();
}


void MainWindow::createCircle() {
  CreateCircleCommand *cmd = new CreateCircleCommand;
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


void MainWindow::createArrow() {
  CreateArrowCommand *cmd = new CreateArrowCommand;
  cmd->createItem();
}


void MainWindow::createPicture() {
  CreatePictureCommand *cmd = new CreatePictureCommand;
  cmd->createItem();
}


void MainWindow::createPlot() {
  CreatePlotCommand *cmd = new CreatePlotCommand;
  cmd->createItem();
}


void MainWindow::createSvg() {
  CreateSvgCommand *cmd = new CreateSvgCommand;
  cmd->createItem();
}


void MainWindow::createLayout() {
  View *view = tabWidget()->currentView();
  QList<QGraphicsItem*> selectedItems = view->scene()->selectedItems();
  if (!selectedItems.isEmpty()) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(selectedItems.first());
    Q_ASSERT(viewItem);
    viewItem->createAutoLayout();
  } else {
    view->createLayout();
  }
}


void MainWindow::demoModel() {
#if DEMO_VECTOR_MODEL
  Q_ASSERT(document() && document()->objectStore());
  GeneratedVectorPtr gv = kst_cast<GeneratedVector>(document()->objectStore()->createObject<GeneratedVector>());
  Q_ASSERT(gv);
  gv->changeRange(0, 100, 1000);
  EquationPtr ep = kst_cast<Equation>(document()->objectStore()->createObject<Equation>());
  Q_ASSERT(ep);
  ep->setExistingXVector(VectorPtr(gv), false);
  ep->setEquation("x^2");
  ep->writeLock();
  ep->update();
  ep->unlock();
#endif
}


void MainWindow::createActions() {
  _undoAct = _undoGroup->createUndoAction(this);
  _undoAct->setShortcut(tr("Ctrl+Z"));
  _redoAct = _undoGroup->createRedoAction(this);
  _redoAct->setShortcut(tr("Ctrl+Shift+Z"));

  // ************************** Layout Mode Actions ******************************* //
  _createLabelAct = new QAction(tr("&Create label"), this);
  _createLabelAct->setStatusTip(tr("Create a label for the current view"));
  _createLabelAct->setIcon(QPixmap(":kst_gfx_label.png"));
  _createLabelAct->setEnabled(false);
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createBoxAct = new QAction(tr("&Create box"), this);
  _createBoxAct->setStatusTip(tr("Create a box for the current view"));
  _createBoxAct->setIcon(QPixmap(":kst_gfx_rectangle.png"));
  _createBoxAct->setEnabled(false);
  connect(_createBoxAct, SIGNAL(triggered()), this, SLOT(createBox()));

  _createCircleAct = new QAction(tr("&Create circle"), this);
  _createCircleAct->setStatusTip(tr("Create a circle for the current view"));
  _createCircleAct->setIcon(QPixmap(":kst_gfx_ellipse.png"));
  _createCircleAct->setEnabled(false);
  connect(_createCircleAct, SIGNAL(triggered()), this, SLOT(createCircle()));

  _createEllipseAct = new QAction(tr("&Create ellipse"), this);
  _createEllipseAct->setStatusTip(tr("Create an ellipse for the current view"));
  _createEllipseAct->setIcon(QPixmap(":kst_gfx_ellipse.png"));
  _createEllipseAct->setEnabled(false);
  connect(_createEllipseAct, SIGNAL(triggered()), this, SLOT(createEllipse()));

  _createLineAct = new QAction(tr("&Create line"), this);
  _createLineAct->setStatusTip(tr("Create a line for the current view"));
  _createLineAct->setIcon(QPixmap(":kst_gfx_line.png"));
  _createLineAct->setEnabled(false);
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _createArrowAct = new QAction(tr("&Create arrow"), this);
  _createArrowAct->setStatusTip(tr("Create a arrow for the current view"));
  _createArrowAct->setIcon(QPixmap(":kst_gfx_arrow.png"));
  _createArrowAct->setEnabled(false);
  connect(_createArrowAct, SIGNAL(triggered()), this, SLOT(createArrow()));

  _createPictureAct = new QAction(tr("&Create picture"), this);
  _createPictureAct->setStatusTip(tr("Create a picture for the current view"));
  _createPictureAct->setIcon(QPixmap(":kst_gfx_picture.png"));
  _createPictureAct->setEnabled(false);
  connect(_createPictureAct, SIGNAL(triggered()), this, SLOT(createPicture()));

  _createPlotAct = new QAction(tr("&Create plot"), this);
  _createPlotAct->setStatusTip(tr("Create a plot for the current view"));
  _createPlotAct->setIcon(QPixmap(":kst_newplot.png"));
  _createPlotAct->setEnabled(false);
  connect(_createPlotAct, SIGNAL(triggered()), this, SLOT(createPlot()));

  _createSvgAct = new QAction(tr("&Create svg"), this);
  _createSvgAct->setStatusTip(tr("Create a svg for the current view"));
  _createSvgAct->setEnabled(false);
  connect(_createSvgAct, SIGNAL(triggered()), this, SLOT(createSvg()));

  _createLayoutAct = new QAction(tr("&Create layout"), this);
  _createLayoutAct->setStatusTip(tr("Create a layout for the current item"));
  _createLayoutAct->setIcon(QPixmap(":kst_gfx_layout.png"));
  _createLayoutAct->setEnabled(false);
  connect(_createLayoutAct, SIGNAL(triggered()), this, SLOT(createLayout()));

  // ****************************************************************************** //

  // ********************* Object Dialog Actions ********************************** //
  _dataManagerAct = new QAction(tr("Data &Manager..."), this);
  _dataManagerAct->setStatusTip(tr("Show Kst's data manager window"));
  _dataManagerAct->setIcon(QPixmap(":kst_datamanager.png"));
  connect(_dataManagerAct, SIGNAL(triggered()), this, SLOT(showDataManager()));

  _vectorEditorAct = new QAction(tr("&View Vectors..."), this);
  _vectorEditorAct->setStatusTip(tr("Show all vectors in a spreadsheet"));
  connect(_vectorEditorAct, SIGNAL(triggered()), this, SLOT(showVectorEditor()));

  _scalarEditorAct = new QAction(tr("View &Scalars..."), this);
  _scalarEditorAct->setStatusTip(tr("Show all scalars in a spreadsheet"));
  connect(_scalarEditorAct, SIGNAL(triggered()), this, SLOT(showScalarEditor()));

  _stringEditorAct = new QAction(tr("View S&trings..."), this);
  _stringEditorAct->setStatusTip(tr("Show all strings in a spreadsheet"));
  connect(_stringEditorAct, SIGNAL(triggered()), this, SLOT(showStringEditor()));

  _matrixEditorAct = new QAction(tr("View &Matrix..."), this);
  _matrixEditorAct->setStatusTip(tr("Show all matrices in a spreadsheet"));
  connect(_matrixEditorAct, SIGNAL(triggered()), this, SLOT(showMatrixEditor()));

  _differentiateCurvesDialogAct = new QAction(tr("&Differentiate Curves"), this);
  _differentiateCurvesDialogAct->setStatusTip(tr("Show Kst's Differentiate Curves Dialog"));
  connect(_differentiateCurvesDialogAct, SIGNAL(triggered()), this, SLOT(showDifferentiateCurvesDialog()));

  _chooseColorDialogAct = new QAction(tr("Assign &Curve Color per File"), this);
  _chooseColorDialogAct->setStatusTip(tr("Show Kst's Choose Color Dialog"));
  connect(_chooseColorDialogAct, SIGNAL(triggered()), this, SLOT(showChooseColorDialog()));

  _changeDataSampleDialogAct = new QAction(tr("Change Data Sample Range"), this);
  _changeDataSampleDialogAct->setStatusTip(tr("Show Kst's Change Data Sample Range Dialog"));
  _changeDataSampleDialogAct->setIcon(QPixmap(":kst_changenpts.png"));
  connect(_changeDataSampleDialogAct, SIGNAL(triggered()), this, SLOT(showChangeDataSampleDialog()));

  _changeFileDialogAct = new QAction(tr("Change Data &File"), this);
  _changeFileDialogAct->setStatusTip(tr("Show Kst's Change Data File Dialog"));
  _changeFileDialogAct->setIcon(QPixmap(":kst_changefile.png"));
  connect(_changeFileDialogAct, SIGNAL(triggered()), this, SLOT(showChangeFileDialog()));

  _dataWizardAct = new QAction(tr("&Data Wizard"), this);
  _dataWizardAct->setStatusTip(tr("Show Kst's Data Wizard"));
  _dataWizardAct->setIcon(QPixmap(":kst_datawizard.png"));
  connect(_dataWizardAct, SIGNAL(triggered()), this, SLOT(showDataWizard()));

  // ****************************************************************************** //

  // ***************************** -> File actions ******************************** //
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

  _exportGraphicsAct = new QAction(tr("&Export..."), this);
  _exportGraphicsAct->setStatusTip(tr("Export graphics to disk"));
  connect(_exportGraphicsAct, SIGNAL(triggered()), this, SLOT(showExportGraphicsDialog()));
  // ****************************************************************************** //

  // *********************** -> Help actions ************************************** //
  _debugDialogAct = new QAction(tr("&Debug Dialog..."), this);
  _debugDialogAct->setStatusTip(tr("Show the Kst debugging dialog"));
  connect(_debugDialogAct, SIGNAL(triggered()), this, SLOT(showDebugDialog()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _settingsDialogAct = new QAction(tr("&Configure Kst"), this);
  _settingsDialogAct->setStatusTip(tr("Show Kst's Configuration Dialog"));
  connect(_settingsDialogAct, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

  // ****************************************************************************** //

  // ************************ Data Range 1 click Actions ************************** //
  _readFromEndAct = new QAction(tr("Read From End..."), this);
  _readFromEndAct->setStatusTip(tr("Set all data vectors to count from end mode"));
  _readFromEndAct->setIcon(QPixmap(":kst_readFromEnd.png"));
  connect(_readFromEndAct, SIGNAL(triggered()), this, SLOT(readFromEnd()));

  _pauseAct = new QAction(tr("Pause..."), this);
  _pauseAct->setStatusTip(tr("Toggle pause updates of data sources"));
  _pauseAct->setIcon(QPixmap(":kst_pause.png"));
  _pauseAct->setCheckable(true);
  connect(_pauseAct, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));

  _backAct = new QAction(tr("Back One Screen..."), this);
  _backAct->setStatusTip(tr("Back one screen"));
  _backAct->setIcon(QPixmap(":kst_back.png"));
  connect(_backAct, SIGNAL(triggered()), this, SLOT(back()));

  _forwardAct = new QAction(tr("Forward One Screen..."), this);
  _forwardAct->setStatusTip(tr("Forward one screen"));
  _forwardAct->setIcon(QPixmap(":kst_forward.png"));
  connect(_forwardAct, SIGNAL(triggered()), this, SLOT(forward()));
  // ****************************************************************************** //

  _layoutModeAct = new QAction(tr("&Layout Mode"), this);
  _layoutModeAct->setStatusTip(tr("Toggle the current view's layout mode"));
  _layoutModeAct->setIcon(QPixmap(":kst_layoutmode.png"));
  _layoutModeAct->setCheckable(true);
  connect(_layoutModeAct, SIGNAL(toggled(bool)), this, SLOT(setLayoutMode(bool)));

  _tiedZoomAct = new QAction(tr("&Tied Zoom"), this);
  _tiedZoomAct->setStatusTip(tr("Toggle the current view's tied zoom"));
  _tiedZoomAct->setIcon(QPixmap(":kst_zoomtie.png"));
  _tiedZoomAct->setCheckable(true);
  connect(_tiedZoomAct, SIGNAL(toggled(bool)), this, SLOT(setTiedZoom(bool)));

  _newTabAct = new QAction(tr("&New tab"), this);
  _newTabAct->setStatusTip(tr("Create a new tab"));
  _newTabAct->setIcon(QPixmap(":kst_newtab.png"));
  connect(_newTabAct, SIGNAL(triggered()), tabWidget(), SLOT(createView()));

  _closeTabAct = new QAction(tr("&Close tab"), this);
  _closeTabAct->setStatusTip(tr("Close the current tab"));
  _closeTabAct->setIcon(QPixmap(":kst_closetab.png"));
  connect(_closeTabAct, SIGNAL(triggered()), tabWidget(), SLOT(closeCurrentView()));

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
  _dataMenu->addSeparator();
  _dataMenu->addAction(_vectorEditorAct);
  _dataMenu->addAction(_scalarEditorAct);
  _dataMenu->addAction(_matrixEditorAct);
  _dataMenu->addAction(_stringEditorAct);

  _viewMenu = menuBar()->addMenu(tr("&View"));
  _viewMenu->addAction(_tiedZoomAct);
  _viewMenu->addSeparator();

  _layoutMenu = _viewMenu->addMenu(tr("&Layout"));

  _layoutMenu->setIcon(QPixmap(":kst_layoutmode.png"));

  _layoutMenu->addAction(_layoutModeAct);
  _layoutMenu->addSeparator();
  _layoutMenu->addAction(_createLabelAct);
  _layoutMenu->addAction(_createBoxAct);
  _layoutMenu->addAction(_createCircleAct);
  _layoutMenu->addAction(_createEllipseAct);
  _layoutMenu->addAction(_createLineAct);
  _layoutMenu->addAction(_createArrowAct);
  _layoutMenu->addAction(_createPictureAct);
  _layoutMenu->addAction(_createPlotAct);
  _layoutMenu->addAction(_createSvgAct);

  _toolsMenu = menuBar()->addMenu(tr("&Tools"));
  _toolsMenu->addAction(_dataWizardAct);
  _toolsMenu->addAction(_changeFileDialogAct);
  _toolsMenu->addAction(_changeDataSampleDialogAct);
  _toolsMenu->addAction(_chooseColorDialogAct);
  _toolsMenu->addAction(_differentiateCurvesDialogAct);

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));
  _settingsMenu->addAction(_settingsDialogAct);

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_debugDialogAct);
  _helpMenu->addAction(_aboutAct);

#if DEMO_VECTOR_MODEL
  QMenu *demoMenu = menuBar()->addMenu("&Demo");

  QAction *demoModel = new QAction("Vector model", this);
  connect(demoModel, SIGNAL(triggered()), this, SLOT(demoModel()));
  demoMenu->addAction(demoModel);
#endif
}


void MainWindow::createToolBars() {
  setToolButtonStyle(Qt::ToolButtonIconOnly);

  _dataToolBar = addToolBar(tr("Data"));
  _dataToolBar->addAction(_dataManagerAct);
  _dataToolBar->addAction(_dataWizardAct);
//   _dataToolBar->addAction(_vectorEditorAct); //no icon
//   _dataToolBar->addAction(_scalarEditorAct); //no icon
//   _dataToolBar->addAction(_matrixEditorAct); //no icon
  _dataToolBar->addAction(_backAct);
  _dataToolBar->addAction(_forwardAct);
  _dataToolBar->addAction(_pauseAct);
  _dataToolBar->addAction(_readFromEndAct);

  _layoutToggleToolBar = addToolBar(tr("Mode"));
  _layoutToggleToolBar->addAction(_layoutModeAct);

  _zoomToolBar = addToolBar(tr("Zoom"));
  _zoomToolBar->addAction(_tiedZoomAct);

  _layoutToolBar = new QToolBar(tr("Layout"), this);
  _layoutToolBar->addAction(_createLabelAct); //no icon
  _layoutToolBar->addAction(_createBoxAct);
  _layoutToolBar->addAction(_createCircleAct);
  _layoutToolBar->addAction(_createEllipseAct);
  _layoutToolBar->addAction(_createLineAct);
  _layoutToolBar->addAction(_createArrowAct);
  _layoutToolBar->addAction(_createPictureAct);
  _layoutToolBar->addAction(_createPlotAct);
//  _layoutToolBar->addAction(_createSvgAct); //no icon

  _layoutToolBar->addSeparator();

  _layoutToolBar->addAction(_createLayoutAct);
  _layoutToolBar->setVisible(false);
  _zoomToolBar->setVisible(true);

  addToolBar(Qt::TopToolBarArea, _layoutToolBar);
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

void MainWindow::readFromEnd() {
  int nf; 
  int skip;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();

  foreach (DataVectorPtr v, dataVectors) {
    v->readLock();
    nf = v->reqNumFrames();
    skip = v->skip();
    do_skip = v->doSkip();
    do_filter = v->doAve();
    v->unlock();

    v->writeLock();
    v->changeFrames(-1, nf, skip, do_skip, do_filter);
    v->immediateUpdate();
    v->unlock();
  }
} 

void MainWindow::pause(bool pause) {
  UpdateManager::self()->setPaused(pause);

  if (!pause) {
     foreach (DataSourcePtr s, document()->objectStore()->dataSourceList()) {
       s->checkUpdate();
     }
  }
}

void MainWindow::forward() {
  int f0;
  int nf;
  int skip;
  int filelength;
  bool count_from_end;
  bool read_to_end;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();

  foreach (DataVectorPtr v, dataVectors) {
    v->readLock();
    f0 = v->startFrame();
    nf = v->numFrames();
    count_from_end = v->countFromEOF();
    read_to_end = v->readToEOF();
    filelength = v->fileLength();

    skip = v->skip();
    do_skip = v->doSkip();
    do_filter = v->doAve();
    v->unlock(); 

    if ((!count_from_end) && (!read_to_end)) {
      f0 += nf;
      if (f0+nf>=filelength) {
        f0 = filelength - nf;
      }

      v->writeLock(); 
      v->changeFrames(f0, nf, skip, do_skip, do_filter);
      v->immediateUpdate();
      v->unlock();
    }
  }
}

void MainWindow::back() {
  int f0;
  int nf;
  int skip;
  int filelength;
  bool count_from_end;
  bool read_to_end;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();

  foreach (DataVectorPtr v, dataVectors) {
    v->readLock();
    f0 = v->startFrame();
    nf = v->numFrames();
    count_from_end = v->countFromEOF();
    read_to_end = v->readToEOF();
    filelength = v->fileLength();

    skip = v->skip();
    do_skip = v->doSkip();
    do_filter = v->doAve();
    v->unlock(); 

    if (f0 != 0) {
      if (count_from_end) {
        f0 = filelength - nf;
      }
      if (read_to_end) {
        nf = filelength - f0;
      }

      f0 -= nf;
      if (f0<0) {
        f0 = 0;
      }

      v->writeLock(); 
      v->changeFrames(f0, nf, skip, do_skip, do_filter);
      v->immediateUpdate();
      v->unlock();
    }
  }
}

void MainWindow::showDataManager() {
  if (!_dataManager) {
    _dataManager = new DataManager(this, _doc);
  }
  _dataManager->show();
}


void MainWindow::showVectorEditor() {
  ViewVectorDialog vectorDialog(this, _doc);
  vectorDialog.exec();
}


void MainWindow::showScalarEditor() {
  ViewPrimitiveDialog scalarDialog(this, _doc, ViewPrimitiveDialog::Scalar);
  scalarDialog.exec();
}


void MainWindow::showStringEditor() {
  ViewPrimitiveDialog stringDialog(this, _doc, ViewPrimitiveDialog::String);
  stringDialog.exec();
}

void MainWindow::showMatrixEditor() {
  MatrixEditorDialog matrixDialog(this, _doc);
  matrixDialog.exec();
}


void MainWindow::showDebugDialog() {
  if (!_debugDialog) {
    _debugDialog = new DebugDialog(this);
  }
  _debugDialog->setObjectStore(document()->objectStore());
  _debugDialog->show();
}


void MainWindow::showExportGraphicsDialog() {
  if (!_exportGraphics) {
    _exportGraphics = new ExportGraphicsDialog(this);
    connect(_exportGraphics, SIGNAL(exportGraphics(const QString &, const QString &, int, int, int)), this, SLOT(exportGraphicsFile(const QString &, const QString &, int, int, int)));
  }
  _exportGraphics->show();
}


void MainWindow::showSettingsDialog() {
  ApplicationSettingsDialog settingsDialog(this);
  settingsDialog.exec();
}


void MainWindow::showDifferentiateCurvesDialog() {
  DifferentiateCurvesDialog differentiateCurvesDialog(this);
  differentiateCurvesDialog.exec();
}


void MainWindow::showChooseColorDialog() {
  ChooseColorDialog chooseColorDialog(this);
  chooseColorDialog.exec();
}


void MainWindow::showChangeDataSampleDialog() {
  ChangeDataSampleDialog changeDataSampleDialog(this);
  changeDataSampleDialog.exec();
}


void MainWindow::showDataWizard() {
  DataWizard dataWizard(this);
  dataWizard.exec();
}


void MainWindow::showChangeFileDialog() {
  ChangeFileDialog changeFileDialog(this);
  changeFileDialog.exec();
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
