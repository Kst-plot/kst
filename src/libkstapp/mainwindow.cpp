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
#include "document.h"
#include "ellipseitem.h"
#include "kstapplication.h"
#include "labelitem.h"
#include "lineitem.h"
#include "memorywidget.h"
#include "pictureitem.h"
#include "plotitem.h"
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
  _vectorEditor = 0;
  _doc = new Document;
  _tabWidget = new TabWidget(this);
  _undoGroup = new QUndoGroup(this);

  connect(_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentViewChanged()));

  _tabWidget->createView();

  setCentralWidget(_tabWidget);

  createActions();
  createMenus();
//   createToolBars();
  createStatusBar();

  readSettings();
}


MainWindow::~MainWindow() {
  delete _vectorEditor;
  _vectorEditor = 0;
  delete _dataManager;
  _dataManager = 0;
  delete _doc;
  _doc = 0;
}


void MainWindow::cleanup() {
  KST::dataObjectList.lock().writeLock();
  KstDataObjectList dol = KST::dataObjectList;
  KST::dataObjectList.clear();
  KST::dataObjectList.lock().unlock();
  dol.clear();
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


void MainWindow::closeEvent(QCloseEvent *e) {
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
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createBoxAct = new QAction(tr("&Create box"), this);
  _createBoxAct->setStatusTip(tr("Create a box for the current view"));
  connect(_createBoxAct, SIGNAL(triggered()), this, SLOT(createBox()));

  _createPictureAct = new QAction(tr("&Create picture"), this);
  _createPictureAct->setStatusTip(tr("Create a picture for the current view"));
  connect(_createPictureAct, SIGNAL(triggered()), this, SLOT(createPicture()));

  _createEllipseAct = new QAction(tr("&Create ellipse"), this);
  _createEllipseAct->setStatusTip(tr("Create an ellipse for the current view"));
  connect(_createEllipseAct, SIGNAL(triggered()), this, SLOT(createEllipse()));

  _createLineAct = new QAction(tr("&Create line"), this);
  _createLineAct->setStatusTip(tr("Create a line for the current view"));
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _createPlotAct = new QAction(tr("&Create plot"), this);
  _createPlotAct->setStatusTip(tr("Create a plot for the current view"));
  connect(_createPlotAct, SIGNAL(triggered()), this, SLOT(createPlot()));

  _newTabAct = new QAction(tr("&New tab"), this);
  _newTabAct->setStatusTip(tr("Create a new tab"));
  connect(_newTabAct, SIGNAL(triggered()), tabWidget(), SLOT(createView()));

  _closeTabAct = new QAction(tr("&Close tab"), this);
  _closeTabAct->setStatusTip(tr("Close the current tab"));
  connect(_closeTabAct, SIGNAL(triggered()), tabWidget(), SLOT(closeCurrentView()));

  _saveAct = new QAction(tr("&Save"), this);
  _saveAct->setStatusTip(tr("Save the current session"));
  //connect(_saveAct, SIGNAL(triggered()), document(), SLOT(save()));

  _saveAsAct = new QAction(tr("Save &as..."), this);
  _saveAsAct->setStatusTip(tr("Save the current session"));
  //connect(_saveAsAct, SIGNAL(triggered()), document(), SLOT(saveAs()));

  _openAct = new QAction(tr("&Open..."), this);
  _openAct->setStatusTip(tr("Open a new session"));
  //connect(_openAct, SIGNAL(triggered()), this, SLOT(openDocument()));

  _printAct = new QAction(tr("&Print..."), this);
  _printAct->setStatusTip(tr("Print the current view"));
  //connect(_printAct, SIGNAL(triggered()), this, SLOT(print()));

  _exitAct = new QAction(tr("E&xit"), this);
  _exitAct->setShortcut(tr("Ctrl+Q"));
  _exitAct->setStatusTip(tr("Exit the application"));
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _dataManagerAct = new QAction(tr("Data &Manager..."), this);
  _dataManagerAct->setStatusTip(tr("Show Kst's data manager window"));
  connect(_dataManagerAct, SIGNAL(triggered()), this, SLOT(showDataManager()));

  _vectorEditorAct = new QAction(tr("&Vectors..."), this);
  _vectorEditorAct->setStatusTip(tr("Show all vectors in a spreadsheet"));
  connect(_vectorEditorAct, SIGNAL(triggered()), this, SLOT(showVectorEditor()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _aboutQtAct = new QAction(tr("About &Qt"), this);
  _aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_newTabAct);
  _fileMenu->addAction(_saveAct);
  _fileMenu->addAction(_saveAsAct);
  _fileMenu->addAction(_openAct);
  _fileMenu->addSeparator();
  _fileMenu->addAction(_printAct);
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

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAct);
  _helpMenu->addAction(_aboutQtAct);

  // FIXME: remove this later.
  QMenu *demoMenu = menuBar()->addMenu("&Demo");
  QAction *demo = new QAction("Vector model", this);
  connect(demo, SIGNAL(triggered()), this, SLOT(demoModel()));
  demoMenu->addAction(demo);
}


void MainWindow::createToolBars() {
  _fileToolBar = addToolBar(tr("File"));
  _editToolBar = addToolBar(tr("Edit"));
}


void MainWindow::createStatusBar() {
  MemoryWidget *mw = new MemoryWidget(statusBar());
  statusBar()->addPermanentWidget(mw);
  _progressBar = new QProgressBar(statusBar());
  _progressBar->hide();
  statusBar()->addPermanentWidget(_progressBar);
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

#include "mainwindow.moc"

// vim: ts=2 sw=2 et
