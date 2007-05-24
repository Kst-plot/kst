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

class KstPlotView;

class KstMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  KstMainWindow();
  virtual ~KstMainWindow();

  QUndoGroup *undoGroup() const;
  QTabWidget *tabWidget() const;

  KstPlotView *createPlotView();

private Q_SLOTS:
  void aboutToQuit();
  void about();
  void plotViewDestroyed(QObject *object);

private:
  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();

  void readSettings();
  void writeSettings();

private:
  QUndoGroup *_undoGroup;
  QTabWidget *_tabWidget;

  QMenu *_fileMenu;
  QMenu *_editMenu;
  QMenu *_settingsMenu;
  QMenu *_helpMenu;

  QToolBar *_fileToolBar;
  QToolBar *_editToolBar;

  QAction *_exitAct;
  QAction *_aboutAct;
  QAction *_aboutQtAct;
};

#endif

// vim: ts=2 sw=2 et
