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
#include <QPointer>

class QMenu;
class QAction;
class QProgressBar;
class QUndoGroup;

namespace Kst {

class DataManager;
class DebugDialog;
class Document;
class ExportGraphicsDialog;
class MatrixEditorDialog;
class ScalarEditorDialog;
class StringEditorDialog;
class TabWidget;
class VectorEditorDialog;
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
    QProgressBar *progressBar() const;
    void initFromCommandLine();

  public Q_SLOTS:
    void showDataManager();
    void showDebugDialog();
    void showExportGraphicsDialog();
    void showVectorEditor();
    void showScalarEditor();
    void showMatrixEditor();
    void showStringEditor();
    void showSettingsDialog();
    void showDifferentiateCurvesDialog();
    void showChooseColorDialog();
    void showChangeDataSampleDialog();
    void showChangeFileDialog();
    void showDataWizard();

    void readFromEnd();
    void pause(bool pause);
    void back();
    void forward();

    void save();
    void saveAs();
    void open();
    void openFile(const QString &file);
    void print();
    void exportGraphicsFile(const QString &filename, const QString &format, int w, int h, int display);

  private Q_SLOTS:
    void aboutToQuit();
    void about();
    void currentViewChanged();

    void createBox();
    void createEllipse();
    void createLabel();
    void createCircle();
    void createLine();
    void createArrow();
    void createPicture();
    void createPlot();
    void createSvg();
    void createLayout();

    void demoModel();

    void performHeavyStartupActions();
    void cleanup();

    void setLayoutMode(bool layoutMode);
    void setTiedZoom(bool tiedZoom);

  protected:
    void closeEvent(QCloseEvent *e);

  private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBars();

    void readSettings();
    void writeSettings();
    bool promptSave();

  private:
    Document *_doc;
    TabWidget *_tabWidget;
    QUndoGroup *_undoGroup;

    DataManager *_dataManager;
    DebugDialog *_debugDialog;
    ExportGraphicsDialog *_exportGraphics;
    VectorEditorDialog *_vectorEditor;
    ScalarEditorDialog *_scalarEditor;
    StringEditorDialog * _stringEditor;
    MatrixEditorDialog *_matrixEditor;

    QPointer<QProgressBar> _progressBar;

    // Do we need these?  I don't think so...
    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_dataMenu;
    QMenu *_viewMenu;
    QMenu *_layoutMenu;
    QMenu *_toolsMenu;
    QMenu *_settingsMenu;
    QMenu *_helpMenu;

    QToolBar *_dataToolBar;
    QToolBar *_layoutToggleToolBar;
    QToolBar *_zoomToolBar;
    QToolBar *_layoutToolBar;

    QAction *_undoAct;
    QAction *_redoAct;

    // FIXME: move these into each object, along with the creation slot?
    QAction *_createLabelAct;
    QAction *_createBoxAct;
    QAction *_createCircleAct;
    QAction *_createEllipseAct;
    QAction *_createLineAct;
    QAction *_createArrowAct;
    QAction *_createPictureAct;
    QAction *_createPlotAct;
    QAction *_createSvgAct;
    QAction *_createLayoutAct;

    QAction *_aboutAct;
    QAction *_closeTabAct;
    QAction *_dataManagerAct;
    QAction *_debugDialogAct;
    QAction *_exitAct;
    QAction *_exportGraphicsAct;
    QAction *_newTabAct;
    QAction *_openAct;
    QAction *_printAct;
    QAction *_saveAct;
    QAction *_saveAsAct;
    QAction *_scalarEditorAct;
    QAction *_vectorEditorAct;
    QAction *_matrixEditorAct;
    QAction *_stringEditorAct;

    QAction *_layoutModeAct;
    QAction *_tiedZoomAct;

    QAction *_settingsDialogAct;

    QAction *_differentiateCurvesDialogAct;
    QAction *_chooseColorDialogAct;
    QAction *_changeDataSampleDialogAct;
    QAction *_changeFileDialogAct;
    QAction *_dataWizardAct;

    QAction *_readFromEndAct;
    QAction *_pauseAct;
    QAction *_backAct;
    QAction *_forwardAct;
};

}
#endif

// vim: ts=2 sw=2 et
