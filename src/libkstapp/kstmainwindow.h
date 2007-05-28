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

#ifndef KSTMAINWINDOW_H
#define KSTMAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class QTabWidget;
class QUndoGroup;

namespace Kst {
class View;
}

class KstMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  KstMainWindow();
  virtual ~KstMainWindow();

  QUndoGroup *undoGroup() const;
  QTabWidget *tabWidget() const;
  Kst::View *currentPlotView() const;

  Kst::View *createPlotView();

private Q_SLOTS:
  void aboutToQuit();
  void about();
  void currentPlotChanged();
  void plotViewDestroyed(QObject *object);

  void createLabel();
  void createBox();
  void createLine();

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  void readSettings();
  void writeSettings();

private:
  QTabWidget *_tabWidget;
  QUndoGroup *_undoGroup;

  QMenu *_fileMenu;
  QMenu *_editMenu;
  QMenu *_plotMenu;
  QMenu *_settingsMenu;
  QMenu *_helpMenu;

  QToolBar *_fileToolBar;
  QToolBar *_editToolBar;

  QAction *_undoAct;
  QAction *_redoAct;
  QAction *_createLabelAct;
  QAction *_createBoxAct;
  QAction *_createLineAct;

  QAction *_exitAct;
  QAction *_aboutAct;
  QAction *_aboutQtAct;
};

#endif

// vim: ts=2 sw=2 et
