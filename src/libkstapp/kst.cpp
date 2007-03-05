/***************************************************************************
                          kst.cpp  -  description
                             -------------------
    begin                : Tue Aug 22 13:46:13 CST 2000
    copyright            : (C) 2000 by Barth Netterfield
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <assert.h>

// include files for Qt
#include <qclipboard.h>
#include <q3deepcopy.h>
#include <qeventloop.h>
#include <q3paintdevicemetrics.h>
#include <q3popupmenu.h>
#include <qprogressbar.h>
#include <qvalidator.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QCustomEvent>

// include files for KDE
#include <kcmdlineargs.h>
#include <qdebug.h>
#include <kfiledialog.h>
#include <kkeydialog.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <ktabwidget.h>
#include <qdatetime.h>

// application specific includes
#include "extensiondlg.h"
#include "extensionmgr.h"
#include "kst.h"
#include "kst2dplot.h"
#include "kstchangefiledialog.h"
#include "kstchangenptsdialog.h"
#include "kstchoosecolordialog.h"
#include "kstcurvedifferentiate.h"
#include "kstcurvedialog.h"
#include "kstcsddialog.h"
#include "kstdatamanager.h"
#include "kstdatanotifier.h"
#include "datawizard.h"
#include "kstdebugdialog.h"
#include "kstdebugnotifier.h"
#include "kstdoc.h"
#include "ksteqdialog.h"
#include "kstevents.h"
#include "ksteventmonitor.h"
#include "ksteventmonitorentry.h"
#include "kstfitdialog.h"
#include "kstfilterdialog.h"
#include "kstgraphfiledialog.h"
#include "ksthsdialog.h"
#include "kstiface_impl.h"
#include "kstimagedialog.h"
#include "kstlogwidget.h"
#include "kstmatrixdialog.h"
#include "kstmatrixdefaults.h"
#include "kstmonochromedialog.h"
#include "kstplugindialog.h"
#include "kstprintoptionspage.h"
#include "kstpsddialog.h"
#include "kstquickstartdialog.h"
#include "kstsettingsdlg.h"
#include "kstuinames.h"
#include "kstvectordefaults.h"
#include "kstvectordialog.h"
#include "kstviewmanager.h"
#include "kstviewscalarsdialog.h"
#include "kstviewstringsdialog.h"
#include "kstviewvectorsdialog.h"
#include "kstviewmatricesdialog.h"
#include "kstviewfitsdialog.h"
#include "kstviewwindow.h"
#include "plotmimesource.h"
#include "plugincollection.h"
#include "pluginmanager.h"
#include "psversion.h"
#include "statuslabel.h"
#include "sysinfo.h"
#include "updatethread.h"
#include "vectorsavedialog.h"
#include "kstobjectdefaults.h"

#define KST_STATUSBAR_DATA   1
#define KST_STATUSBAR_STATUS 2

#define KST_QUICKSTART_DLG

static KstApp *inst = 0L;

const QString& KstApp::defaultTag = KGlobal::staticQString("<Auto Name>");

void KstApp::doubleCheckCleanup() {
  KstApp *ptr = ::inst; // guard to prevent double delete on recursion
  ::inst = 0L;
  delete ptr;
}


KstApp* KstApp::inst() {
  return ::inst;
}


static KConfig *kConfigObject = 0L;

KstApp::KstApp(QWidget *parent)
: KMdiMainFrm(parent) {
  assert(!::inst);
  ::inst = this;

  _plotHolderWhileOpeningDocument = new Kst2DPlotMap;
  KGlobal::dirs()->addResourceType("kst", KStandardDirs::kde_default("data") + "kst");

  _dataSourceConfig = kConfigObject;

  _stopping = false;
  config = kapp->config();
  initStatusBar();
  setStandardToolBarMenuEnabled(true);

  initDocument();
  KstDebug::self()->setHandler(doc);
  setWindowTitle(doc->title());

  /* create dialogs */
  debugDialog = new KstDebugDialogI(this);
  dataManager = new KstDataManagerI(doc, this);
  viewManager = new KstViewManagerI(doc, this);
  viewScalarsDialog = new KstViewScalarsDialogI(this);
  viewStringsDialog = new KstViewStringsDialogI(this);
  viewVectorsDialog = new KstViewVectorsDialogI(this);
  viewMatricesDialog = new KstViewMatricesDialogI(this);
  viewFitsDialog = new KstViewFitsDialogI(this);
  changeFileDialog = new KstChangeFileDialogI(this);
  chooseColorDialog = new KstChooseColorDialogI(this);
  differentiateCurvesDialog = new KstCurveDifferentiateI(this);
  changeNptsDialog = new KstChangeNptsDialogI(this);
  graphFileDialog = new KstGraphFileDialogI(this);
  vectorSaveDialog = new VectorSaveDialog(this);
  _monochromeDialog = new KstMonochromeDialogI(this);
#ifdef KST_QUICKSTART_DLG
  _quickStartDialog = new KstQuickStartDialogI(this);
#endif

  connect(this, SIGNAL(mdiModeHasBeenChangedTo(KMdi::MdiMode)), SLOT(fixKMdi()));

  initActions();
  readOptions();
  XYZoomAction->setChecked(true);
  toggleMouseMode();

  fixKMdi();

  _updateThread = new UpdateThread(doc);
  _updateThread->setUpdateTime(KstSettings::globalSettings()->plotUpdateTimer);
  _updateThread->start();

  /*** Plot Dialog signals */
  connect(changeFileDialog, SIGNAL(docChanged()),
          this,             SLOT(registerDocChange()));
  connect(changeNptsDialog, SIGNAL(docChanged()),
          this,             SLOT(registerDocChange()));
  connect(graphFileDialog, SIGNAL(graphFileReq(const QString&,const QString&,int,int,bool,int)), this, SLOT(immediatePrintToPng(const QString&,const QString&,int,int,bool,int)));
  connect(graphFileDialog, SIGNAL(graphFileEpsReq(const QString&,int,int,bool,int)), this, SLOT(immediatePrintToEps(const QString&,int,int,bool,int)));

  // data manager signals
  connect(dataManager, SIGNAL(docChanged()), this, SLOT(registerDocChange()));
  connect(doc, SIGNAL(updateDialogs()), this, SLOT(updateDialogs()));
  connect(doc, SIGNAL(dataChanged()), this, SLOT(updateDataDialogs()));
  connect(dataManager, SIGNAL(editDataVector(const QString&)), KstVectorDialogI::globalInstance(), SLOT(showEdit(const QString&)));
  connect(dataManager, SIGNAL(editStaticVector(const QString&)), KstVectorDialogI::globalInstance(), SLOT(showEdit(const QString&)));
  connect(dataManager, SIGNAL(editDataMatrix(const QString&)), KstMatrixDialogI::globalInstance(), SLOT(showEdit(const QString&)));
  connect(dataManager, SIGNAL(editStaticMatrix(const QString&)), KstMatrixDialogI::globalInstance(), SLOT(showEdit(const QString&)));


  setAutoSaveSettings("KST-KMainWindow", true);
  _dcopIface = new KstIfaceImpl(doc, this);

  connect(this, SIGNAL(settingsChanged()), this, SLOT(slotSettingsChanged()));

  QTimer::singleShot(0, this, SLOT(updateActions()));

  if (!isFakingSDIApplication()) {
    bool addedWindowMenu = false;
    int menuCount = menuBar()->count();
    for (int menuIndex = 0; menuIndex < menuCount; menuIndex++) {
      int menuId = menuBar()->idAt(menuIndex);
      if (menuId != -1) {
        const QString& menuTitle = menuBar()->text(menuId);
        if (menuTitle == i18n("&Help")) {
          menuBar()->insertItem(i18n("&Window"), windowMenu(), 100, menuIndex);
          addedWindowMenu = true;
          break;
        }
      }
    }

    if (!addedWindowMenu) {
      menuBar()->insertItem(i18n("&Window"), windowMenu());
    }
    connect(windowMenu(), SIGNAL(aboutToShow()), this, SLOT(addNewWindowMenu()));
  }
}


KstApp::~KstApp() {
  destroyDebugNotifier();

  delete _plotHolderWhileOpeningDocument;
  _plotHolderWhileOpeningDocument = 0L;

  if (_updateThread) {
    _updateThread->setFinished(true);
    if (!_updateThread->wait(3000)) { // 3s
      _updateThread->terminate();
    }
  }

  KstDataSource::cleanupForExit(); // must be before deletions
  KstDataObject::cleanupForExit(); // must be before deletions
  delete _updateThread;
  _updateThread = 0L;
  delete _dcopIface;
  _dcopIface = 0L;
  ::inst = 0L;
  if (_dataSourceConfig) {
    _dataSourceConfig->sync();
    _dataSourceConfig = 0L;
  }
  delete kConfigObject; // must be after cleanupForExit
  kConfigObject = 0L;
}


QSize KstApp::sizeHint() const
{
  QSize size(600, 480);

  QDesktopWidget *desktop = QApplication::desktop();
  if (desktop) {
    QRect rect(desktop->availableGeometry());

    size.setWidth(3*rect.width()/4);
    size.setHeight(3*rect.height()/4);
  }

  return size;
}


void KstApp::initialize() {
  KstSettings::checkUpdates();
  kConfigObject = new KConfig("kstdatarc", KConfig::NoGlobals);
  KstDataSource::setupOnStartup(kConfigObject);
  // NOTE: This is leaked in commandline mode if we never create a KstApp.
  //       Not too much of a problem right now, and less messy than hooking in
  //       cleanups in main.
}


void KstApp::loadExtensions() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // Initialise the plugin loader and collection first.
  PluginCollection::self();

  // Do font checks after startup
  //checkFontPresent("Symbol");
  //checkFontPresent("Helvetica");

  QFont defaultFont; // Qt's current default font
  QFontInfo info(defaultFont);
  _defaultFont = info.family();
  // Null QString causes a crash in fontconfig on some systems.
  if (_defaultFont.isNull()) {
    _defaultFont = "helvetica";
  }

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  QCStringList argList = args->getOptionList("E");
  ExtensionMgr *mgr = ExtensionMgr::self();
  mgr->setWindow(this);
  KService::List sl = KServiceType::offers("Kst Extension");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    KService::Ptr service = *it;
    QString name = service->property("Name").toString();
    if (!mgr->enabled(name) && !service->property("X-Kst-Enabled").toBool()) {
      continue;
    }
    mgr->loadExtension(service);
  }
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    KService::Ptr service = *it;
    QString name = service->property("Name").toString();
    KstExtension *e = mgr->extension(name);
    if (e) {
      QString ename = service->property("X-Kst-Extension-Name").toString();
      for (QCStringList::ConstIterator i = argList.begin(); i != argList.end(); ++i) {
        if (QString::fromLatin1(*i).startsWith(ename + ":")) {
          e->processArguments((*i).mid(ename.length() + 1));
        }
      }
    }
  }
  QApplication::restoreOverrideCursor();
}


void KstApp::addNewWindowMenu() {
  int id = windowMenu()->insertItem(i18n("&New..."), this, SLOT(slotFileNewWindow()), 0, -1, 0);
  windowMenu()->setWhatsThis(id, i18n("Create a new plot window."));

  id = windowMenu()->insertItem(i18n("&Rename..."), this, SLOT(slotFileRenameWindow()), 0, -1, 0);
  windowMenu()->setWhatsThis(id, i18n("Rename an existing plot window."));
  if (!activeView()) {
    windowMenu()->setItemEnabled(id, false);
  }
}


const QString& KstApp::defaultFont() const {
  return _defaultFont;
}


void KstApp::checkFontPresent(const QString& font) {
  QFont f(font);
  QFontInfo info(f);
  if (info.family().toLower() != font.toLower()) {
    QString msg = i18n("The %1 font was not found and was replaced by the %2 font; as a result, some labels may not display correctly.").arg(font).arg(info.family());
    KstDebug::self()->log(msg, KstDebug::Warning);
  }
}


void KstApp::EventELOGSubmitEntry(const QString& msg) {
  emit ELOGSubmitEntry(msg);
}


void KstApp::EventELOGConfigure() {
  emit ELOGConfigure();
}


void KstApp::customEvent(QCustomEvent *pEvent) {
  if (pEvent->type() == KstELOGAliveEvent) {
    KstEventMonitorI::globalInstance()->enableELOG();
  } else if (pEvent->type() == KstELOGDeathEvent) {
    KstEventMonitorI::globalInstance()->disableELOG();
  } else if (pEvent->type() == KstELOGDebugInfoEvent) {
    QTextStream *pTextStream = (QTextStream*)pEvent->data();
    (*pTextStream) << KstDebug::self()->text();
  } else if (pEvent->type() == KstELOGConfigureEvent) {
    QTextStream *pTextStream = (QTextStream*)pEvent->data();
    if (document()) {
      document()->saveDocument(*pTextStream, true);
    }
  } else if (pEvent->type() == KstELOGCaptureEvent) {
    KstELOGCaptureStruct* pCapture = (KstELOGCaptureStruct*)pEvent->data();

    if( pCapture ) {
      QDataStream *pStream = pCapture->pBuffer;
      QSize size(pCapture->iWidth, pCapture->iHeight);
      KstViewWindow *vw = dynamic_cast<KstViewWindow*>(activeWindow());

      if (vw) {
        vw->immediatePrintToPng(pStream, size);
      }
    }
  }
}


void KstApp::updateActions() {
  // Hack
  StatusBarAction->setChecked(statusBar()->isVisible());
  QApplication::flushX();
  QTimer::singleShot(0, this, SLOT(loadExtensions()));
}


void KstApp::initActions() {
  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSave->setWhatsThis(i18n("Save to current Kst plot file."));

  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileSaveAs->setWhatsThis(i18n("Save to new Kst plot file."));

  fileQuit = KStdAction::quit(this, SLOT(slotFileClose()), actionCollection());
  fileQuit->setWhatsThis(i18n("Quit Kst."));

#if 0
  // KDE 3.3 only
  fileKeyBindings = KStdAction::keyBindings(guiFactory(), SLOT(slotConfigureKeys()), actionCollection());
#else
  fileKeyBindings = KStdAction::keyBindings(this, SLOT(slotConfigureKeys()), actionCollection());
#endif
  fileKeyBindings->setWhatsThis(i18n("Bring up a dialog box\n"
                                     "to configure shortcuts."));

  filePreferences = KStdAction::preferences(this, SLOT(slotPreferences()), actionCollection());
  filePreferences->setWhatsThis(i18n("Bring up a dialog box\n"
                                     "to configure Kst settings."));

  fileCopy = KStdAction::copy(this, SLOT(slotCopy()), actionCollection());
  fileCopy->setWhatsThis(i18n("Copy cursor position or plots to the clipboard."));

  filePaste = KStdAction::paste(this, SLOT(slotPaste()), actionCollection());
  filePaste->setWhatsThis(i18n("Paste plots from the clipboard."));

  /************/
  filePrint = KStdAction::print(this, SLOT(slotFilePrint()),
                                actionCollection());
  filePrint->setToolTip(i18n("Print"));
  filePrint->setWhatsThis(i18n("Print current display"));

  /************/
  StatusBarAction = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()),
                                              actionCollection());
  StatusBarAction->setWhatsThis(i18n("Toggle Statusbar"));
  connect(StatusBarAction, SIGNAL(activated()), this, SLOT(setSettingsDirty()));

  /************/
  KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());

  /************/
  _recent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KUrl &)), actionCollection());
  _recent->setWhatsThis(i18n("Open a recently used Kst plot."));

  /************/
  PauseAction = new KToggleAction(i18n("P&ause"),"player_pause", 0,
                                  actionCollection(), "pause_action");
  PauseAction->setToolTip(i18n("Pause"));
  PauseAction->setWhatsThis(i18n("When paused, new data will not be read."));
  connect(PauseAction, SIGNAL(toggled(bool)), this, SLOT(updatePausedState(bool)));

  /************/
  _saveData = new KToggleAction(i18n("Save Da&ta"),"save_vector_data", 0,
                                  actionCollection(), "save_vector_data");
  _saveData->setToolTip(i18n("Save Vector Data To Disk"));
  _saveData->setWhatsThis(i18n("When selected, data in vectors will be saved into the Kst file."));

  /************/
  XYZoomAction = new KRadioAction(i18n("XY Mouse &Zoom"), "kst_zoomxy",
                                  KShortcut(Qt::Key_F2),
                                  this, SLOT(toggleMouseMode()),
                                  actionCollection(), "zoomxy_action");
  XYZoomAction->setExclusiveGroup("gfx");
  XYZoomAction->setToolTip(i18n("XY mouse zoom"));
  XYZoomAction->setWhatsThis(i18n("XY zoom: mouse zooming affects\n"
                                  "both X and Y axis"));
  XYZoomAction->setChecked(true);

  /************/
  XZoomAction = new KRadioAction(i18n("&X Mouse Zoom"), "kst_zoomx",
                                 KShortcut(Qt::Key_F3),
                                 this, SLOT(toggleMouseMode()),
                                 actionCollection(), "zoomx_action");
  XZoomAction->setExclusiveGroup("gfx");
  XZoomAction->setToolTip(i18n("X mouse zoom"));
  XZoomAction->setWhatsThis(i18n("X zoom: Mouse zooming affects only the\n"
                                 "X axis (CTRL-mouse also does this)"));

  /************/
  YZoomAction = new KRadioAction(i18n("&Y Mouse Zoom"), "kst_zoomy",
                                  KShortcut(Qt::Key_F4),
                                  this, SLOT(toggleMouseMode()),
                                  actionCollection(), "zoomy_action");
  YZoomAction->setExclusiveGroup("gfx");
  YZoomAction->setToolTip(i18n("Y mouse zoom"));
  YZoomAction->setWhatsThis(i18n("Y zoom: Mouse zooming affects only the\n"
                                 "Y axis (SHIFT-mouse also does this)"));

  /************/
  GfxAction = new KRadioAction(i18n("&Graphics Mode"), "kst_graphics", 0,
                                this, SLOT(toggleMouseMode()),
                                actionCollection(), "graphics_action");
  GfxAction->setExclusiveGroup("zoom");
  GfxAction->setToolTip(i18n("Graphics Editor"));
  GfxAction->setWhatsThis(i18n("Use the mouse to create and edit graphics objects."));

  /************/


  /************/
  NewPlotAction = new KAction(i18n("New Plot..."), "kst_newplot", 0,
                                 this, SLOT(newPlot()),
                                 actionCollection(), "newplot_action");
  NewPlotAction->setWhatsThis(i18n("Create a new plot in the\n"
                                      "current window."));

  /************/
  DataManagerAction = new KAction(i18n("&Data Manager"), "kst_datamanager", 0,
                                  dataManager, SLOT(show_I()),
                                  actionCollection(), "datamanager_action");
  DataManagerAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                       "to manage data."));

  /************/
  ViewManagerAction = new KAction(i18n("&View Manager"), "kst_viewmanager", 0,
                                  viewManager, SLOT(show_I()),
                                  actionCollection(), "viewmanager_action");
  ViewManagerAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                       "to manage views."));
  
  /************/
  VectorDialogAction = new KAction(i18n("New &Vector..."), "kst_vectornew", 0,
                                 KstVectorDialogI::globalInstance(),
                                 SLOT(show()), actionCollection(),
                                 "vectordialog_action");
  VectorDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                        "to create a new vector."));

  /************/
  CurveDialogAction = new KAction(i18n("New &Curve..."), "kst_curvenew", 0,
                                  KstCurveDialogI::globalInstance(),
                                  SLOT(show()), actionCollection(),
                                  "curvedialog_action");
  CurveDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                       "to create a new curve."));

  /************/
  CsdDialogAction = new KAction(i18n("New Cumulative &Spectral Decay..."), "kst_csdnew", 0,
                                KstCsdDialogI::globalInstance(),
                                SLOT(show()), actionCollection(),
                                "csddialog_action");
  CsdDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                     "to create a new cumulative spectral decay."));


  /************/
  EqDialogAction = new KAction(i18n("New &Equation..."),
                               "kst_equationnew", 0,
                               KstEqDialogI::globalInstance(),
                               SLOT(show()),
                               actionCollection(), "eqdialog_action");
  EqDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                    "to create a new equation."));

  /************/
  HsDialogAction = new KAction(i18n("New &Histogram..."),
                               "kst_histogramnew", 0,
                               KstHsDialogI::globalInstance(),
                               SLOT(show()), actionCollection(),
                               "hsdialog_action");
  HsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                    "to create a new histogram."));

  /************/
  PsdDialogAction = new KAction(i18n("New Power &Spectrum..."),
                                "kst_psdnew", 0,
                                KstPsdDialogI::globalInstance(),
                                SLOT(show()), actionCollection(),
                                "psddialog_action");
  PsdDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                     "to create a new power spectrum."));

  /************/
  PluginDialogAction = new KAction(i18n("New &Plugin..."),
                                  "kst_pluginnew", 0,
                                   this, SLOT(selectDataPlugin()), actionCollection(),
                                   "plugindialog_action");
  PluginDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                        "to create a new plugin instance."));

  /************/
  MatrixDialogAction = new KAction(i18n("New M&atrix..."), "kst_matrixnew", 0,
                                   KstMatrixDialogI::globalInstance(),
                                   SLOT(show()), actionCollection(),
                                   "matrixdialog_action");
  MatrixDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                        "to create a new matrix."));

  /************/
  ImageDialogAction = new KAction(i18n("New &Image..."),
                                   "kst_imagenew", 0,
                                   KstImageDialogI::globalInstance(),
                                   SLOT(show()), actionCollection(),
                                   "imagedialog_action");
  ImageDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                        "to create a new image instance."));

  /************/
  ChangeFileDialogAction = new KAction(i18n("Change Data &File..."),
                                       "kst_changefile", 0, this,
                                       SLOT(showChangeFileDialog()),
                                       actionCollection(),
                                       "changefiledialog_action");
  ChangeFileDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to change input files."));

  /************/
  ChooseColorDialogAction = new KAction(i18n("Assign Curve &Color Per File..."),
                                        "kst_choosecolor", 0, this,
                                        SLOT(showChooseColorDialog()),
                                        actionCollection(),
                                        "choosecolordialog_action");
  ChooseColorDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                             "to change curve colors\n"
                                             "based on data file."));

  /************/
  DifferentiateCurvesDialogAction = new KAction(i18n("&Differentiate Between Curves..."),
                                        "kst_differentiatecurves", 0, this,
                                        SLOT(showDifferentiateCurvesDialog()),
                                        actionCollection(),
                                        "differentiatecurves_action");
  DifferentiateCurvesDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                             "to differentiate between curves."));

  /************/
  ViewScalarsDialogAction = new KAction(i18n("View &Scalar Values"),
                                       0, 0, this,
                                       SLOT(showViewScalarsDialog()),
                                       actionCollection(),
                                       "viewscalarsdialog_action");
  ViewScalarsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to view scalar values."));
  ViewScalarsDialogAction->setEnabled(false);

  /************/
  ViewStringsDialogAction = new KAction(i18n("View Strin&g Values"),
                                       0, 0, this,
                                       SLOT(showViewStringsDialog()),
                                       actionCollection(),
                                       "viewstringsdialog_action");
  ViewStringsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to view string values."));

  /************/
  ViewVectorsDialogAction = new KAction(i18n("View Vec&tor Values"),
                                       0, 0, this,
                                       SLOT(showViewVectorsDialog()),
                                       actionCollection(),
                                       "viewvectorsdialog_action");
  ViewVectorsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to view vector values."));
  ViewVectorsDialogAction->setEnabled(false);

  /************/
  ViewMatricesDialogAction = new KAction(i18n("View &Matrix Values"),
                                       0, 0, this,
                                       SLOT(showViewMatricesDialog()),
                                       actionCollection(),
                                       "viewmatricesdialog_action");
  ViewMatricesDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to view matrix values."));
  ViewMatricesDialogAction->setEnabled(false);

  /************/
  ViewFitsDialogAction = new KAction(i18n("View &Fit Results"),
                                       0, 0, this,
                                       SLOT(showViewFitsDialog()),
                                       actionCollection(),
                                       "viewfitsdialog_action");
  ViewFitsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to view fit values."));
  ViewFitsDialogAction->setEnabled(false);

  /************/
  ChangeNptsDialogAction = new KAction(i18n("Change Data Sample &Ranges..."),
                                       "kst_changenpts", 0, this,
                                       SLOT(showChangeNptsDialog()),
                                       actionCollection(),
                                       "changenptsdialog_action");
  ChangeNptsDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                            "to change data sample ranges."));

  /************/
  EventMonitorAction = new KAction(i18n("New Event &Monitor..."),
                                     "kst_eventnew", 0,
                                     KstEventMonitorI::globalInstance(),
                                     SLOT(show()),
                                     actionCollection(),
                                     "eventmonitor_action");
  EventMonitorAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                          "create a new event monitor."));

  /************/
  GraphFileDialogAction = new KAction(i18n("Export to Graphics File..."),
                                  "thumbnail", 0,
                                  this, SLOT(showGraphFileDialog()),
                                  actionCollection(),
                                  "graphfiledialog_action");
  GraphFileDialogAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                           "to export the plot as a\n"
                                           "graphics file."));

  /************/
  _vectorSaveAction = new KAction(i18n("Save Vectors to Disk..."),
                                  0, 0,
                                  vectorSaveDialog, SLOT(show()),
                                  actionCollection(),
                                  "vectorsavedialog_action");
  _vectorSaveAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                       "to save vectors to text files."));

  /************/
  SamplesDownAction = new KAction(i18n("&Back 1 Screen"),
                                  "player_rew",
                                  KShortcut(Qt::CTRL + Qt::Key_Left),
                                  this, SLOT(samplesDown()),
                                  actionCollection(),
                                  "samplesdown_action");
  //SamplesDownAction->setToolTip(i18n("Back"));
  SamplesDownAction->setWhatsThis(i18n("Reduce the starting frame by\n"
                                       "the current number of frames."));


  /************/
  SamplesUpAction = new KAction(i18n("&Advance 1 Screen"),
                                "player_fwd",
                                KShortcut(Qt::CTRL + Qt::Key_Right),
                                this, SLOT(samplesUp()),
                                actionCollection(),
                                "samplesup_action");

  //SamplesUpAction->setToolTip(i18n("Advance"));
  SamplesUpAction->setWhatsThis(i18n("Increase the starting frame by\n"
                                     "the current number of frames."));

  /************/
  SamplesFromEndAction = new KAction(i18n("Read From &End"),
                                     "player_end",
                                     KShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_Right),
                                     this, SLOT(fromEnd()),
                                     actionCollection(),
                                     "samplesend_action");
  SamplesFromEndAction->setToolTip(i18n("Read from end"));
  SamplesFromEndAction->setWhatsThis(i18n("Read current data from end of file."));

  /************/
  PluginManagerAction = new KAction(i18n("&Plugins..."), 0, 0,
                                 this, SLOT(showPluginManager()),
                                 actionCollection(), "pluginmanager_action");
  PluginManagerAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                           "to manage plugins."));


  /************/
  ExtensionManagerAction = new KAction(i18n("&Extensions..."), 0, 0,
                                 this, SLOT(showExtensionManager()),
                                 actionCollection(), "extensionmanager_action");
  ExtensionManagerAction->setWhatsThis(i18n("Bring up a dialog box\n"
                                           "to manage extensions."));


  /************/
  DataWizardAction = new KAction(i18n("Data &Wizard"), "wizard", 
                                 KShortcut(Qt::Key_W),
                                 this, SLOT(showDataWizard()),
                                 actionCollection(), "datawizard_action");
  DataWizardAction->setWhatsThis(i18n("Bring up a wizard\n"
                                           "to easily load data."));


  /************/
  DebugDialogAction = new KAction(i18n("Debug Kst..."), 0, 0,
                                 this, SLOT(showDebugDialog()),
                                 actionCollection(), "debug_action");
  DebugDialogAction->setWhatsThis(i18n("Bring up a dialog\n"
                                           "to display debugging information."));


  /************/
  DataMode = new KToggleAction(i18n("Data Mode"), "kst_datamode", 0,
                                 this, SLOT(toggleDataMode()),
                                 actionCollection(), "datamode_action");
  DataMode->setWhatsThis(i18n("Toggle between cursor mode and data mode."));
  DataMode->setToolTip(i18n("Data mode"));
  /************/
  _reloadAction = new KAction(i18n("Reload"), "reload", Qt::Key_F5, this, SLOT(reload()),
                              actionCollection(), "reload");
  _reloadAction->setWhatsThis(i18n("Reload the data from file."));

  _tiedZoomAction = new KAction(i18n("&Tied Zoom"),"kst_zoomtie", 0,
                               this, SLOT(tieAll()),
                               actionCollection(), "zoomtie_action");
  _tiedZoomAction->setToolTip(i18n("Enable tied zoom"));
  _tiedZoomAction->setWhatsThis(i18n("Apply zoom actions to all plots\n"
                                     "(not just the active one)."));

  _gfxRectangleAction = new KRadioAction(i18n("&Rectangle"), "kst_gfx_rectangle", 
                                  KShortcut(Qt::Key_F8),
                                  this, SLOT(toggleMouseMode()),
                                  actionCollection(), "rectangle_action");
  _gfxRectangleAction->setExclusiveGroup("gfx");
  _gfxRectangleAction->setToolTip(i18n("Draw rectangle"));
  _gfxRectangleAction->setWhatsThis(i18n("Draw rectangle"));

  _gfxEllipseAction = new KRadioAction(i18n("&Ellipse"), "kst_gfx_ellipse",
                                  KShortcut(Qt::Key_F9),
                                  this, SLOT(toggleMouseMode()),
                                  actionCollection(), "ellipse_action");
  _gfxEllipseAction->setExclusiveGroup("gfx");
  _gfxEllipseAction->setToolTip(i18n("Draw ellipse"));
  _gfxEllipseAction->setWhatsThis(i18n("Draw ellipse"));

  _gfxPictureAction = new KRadioAction(i18n("&Picture"), "kst_gfx_picture",
                                   KShortcut(Qt::Key_F12),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "picture_action");
  _gfxPictureAction->setExclusiveGroup("gfx");
  _gfxPictureAction->setToolTip(i18n("Insert picture"));
  _gfxPictureAction->setWhatsThis(i18n("Insert picture"));
  
  _gfx2DPlotAction = new KRadioAction(i18n("&2DPlot"), "kst_newplot",
                                   KShortcut(Qt::Key_2),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "2dplot_action");
  _gfx2DPlotAction->setExclusiveGroup("gfx");
  _gfx2DPlotAction->setToolTip(i18n("Insert Plot"));
  _gfx2DPlotAction->setWhatsThis(i18n("Insert Plot"));

  _gfxArrowAction = new KRadioAction(i18n("&Arrow"), "kst_gfx_arrow",
                                   KShortcut(Qt::Key_F11),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "arrow_action");
  _gfxArrowAction->setExclusiveGroup("gfx");
  _gfxArrowAction->setToolTip(i18n("Draw arrow"));
  _gfxArrowAction->setWhatsThis(i18n("Draw arrow"));

  _gfxLineAction = new KRadioAction(i18n("&Line"), "kst_gfx_line",
                                   KShortcut(Qt::Key_F10),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "line_action");
  _gfxLineAction->setExclusiveGroup("gfx");
  _gfxLineAction->setToolTip(i18n("Draw line"));
  _gfxLineAction->setWhatsThis(i18n("Draw line"));
  _gfxLineAction->setChecked(true);

  /************/
  _gfxLabelAction = new KRadioAction(i18n("L&abel"), "text",
                                   KShortcut(Qt::Key_F7),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "label_action");
  _gfxLabelAction->setExclusiveGroup("gfx");
  _gfxLabelAction->setToolTip(i18n("Draw label"));
  _gfxLabelAction->setWhatsThis(i18n("Draw label"));

  LayoutAction = new KRadioAction(i18n("Layout Mode"), "kst_layoutmode",
                                   KShortcut(Qt::Key_F6),
                                   this, SLOT(toggleMouseMode()),
                                   actionCollection(), "layoutmode_action");
  LayoutAction->setExclusiveGroup("gfx");
  LayoutAction->setToolTip(i18n("Layout mode"));
  LayoutAction->setWhatsThis(i18n("Use this mode to move, resize, and group plots."));

  // this is the mouse mode menu
  Q3PopupMenu* mouseModeMenu = new Q3PopupMenu(this);

  XYZoomAction->plug(mouseModeMenu);
  XZoomAction->plug(mouseModeMenu);
  YZoomAction->plug(mouseModeMenu);
  mouseModeMenu->insertSeparator();
  LayoutAction->plug(mouseModeMenu);
  mouseModeMenu->insertSeparator();
  _gfxLabelAction->plug(mouseModeMenu);
  _gfxRectangleAction->plug(mouseModeMenu);
  _gfxEllipseAction->plug(mouseModeMenu);
  _gfxLineAction->plug(mouseModeMenu);
  _gfxArrowAction->plug(mouseModeMenu);
  _gfxPictureAction->plug(mouseModeMenu);
  _gfx2DPlotAction->plug(mouseModeMenu);

  toolBar()->insertButton("thumbnail", 5, mouseModeMenu, true, i18n("Select the desired mode"));
  toggleMouseMode();

  createGUI(0L);
}


void KstApp::slotConfigureKeys() {
#if 0
  // KDE 3.3 only:
  KKeyDialog::configure(actionCollection(), this);
#else
  KKeyDialog dlg(true, this);
  dlg.insert(actionCollection());
  ExtensionMgr *mgr = ExtensionMgr::self();
  const QMap<QString, bool>& extensions = mgr->extensions();
  for (QMap<QString, bool>::ConstIterator it = extensions.begin(); it != extensions.end(); ++it) {
    if (it.data()) {
      KXMLGUIClient *gc = dynamic_cast<KXMLGUIClient*>(mgr->extension(it.key()));
      if (gc) {
        KActionCollection *ac = gc->actionCollection();
        if (ac) {
          dlg.insert(ac);
        }
      }
    }
  }
  dlg.configure(true);
#endif
}


bool KstApp::paused() const {
  return _updateThread->paused();
}


void KstApp::setPaused(bool in_paused) {
  PauseAction->setChecked(in_paused);
  _updateThread->setPaused(in_paused);
}


void KstApp::togglePaused() {
  setPaused(!PauseAction->isChecked());
}


KstApp::KstZoomType KstApp::getZoomRadio() {
  if (XZoomAction->isChecked()) {
    return XZOOM;
  } else if (YZoomAction->isChecked()) {
    return YZOOM;
  } else if (LayoutAction->isChecked()) {
    return LAYOUT;
  } else if (GfxAction->isChecked()) {
    return GRAPHICS;
  } else {
    return XYZOOM;
  }
}


void KstApp::initStatusBar() {
  _dataNotifier = new KstDataNotifier(statusBar());
  statusBar()->addPermanentWidget(_dataNotifier, 0);

  _dataBar = new StatusLabel(QString::null, statusBar());
  _dataBar->setTextFormat(Qt::PlainText);
  statusBar()->addPermanentWidget(_dataBar, 5);

  _readyBar = new StatusLabel(i18n("Almost Ready"), statusBar());
  _readyBar->setTextFormat(Qt::PlainText);
  _readyBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  statusBar()->addPermanentWidget(_readyBar, 5);

  _progressBar = new QProgressBar(statusBar());
  _progressBar->setPercentageVisible(false);
  _progressBar->setCenterIndicator(true);
  statusBar()->addPermanentWidget(_progressBar, 2);
  _progressBar->setMaximumHeight( fontMetrics().height() );
  _progressBar->hide();

#ifdef HAVE_LINUX
  _memoryBar = new StatusLabel(i18n("0 MB available"), statusBar());
  _memoryBar->setTextFormat(Qt::PlainText);
  statusBar()->addPermanentWidget(_memoryBar, 0);
  connect(&_memTimer, SIGNAL(timeout()), this, SLOT(updateMemoryStatus()));
  _memTimer.start(5000);
#endif

  statusBar()->show();

  slotUpdateMemoryMsg(i18n("0 MB available"));
  slotUpdateStatusMsg(i18n("Ready"));
  slotUpdateDataMsg(QString::null);
}


void KstApp::initDocument() {
  doc = new KstDoc(this);
  QTimer::singleShot(0, this, SLOT(delayedDocInit()));
}


void KstApp::delayedDocInit() {
  if (!activeWindow()) {
    doc->newDocument();
  }
}


void KstApp::selectDataPlugin() {
  QStringList l;

  //The new KstDataObject plugins...
  QStringList newPlugins;
  const KstPluginInfoList pluginInfo = KstDataObject::pluginInfoList();
  {
    KstPluginInfoList::ConstIterator it = pluginInfo.begin();
    for (; it != pluginInfo.end(); ++it) {
      newPlugins << it.key();
    }
  }

  l += newPlugins;

  //The old C style plugins...
  QStringList oldPlugins;
  const QMap<QString,QString> readable = PluginCollection::self()->readableNameList();
  {
    QMap<QString,QString>::const_iterator it = readable.begin();
    for (; it != readable.end(); ++it) {
      oldPlugins << it.key();
    }
  }

  l += oldPlugins;

  bool ok = false;
  QStringList plugin = KInputDialog::getItemList(i18n("Data Plugins"), i18n("Create..."), l, QStringList(), false, &ok, this);

  if (!ok || plugin.isEmpty()) {
    return;
  }

  const QString p = plugin.join("");

  if (newPlugins.contains(p)) {
    KstDataObjectPtr ptr = KstDataObject::plugin(p);
    ptr->showDialog(true);
  } else if (oldPlugins.contains(p)) {
    KstPluginDialogI::globalInstance()->showNew(readable[p]);
  }
}


void KstApp::forceUpdate() {
  _updateThread->forceUpdate();
}


void KstApp::addRecentFile(const KUrl& url) {
  _recent->addUrl(url);
}


void KstApp::selectRecentFile(const KUrl &url) {
  if (url.isEmpty()) {
    _recent->setCurrentItem(-1);
  } else {
    QStringList urls = _recent->items();
    int count = urls.count();
    for (int i = 0; i < count; ++i) {
      if (KUrl(urls[i]) == url) {
        _recent->setCurrentItem(i);
        break;
      }
    }
  }
}


void KstApp::doDelayedOpens() {
  Q3ValueList<KstOpen> queueCopy = Q3DeepCopy<Q3ValueList<KstOpen> >(_openQueue);
  _openQueue.clear();

  for (Q3ValueList<KstOpen>::ConstIterator i = queueCopy.begin(); i != queueCopy.end(); ++i) {
    openDocumentFile((*i).filename, (*i).file, (*i).n, (*i).f, (*i).s, (*i).ave);
  }
}


bool KstApp::openDocumentFile(const QString& in_filename,
    const QString& o_file, int o_n, int o_f, int o_s, bool o_ave, bool delayed) {
  static bool opening = false;

  if (delayed || opening) {
    KstOpen job;
    job.filename = in_filename;
    job.file = o_file;
    job.n = o_n;
    job.f = o_f;
    job.s = o_s;
    job.ave = o_ave;
    _openQueue.append(job);
    QTimer::singleShot(0, this, SLOT(doDelayedOpens()));
    return true;
  }

  opening = true;
  KUrl url;
  bool rc = false;

  if (QFile::exists(in_filename) && QFileInfo(in_filename).isRelative()) {
    url.setPath(QFileInfo(in_filename).absoluteFilePath());
  } else {
    url = KUrl(in_filename);
  }

  slotUpdateStatusMsg(i18n("Opening %1...").arg(url.prettyUrl()));

  if (doc->openDocument(url, o_file, o_n, o_f, o_s, o_ave)) {
    setWindowTitle(doc->title());
    if (url.isLocalFile()) {
      QFileInfo finfo(in_filename);
      addRecentFile(finfo.absoluteFilePath());
    } else {
      addRecentFile(url);
    }
    rc = true;
  }
  slotUpdateStatusMsg(i18n("Ready"));
  opening = false;
  return rc;
}

KstMonochromeDialogI *KstApp::monochromeDialog() const {
  return _monochromeDialog;
}


KstDoc *KstApp::document() const {
  return doc;
}


void KstApp::saveOptions() {
  config->setGroup("General Options");
  _recent->saveEntries(config, "Recent Files");

  config->writeEntry("MDIMode", (int)mdiMode());

  KST::vectorDefaults.writeConfig(config);
  KST::matrixDefaults.writeConfig(config);
  KST::objectDefaults.writeConfig(config);
  config->sync();
}


void KstApp::readOptions() {
  config->setGroup("General Options");
  _recent->loadEntries(config, "Recent Files");

  int mdiMode = config->readNumEntry("MDIMode", (int)KMdi::TabPageMode);

  KST::vectorDefaults.readConfig(config);
  KST::matrixDefaults.readConfig(config);
  KST::objectDefaults.readConfig(config);

  switch (mdiMode) {
    case KMdi::ToplevelMode:
      switchToToplevelMode();
      break;
    case KMdi::ChildframeMode:
      switchToChildframeMode();
      break;
    case KMdi::TabPageMode:
      switchToTabPageMode();
      break;
    default:
      switchToTabPageMode();
      break;
  }
}


void KstApp::saveProperties(KConfig *config) {
  QString name = doc->absFilePath();
  if (!name.isEmpty() && doc->title() != "Untitled") {
    config->writePathEntry("Document", name);
    config->writeEntry("NamedDocument", true);
  } else {
    QString sl = KGlobal::dirs()->saveLocation("kst", "kst/");
    int i = 0;
    do {
      name = sl + QString("unsaved%1.kst").arg(i);
    } while(QFile::exists(name));
    doc->saveDocument(name, false, false);
    config->writePathEntry("Document", name);
    config->writeEntry("NamedDocument", false);
  }
}


void KstApp::readProperties(KConfig* config) {
  QString name = config->readPathEntry("Document");

  if (name.isEmpty()) {
    return;
  }

  if (config->readBoolEntry("NamedDocument", false)) {
    doc->openDocument(name);
  } else {
    doc->openDocument(name);
    QFile::remove(name);
    doc->setTitle("Untitled");
  }
}


bool KstApp::queryClose() {
  if (doc->saveModified()) {
     doc->cancelUpdate();
    _stopping = true;
    QTimer::singleShot(0, doc, SLOT(deleteContents()));
    return true;
  }
  return false;
}


bool KstApp::queryExit() {
  saveOptions();
  return true;
}


void KstApp::slotFileNew() {
  slotUpdateStatusMsg(i18n("Creating new document..."));

  if (doc->saveModified()) {
    doc->newDocument();
    setWindowTitle(doc->title());
    selectRecentFile(KUrl(""));
  }

  slotUpdateStatusMsg(i18n("Ready"));
}


void KstApp::slotFileOpen() {
  slotUpdateStatusMsg(i18n("Opening file..."));

  if (doc->saveModified(false)) {
    KUrl url = KFileDialog::getOpenUrl("::<kstfiledir>", i18n("*.kst|Kst Plot File (*.kst)\n*|All Files"), this, i18n("Open File"));
    if (!url.isEmpty()) {
      doc->deleteContents();
      doc->setModified(false);
      if (doc->openDocument(url)) {
        setWindowTitle(doc->title());
        addRecentFile(url);
      }
    }
  }

  slotUpdateStatusMsg(i18n("Ready"));
}


bool KstApp::slotFileOpenRecent(const KUrl& newfile) {
  bool returnVal = false;
  slotUpdateStatusMsg(i18n("Opening file..."));

  if (doc->saveModified()) {
    if (doc->openDocument(newfile)) {
      returnVal = true;
    }
    setWindowTitle(qApp->applicationName() + ": " + doc->title());
  }

  slotUpdateStatusMsg(i18n("Ready"));
  return returnVal;
}


void KstApp::slotFileSave() {
  if (doc->title() == "Untitled") {
    slotFileSaveAs();
  } else {
    slotUpdateStatusMsg(i18n("Saving file..."));
    doc->saveDocument(doc->absFilePath(), false, false);
    slotUpdateStatusMsg(i18n("Ready"));
  }
}


bool KstApp::slotFileSaveAs() {
  slotUpdateStatusMsg(i18n("Saving file with a new filename..."));

  while (true) {
    QString folder;

    if (doc->lastFilePath().isEmpty()) {
      folder = QDir::currentDirPath();
    } else {
      folder = doc->lastFilePath();
    }

    QString newName = KFileDialog::getSaveFileName(folder,
                  i18n("*.kst|Kst Plot File (*.kst)\n*|All Files"),
                                  this, i18n("Save As"));
    if (!newName.isEmpty()) {
      QRegExp extension("*.kst", Qt::CaseInsensitive, QRegExp::Wildcard);
      QString longName = newName;

      if (!extension.exactMatch(newName)) {
        longName = newName + QString(".kst");
      }
      if (doc->saveDocument(longName, false, true)) {
        QFileInfo saveAsInfo(longName);

        addRecentFile(longName);
        doc->setTitle(saveAsInfo.fileName());
        doc->setAbsFilePath(saveAsInfo.absoluteFilePath());

        setWindowTitle(qApp->applicationName() + ": " + doc->title());

        slotUpdateStatusMsg(i18n("Ready"));
        return true;
      }
    } else {
      slotUpdateStatusMsg(i18n("Ready"));
      return false;
    }
  }
}


void KstApp::slotFileClose() {
  if (doc->saveModified()) {
     doc->cancelUpdate();
    _stopping = true;
    QTimer::singleShot(0, doc, SLOT(deleteContents()));
    close();
  }
}


void KstApp::immediatePrintWindowToFile(KMdiChildView *win, const QString& filename) {
  KstViewWindow *view = dynamic_cast<KstViewWindow*>(win);
  if (view && !view->view()->children().isEmpty()) {
    view->immediatePrintToFile(filename);
  }
}


void KstApp::immediatePrintActiveWindowToFile(const QString& filename) {
  KstViewWindow *view = dynamic_cast<KstViewWindow*>(activeWindow());
  if (view) {
    view->immediatePrintToFile(filename);
  }
}


void KstApp::immediatePrintWindowToPng(KMdiChildView *win, const QString& filename, const QString& format, int width, int height, int display) {
  KstViewWindow *view = dynamic_cast<KstViewWindow*>(win);
  if (view && !view->view()->children().isEmpty()) {
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

    view->immediatePrintToPng(filename, size, format);
  }
}


void KstApp::immediatePrintActiveWindowToPng(const QString& filename, const QString& format, int width, int height, int display) {
  KMdiChildView *win = activeWindow();

  if (win) {
    immediatePrintWindowToPng(win, filename, format, width, height, display);
  }
}


void KstApp::immediatePrintWindowToEps(KMdiChildView *win, const QString& filename, int width, int height, int display) {
  KstViewWindow *view = dynamic_cast<KstViewWindow*>(win);
  if (view && !view->view()->children().isEmpty()) {
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

    view->immediatePrintToEps(filename, size);
  }
}


void KstApp::immediatePrintActiveWindowToEps(const QString& filename, int width, int height, int display) {
  KMdiChildView *win = activeWindow();

  if (win) {
    immediatePrintWindowToEps(win, filename, width, height, display);
  }
}


void KstApp::slotFilePrint() {
  KstViewWindow *currentWin = dynamic_cast<KstViewWindow*>(activeWindow());
  int currentPage = 0;
  int pages = 0;

  KMdiIterator<KMdiChildView*> *it = createIterator();
  if (it) {
    while (it->currentItem()) {
      KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (win && !win->view()->children().isEmpty()) {
        ++pages;
      }
      if (win == currentWin) {
        currentPage = pages;
      }
      it->next();
    }
    deleteIterator(it);
  }

  if (pages <= 0) {
    slotUpdateStatusMsg(i18n("Nothing to print"));
    return;
  }

  KPrinter printer(true, QPrinter::HighResolution);
  KstSettings *ks = KstSettings::globalSettings();

  printer.setOption("kde-pagesize", ks->printing.pageSize);
  printer.setOption("kde-orientation", ks->printing.orientation);
  printer.setOption("kst-plot-datetime-footer", ks->printing.plotDateTimeFooter);
  printer.setOption("kst-plot-maintain-aspect-ratio", ks->printing.maintainAspect);
  printer.setOption("kst-plot-curve-width-adjust", ks->printing.curveWidthAdjust);
  printer.setOption("kst-plot-monochrome", ks->printing.monochrome);
  // additional monochrome settings
  printer.setOption("kst-plot-monochromesettings-enhancereadability", ks->printing.monochromeSettings.enhanceReadability);
  printer.setOption("kst-plot-monochromesettings-pointstyleorder", ks->printing.monochromeSettings.pointStyleOrder);
  printer.setOption("kst-plot-monochromesettings-linestyleorder", ks->printing.monochromeSettings.lineStyleOrder);
  printer.setOption("kst-plot-monochromesettings-linewidthorder", ks->printing.monochromeSettings.lineWidthOrder);
  printer.setOption("kst-plot-monochromesettings-maxlinewidth", ks->printing.monochromeSettings.maxLineWidth);
  printer.setOption("kst-plot-monochromesettings-pointdensity", ks->printing.monochromeSettings.pointDensity);

  printer.setFromTo(0, 0);
  printer.setMinMax(1, pages);
  printer.setCurrentPage(currentPage);
  printer.setPageSelection(KPrinter::ApplicationSide);

  pages = 0;
  printer.addDialogPage(new KstPrintOptionsPage);
  if (!printer.setup(this, i18n("Print"))) {
    return;
  }

  KstPainter paint(KstPainter::P_PRINT);
  paint.begin(&printer);
  Q3PaintDeviceMetrics metrics(&printer);
  QSize size(metrics.width(), metrics.height());
  bool datetimeFooter;
  bool maintainAspectRatio;
  bool monochrome;
  int lineAdjust;
  // additional monochrome options
  bool enhanceReadability;
  int pointStyleOrder, lineStyleOrder, lineWidthOrder, maxLineWidth, pointDensity;

  slotUpdateStatusMsg(i18n("Printing..."));

  // make sure defaults are set for settings that are not overwritten
  ks->setPrintingDefaults();

  if (!printer.option("kde-pagesize").isEmpty()) {
    ks->printing.pageSize = printer.option("kde-pagesize");
  }
  if (!printer.option("kde-orientation").isEmpty()) {
    ks->printing.orientation = printer.option("kde-orientation");
  }
  if (printer.option("kst-plot-datetime-footer").isEmpty()) {
    datetimeFooter = ks->printing.plotDateTimeFooter == "1";
  } else {
    ks->printing.plotDateTimeFooter = printer.option("kst-plot-datetime-footer");
    datetimeFooter = printer.option("kst-plot-datetime-footer") == "1";
  }
  if (printer.option("kst-plot-maintain-aspect-ratio").isEmpty()) {
    maintainAspectRatio = ks->printing.maintainAspect == "1";
  } else {
    ks->printing.maintainAspect = printer.option("kst-plot-maintain-aspect-ratio");
    maintainAspectRatio = printer.option("kst-plot-maintain-aspect-ratio") == "1";
  }
  if (printer.option("kst-plot-curve-width-adjust").isEmpty()) {
    lineAdjust = ks->printing.curveWidthAdjust.toInt();
  } else {
    ks->printing.curveWidthAdjust = printer.option("kst-plot-curve-width-adjust");
    lineAdjust = printer.option("kst-plot-curve-width-adjust").toInt();
  }
  if (printer.option("kst-plot-monochrome").isEmpty()) {
    monochrome = ks->printing.monochrome == "1";
  } else {
    ks->printing.monochrome = printer.option("kst-plot-monochrome");
    monochrome = printer.option("kst-plot-monochrome") == "1";
  }
  // save additional monochrome settings
  if (printer.option("kst-plot-monochromesettings-enhancereadability").isEmpty()) {
    enhanceReadability = ks->printing.monochromeSettings.enhanceReadability == "1";
  } else {
    ks->printing.monochromeSettings.enhanceReadability = printer.option("kst-plot-monochromesettings-enhancereadability");
    enhanceReadability = printer.option("kst-plot-monochromesettings-enhancereadability") == "1";
  }
  if (printer.option("kst-plot-monochromesettings-pointstyleorder").isEmpty()) {
    pointStyleOrder = ks->printing.monochromeSettings.pointStyleOrder.toInt();
  } else {
    ks->printing.monochromeSettings.pointStyleOrder = printer.option("kst-plot-monochromesettings-pointstyleorder");
    pointStyleOrder = printer.option("kst-plot-monochromesettings-pointstyleorder").toInt();
  }
  if (printer.option("kst-plot-monochromesettings-linestyleorder").isEmpty()) {
    lineStyleOrder = ks->printing.monochromeSettings.lineStyleOrder.toInt();
  } else {
    ks->printing.monochromeSettings.lineStyleOrder = printer.option("kst-plot-monochromesettings-linestyleorder");
    lineStyleOrder = printer.option("kst-plot-monochromesettings-linestyleorder").toInt();
  }
  if (printer.option("kst-plot-monochromesettings-linewidthorder").isEmpty()) {
    lineWidthOrder = ks->printing.monochromeSettings.lineWidthOrder.toInt();
  } else {
    ks->printing.monochromeSettings.lineWidthOrder = printer.option("kst-plot-monochromesettings-linewidthorder");
    lineWidthOrder = printer.option("kst-plot-monochromesettings-linewidthorder").toInt();
  }
  if (printer.option("kst-plot-monochromesettings-maxlinewidth").isEmpty()) {
    maxLineWidth = ks->printing.monochromeSettings.maxLineWidth.toInt();
  } else {
    ks->printing.monochromeSettings.maxLineWidth = printer.option("kst-plot-monochromesettings-maxlinewidth");
    maxLineWidth = printer.option("kst-plot-monochromesettings-maxlinewidth").toInt();
  }
  if (printer.option("kst-plot-monochromesettings-pointdensity").isEmpty()) {
    pointDensity = ks->printing.monochromeSettings.pointDensity.toInt();
  } else {
    ks->printing.monochromeSettings.pointDensity = printer.option("kst-plot-monochromesettings-pointdensity");
    pointDensity = printer.option("kst-plot-monochromesettings-pointdensity").toInt();
  }

  ks->save();

#if KDE_VERSION < KDE_MAKE_VERSION(3,3,0)
  int iFromPage = printer.fromPage();
  int iToPage = printer.toPage();

  if (iFromPage == 0 && iToPage == 0) {
    printer.setPageSelection(KPrinter::SystemSide);
  }
#else
  Q3ValueList<int> pageList = printer.pageList();
#endif

  it = createIterator();
  if (!it) {
    return;
  }

  bool firstPage = true;
  while (it->currentItem()) {
    KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
    KstTopLevelViewPtr tlv = win ? kst_cast<KstTopLevelView>(win->view()) : 0L;
    if (win && tlv && !tlv->children().isEmpty()) {
      ++pages;
#if KDE_VERSION < KDE_MAKE_VERSION(3,3,0)
      if ((iFromPage == 0 && iToPage == 0) || (iFromPage <= pages && iToPage >= pages)) {
#else
      if (pageList.contains(pages)) {
#endif
        if (!firstPage && !printer.newPage()) {
          break;
        }

        win->print(paint, size, pages, lineAdjust, monochrome, enhanceReadability, datetimeFooter,  maintainAspectRatio, pointStyleOrder, lineStyleOrder, lineWidthOrder, maxLineWidth, pointDensity);

        firstPage = false;
      }
    }
    it->next();
  }
  paint.end();
  deleteIterator(it);
  slotUpdateStatusMsg(i18n("Ready"));
}


void KstApp::immediatePrintToFile(const QString& filename, bool revert) {
  KMdiIterator<KMdiChildView*> *it = createIterator();
  if (!it) {
    return;
  }

  KPrinter printer(true, QPrinter::HighResolution);
  printer.setPageSize(KPrinter::Letter);
  printer.setOrientation(KPrinter::Landscape);
  printer.setOutputToFile(true);
  printer.setOutputFileName(filename);
  printer.setFromTo(0, 0);

  bool firstPage = true;
  KstPainter paint(KstPainter::P_PRINT);
  paint.begin(&printer);
  Q3PaintDeviceMetrics metrics(&printer);
  QRect rect;
  const QSize size(metrics.width(), metrics.height());

  rect.setLeft(0);
  rect.setRight(size.height());
  rect.setBottom(size.height());
  rect.setTop(size.height());

  while (it->currentItem()) {
    KstViewWindow *view = dynamic_cast<KstViewWindow*>(it->currentItem());
    if (view && !view->view()->children().isEmpty()) {
      if (!firstPage && !printer.newPage()) {
        break;
      }

      view->view()->resizeForPrint(size);
      view->view()->paint(paint, QRegion());
      if (revert) {
        view->view()->revertForPrint();
      }

      firstPage = false;
    }
    it->next();
  }
  paint.end();
  deleteIterator(it);
}


void KstApp::immediatePrintToPng(const QString& filename, const QString& format, int width, int height, bool all, int display) {
  if (all) {
    QString filenameSub;
    int pages = 0;

    QString dotFormat = i18n(".%1").arg(format);
    int iPos = filename.findRev(dotFormat, -1, false);
    if (iPos != -1 && iPos == (int)(filename.length() - dotFormat.length())) {
      filenameSub = filename.left(filename.length() - dotFormat.length());
    } else {
      filenameSub = filename;
    }

    KMdiIterator<KMdiChildView*>* it = createIterator();
    if (it) {
      while (it->currentItem()) {
        pages++;
        QString filenameNew = i18n("%1_%2").arg(filenameSub).arg(pages);
        immediatePrintWindowToPng(it->currentItem(), filenameNew, format, width, height, display);
        it->next();
      }
      deleteIterator(it);
    }
  } else {
    immediatePrintActiveWindowToPng(filename, format, width, height, display);
  }
}

void KstApp::immediatePrintToEps(const QString& filename, int width, int height, bool all, int display) {
  if (all) {
    QString filenameSub;
    int pages = 0;

    QString dotFormat = ".eps";
    int iPos = filename.findRev(dotFormat, -1, false);
    if (iPos != -1 && iPos == (int)(filename.length() - dotFormat.length())) {
      filenameSub = filename.left(filename.length() - dotFormat.length());
    } else {
      filenameSub = filename;
    }

    KMdiIterator<KMdiChildView*>* it = createIterator();
    if (it) {
      while (it->currentItem()) {
        pages++;
        QString filenameNew = i18n("%1_%2").arg(filenameSub).arg(pages);
        immediatePrintWindowToEps(it->currentItem(), filenameNew, width, height, display);
        it->next();
      }
      deleteIterator(it);
    }
  } else {
    immediatePrintActiveWindowToEps(filename, width, height, display);
  }
}


void KstApp::slotFileQuit() {
  slotFileClose();
}


void KstApp::slotViewStatusBar() {
  if (StatusBarAction->isChecked()) {
    statusBar()->show();
    updateStatusBarText();
  } else {
    statusBar()->hide();
  }
}


void KstApp::updateStatusBarText() {
  if (statusBar()->isVisible()) {
    QFontMetrics fm(fontMetrics());
    int widthUsed;
    int margin = 3;
    int spacing = 6;
    int widthCurrent = statusBar()->width();
    int widthAvailable = widthCurrent - (2*margin) - spacing;
    int widthData = fm.width(_dataBar->fullText());
    int widthReady = fm.width(_readyBar->fullText());

    if (_progressBar->isVisible()) {
      widthAvailable -= _progressBar->width();
      widthAvailable -= spacing;
    }
    if (_dataNotifier->isVisible()) {
      widthAvailable -= _dataNotifier->geometry().width();
      widthAvailable -= spacing;
    }
    
    widthUsed  = widthData;
    widthUsed += widthReady;
#ifdef HAVE_LINUX
    widthUsed += fm.width(_memoryBar->fullText());
    widthAvailable -= spacing;
#endif

    if (widthUsed > widthAvailable) {
      if (widthData < widthAvailable) {
        statusBar()->setMaximumWidth(widthCurrent);

#ifdef HAVE_LINUX
        if ((widthAvailable - widthData)/2 > widthReady) {
          _memoryBar->setTextWidth(fm, widthAvailable - widthData - widthReady);
          _readyBar->setTextWidth(fm, widthReady);
        } else {
          _memoryBar->setTextWidth(fm, (widthAvailable - widthData)/2);
          _readyBar->setTextWidth(fm, (widthAvailable - widthData)/2);
        }
#else
        _readyBar->setTextWidth(fm, widthAvailable - widthData);
#endif
        _dataBar->setTextWidth(fm, widthData);

        statusBar()->setMaximumWidth(32767);
      } else {
#ifdef HAVE_LINUX
        _memoryBar->setTextWidth(fm, 0);
#endif
        _readyBar->setTextWidth(fm, 0);
        _dataBar->setTextWidth(fm, widthAvailable);
      }
    } else {
#ifdef HAVE_LINUX
      _memoryBar->setFullText();
#endif
      _readyBar->setFullText();
      _dataBar->setFullText();
    }
  }
}


void KstApp::slotUpdateStatusMsg(const QString& msg) {
  _readyBar->setFullText( msg );
  updateStatusBarText();
}


void KstApp::slotUpdateDataMsg(const QString& msg) {
  _dataBar->setFullText( msg );
  updateStatusBarText();
}


void KstApp::slotUpdateMemoryMsg(const QString& msg) {
#ifdef HAVE_LINUX
  _memoryBar->setFullText( msg );
  updateStatusBarText();
#endif
}


void KstApp::slotUpdateProgress(int total, int step, const QString &msg) {
  if (step == 0 && msg.isNull()) {
    slotUpdateStatusMsg(i18n("Ready"));
    _progressBar->hide();
    updateStatusBarText();
    return;
  }

  _progressBar->show();
  if (step > 0) {
    if (!_progressBar->textVisible()) {
      _progressBar->setTextVisible(true);
    }
    if (total != _progressBar->maximum()) {
      _progressBar->setMaximum(total);
    }
    if (_progressBar->value() != step) {
      _progressBar->setValue(step);
    }
  } else {
    _progressBar->setTextVisible(false);
    _progressBar->reset();
  }

  if (msg.isEmpty()) {
    slotUpdateStatusMsg(i18n("Ready"));
  } else {
    slotUpdateStatusMsg(msg);
  }

  updateStatusBarText();

  QCoreApplication::processEvents(QEventLoop::ExcludeSocketNotifiers, 10);
}


bool KstApp::dataMode() const {
  return DataMode->isChecked();
}


void KstApp::toggleDataMode() {
  //DataMode->setChecked(!DataMode->isChecked());
  KstTopLevelViewPtr pView = activeView();
  if (pView) {
    pView->widget()->paint();
  }
  slotUpdateDataMsg(QString::null);
}


void KstApp::toggleMouseMode() {
  KstTopLevelView::ViewMode mode = KstTopLevelView::DisplayMode;
  KAction *action = 0L;
  QString createType;

  if (_gfxLineAction->isChecked()) {
    action = _gfxLineAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Line";
  } else if (_gfxRectangleAction->isChecked()) {
    action = _gfxRectangleAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Box";
  } else if (_gfxEllipseAction->isChecked()) {
    action = _gfxEllipseAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Ellipse";
  } else if (_gfxLabelAction->isChecked()) {
    action = _gfxLabelAction;
    mode = KstTopLevelView::LabelMode;
    createType = "Label";
  } else if (_gfxPictureAction->isChecked()) {
    action = _gfxPictureAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Picture";
  } else if (_gfx2DPlotAction->isChecked()) {
    action = _gfx2DPlotAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Plot";
  } else if (_gfxArrowAction->isChecked()) {
    action = _gfxArrowAction;
    mode = KstTopLevelView::CreateMode;
    createType = "Arrow";
  } else if (LayoutAction->isChecked()) {
    action = LayoutAction;
    mode = KstTopLevelView::LayoutMode;
  } else if (XYZoomAction->isChecked()) {
    action = XYZoomAction;
    mode = KstTopLevelView::DisplayMode;
  } else if (XZoomAction->isChecked()) {
    action = XZoomAction;
    mode = KstTopLevelView::DisplayMode;
  } else if (YZoomAction->isChecked()) {
    action = YZoomAction;
    mode = KstTopLevelView::DisplayMode;
  }

  DataMode->setEnabled(!LayoutAction->isChecked());
  _tiedZoomAction->setEnabled(!LayoutAction->isChecked());

  if (action) {
    KToolBarButton* button = toolBar()->getButton(5);

    if (button) {
      button->setText(action->toolTip());
      button->setIcon(action->icon());
    }
  }

  KMdiIterator<KMdiChildView*> *it = createIterator();
  if (it) {
    while (it->currentItem()) {
      KstViewWindow *pView = dynamic_cast<KstViewWindow*>(it->currentItem());
      if (pView) {
        pView->view()->setViewMode(mode, createType);
      }
      it->next();
    }
    deleteIterator(it);
  }

  _viewMode = mode;
  _createType = createType;
}


KstTopLevelView::ViewMode KstApp::currentViewMode() {
  return _viewMode;
}


QString KstApp::currentCreateType() {
  return _createType;
}


KstApp::KstGraphicType KstApp::getGraphicType() {
  return _graphicType;
}


void KstApp::tieAll() {
  int tied = 0;
  Kst2DPlotList pl = Kst2DPlot::globalPlotList();
  for (Kst2DPlotList::ConstIterator i = pl.begin(); i != pl.end(); ++i) {
    if ((*i)->isTied()) {
      ++tied;
    } else {
      --tied;
    }
  }

  for (Kst2DPlotList::Iterator i = pl.begin(); i != pl.end(); ++i) {
    (*i)->setTied(tied <= 0);
  }

  paintAll(KstPainter::P_PAINT);
}

//#define PAINTTIMER
void KstApp::paintAll(KstPainter::PaintType pt) {
#ifdef PAINTTIMER
  QTime t;
  t.start();
#endif
  if (mdiMode() == KMdi::TabPageMode || mdiMode() == KMdi::IDEAlMode) { // Optimization
    KstViewWindow *view = dynamic_cast<KstViewWindow*>(activeWindow());
    if (view) {
      view->view()->paint(pt);
    }
  } else {
    KMdiIterator<KMdiChildView*> *it = createIterator();
    if (it) {
      while (it->currentItem()) {
        KstViewWindow *view = dynamic_cast<KstViewWindow*>(it->currentItem());
        if (view) {
          view->view()->paint(pt);
        }
        it->next();
      }
      deleteIterator(it);
    }
  }
#ifdef PAINTTIMER
  int x = t.elapsed();
  qDebug() << "paintAll with painttype " << (int)pt << " - " << x << "ms" << endl;
#endif
}


void KstApp::newPlot() {
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(activeWindow());
  if (!w) {
    newWindow(false);
    w = dynamic_cast<KstViewWindow*>(activeWindow());
    assert(w);
  }
  w->createObject<Kst2DPlot>(KST::suggestPlotName(), false);
}


void KstApp::showDataManager() {
  dataManager->show_I();
}


void KstApp::showViewManager() {
  viewManager->show_I();
}


void KstApp::showViewScalarsDialog() {
  viewScalarsDialog->showViewScalarsDialog();
}


void KstApp::showViewStringsDialog() {
  viewStringsDialog->showViewStringsDialog();
}


void KstApp::showViewVectorsDialog() {
  viewVectorsDialog->showViewVectorsDialog();
}


void KstApp::showViewMatricesDialog() {
  viewMatricesDialog->showViewMatricesDialog();
}


void KstApp::showViewFitsDialog() {
  viewFitsDialog->showViewFitsDialog();
}


void KstApp::showChangeFileDialog() {
  changeFileDialog->showChangeFileDialog();
}


void KstApp::showChooseColorDialog() {
  chooseColorDialog->showChooseColorDialog();
}


void KstApp::showDifferentiateCurvesDialog() {
  differentiateCurvesDialog->showCurveDifferentiate();
}


void KstApp::showChangeNptsDialog() {
  changeNptsDialog->showChangeNptsDialog();
}


void KstApp::showGraphFileDialog() {
  graphFileDialog->show_I();
}


void KstApp::showDebugDialog() {
  debugDialog->show_I();
}


void KstApp::showDebugLog() {
  debugDialog->show_I();
  debugDialog->_tabs->setCurrentPage(1);
  debugDialog->logWidget()->scrollToBottom();
}


void KstApp::showMonochromeDialog() {
  _monochromeDialog->showMonochromeDialog();
}


void KstApp::samplesUp() {
  setPaused(false);
  doc->samplesUp();
}


void KstApp::samplesDown() {
  setPaused(false);
  doc->samplesDown();
}


void KstApp::updateDataNotifier() {
  if (_dataNotifier) {
    _dataNotifier->arrived();
  }
}


void KstApp::updateDataDialogs(bool dm, bool vm) {

  ViewScalarsDialogAction->setEnabled(viewScalarsDialog->hasContent());
  ViewStringsDialogAction->setEnabled(viewStringsDialog->hasContent());
  ViewVectorsDialogAction->setEnabled(viewVectorsDialog->hasContent());
  ViewMatricesDialogAction->setEnabled(viewMatricesDialog->hasContent());
  ViewFitsDialogAction->setEnabled(viewFitsDialog->hasContent());

  if (!viewScalarsDialog->isHidden()) {
    viewScalarsDialog->updateViewScalarsDialog();
  }
  if (!viewStringsDialog->isHidden()) {
    viewStringsDialog->updateViewStringsDialog();
  }
  if (!viewVectorsDialog->isHidden()) {
    viewVectorsDialog->updateViewVectorsDialog();
  }
  if (!viewMatricesDialog->isHidden()) {
    viewMatricesDialog->updateViewMatricesDialog();
  }
  if (!viewFitsDialog->isHidden()) {
    viewFitsDialog->updateViewFitsDialog();
  }

  if (dm) {
    dataManager->updateContents();
  }
  if (vm) {
    viewManager->updateContents();
  }
  
  updateMemoryStatus();
}


void KstApp::updateVisibleDialogs() {
  updateDialogs(true);
}


void KstApp::updateDialogs(bool onlyVisible) {
  if (!_stopping) {
#ifdef BENCHMARK
    QTime t;
    t.start();
#endif
    if (!onlyVisible || KstVectorDialogI::globalInstance()->isVisible()) {
      KstVectorDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstPluginDialogI::globalInstance()->isVisible()) {
      KstPluginDialogI::globalInstance()->updateForm();
    }
    if (!onlyVisible || KstFitDialogI::globalInstance()->isVisible()) {
      KstFitDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstFilterDialogI::globalInstance()->isVisible()) {
      KstFilterDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstEqDialogI::globalInstance()->isVisible()) {
      KstEqDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstHsDialogI::globalInstance()->isVisible()) {
      KstHsDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstPsdDialogI::globalInstance()->isVisible()) {
      KstPsdDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstCsdDialogI::globalInstance()->isVisible()) {
      KstCsdDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstCurveDialogI::globalInstance()->isVisible()) {
      KstCurveDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstEventMonitorI::globalInstance()->isVisible()) {
      KstEventMonitorI::globalInstance()->update();
    }
    if (!onlyVisible || KstImageDialogI::globalInstance()->isVisible()) {
      KstImageDialogI::globalInstance()->update();
    }
    if (!onlyVisible || KstMatrixDialogI::globalInstance()->isVisible()) {
      KstMatrixDialogI::globalInstance()->update();
    }
    if (!onlyVisible || changeFileDialog->isVisible()) {
      changeFileDialog->updateChangeFileDialog();
    }
    if (!onlyVisible || chooseColorDialog->isVisible()) {
      chooseColorDialog->updateChooseColorDialog();
    }
    if (!onlyVisible || differentiateCurvesDialog->isVisible()) {
      differentiateCurvesDialog->updateCurveDifferentiate();
    }
    if (!onlyVisible || changeNptsDialog->isVisible()) {
      changeNptsDialog->updateChangeNptsDialog();
    }
    if (!onlyVisible || vectorSaveDialog->isVisible()) {
      vectorSaveDialog->init();
    }
    updateDataDialogs(false);
    updateDataManager(onlyVisible);
    updateViewManager(onlyVisible);
#ifdef BENCHMARK
    qDebug() << "Dialogs updated in " << t.elapsed() << "ms" << endl;
#endif
  }
}


void KstApp::updateDialogsForWindow() {
  if (!_stopping) {
    KstCsdDialogI::globalInstance()->updateWindow();
    KstEqDialogI::globalInstance()->updateWindow();
    KstHsDialogI::globalInstance()->updateWindow();
    KstPsdDialogI::globalInstance()->updateWindow();
    KstCurveDialogI::globalInstance()->updateWindow();
    KstImageDialogI::globalInstance()->updateWindow();
    updateDataManager(false);
    updateViewManager(false);
  }
}


void KstApp::updateDataManager(bool onlyVisible) {
  if (!onlyVisible || dataManager->isVisible()) {
    dataManager->update();
  }
}


void KstApp::updateViewManager(bool onlyVisible) {
  if (!onlyVisible || viewManager->isVisible()) {
    viewManager->update();
  }
}


void KstApp::showPluginManager() {
  PluginManager *pm = new PluginManager(this);
  pm->setObjectName("Plugin Manager");
  pm->exec();
  delete pm;

  KstPluginDialogI::globalInstance()->updatePluginList();
}


void KstApp::showExtensionManager() {
  ExtensionDialog *dlg = new ExtensionDialog(this);
  dlg->setObjectName("Extension Manager");
  dlg->exec();
  delete dlg;
}


void KstApp::showDataWizard() {
  DataWizard *dw = new DataWizard(this, "DataWizard");
  dw->setObjectName("DataWizard");
  dw->exec();
  if (dw->result() == QDialog::Accepted) {
    delete dw; // leave this here - releases references
    forceUpdate();
    doc->setModified();
    updateDialogs();
  } else {
    delete dw;
  }
}


void KstApp::showDataWizardWithFile(const QString &input) {
  DataWizard *dw = new DataWizard(this, "DataWizard");
  dw->setInput(input);
  dw->exec();
  if (dw->result() == QDialog::Accepted) {
    delete dw; // leave this here - releases references
    forceUpdate();
    doc->setModified();
    updateDialogs();
  } else {
    delete dw;
  }
}


void KstApp::registerDocChange() {
  qDebug() << "register doc changed" << endl;
  forceUpdate();
  updateVisibleDialogs();
  doc->setModified();
}


void KstApp::reload() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  KST::vectorList.lock().readLock();
  for (KstVectorList::ConstIterator i = KST::vectorList.begin(); i != KST::vectorList.end(); ++i) {
    KstRVectorPtr r = kst_cast<KstRVector>(*i);
    if (r) {
      r->writeLock();
      r->reload();
      r->unlock();
    }
  }
  KST::vectorList.lock().unlock();
  KST::matrixList.lock().readLock();
  for (KstMatrixList::ConstIterator i = KST::matrixList.begin(); i != KST::matrixList.end(); ++i) {
    KstRMatrixPtr r = kst_cast<KstRMatrix>(*i);
    if (r) {
      r->writeLock();
      r->reload();
      r->unlock();
    }
  }
  KST::matrixList.lock().unlock();
  QApplication::restoreOverrideCursor();
}


void KstApp::slotPreferences() {
  KstSettingsDlg *ksd = new KstSettingsDlg(this);
  ksd->setObjectName("Kst Settings Dialog");
  connect(ksd, SIGNAL(settingsChanged()), this, SIGNAL(settingsChanged()));
  ksd->exec();
  delete ksd;
}


void KstApp::slotSettingsChanged() {
  _updateThread->setUpdateTime(KstSettings::globalSettings()->plotUpdateTimer);
}


void KstApp::slotCopy() {
  if (!LayoutAction->isChecked()) {
    KstTopLevelViewPtr tlv = activeView();
    if (tlv) {
      KstViewWidget *w = tlv->widget();
      KstViewObjectPtr o = w->findChildFor(w->mapFromGlobal(QCursor::pos()));
      Kst2DPlotPtr p = kst_cast<Kst2DPlot>(o);
      if (p) {
        p->copy();
      }
    }
  } else {
    KstViewWindow *vw = dynamic_cast<KstViewWindow*>(activeWindow());
    if (vw) {
      QApplication::clipboard()->setData(vw->view()->widget()->dragObject(), QClipboard::Clipboard);
    }
  }
}


void KstApp::slotPaste() {
  if (LayoutAction->isChecked()) {
    KstTopLevelViewPtr tlv = activeView();

    if (tlv) {
      QMimeSource* source;

      source = QApplication::clipboard()->data(QClipboard::Clipboard);
      if (!tlv->paste(source)) {
        KstDebug::self()->log(i18n("Paste operation failed: clipboard data was not found or of the wrong format."));
      }
    } else {
      KstDebug::self()->log(i18n("Paste operation failed: there is currently no active view."));
    }
  } else {
    KstDebug::self()->log(i18n("Paste operation failed: must be in layout mode."));
  }
}


void KstApp::slotFileNewWindow(QWidget *parent) {
  newWindow(true, parent);
  doc->setModified();
}


void KstApp::slotFileRenameWindow() {
  KstViewWindow *vw = dynamic_cast<KstViewWindow*>(activeWindow());
  if (vw) {
    renameWindow(vw);
  }
}


void KstApp::renameWindow(KstViewWindow *vw) {
  QString name = windowName(true, vw->caption(), true);

  if (!name.isEmpty() && vw->caption() != name) {
    vw->setWindowTitle(name);
    vw->setTabCaption(name);
    updateDialogsForWindow();
    doc->setModified();
  }
}


QString KstApp::newWindow(bool prompt, QWidget *parent) {
  QString nameUsed;
  QString name = windowName(prompt, defaultTag, false, parent);
  if (!name.isEmpty()) {
    nameUsed = newWindow(name);
  }

  return nameUsed;
}


QString KstApp::newWindow(const QString& name_in) {
  KstViewWindow *w = new KstViewWindow;
  QString nameToUse;
  QString name  = name_in;

  while (name.isEmpty() || findWindow(name)) {
    name = KST::suggestWinName();
  }
  nameToUse = name;

  w->setWindowTitle(nameToUse);
  w->setTabCaption(nameToUse);
  addWindow(w, KMdi::StandardAdd | KMdi::UseKMdiSizeHint);
  w->activate();
  updateDialogsForWindow();
  return nameToUse;
}


QString KstApp::windowName(bool prompt, const QString& nameOriginal, bool rename, QWidget *parent) {
  bool ok = false;
  QString name = nameOriginal;

  do {
    if (prompt) {
      QRegExp exp("\\S+.*");
      QRegExpValidator val(exp, 0L);

      if (rename) {
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
        name = KInputDialog::getText(i18n("Kst"), i18n("Enter a new name for the window:"), name, &ok, parent, 0L, &val).trimmed();
#else
        name = KLineEditDlg::getText(i18n("Enter a new name for the window:"), name, &ok, parent, &val).trimmed();
#endif
      } else {
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
        name = KInputDialog::getText(i18n("Kst"), i18n("Enter a name for the new window:"), name, &ok, parent, 0L, &val).trimmed();
#else
        name = KLineEditDlg::getText(i18n("Enter a name for the new window:"), name, &ok, parent, &val).trimmed();
#endif
      }
      if (ok && name==defaultTag) {
        name = KST::suggestWinName();
      }
    } else {
      name = KST::suggestWinName();
      ok = true;
    }
    if (ok) {
      if (!findWindow(name)) {
        break;
      }
    } else {
      return QString::null;
    }
    if (prompt) {
      KMessageBox::sorry(this, i18n("A window with the same name already exists.  Enter a unique window name."));
    }
  } while(true);

  return name;
}


void KstApp::tiedZoomPrev(KstViewWidget* view, const QString& plotName) {
  if (KstSettings::globalSettings()->tiedZoomGlobal) {
    KMdiIterator<KMdiChildView*> *it = createIterator();
    if (it) {
      while (it->currentItem()) {
        KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
        if (win && win->view()->tiedZoomPrev(plotName)) {
          win->view()->widget()->paint();
        }
        it->next();
      }
      deleteIterator(it);
    }
  } else {
    view->viewObject()->tiedZoomPrev(plotName);
  }
}


void KstApp::tiedZoomMode(int zoom, bool flag, double center, int mode, int modeExtra, KstViewWidget* view, const QString& plotName) {
  if (KstSettings::globalSettings()->tiedZoomGlobal) {
    KMdiIterator<KMdiChildView*> *it = createIterator();
    if (it) {
      while (it->currentItem()) {
        KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
        if (win && win->view()->tiedZoomMode(zoom, flag, center, mode, modeExtra, plotName)) {
          win->view()->widget()->paint();
        }
        it->next();
      }
      deleteIterator(it);
    }
  } else {
    view->viewObject()->tiedZoomMode(zoom, flag, center, mode, modeExtra, plotName);    
  }
}


void KstApp::tiedZoom(bool x, double xmin, double xmax, bool y, double ymin, double ymax, KstViewWidget* view, const QString& plotName) {
  if (KstSettings::globalSettings()->tiedZoomGlobal) {
    KMdiIterator<KMdiChildView*> *it = createIterator();
    if (it) {
      while (it->currentItem()) {
        KstViewWindow *win = dynamic_cast<KstViewWindow*>(it->currentItem());
        if (win && win->view()->tiedZoom(x, xmin, xmax, y, ymin, ymax, plotName)) {
          win->view()->widget()->paint();
        }
        it->next();
      }
      deleteIterator(it);
    }
  } else {
    view->viewObject()->tiedZoom(x, xmin, xmax, y, ymin, ymax, plotName);    
  }
}


KstTopLevelViewPtr KstApp::activeView() {
  KstViewWindow *vw = dynamic_cast<KstViewWindow*>(activeWindow());

  if (!vw) {
    return 0L;
  }

  return vw->view();
}


Kst2DPlotMap* KstApp::plotHolderWhileOpeningDocument() {
  return _plotHolderWhileOpeningDocument;
}


void KstApp::updatePausedState(bool state) {
  _updateThread->setPaused(state);
}


void KstApp::fromEnd() {
  doc->samplesEnd();
  setPaused(false);
}


void KstApp::updateMemoryStatus() {
#ifdef HAVE_LINUX
  meminfo();
  unsigned long mi = S(kb_main_free + kb_main_buffers + kb_main_cached);
  slotUpdateMemoryMsg(i18n("%1 MB available").arg(mi / (1024 * 1024)));
#endif
}


const QStringList KstApp::recentFiles() const {
  return _recent->items();
}


void KstApp::showQuickStartDialog() {
#ifdef KST_QUICKSTART_DLG
  if (KstSettings::globalSettings()->showQuickStart) {
    _quickStartDialog->show_I();
  }
#endif
}


void KstApp::fixKMdi() {
  KTabWidget *tw = tabWidget();
  if (tw) {
    tw->setHoverCloseButton(false);
    disconnect(tw, SIGNAL(contextMenu(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    disconnect(tw, SIGNAL(contextMenu(QWidget*, const QPoint&)), this, SLOT(showContextMenu(QWidget*, const QPoint&)));
    connect(tw, SIGNAL(contextMenu(const QPoint&)), SLOT(showContextMenu(const QPoint&)));
    connect(tw, SIGNAL(contextMenu(QWidget*, const QPoint&)), SLOT(showContextMenu(QWidget*, const QPoint&)));
    tw->setTabReorderingEnabled(true);
  }
}


void KstApp::createDebugNotifier() {
  if (!_debugNotifier) {
    _debugNotifier = new KstDebugNotifier(statusBar());
    statusBar()->addPermanentWidget(_debugNotifier, 0);
  } else {
    _debugNotifier->reanimate();
  }
}


void KstApp::destroyDebugNotifier() {
  delete _debugNotifier;
}


void KstApp::showContextMenu(QWidget *w, const QPoint& pos) {
  KMenu *pm = new KMenu(this);
  KstViewWindow *vw = dynamic_cast<KstViewWindow*>(w);
  if (vw) {
    pm->insertTitle(vw->caption());
  }

  pm->insertItem(i18n("&New..."), this, SLOT(slotFileNewWindow()));
  if (vw) {
    KTabWidget *tw = tabWidget();
    if (tw) { // should always be true, but who knows how KMdi might change
      int id = pm->insertItem(i18n("Move &Left"), vw, SLOT(moveTabLeft()));
      pm->setItemEnabled(id, tw->indexOf(w) > 0);
      id = pm->insertItem(i18n("Move &Right"), vw, SLOT(moveTabRight()));
      pm->setItemEnabled(id, tw->indexOf(w) < tw->count() - 1);
    }
    pm->insertItem(i18n("R&ename..."), vw, SLOT(rename()));
    pm->insertItem(i18n("&Close"), vw, SLOT(close()));
  }

  pm->exec(pos);
  delete pm;
}


void KstApp::showContextMenu(const QPoint& pos) {
  KMenu *pm = new KMenu(this);
  pm->insertItem(i18n("&New..."), this, SLOT(slotFileNewWindow()));
  pm->exec(pos);
  delete pm;
}


void KstApp::moveTabLeft(KstViewWindow *tab) {
  KTabWidget *tw = tabWidget();
  if (tw) {
    int cur = tw->indexOf(tab);
    if (cur > 0) {
      tw->moveTab(cur, cur - 1);
    }
  }
}


void KstApp::moveTabRight(KstViewWindow *tab) {
  KTabWidget *tw = tabWidget();
  if (tw) {
    int cur = tw->indexOf(tab);
    if (cur >= 0 && cur < tw->count() - 1) {
      tw->moveTab(cur, cur + 1);
    }
  }
}


void KstApp::emitTimezoneChanged(const QString& tz, int utcOffset) {
  emit timezoneChanged(tz, utcOffset);
}

#include "kst.moc"
// vim: ts=2 sw=2 et
