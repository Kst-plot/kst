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
#include <QLabel>


class QMenu;
class QAction;
class QProgressBar;
class QUndoGroup;
class QPrinter;

namespace Kst {

class DataManager;
class DebugDialog;
class Document;
class ExportGraphicsDialog;
class DifferentiateCurvesDialog;
class ChooseColorDialog;
class ChangeDataSampleDialog;
class ChangeFileDialog;
class BugReportWizard;
class ApplicationSettingsDialog;
class AboutDialog;
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
    QProgressBar *progressBar() const;
    bool initFromCommandLine();
    bool isDataMode() { return _dataMode; }
    void setStatusMessage(QString message);

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
    void showBugReportWizard();

    void readFromEnd();
    void pause(bool pause);
    void back();
    void forward();
    void reload();

    void updateViewItems(qint64 serial);

    void save();
    void saveAs();
    void open();
    void newDoc();
    void openFile(const QString &file);
    void print();
    void setPrinterDefaults(QPrinter *printer);
    void savePrinterDefaults(QPrinter *printer);
    void printToPrinter(QPrinter *printer);
    void printFromCommandLine(const QString &printFileName);
    void exportGraphicsFile(const QString &filename, const QString &format, int w, int h, int display);

    void clearDrawingMarker();
    void tiedZoomRemoved();
    void allPlotsTiedZoom();

  private Q_SLOTS:
    void aboutToQuit();
    void about();
    void currentViewChanged();
    void currentViewModeChanged();

    void createBox();
    void createEllipse();
    void createLabel();
    void createCircle();
    void createLine();
    void createArrow();
    void createPicture();
    void createPlot();
    void createSvg();
    void createSharedAxisBox();

    void createCurve();
    void createPSD();
    void createEquation();
    void createHistogram();
    void createImage();
    void createSpectogram();
    void createEventMonitor();

    void performHeavyStartupActions();
    void cleanup();

    void setLayoutMode(bool layoutMode);
    void toggleTiedZoom();

    void setDataMode(bool dataMode);
    void setXZoomMode();

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
    DifferentiateCurvesDialog *_differentiateCurvesDialog;
    ChooseColorDialog *_chooseColorDialog;
    ChangeDataSampleDialog *_changeDataSampleDialog;
    ChangeFileDialog *_changeFileDialog;
    BugReportWizard *_bugReportWizard;
    ApplicationSettingsDialog *_applicationSettingsDialog;
    AboutDialog* _aboutDialog;

    QPointer<QProgressBar> _progressBar;
    QLabel *_messageLabel;

    bool _dataMode;

    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_viewMenu;
    QMenu *_createMenu;
    QMenu *_modeMenu;
    QMenu *_rangeMenu;
    QMenu *_toolsMenu;
    QMenu *_settingsMenu;
    QMenu *_helpMenu;

    QToolBar *_fileToolBar;
    QToolBar *_editToolBar;
    QToolBar *_rangeToolBar;
    QToolBar *_modeToolBar;
    QToolBar *_toolsToolBar;
    QToolBar *_annotationToolBar;

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
    QAction *_createSharedAxisBoxAct;

    QAction *_aboutAct;
    QAction *_closeTabAct;
    QAction *_dataManagerAct;
    QAction *_debugDialogAct;
    QAction *_exitAct;
    QAction *_exportGraphicsAct;
    QAction *_newTabAct;
    QAction *_clearSession;
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
    QAction *_dataModeAct;
    QAction *_dataXZoomAct;
    QAction *_dataYZoomAct;

    QAction *_settingsDialogAct;

    QAction *_differentiateCurvesDialogAct;
    QAction *_chooseColorDialogAct;
    QAction *_changeDataSampleDialogAct;
    QAction *_changeFileDialogAct;
    QAction *_dataWizardAct;
    QAction *_bugReportWizardAct;

    QAction *_readFromEndAct;
    QAction *_pauseAct;
    QAction *_backAct;
    QAction *_forwardAct;
    QAction *_reloadAct;

    QAction *_newCurveAct;
    QAction *_newEquationAct;
    QAction *_newPSDAct;
    QAction *_newHistogramAct;
    QAction *_newImageAct;
    QAction *_newSpectrogramAct;
    QAction *_newEventMonitorAct;

};

}
#endif

// vim: ts=2 sw=2 et
