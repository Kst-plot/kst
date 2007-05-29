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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class QAction;
class QUndoGroup;

namespace Kst {

class DataManager;
class Document;
class TabWidget;
class View;

class MainWindow : public QMainWindow
{
  Q_OBJECT
  public:
    MainWindow();
    ~MainWindow();

    QUndoGroup *undoGroup() const;
    TabWidget *tabWidget() const;
    Document *document() const;

  public Q_SLOTS:
    void showDataManager();

  private Q_SLOTS:
    void aboutToQuit();
    void about();
    void currentViewChanged();

    void createLabel();
    void createBox();
    void createEllipse();
    void createLine();
    void createPicture();

    void demoModel();

  private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void readSettings();
    void writeSettings();

  private:
    Document *_doc;
    TabWidget *_tabWidget;
    QUndoGroup *_undoGroup;

    DataManager *_dataManager;

    // Do we need these?  I don't think so...
    QMenu *_fileMenu;
    QMenu *_dataMenu;
    QMenu *_editMenu;
    QMenu *_plotMenu;
    QMenu *_settingsMenu;
    QMenu *_helpMenu;

    // Do we need these?  I don't think so...
    QToolBar *_fileToolBar;
    QToolBar *_editToolBar;

    QAction *_undoAct;
    QAction *_redoAct;
    // FIXME: move these into each object, along with the creation slot?
    QAction *_createLabelAct;
    QAction *_createBoxAct;
    QAction *_createEllipseAct;
    QAction *_createLineAct;
    QAction *_createPictureAct;

    QAction *_newTabAct;
    QAction *_saveAct;
    QAction *_saveAsAct;
    QAction *_openAct;
    QAction *_printAct;
    QAction *_closeTabAct;
    QAction *_exitAct;
    QAction *_dataManagerAct;
    QAction *_aboutAct;
    QAction *_aboutQtAct;
};

}
#endif

// vim: ts=2 sw=2 et
