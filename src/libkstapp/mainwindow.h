/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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

#if defined(__QNX__)
#include <QDateTime>
#endif // defined(__QNX__)

class QMenu;
class QAction;
class QProgressBar;
class QUndoGroup;
class QPrinter;
class QSettings;
class QSignalMapper;

namespace Kst {

class DataManager;
class DebugDialog;
class Document;
class ExportGraphicsDialog;
class ExportVectorsDialog;
class LogDialog;
class DifferentiateCurvesDialog;
class FilterMultipleDialog;
class ChooseColorDialog;
class ChangeDataSampleDialog;
class ChangeFileDialog;
class BugReportWizard;
class ApplicationSettingsDialog;
class ThemeDialog;
class AboutDialog;
class TabWidget;
class View;
class ScriptServer;
class ViewVectorDialog;


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
    bool isHighlightPoint() { return _highlightPoint; }
    bool isTiedTabs();
    void setStatusMessage(QString message, int timeout=0, bool delayed = false);
    void updateStatusMessage();
    QString statusMessage();

    static void setWidgetFlags(QWidget*);
    void updateRecentKstFiles(const QString& newfilename = QString());

    void setKstWindowTitle();
    QString scriptServerName();
    bool scriptServerNameSet();
    void setScriptServerName(QString server_name);

public Q_SLOTS:
    void copyTab();
    void showDataManager();
    void showDebugDialog();
    void showExportGraphicsDialog();
    void showExportVectorsDialog();
    void showLogDialog();
    void showVectorEditor();
    void showScalarEditor();
    void showMatrixEditor();
    void showStringEditor();
    void showSettingsDialog();
    void showThemeDialog();
    void clearDefaults();
    void clearDataSourceSettings();
    void showDifferentiateCurvesDialog();
    void filterMultipleDialog();
    void showChooseColorDialog();
    void showChangeDataSampleDialog();
    void showChangeFileDialog();
    void showDataWizard();
    void showDataWizard(const QString &dataFile);
    void showBugReportWizard();
    void showPluginDialog(QString &pluginName);

    void readFromEnd();
    void readToEnd();
    void pause(bool pause);
    void back();
    void forward();
    void reload();

    void updateViewItems(qint64 serial);
    void updateProgress(int percent, const QString& message);

    void save();
    void saveAs();
    void open();
    void newDoc(bool force=false);
    void openFile(const QString &file);
    void cleanUpDataSourceList();
#ifndef KST_NO_PRINTER
    void print();
    void setPrinterDefaults(QPrinter *printer);
    void savePrinterDefaults(QPrinter *printer);
    void printToPrinter(QPrinter *printer);
    void printFromCommandLine(const QString &printFileName);
#endif
    void exportGraphicsFile(const QString &filename, const QString &format, int w, int h, int display, bool export_all, int autosave_period);
    void exportLog(const QString &imagename, QString &msgfilename, const QString &_format, int x_size, int y_size,
                   int size_option_index, const QString &message);


    void clearDrawingMarker();
    void tiedZoomRemoved();
    void allPlotsTiedZoom();

    void updateRecentDataFiles(const QString& newfilename = QString());
    void videoTutorial(int);

    QStringList recentDataFiles();

    void cleanupRecentDataFilesList();

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
    void insertPlot();
#ifndef KST_NO_SVG
    void createSvg();
#endif
    void createSharedAxisBox();

    void createScalar();
    void createVector();
    void createMatrix();
    void createString();
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

    void setHighlightPoint(bool);
    void changeZoomOnlyMode(QAction*);

    void openRecentKstFile();
    void openRecentDataFile();
    void checkRecentFilesOnExistence();

    void autoExportImage();

  protected:
    void closeEvent(QCloseEvent *e);

  private:
    void createActions();
    void createMenus();
    void createStatusBar();
    void createToolBars();

    void readSettings();
    void writeSettings();
    bool promptSaveDone();

    QAction* createRecentFileAction(const QString& filename, int idx, const QString& text, const char* openslot);
    void updateRecentFiles(const QString& key, QMenu *menu, QList<QAction*>& actions, QMenu* submenu, const QString& newfilename, const char* openslot);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
#if defined(__QNX__)
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void qnxToggleToolbarVisibility();
#endif // defined(__QNX__)

  private:
    Document *_doc;
    TabWidget *_tabWidget;
    QUndoGroup *_undoGroup;
    ScriptServer* _scriptServer;
    QSettings& _settings;

    DataManager *_dataManager;
    DebugDialog *_debugDialog;
    ExportGraphicsDialog *_exportGraphics;
    ExportVectorsDialog *_exportVectors;
    LogDialog *_logDialog;
    DifferentiateCurvesDialog *_differentiateCurvesDialog;
    FilterMultipleDialog *_filterMultipleDialog;
    ChooseColorDialog *_chooseColorDialog;
    ChangeDataSampleDialog *_changeDataSampleDialog;
    ChangeFileDialog *_changeFileDialog;
    BugReportWizard *_bugReportWizard;
    ApplicationSettingsDialog *_applicationSettingsDialog;
    ThemeDialog *_themeDialog;
    AboutDialog* _aboutDialog;
    ViewVectorDialog* _viewVectorDialog;
    QSignalMapper* _videoMapper;
    QPointer<QProgressBar> _progressBar;
    QLabel *_messageLabel;

    bool _highlightPoint;

    QMenu *_fileMenu;
    QMenu *_editMenu;
    QMenu *_viewMenu;
    QMenu *_createMenu;
    QMenu *_pluginsMenu;
    QMenu *_fitsPluginsMenu;
    QMenu *_filterPluginsMenu;
    QMenu *_modeMenu;
    QMenu *_rangeMenu;
    QMenu *_toolsMenu;
    QMenu *_settingsMenu;
    QMenu *_helpMenu;
    QMenu* _videoTutorialsMenu;

    QToolBar *_fileToolBar;
    QToolBar *_editToolBar;
    QToolBar *_rangeToolBar;
    QToolBar *_modeToolBar;
    QToolBar *_plotLayoutToolBar;
    QToolBar *_toolsToolBar;
    QToolBar *_annotationToolBar;

    QAction *_undoAct;
    QAction *_redoAct;
    QAction *_copyTabAct;

    // FIXME: move these into each object, along with the creation slot?
    QAction *_createLabelAct;
    QAction *_createBoxAct;
    QAction *_createCircleAct;
    QAction *_createEllipseAct;
    QAction *_createLineAct;
    QAction *_createArrowAct;
    QAction *_createPictureAct;
    QAction *_createPlotAct;
    QAction *_insertPlotAct;
#ifndef KST_NO_SVG
    QAction *_createSvgAct;
#endif
    QAction *_createSharedAxisBoxAct;

    QAction *_aboutAct;
    QAction *_dataManagerAct;
    QAction *_debugDialogAct;
    QAction *_video1Act;
    QAction *_video2Act;
    QAction *_video3Act;
    QAction *_video4Act;
    QAction *_video5Act;
    QAction *_video6Act;
    QAction *_video7Act;
    QAction *_exitAct;
    QAction *_exportGraphicsAct;
    QAction *_exportVectorsAct;
    QAction *_logAct;
    QAction *_openAct;
    QAction *_saveAct;
    QAction *_saveAsAct;
    QAction *_closeAct;
    QAction *_printAct;
    QAction *_newTabAct;
    QAction *_closeTabAct;
    QAction *_scalarEditorAct;
    QAction *_vectorEditorAct;
    QAction *_matrixEditorAct;
    QAction *_stringEditorAct;

    QAction *_layoutModeAct;
    QAction *_tiedZoomAct;
    QAction *_tabTiedAct;
    QAction *_highlightPointAct;
    QAction *_standardZoomAct;
    QAction *_xOnlyZoomAct;
    QAction *_yOnlyZoomAct;

    QAction *_settingsDialogAct;
    QAction *_themeDialogAct;

    QAction *_clearUISettings;
    QAction *_clearDataSourceSettings;

    QAction *_differentiateCurvesDialogAct;
    QAction *_filterMultipleDialogAct;
    QAction *_chooseColorDialogAct;
    QAction *_changeDataSampleDialogAct;
    QAction *_changeFileDialogAct;
    QAction *_dataWizardAct;
    QAction *_bugReportWizardAct;

    QAction *_readFromEndAct;
    QAction *_readToEndAct;
    QAction *_pauseAct;
    QAction *_backAct;
    QAction *_forwardAct;
    QAction *_reloadAct;

    QAction *_newScalarAct;
    QAction *_newVectorAct;
    QAction *_newMatrixAct;
    QAction *_newStringAct;
    QAction *_newCurveAct;
    QAction *_newEquationAct;
    QAction *_newPSDAct;
    QAction *_newHistogramAct;
    QAction *_newImageAct;
    QAction *_newSpectrogramAct;
    QAction *_newEventMonitorAct;

    QMenu* _recentKstFilesMenu;
    QMenu* _recentDataFilesMenu;
    QList<QAction*> _bottomRecentKstActions;
    QList<QAction*> _bottomRecentDataActions;
    QString _statusBarMessage;
    int _statusBarTimeout;
#if defined(__QNX__)
    bool _qnxToolbarsVisible;
    QDateTime _qnxLastToolbarEvent;
#endif // defined(__QNX__)

    // autoExportGraphics fields
    QString _ae_filename;
    QString _ae_format;
    int _ae_width;
    int _ae_height;
    int _ae_display;
    bool _ae_export_all;
    int _ae_autosave_period;
    QTimer *_ae_Timer;
    QString _sessionFileName;

    friend class ScriptServer;
};

}
#endif

// vim: ts=2 sw=2 et
