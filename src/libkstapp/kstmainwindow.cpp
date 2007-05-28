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

#include "kstmainwindow.h"
#include "kstapplication.h"
#include "kstplotview.h"

#include "kstplotcommands.h"

#include <QtGui>

using namespace Kst;

KstMainWindow::KstMainWindow() {

  _tabWidget = new QTabWidget(this);
  _undoGroup = new QUndoGroup(this);

  connect(_tabWidget, SIGNAL(currentChanged(int)),
          this, SLOT(currentPlotChanged()));

  createPlotView();

  setCentralWidget(_tabWidget);

  createActions();
  createMenus();
//   createToolBars();
  createStatusBar();

  readSettings();
}


KstMainWindow::~KstMainWindow() {
}


QUndoGroup *KstMainWindow::undoGroup() const {
  return _undoGroup;
}


QTabWidget *KstMainWindow::tabWidget() const {
  return _tabWidget;
}


KstPlotView *KstMainWindow::currentPlotView() const {
  return qobject_cast<KstPlotView*>(_tabWidget->currentWidget());
}


KstPlotView *KstMainWindow::createPlotView() {
  KstPlotView *plotView = new KstPlotView;
  connect(plotView, SIGNAL(destroyed(QObject*)),
          this, SLOT(plotViewDestroyed(QObject*)));
  _undoGroup->addStack(plotView->undoStack());
  _undoGroup->setActiveStack(plotView->undoStack());

  QString label = plotView->objectName().isEmpty() ?
                  QString("Plot %1").arg(QString::number(_tabWidget->count())) :
                  plotView->objectName();

  _tabWidget->addTab(plotView, label);
  return plotView;
}


void KstMainWindow::currentPlotChanged() {
  _undoGroup->setActiveStack(currentPlotView()->undoStack());
}


void KstMainWindow::plotViewDestroyed(QObject *object) {
  KstPlotView *plotView = qobject_cast<KstPlotView*>(object);
  _tabWidget->removeTab(_tabWidget->indexOf(plotView));
}


void KstMainWindow::aboutToQuit() {
  writeSettings();
}


void KstMainWindow::about() {
  //FIXME Build a proper about box...
  QMessageBox::about(this, tr("About Kst"),
          tr("FIXME."));
}


void KstMainWindow::createLabel() {
  CreateLabelCommand *cmd = new CreateLabelCommand;
  cmd->createItem();
}


void KstMainWindow::createLine() {
  CreateLineCommand *cmd = new CreateLineCommand;
  cmd->createItem();
}


void KstMainWindow::createActions() {
  _undoAct = _undoGroup->createUndoAction(this);
  _undoAct->setShortcut(tr("Ctrl+Z"));
  _redoAct = _undoGroup->createRedoAction(this);
  _redoAct->setShortcut(tr("Ctrl+Shift+Z"));

  _createLabelAct = new QAction(tr("&Create label"), this);
  _createLabelAct->setStatusTip(tr("Create a label for the current plot"));
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createLineAct = new QAction(tr("&Create line"), this);
  _createLineAct->setStatusTip(tr("Create a line for the current plot"));
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _exitAct = new QAction(tr("E&xit"), this);
  _exitAct->setShortcut(tr("Ctrl+Q"));
  _exitAct->setStatusTip(tr("Exit the application"));
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _aboutQtAct = new QAction(tr("About &Qt"), this);
  _aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void KstMainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addSeparator();
  _fileMenu->addAction(_exitAct);

  _editMenu = menuBar()->addMenu(tr("&Edit"));
  _editMenu->addAction(_undoAct);
  _editMenu->addAction(_redoAct);

  _plotMenu = menuBar()->addMenu(tr("&Plot"));
  _plotMenu->addAction(_createLabelAct);
  _plotMenu->addAction(_createLineAct);

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAct);
  _helpMenu->addAction(_aboutQtAct);
}


void KstMainWindow::createToolBars() {
  _fileToolBar = addToolBar(tr("File"));
  _editToolBar = addToolBar(tr("Edit"));
}


void KstMainWindow::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}


void KstMainWindow::readSettings() {
  QSettings settings;
  QPoint pos = settings.value("pos", QPoint(20, 20)).toPoint();
  QSize size = settings.value("size", QSize(800, 600)).toSize();
  resize(size);
  move(pos);
}


void KstMainWindow::writeSettings() {
  QSettings settings;
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

#include "kstmainwindow.moc"

// vim: ts=2 sw=2 et
