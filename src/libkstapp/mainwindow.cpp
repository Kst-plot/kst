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

#include "mainwindow.h"
#include "boxitem.h"
#include "datamanager.h"
#include "debugdialog.h"
#include "debugnotifier.h"
#include "document.h"
#include "ellipseitem.h"
#include "exportgraphicsdialog.h"
#include "exportvectorsdialog.h"
#include "logdialog.h"
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
#include "sharedaxisboxitem.h"
#include "ui_aboutdialog.h"
#include "viewvectordialog.h"
#include "viewmatrixdialog.h"
#include "viewprimitivedialog.h"
#include "view.h"
#include "applicationsettings.h"
#include "updatemanager.h"
#include "datasourcepluginmanager.h"
#include "pluginmenuitemaction.h"

#include "applicationsettingsdialog.h"
#include "themedialog.h"
#include "differentiatecurvesdialog.h"
#include "filtermultipledialog.h"
#include "choosecolordialog.h"
#include "changedatasampledialog.h"
#include "changefiledialog.h"
#include "bugreportwizard.h"
#include "datawizard.h"
#include "aboutdialog.h"
#include "datavector.h"
#include "commandlineparser.h"
#include "dialogdefaults.h"
#include "settings.h"

#include "dialoglauncher.h"
#include "scriptserver.h"

#include "geticon.h"

#ifndef KST_NO_SVG
#include <QSvgGenerator>
#endif

#include <QUndoGroup>
#ifndef KST_NO_PRINTER
#include <QPrintDialog>
#endif
#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QMessageBox>
#include <QImageWriter>
#include <QToolBar>
#include <QDebug>
#include <QDesktopServices>
#include <QSignalMapper>
#include <QClipboard>

namespace Kst {

MainWindow::MainWindow() :
    _settings(createSettings("application")),
    _dataManager(0),
    _exportGraphics(0),
    _exportVectors(0),
    _logDialog(0),
    _differentiateCurvesDialog(0),
    _filterMultipleDialog(0),
    _chooseColorDialog(0),
    _changeDataSampleDialog(0),
    _changeFileDialog(0),
    _bugReportWizard(0),
    _applicationSettingsDialog(0),
    _themeDialog(0),
    _aboutDialog(0),
    _viewVectorDialog(0),
    _highlightPoint(false),
    _statusBarTimeout(0),
#if defined(__QNX__)
    qnxToolbarsVisible(true),
#endif
    _ae_width(1280),
    _ae_height(1024),
    _ae_display(2),
    _ae_export_all(false),
    _ae_autosave_period(0),
    _ae_Timer(0),
    _sessionFileName(QString())
{
  _doc = new Document(this);
  _scriptServer = new ScriptServer(_doc->objectStore());
  _videoMapper = new QSignalMapper(this);

  _tabWidget = new TabWidget(this);
  _undoGroup = new QUndoGroup(this);
  _debugDialog = new DebugDialog(this);

  Debug::self()->setHandler(_debugDialog);

  setKstWindowTitle();

  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  _tabWidget->createView();

  setCentralWidget(_tabWidget);
  _tabWidget->setAcceptDrops(false); // Force drops to be passed to parent
  connect(_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentViewChanged()));
  connect(_tabWidget, SIGNAL(currentViewModeChanged()), this, SLOT(currentViewModeChanged()));
  connect(PlotItemManager::self(), SIGNAL(tiedZoomRemoved()), this, SLOT(tiedZoomRemoved()));
  connect(PlotItemManager::self(), SIGNAL(allPlotsTiedZoom()), this, SLOT(allPlotsTiedZoom()));

  readSettings();
  connect(UpdateManager::self(), SIGNAL(objectsUpdated(qint64)), this, SLOT(updateViewItems(qint64)));

  QTimer::singleShot(0, this, SLOT(performHeavyStartupActions()));

  updateRecentKstFiles();
  setAcceptDrops(true);

#if defined(__QNX__)
  // We want to be able to intercept bezel gestures, which show up in Qt as menu button keyPressEvents.
  qApp->installEventFilter(this);
  qnxToggleToolbarVisibility();
#endif

}


MainWindow::~MainWindow() {
  delete _dataManager;
  _dataManager = 0;
  delete _doc;
  _doc = 0;
  delete _viewVectorDialog;
  _viewVectorDialog = 0;
  delete _scriptServer;
  _scriptServer = 0;
}


void MainWindow::performHeavyStartupActions() {
  // Set the timer for the UpdateManager.
  UpdateManager::self()->setMinimumUpdatePeriod(ApplicationSettings::self()->minimumUpdatePeriod());
  DataObject::init();
  DataSourcePluginManager::init();
}

void MainWindow::cleanup() {
  if (document() && document()->objectStore()) {
    document()->objectStore()->clear();
  }
}


void MainWindow::setLayoutMode(bool layoutMode) {
  foreach (View *v, tabWidget()->views() ) {

    Q_ASSERT(v);

    if (layoutMode) {
      v->setViewMode(View::Layout);
      _highlightPointAct->setEnabled(false);
      _tiedZoomAct->setEnabled(false);
    } else {
      v->setViewMode(View::Data);
      _highlightPointAct->setEnabled(true);
      _tiedZoomAct->setEnabled(true);
    }
  }
}

void MainWindow::setHighlightPoint(bool highlight) {
  _highlightPoint = highlight;
}

void MainWindow::changeZoomOnlyMode(QAction* act) {
  if (act == _layoutModeAct && act->isChecked()) {
    setLayoutMode(true);
  } else {
    setLayoutMode(false);
  }
  foreach (View *view, tabWidget()->views() ) {
    view->setZoomOnly((View::ZoomOnlyMode)act->data().toInt());
  }

}

void MainWindow::toggleTiedZoom() {
  if (isTiedTabs()) {
    PlotItemManager::self()->toggleAllTiedZoom(0);
    /*
    QList<View*> views = tabWidget()->views();
    foreach (View* view, views) {
      PlotItemManager::self()->toggleAllTiedZoom(view);
    }
    */
  } else {
    PlotItemManager::self()->toggleAllTiedZoom(tabWidget()->currentView());
  }
}

void MainWindow::tiedZoomRemoved() {
  _tiedZoomAct->setChecked(false);
}


void MainWindow::allPlotsTiedZoom() {
  _tiedZoomAct->setChecked(true);
}


bool MainWindow::promptSaveDone() {
  if (! _doc->isChanged()) {
    return true; // No need to ask if there is no unsaved change -> we're done
  }
  else { // Changes registered: ask the user
    int rc = QMessageBox::warning(this, tr("Kst: Save Prompt"), tr("Your document has been modified.\nSave changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (rc == QMessageBox::Save) {
      save();
    } else if (rc == QMessageBox::Cancel) {
      return false;
    }
    return true;
  }
}


void MainWindow::closeEvent(QCloseEvent *e) {
  if (!promptSaveDone()) {
    e->ignore();
    return;
    }
    //cleanup();
    QMainWindow::closeEvent(e);
    kstApp->closeAllWindows();
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
  QString restorePath = QDir::currentPath();
  QString kstfiledir = fn.left(fn.lastIndexOf('/')) + '/';
  QDir::setCurrent(kstfiledir);
  //QString currentP = QDir::currentPath();
  _doc->save(fn);
  QDir::setCurrent(restorePath);
  _sessionFileName = fn;
  setKstWindowTitle();
  updateRecentKstFiles(fn);
}

void MainWindow::newDoc(bool force) {
  bool clearApproved = false;
  if (force) {
    clearApproved = true;
  } else {
    clearApproved = promptSaveDone();
  }

  if (clearApproved) {
    cleanup();
    delete _dataManager;
    _dataManager = 0;
    delete _doc;
    resetNameIndexes();
    _doc = new Document(this);
    _scriptServer->setStore(_doc->objectStore());
    tabWidget()->clear();
    tabWidget()->createView();
    return;
  } else {
    return;
  }

}

void MainWindow::open() {
  if (!promptSaveDone()) {
    return;
  }
  const QString key = "lastOpenedKstFile";
  QString fn = _settings.value(key).toString();
  if (fn.isEmpty()) {
      fn = _doc->fileName();
  }
  fn = QFileDialog::getOpenFileName(this, tr("Kst: Open File"), fn, tr("Kst Sessions (*.kst)"));
  if (fn.isEmpty()) {
    return;
  }
  _settings.setValue(key, fn);
  openFile(fn);
}



QAction* MainWindow::createRecentFileAction(const QString& filename, int idx, const QString& name, const char* openslot)
{
  QAction* action = new QAction(this);
  QString text = tr("&%1 %2", "Part of a list.  %1 is an index.  %2 is a file name").arg(idx).arg(name);
  action->setText(text);
  action->setData(filename);
  action->setStatusTip(filename);
  action->setVisible(true);
  connect(action, SIGNAL(triggered()), this, openslot);
  return action;
}


void MainWindow::updateRecentKstFiles(const QString& filename)
{
  updateRecentFiles("recentKstFileList", _fileMenu, _bottomRecentKstActions, _recentKstFilesMenu, filename, SLOT(openRecentKstFile()));
}

void MainWindow::setKstWindowTitle()
{
  QString title = "Kst";
  QString server_name = _scriptServer->serverName;
  QString user_name = "--"+kstApp->userName();

  if (server_name.endsWith(user_name)) {
    server_name.remove(server_name.lastIndexOf(user_name),10000);
  }

  if (!_sessionFileName.isEmpty()) {
    title += " - " + _sessionFileName;
  }
  if (scriptServerNameSet()) {
    title += " -- " + server_name;
  }
  setWindowTitle(title);
}

QString MainWindow::scriptServerName()
{
  return _scriptServer->serverName;
}

bool MainWindow::scriptServerNameSet()
{
  return _scriptServer->serverNameSet;
}

void MainWindow::setScriptServerName(QString server_name)
{
  _scriptServer->setScriptServerName(server_name);
  setKstWindowTitle();
}

void MainWindow::copyTab()
{
  View *view = _tabWidget->currentView();
  QClipboard *clipboard = QApplication::clipboard();
  QMimeData *mimedata = new QMimeData();

  QPainter painter;
  QPixmap pixmap(view->size());

  painter.begin(&pixmap);

  view->setPrinting(true);
  view->render(&painter);
  view->setPrinting(false);

  painter.end();

  //mimedata->setText("Some text from kst, just for you!");
  mimedata->setImageData(pixmap.toImage());

  clipboard->setMimeData(mimedata);
}


void MainWindow::updateRecentDataFiles(const QString& filename)
{
  updateRecentFiles("recentDataFileList", _toolsMenu, _bottomRecentDataActions, _recentDataFilesMenu, filename, SLOT(openRecentDataFile()));
  if (!filename.isEmpty()) {
    dialogDefaults().setValue("vector/datasource", filename);
  }
}


QStringList MainWindow::recentDataFiles() {
  QStringList recentFiles = _settings.value("recentDataFileList").toStringList();
  if (recentFiles.removeDuplicates() > 0) {
    _settings.setValue("recentDataFileList", recentFiles);
  }
  return recentFiles;
}

void MainWindow::cleanupRecentDataFilesList() {
  QStringList recentFiles = _settings.value("recentDataFileList").toStringList();
  recentFiles.removeDuplicates();
  foreach(const QString& it, recentFiles) {
    if (!QFileInfo(it).exists()) {
      recentFiles.removeOne(it);
    }
  }
  _settings.setValue("recentDataFileList", recentFiles);
  updateRecentKstFiles();
  updateRecentDataFiles();
}


void MainWindow::updateRecentFiles(const QString& key ,QMenu* menu, QList<QAction*>& actions, QMenu* submenu, const QString& newfilename, const char* openslot)
{
  // Always add absolute paths to the recent file lists, otherwise they are not very reusable
  QString absoluteFilePath = DataSource::cleanPath(newfilename);
  if (!newfilename.isEmpty() && !QDir::isAbsolutePath(newfilename)) { // If it's not empty and not absolute either, add the dir
    absoluteFilePath = DataSource::cleanPath(QDir::currentPath() + '/' + newfilename);
  }
  foreach(QAction* it, actions) {
    menu->removeAction(it);
    delete it;
  }
  actions.clear();
  QStringList recentFiles = _settings.value(key).toStringList();
  if (recentFiles.removeDuplicates() > 0) {
    _settings.setValue(key, recentFiles);
  }
  if (!absoluteFilePath.isEmpty()) {
    recentFiles.removeOne(absoluteFilePath);
    recentFiles.push_front(absoluteFilePath);
    recentFiles = recentFiles.mid(0, 30);
    _settings.setValue(key, recentFiles);
  }

  submenu->clear();
  QAction* check = new QAction(this);
  check->setText(tr("&Cleanup Non-Existent Files"));
  check->setData(key);
  check->setVisible(true);
  connect(check, SIGNAL(triggered()), this, SLOT(checkRecentFilesOnExistence()));
  submenu->addAction(check);
  submenu->addSeparator();
  int i = 0;
  foreach(const QString& it, recentFiles) {
    i++;
    if (i <= 5) {
      // don't make file menu too wide, show complete path in statusbar
      QAction* action = createRecentFileAction(it, i, QFileInfo(it).fileName(), openslot);
      actions << action;
      menu->addAction(action);
    }
    submenu->addAction(createRecentFileAction(it, i, it, openslot));
  }
}


void MainWindow::openRecentKstFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    openFile(action->data().toString());
  }
}


void MainWindow::checkRecentFilesOnExistence()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    QStringList recentFiles = _settings.value(action->data().toString()).toStringList();
    recentFiles.removeDuplicates();
    foreach(const QString& it, recentFiles) {
      if (!QFileInfo(it).exists()) {
        recentFiles.removeOne(it);
      }
    }
    _settings.setValue(action->data().toString(), recentFiles);
    updateRecentKstFiles();
    updateRecentDataFiles();
  }
}

void MainWindow::autoExportImage() {
  exportGraphicsFile(_ae_filename, _ae_format, _ae_width, _ae_height, _ae_display, _ae_export_all, _ae_autosave_period);
}



bool MainWindow::initFromCommandLine() {
  delete _doc;
  _doc = new Document(this);
  _scriptServer->setStore(_doc->objectStore());

  CommandLineParser P(_doc, this);

  bool ok = _doc->initFromCommandLine(&P);
  if (!P.pngFile().isEmpty()) {
    int w = 1280;
    int h = 1024;

    if (P.pngWidth()>1) {
      w = P.pngWidth();
    }
    if (P.pngHeight()>1) {
      h = P.pngHeight();
    }
    exportGraphicsFile(P.pngFile(), "png", w, h, 2, true, 0);
    ok = false;
  }
  if (!P.printFile().isEmpty()) {
#ifndef KST_NO_PRINTER
    printFromCommandLine(P.printFile());
#endif
    ok = false;
  }
  if (!P.kstFileName().isEmpty()) {
    _sessionFileName = P.kstFileName();
    setKstWindowTitle();
  }
  _doc->setChanged(false);
  return ok;
}

void MainWindow::openFile(const QString &file) {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  cleanup();
  newDoc(true); // Does all the init stuff, but does not ask for override as it's supposed to be done elsewhere
  bool ok = _doc->open(file);
  updateProgress(100, "");
  QApplication::restoreOverrideCursor();

  if (!ok) {
    QString lastError = _doc->lastError();
    if (lastError.isEmpty())
      lastError = tr("For details see Help->Debug Dialog->Log.");
    QMessageBox::critical(this, tr("Kst"),tr("Error opening document:\n  '%1'\n%2\n").arg(file, lastError));
  }

  _sessionFileName = file;
  setKstWindowTitle();
  updateRecentKstFiles(file);
}


void MainWindow::exportGraphicsFile(const QString &filename, const QString &format, int width, int height, int display, bool export_all, int autosave_period) {
  int viewCount = 0;
  int n_views = _tabWidget->views().size();
  int i_startview, i_endview;

  _ae_filename = filename;
  _ae_format = format;
  _ae_width = width;
  _ae_height = height;
  _ae_display = display;
  _ae_export_all = export_all;
  _ae_autosave_period = autosave_period;

  dialogDefaults().setValue("export/filename", filename);
  dialogDefaults().setValue("export/format", format);
  dialogDefaults().setValue("export/xsize", width);
  dialogDefaults().setValue("export/ysize", height);
  dialogDefaults().setValue("export/sizeOption", display);

  if (export_all) {
    i_startview = 0;
    i_endview = n_views-1;
  } else {
    i_startview = i_endview = _tabWidget->currentIndex();
  }

  for (int i_view = i_startview; i_view<=i_endview; i_view++) {
    View *view = _tabWidget->views().at(i_view);
    QSize size;
    if (display == 0) { // Width set by user, maintain aspect ratio
      QSize sizeWindow(view->geometry().size());
      size.setWidth(width);
      size.setHeight((int)((qreal)width * (qreal)sizeWindow.height() / (qreal)sizeWindow.width()));
    } else if (display == 1) { // Height set by user, maintain aspect ratio
      QSize sizeWindow(view->geometry().size());
      size.setHeight(height);
      size.setWidth((int)((qreal)height * (qreal)sizeWindow.width() / (qreal)sizeWindow.height()));
    } else if (display == 2) { // Width and height set by user
      size.setWidth(width);
      size.setHeight(height);
    } else { //if (display == 3) { // Square (width x width)
      size.setWidth(width);
      size.setHeight(width);
    }

    QString file = filename;
    if (i_startview - i_endview != 0) {
      QFileInfo QFI(filename);
      file = QFI.dir().path() + '/' + QFI.completeBaseName() +
             '_' +
          _tabWidget->tabBar()->tabText(viewCount).replace(QString("&"),QString()) + '.' +
             QFI.suffix();
    }
    if (format == QString("svg")) {
#ifndef KST_NO_SVG
      QPainter painter;
      QSvgGenerator generator;

      QSize currentSize(view->size());
      view->resize(size);
      view->processResize(size);
      view->setPrinting(true);

      generator.setFileName(file);
      generator.setResolution(300);
      generator.setSize(view->size());
      generator.setViewBox(view->rect());

      painter.begin(&generator);
      view->render(&painter);
      painter.end();

      view->setPrinting(false);
      view->resize(currentSize);
      view->processResize(currentSize);
#endif
    } else if (format == QString("eps")) {
#ifndef KST_NO_PRINTER
      QPrinter printer(QPrinter::ScreenResolution);
#ifdef QT5
      printer.setOutputFormat(QPrinter::PdfFormat);
#else
      printer.setOutputFormat(QPrinter::PostScriptFormat);
#endif
      printer.setOutputFileName(file);
      printer.setOrientation(QPrinter::Portrait);

      printer.setPrintRange(QPrinter::PageRange);
      printer.setFromTo(i_view+1, i_view+1);

      printer.setPaperSize(size, QPrinter::DevicePixel);
      printToPrinter(&printer);

    } else if (format == QString("pdf")) {
      QPrinter printer(QPrinter::ScreenResolution);
      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setOutputFileName(file);
      //setPrinterDefaults(&printer);
      printer.setOrientation(QPrinter::Portrait);

      printer.setPrintRange(QPrinter::PageRange);
      printer.setFromTo(i_view+1, i_view+1);

      printer.setPaperSize(size, QPrinter::DevicePixel);
      printToPrinter(&printer);
#endif
    } else {
      QPainter painter;
      QImage image(size, QImage::Format_ARGB32);

      painter.begin(&image);

      QSize currentSize(view->size());
      view->resize(size);
      view->processResize(size);
      view->setPrinting(true);
      view->render(&painter);
      view->setPrinting(false);
      view->resize(currentSize);
      view->processResize(currentSize);

      painter.end();

      QImageWriter imageWriter(file, format.toLatin1());
      imageWriter.write(image);
    }
    viewCount++;
  }

  if (_ae_autosave_period > 0) {
    if (_ae_Timer == 0) { // create timer (only once)
      _ae_Timer = new QTimer(this);
      _ae_Timer->setSingleShot(true);
      connect(_ae_Timer, SIGNAL(timeout()), this, SLOT(autoExportImage()));
    }
    _ae_Timer->start(_ae_autosave_period); // one shot timer...
  }
}

void MainWindow::exportLog(const QString &imagename, QString &msgfilename, const QString &format, int x_size, int y_size,
                           int size_option_index, const QString &message) {
  View *view = _tabWidget->currentView();

  QSize size;
  if (size_option_index == 0) {
    size.setWidth(x_size);
    size.setHeight(y_size);
  } else if (size_option_index == 1) {
    size.setWidth(x_size);
    size.setHeight(y_size);
  } else if (size_option_index == 2) {
    QSize sizeWindow(view->geometry().size());

    size.setWidth(x_size);
    size.setHeight((int)((qreal)x_size * (qreal)sizeWindow.height() / (qreal)sizeWindow.width()));
  } else {
    QSize sizeWindow(view->geometry().size());

    size.setHeight(y_size);
    size.setWidth((int)((qreal)y_size * (qreal)sizeWindow.width() / (qreal)sizeWindow.height()));
  }

  QImage image(size, QImage::Format_ARGB32);

  QPainter painter(&image);
  QSize currentSize(view->size());
  view->resize(size);
  view->processResize(size);
  view->setPrinting(true);
  view->render(&painter);
  view->setPrinting(false);
  view->resize(currentSize);
  view->processResize(currentSize);

  QImageWriter imageWriter(imagename, format.toLatin1());
  imageWriter.write(image);

  QFile file(msgfilename);

  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << message;
    file.close();
  }

}

#ifndef KST_NO_PRINTER
void MainWindow::printToPrinter(QPrinter *printer) {
  QPainter painter(printer);
  QList<View*> pages;

  switch (printer->printRange()) {
   case QPrinter::PageRange:
    if (printer->fromPage()>0) {
      for (int i_page = printer->fromPage(); i_page<=printer->toPage(); i_page++) {
        pages.append(_tabWidget->views().at(i_page-1));
      }
    }
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

  QSize printerPageSize = printer->pageRect().size();
  for (int i = 0; i < printer->numCopies(); ++i) {
    for (int i_page = 0; i_page<pages.count(); i_page++) {
      View *view = pages.at(i_page);
      QSize currentSize(view->size());
      view->resize(printerPageSize);
      view->processResize(printerPageSize);
      view->setPrinting(true);
      view->render(&painter);
      view->setPrinting(false);
      view->resize(currentSize);
      view->processResize(currentSize);
      if (i_page<pages.count()-1)
        printer->newPage();

    }
  }
}

void MainWindow::printFromCommandLine(const QString &printFileName) {
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setResolution(300);
  printer.setOutputFileName(printFileName);
  setPrinterDefaults(&printer);

  printer.setPrintRange(QPrinter::AllPages);
  printToPrinter(&printer);
}

void MainWindow::setPrinterDefaults(QPrinter *printer) {
  if (dialogDefaults().value("print/landscape",true).toBool()) {
    printer->setOrientation(QPrinter::Landscape);
  } else {
    printer->setOrientation(QPrinter::Portrait);
  }

  printer->setPaperSize(QPrinter::PaperSize(dialogDefaults().value("print/paperSize", QPrinter::Letter).toInt()));

  QPointF topLeft =dialogDefaults().value("print/topLeftMargin", QPointF(15.0,15.0)).toPointF();
  QPointF bottomRight =dialogDefaults().value("print/bottomRightMargin", QPointF(15.0,15.0)).toPointF();

  printer->setPageMargins(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y(), QPrinter::Millimeter);
  // Apparent Qt bug: setting the page margins here doesn't set the correspoding values in the print
  // dialog->printer-options sub-dialog under linux.  If you don't open the printer-options sub-dialog,
  // the values here are honored.
}

void MainWindow::savePrinterDefaults(QPrinter *printer) {
  dialogDefaults().setValue("print/landscape", printer->orientation() == QPrinter::Landscape);
  dialogDefaults().setValue("print/paperSize", int(printer->paperSize()));

  qreal left, top, right, bottom;
  printer->getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
  dialogDefaults().setValue("print/topLeftMargin", QPointF(left, top));
  dialogDefaults().setValue("print/bottomRightMargin", QPointF(right, bottom));

}

void MainWindow::print() {
  QPrinter printer(QPrinter::ScreenResolution);
  printer.setResolution(300);

  setPrinterDefaults(&printer);

#ifndef Q_OS_WIN
  // QPrintDialog: Cannot be used on non-native printers
  printer.setOutputFileName(dialogDefaults().value("print/path", "./print.pdf").toString());
#endif
  QPointer<QPrintDialog> pd = new QPrintDialog(&printer, this);
#if QT_VERSION >= 0x040500
  pd->setOption(QPrintDialog::PrintToFile);
  pd->setOption(QPrintDialog::PrintPageRange, true);
  pd->setOption(QAbstractPrintDialog::PrintShowPageSize,true);
#endif

  if (pd->exec() == QDialog::Accepted) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    printToPrinter(&printer);
    QApplication::restoreOverrideCursor();
    savePrinterDefaults(&printer);
  }
  dialogDefaults().setValue("print/path", printer.outputFileName());
  delete pd;
}
#endif

void MainWindow::currentViewChanged() {
  if(!_tabWidget->currentView())
    return;
  _undoGroup->setActiveStack(_tabWidget->currentView()->undoStack());
  currentViewModeChanged();
}


void MainWindow::currentViewModeChanged() {
  if(!_tabWidget->currentView())
    return;
  _layoutModeAct->setChecked(_tabWidget->currentView()->viewMode() == View::Layout);
}

void MainWindow::aboutToQuit() {
  writeSettings();
}


void MainWindow::about() {
  if (!_aboutDialog) {
    _aboutDialog = new AboutDialog(this);
  }
  if (_aboutDialog->isVisible()) {
    _aboutDialog->raise();
    _aboutDialog->activateWindow();
  }
  _aboutDialog->show();
}


void MainWindow::clearDrawingMarker() {
  _createBoxAct->setChecked(false);
  _createSharedAxisBoxAct->setChecked(false);
  _createCircleAct->setChecked(false);
  _createEllipseAct->setChecked(false);
  _createLabelAct->setChecked(false);
  _createLineAct->setChecked(false);
  _createArrowAct->setChecked(false);
  _createPictureAct->setChecked(false);
  _createPlotAct->setChecked(false);
#ifndef KST_NO_SVG
  _createSvgAct->setChecked(false);
#endif
  _tabWidget->currentView()->setMouseMode(View::Default);
}


void MainWindow::createBox() {
  if (_createBoxAct->isChecked()) {
    clearDrawingMarker();
    _createBoxAct->setChecked(true);
    CreateBoxCommand *cmd = new CreateBoxCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}



void MainWindow::createSharedAxisBox() {
  if (_createSharedAxisBoxAct->isChecked()) {
    clearDrawingMarker();
    _createSharedAxisBoxAct->setChecked(true);
    CreateSharedAxisBoxCommand *cmd = new CreateSharedAxisBoxCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}

void MainWindow::createVector() {
  QString tmp;
  DialogLauncher::self()->showVectorDialog(tmp);
}


void MainWindow::createMatrix() {
  QString tmp;
  DialogLauncher::self()->showMatrixDialog(tmp);
}


void MainWindow::createScalar() {
  QString scalarName;
  DialogLauncher::self()->showScalarDialog(scalarName);
}


void MainWindow::createString() {
  QString stringName;
  DialogLauncher::self()->showStringDialog(stringName);
}


void MainWindow::createCircle() {
  if (_createCircleAct->isChecked()) {
    clearDrawingMarker();
    _createCircleAct->setChecked(true);
    CreateCircleCommand *cmd = new CreateCircleCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createEllipse() {
  if (_createEllipseAct->isChecked()) {
    clearDrawingMarker();
    _createEllipseAct->setChecked(true);
    CreateEllipseCommand *cmd = new CreateEllipseCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createLabel() {
  if (_createLabelAct->isChecked()) {
    clearDrawingMarker();
    _createLabelAct->setChecked(true);
    CreateLabelCommand *cmd = new CreateLabelCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createLine() {
  if (_createLineAct->isChecked()) {
    clearDrawingMarker();
    _createLineAct->setChecked(true);
    CreateLineCommand *cmd = new CreateLineCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createArrow() {
  if (_createArrowAct->isChecked()) {
    clearDrawingMarker();
    _createArrowAct->setChecked(true);
    CreateArrowCommand *cmd = new CreateArrowCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createPicture() {
  if (_createPictureAct->isChecked()) {
    clearDrawingMarker();
    _createPictureAct->setChecked(true);
    CreatePictureCommand *cmd = new CreatePictureCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}


void MainWindow::createPlot() {
  if (_createPlotAct->isChecked()) {
    clearDrawingMarker();
    _createPlotAct->setChecked(true);
    CreatePlotCommand *cmd = new CreatePlotCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}

void MainWindow::insertPlot() {
  CreatePlotForCurve *cmd = new CreatePlotForCurve();
  cmd->createItem();

  PlotItem *plotItem = static_cast<PlotItem*>(cmd->item());
  plotItem->view()->resetPlotFontSizes(plotItem);
  plotItem->view()->configurePlotFontDefaults(plotItem); // copy plots already in window
  plotItem->view()->appendToLayout(CurvePlacement::Auto, plotItem, 0);

}

#ifndef KST_NO_SVG
void MainWindow::createSvg() {
  if (_createSvgAct->isChecked()) {
    clearDrawingMarker();
    _createSvgAct->setChecked(true);
    CreateSvgCommand *cmd = new CreateSvgCommand;
    cmd->createItem();
  } else {
    _tabWidget->currentView()->setMouseMode(View::Default);
  }
}
#endif

void MainWindow::createCurve() {
  DialogLauncher::self()->showCurveDialog();
}

void MainWindow::createPSD() {
  DialogLauncher::self()->showPowerSpectrumDialog();
}

void MainWindow::createEquation() {
  DialogLauncher::self()->showEquationDialog();
}

void MainWindow::createHistogram() {
  DialogLauncher::self()->showHistogramDialog();
}

void MainWindow::createImage() {
  DialogLauncher::self()->showImageDialog();
}

void MainWindow::createSpectogram() {
  DialogLauncher::self()->showCSDDialog();
}

void MainWindow::createEventMonitor() {
  DialogLauncher::self()->showEventMonitorDialog();
}

void MainWindow::createActions() {
  // ***************************** File actions ******************************** //
  _openAct = new QAction(tr("&Open..."), this);
  _openAct->setStatusTip(tr("Open a new session"));
  _openAct->setShortcuts(QKeySequence::Open);
  _openAct->setIcon(KstGetIcon("document-open"));
  connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));

  _saveAct = new QAction(tr("&Save"), this);
  _saveAct->setStatusTip(tr("Save the current session"));
  _saveAct->setShortcuts(QKeySequence::Save);
  _saveAct->setIcon(KstGetIcon("document-save"));
  connect(_saveAct, SIGNAL(triggered()), this, SLOT(save()));

  _saveAsAct = new QAction(tr("Save &as..."), this);
  _saveAsAct->setStatusTip(tr("Save the current session"));
  _saveAsAct->setIcon(KstGetIcon("document-save-as"));
  _saveAsAct->setShortcuts(QKeySequence::SaveAs);
  connect(_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  _closeAct = new QAction(tr("Ne&w Session"), this);
  _closeAct->setStatusTip(tr("Close current session and start a new, empty one"));
  _closeAct->setIcon(KstGetIcon("document-close"));
  _closeAct->setShortcuts(QKeySequence::Close);
  connect(_closeAct, SIGNAL(triggered()), this, SLOT(newDoc()));

  _reloadAct = new QAction(tr("Reload All &Data Sources"), this);
  _reloadAct->setStatusTip(tr("Reload all data sources"));
  _reloadAct->setIcon(KstGetIcon("kst_reload"));
  connect(_reloadAct, SIGNAL(triggered()), this, SLOT(reload()));

  _printAct = new QAction(tr("&Print..."), this);
  _printAct->setStatusTip(tr("Print the current view"));
  _printAct->setIcon(KstGetIcon("document-print"));
  _printAct->setShortcuts(QKeySequence::Print);
  connect(_printAct, SIGNAL(triggered()), this, SLOT(print()));
#ifdef KST_NO_PRINTER
  _printAct->setEnabled(false);
#endif

  _exportGraphicsAct = new QAction(tr("&Export as Image(s)..."), this);
  _exportGraphicsAct->setStatusTip(tr("Export graphics to disk"));
  _exportGraphicsAct->setIcon(KstGetIcon("insert-image"));
  connect(_exportGraphicsAct, SIGNAL(triggered()), this, SLOT(showExportGraphicsDialog()));

  _exportVectorsAct = new QAction(tr("Save &Vectors to Disk..."), this);
  _exportVectorsAct->setStatusTip(tr("Export vectors to ascii file"));
  _exportVectorsAct->setIcon(KstGetIcon("save-vectors"));
  connect(_exportVectorsAct, SIGNAL(triggered()), this, SLOT(showExportVectorsDialog()));

  _logAct = new QAction(tr("&Log Entry..."), this);
  _logAct->setStatusTip(tr("Commit a log entry"));
  _logAct->setIcon(KstGetIcon("new-log-event"));
  connect(_logAct, SIGNAL(triggered()), this, SLOT(showLogDialog()));

  _newTabAct = new QAction(tr("&New Tab"), this);
  _newTabAct->setStatusTip(tr("Create a new tab"));
  _newTabAct->setIcon(KstGetIcon("tab-new"));

  connect(_newTabAct, SIGNAL(triggered()), tabWidget(), SLOT(createView()));

  _closeTabAct = new QAction(tr("&Close Tab"), this);
  _closeTabAct->setStatusTip(tr("Close the current tab"));
  _closeTabAct->setIcon(KstGetIcon("tab-close"));
  connect(_closeTabAct, SIGNAL(triggered()), tabWidget(), SLOT(closeCurrentView()));

  _exitAct = new QAction(tr("E&xit"), this);
  _exitAct->setShortcuts(QKeySequence::Quit);
  _exitAct->setStatusTip(tr("Exit the application"));
  _exitAct->setIcon(KstGetIcon("application-exit"));
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  // ************************** Edit Actions ******************************* //
  _undoAct = _undoGroup->createUndoAction(this);
  _undoAct->setShortcuts(QKeySequence::Undo);
  _undoAct->setIcon(KstGetIcon("edit-undo"));

  _redoAct = _undoGroup->createRedoAction(this);
  _redoAct->setShortcuts(QKeySequence::Redo);
  _redoAct->setIcon(KstGetIcon("edit-redo"));

  _copyTabAct = new QAction(tr("Copy Tab Image"), this);
  _copyTabAct->setStatusTip("Copy image of the current tab to the desktop clipboard");
  _copyTabAct->setIcon(KstGetIcon("edit-copy"));
  connect(_copyTabAct, SIGNAL(triggered()), this, SLOT(copyTab()));

  // ********************* View Actions ********************************** //
  _vectorEditorAct = new QAction(tr("&Vectors"), this);
  _vectorEditorAct->setStatusTip(tr("Show all vectors in a spreadsheet"));
  connect(_vectorEditorAct, SIGNAL(triggered()), this, SLOT(showVectorEditor()));

  _scalarEditorAct = new QAction(tr("&Scalars"), this);
  _scalarEditorAct->setStatusTip(tr("Show all scalars in a spreadsheet"));
  connect(_scalarEditorAct, SIGNAL(triggered()), this, SLOT(showScalarEditor()));

  _matrixEditorAct = new QAction(tr("&Matrices"), this);
  _matrixEditorAct->setStatusTip(tr("Show all matrices in a spreadsheet"));
  connect(_matrixEditorAct, SIGNAL(triggered()), this, SLOT(showMatrixEditor()));

  _stringEditorAct = new QAction(tr("S&trings"), this);
  _stringEditorAct->setStatusTip(tr("Show all strings in a spreadsheet"));
  connect(_stringEditorAct, SIGNAL(triggered()), this, SLOT(showStringEditor()));

  // ************************ Data Range 1 click Actions ************************** //
  _backAct = new QAction(tr("&Back One Screen"), this);
  _backAct->setStatusTip(tr("Back one screen (Page Up)"));
  _backAct->setIcon(KstGetIcon("page-previous"));
  _backAct->setShortcut(Qt::Key_PageUp);
  connect(_backAct, SIGNAL(triggered()), this, SLOT(back()));

  _forwardAct = new QAction(tr("&Forward One Screen"), this);
  _forwardAct->setStatusTip(tr("Forward one screen (Page Down)"));
  _forwardAct->setIcon(KstGetIcon("page-next"));
  _forwardAct->setShortcut(Qt::Key_PageDown);
  connect(_forwardAct, SIGNAL(triggered()), this, SLOT(forward()));

  _readFromEndAct = new QAction(tr("&Count from End"), this);
  _readFromEndAct->setStatusTip(tr("Count from end mode (End)"));
  _readFromEndAct->setIcon(KstGetIcon("count-from-end"));
  _readFromEndAct->setShortcut(Qt::Key_End);
  connect(_readFromEndAct, SIGNAL(triggered()), this, SLOT(readFromEnd()));

  _readToEndAct = new QAction(tr("&Read to End"), this);
  _readToEndAct->setStatusTip(tr("Read to end mode"));
  _readToEndAct->setIcon(KstGetIcon("read-to-end"));
  _readToEndAct->setShortcut(Qt::ShiftModifier+Qt::Key_End);
  connect(_readToEndAct, SIGNAL(triggered()), this, SLOT(readToEnd()));

  _pauseAct = new QAction(tr("&Pause"), this);
  _pauseAct->setIcon(KstGetIcon("pause"));
  _pauseAct->setCheckable(true);
  _pauseAct->setShortcut(tr("p", "shortcut for pause"));
  _pauseAct->setStatusTip(tr("Toggle pause updates of data sources (%1)").arg(_pauseAct->shortcut().toString()));
  connect(_pauseAct, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));

  _changeDataSampleDialogAct = new QAction(tr("Change Data &Sample Range..."), this);
  _changeDataSampleDialogAct->setStatusTip(tr("Show Kst's Change Data Sample Range Dialog"));
  _changeDataSampleDialogAct->setIcon(KstGetIcon("kst_changenpts"));
  connect(_changeDataSampleDialogAct, SIGNAL(triggered()), this, SLOT(showChangeDataSampleDialog()));

  // ************************ Create Actions ************************** //
  _insertPlotAct = new QAction(tr("&Plot"), this);
  _insertPlotAct->setIcon(KstGetIcon("kst_newplot"));
  _insertPlotAct->setShortcut(QString("F11"));
  _insertPlotAct->setStatusTip(tr("Insert a plot in the current view (%1)").arg(_insertPlotAct->shortcut().toString()));
  _insertPlotAct->setCheckable(false);
  connect(_insertPlotAct, SIGNAL(triggered()), this, SLOT(insertPlot()));

  _createPlotAct = new QAction(tr("&Plot"), this);
  _createPlotAct->setIcon(KstGetIcon("kst_newplot"));
  //_createPlotAct->setShortcut(QString("F11"));
  _createPlotAct->setStatusTip(tr("Create a plot for the current view"));
  _createPlotAct->setCheckable(true);
  connect(_createPlotAct, SIGNAL(triggered()), this, SLOT(createPlot()));

  _newScalarAct = new QAction(tr("&Scalar"), this);
  connect(_newScalarAct, SIGNAL(triggered()), this, SLOT(createScalar()));

  _newVectorAct = new QAction(tr("&Vector"), this);
  connect(_newVectorAct, SIGNAL(triggered()), this, SLOT(createVector()));

  _newMatrixAct = new QAction(tr("&Matrix"), this);
  connect(_newMatrixAct, SIGNAL(triggered()), this, SLOT(createMatrix()));

  _newStringAct = new QAction(tr("Strin&g"), this);
  connect(_newStringAct, SIGNAL(triggered()), this, SLOT(createString()));

  _newCurveAct = new QAction(tr("&Curve"), this);
  connect(_newCurveAct, SIGNAL(triggered()), this, SLOT(createCurve()));

  _newEquationAct = new QAction(tr("&Equation"), this);
  connect(_newEquationAct, SIGNAL(triggered()), this, SLOT(createEquation()));

  _newPSDAct = new QAction(tr("Po&wer Spectrum"), this);
  connect(_newPSDAct, SIGNAL(triggered()), this, SLOT(createPSD()));

  _newHistogramAct = new QAction(tr("&Histogram"), this);
  connect(_newHistogramAct, SIGNAL(triggered()), this, SLOT(createHistogram()));

  _newImageAct = new QAction(tr("&Image"), this);
  connect(_newImageAct, SIGNAL(triggered()), this, SLOT(createImage()));

  _newSpectrogramAct = new QAction(tr("Spectr&ogram"), this);
  connect(_newSpectrogramAct, SIGNAL(triggered()), this, SLOT(createSpectogram()));

  _newEventMonitorAct = new QAction(tr("Eve&nt Monitor"), this);
  connect(_newEventMonitorAct, SIGNAL(triggered()), this, SLOT(createEventMonitor()));

  // Advanced layout
  _createLabelAct = new QAction(tr("&Label"), this);
  _createLabelAct->setIcon(KstGetIcon("kst_gfx_label"));
  _createLabelAct->setShortcut(QString("F3"));
  _createLabelAct->setStatusTip(tr("Create a label for the current view (%1)").arg(_createLabelAct->shortcut().toString()));
  _createLabelAct->setCheckable(true);
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createBoxAct = new QAction(tr("&Box"), this);
  _createBoxAct->setIcon(KstGetIcon("kst_gfx_rectangle"));
  _createBoxAct->setShortcut(QString("F4"));
  _createBoxAct->setStatusTip(tr("Create a box for the current view (%1)").arg(_createBoxAct->shortcut().toString()));
  _createBoxAct->setCheckable(true);
  connect(_createBoxAct, SIGNAL(triggered()), this, SLOT(createBox()));

  _createCircleAct = new QAction(tr("&Circle"), this);
  _createCircleAct->setIcon(KstGetIcon("kst_gfx_circle"));
  _createCircleAct->setShortcut(QString("F5"));
  _createCircleAct->setStatusTip(tr("Create a circle for the current view (%1)").arg(_createCircleAct->shortcut().toString()));
  _createCircleAct->setCheckable(true);
  connect(_createCircleAct, SIGNAL(triggered()), this, SLOT(createCircle()));

  _createEllipseAct = new QAction(tr("&Ellipse"), this);
  _createEllipseAct->setIcon(KstGetIcon("kst_gfx_ellipse"));
  _createEllipseAct->setShortcut(QString("F6"));
  _createEllipseAct->setStatusTip(tr("Create an ellipse for the current view (%1)").arg(_createEllipseAct->shortcut().toString()));
  _createEllipseAct->setCheckable(true);
  connect(_createEllipseAct, SIGNAL(triggered()), this, SLOT(createEllipse()));

  _createLineAct = new QAction(tr("&Line"), this);
  _createLineAct->setIcon(KstGetIcon("kst_gfx_line"));
  _createLineAct->setShortcut(QString("F7"));
  _createLineAct->setStatusTip(tr("Create a line for the current view (%1)").arg(_createLineAct->shortcut().toString()));
  _createLineAct->setCheckable(true);
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _createArrowAct = new QAction(tr("&Arrow"), this);
  _createArrowAct->setIcon(KstGetIcon("kst_gfx_arrow"));
  _createArrowAct->setShortcut(QString("F8"));
  _createArrowAct->setStatusTip(tr("Create a arrow for the current view (%1)").arg(_createArrowAct->shortcut().toString()));
  _createArrowAct->setCheckable(true);
  connect(_createArrowAct, SIGNAL(triggered()), this, SLOT(createArrow()));

  _createPictureAct = new QAction(tr("&Picture"), this);
  _createPictureAct->setIcon(KstGetIcon("insert-image"));
  _createPictureAct->setShortcut(QString("F9"));
  _createPictureAct->setStatusTip(tr("Create a picture for the current view (%1)").arg(_createPictureAct->shortcut().toString()));
  _createPictureAct->setCheckable(true);
  connect(_createPictureAct, SIGNAL(triggered()), this, SLOT(createPicture()));

#ifndef KST_NO_SVG
  _createSvgAct = new QAction(tr("&Svg"), this);
  _createSvgAct->setIcon(KstGetIcon("draw-bezier-curves"));
  _createSvgAct->setShortcut(QString("F10"));
  _createSvgAct->setStatusTip(tr("Create a svg for the current view (%1)").arg(_createSvgAct->shortcut().toString()));
  _createSvgAct->setCheckable(true);
  connect(_createSvgAct, SIGNAL(triggered()), this, SLOT(createSvg()));
#endif

  _createSharedAxisBoxAct = new QAction(tr("Shared Axis &Box"), this);
  _createSharedAxisBoxAct->setStatusTip(tr("Create a shared axis box for the current item"));
  _createSharedAxisBoxAct->setIcon(KstGetIcon("kst_gfx_sharedaxisbox"));
  _createSharedAxisBoxAct->setCheckable(true);
  //_createSharedAxisBoxAct->setEnabled(false);
  connect(_createSharedAxisBoxAct, SIGNAL(triggered()), this, SLOT(createSharedAxisBox()));

  // ************************** Mode Actions ******************************* //
  // First, general options
  _tiedZoomAct = new QAction(tr("&Toggle Tied Zoom"), this);
  _tiedZoomAct->setIcon(KstGetIcon("tied-zoom"));
  _tiedZoomAct->setCheckable(false);
  _tiedZoomAct->setStatusTip(tr("Toggle the current view's tied zoom (%1)").arg(_tiedZoomAct->shortcut().toString()));
  connect(_tiedZoomAct, SIGNAL(triggered()), this, SLOT(toggleTiedZoom()));

  _tabTiedAct = new QAction(tr("Tie &Across All Tabs"), this);
  _tabTiedAct->setStatusTip(tr("Tied zoom applies between tabs"));
  //_tiedZoomAct->setIcon(KstGetIcon("tied-zoom"));
  _tabTiedAct->setCheckable(true);
  //connect(_tiedZoomAct, SIGNAL(triggered()), this, SLOT(toggleTiedZoom()));


  _highlightPointAct = new QAction(tr("&Highlight Data Points"), this);
  _highlightPointAct->setStatusTip(tr("Highlight closest data point"));
  _highlightPointAct->setIcon(KstGetIcon("kst_datamode"));
  _highlightPointAct->setCheckable(true);
  connect(_highlightPointAct, SIGNAL(toggled(bool)), this, SLOT(setHighlightPoint(bool)));


  // Then, exclusive interaction modes
  QActionGroup* _interactionModeGroup = new QActionGroup(this);

  _standardZoomAct = _interactionModeGroup->addAction(tr("X-Y &Zoom/Scroll"));
  _standardZoomAct->setStatusTip(tr("Zoom arbitrarily in X- or Y-direction"));
  //TODO _standardZoomAct->setShortcut(QString("a"));
  _standardZoomAct->setCheckable(true);
  _standardZoomAct->setData(View::ZoomOnlyDisabled);
  _standardZoomAct->setIcon(KstGetIcon("xy-zoom"));

  _xOnlyZoomAct = _interactionModeGroup->addAction(tr("&X-only Zoom"));
  _xOnlyZoomAct->setStatusTip(tr("Zoom only in X direction"));
  //TODO _xOnlyZoomAct->setShortcut(QString("x"));
  _xOnlyZoomAct->setCheckable(true);
  _xOnlyZoomAct->setData(View::ZoomOnlyX);
  _xOnlyZoomAct->setIcon(KstGetIcon("x-zoom"));

  _yOnlyZoomAct = _interactionModeGroup->addAction(tr("&Y-only Zoom"));
  _yOnlyZoomAct->setStatusTip(tr("Zoom only in X direction"));
  //TODO _yOnlyZoomAct->setShortcut(QString("y"));
  _yOnlyZoomAct->setData(View::ZoomOnlyY);
  _yOnlyZoomAct->setCheckable(true);
  _yOnlyZoomAct->setIcon(KstGetIcon("y-zoom"));

  _layoutModeAct = _interactionModeGroup->addAction(tr("&Layout Mode"));
  _layoutModeAct->setIcon(KstGetIcon("transform-move"));
  _layoutModeAct->setCheckable(true);
  _layoutModeAct->setShortcut(QString("F2"));
  _layoutModeAct->setStatusTip(tr("Toggle the current view's layout mode (%1)").arg(_layoutModeAct->shortcut().toString()));
  connect(_layoutModeAct, SIGNAL(toggled(bool)), this, SLOT(setLayoutMode(bool)));

  _interactionModeGroup->setExclusive(true);
  connect(_interactionModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(changeZoomOnlyMode(QAction*)));

  // *********************** Tools actions ************************************** //
  _dataManagerAct = new QAction(tr("Data &Manager"), this);
  _dataManagerAct->setIcon(KstGetIcon("data-manager"));
  _dataManagerAct->setShortcut(QString("d"));
  _dataManagerAct->setStatusTip(tr("Show Kst's data manager window (%1)").arg(_dataManagerAct->shortcut().toString()));
  connect(_dataManagerAct, SIGNAL(triggered()), this, SLOT(showDataManager()));

  _dataWizardAct = new QAction(tr("&Data Wizard"), this);
  _dataWizardAct->setIcon(KstGetIcon("tools-wizard"));
  _dataWizardAct->setShortcut(QString("w"));
  _dataWizardAct->setStatusTip(tr("Show Kst's Data Wizard (%1)").arg(_dataWizardAct->shortcut().toString()));
  connect(_dataWizardAct, SIGNAL(triggered()), this, SLOT(showDataWizard()));

  _changeFileDialogAct = new QAction(tr("Change Data &File"), this);
  _changeFileDialogAct->setIcon(KstGetIcon("change-data-file"));
  _changeFileDialogAct->setStatusTip(tr("Show Kst's Change Data File Dialog (%1)").arg(_changeFileDialogAct->shortcut().toString()));
  connect(_changeFileDialogAct, SIGNAL(triggered()), this, SLOT(showChangeFileDialog()));

  _chooseColorDialogAct = new QAction(tr("Assign Curve &Color per File"), this);
  _chooseColorDialogAct->setStatusTip(tr("Show Kst's Choose Color Dialog"));
  _chooseColorDialogAct->setIcon(KstGetIcon("code-class"));
  connect(_chooseColorDialogAct, SIGNAL(triggered()), this, SLOT(showChooseColorDialog()));

  _differentiateCurvesDialogAct = new QAction(tr("D&ifferentiate Curves"), this);
  _differentiateCurvesDialogAct->setStatusTip(tr("Show Kst's Differentiate Curves Dialog"));
  _differentiateCurvesDialogAct->setIcon(KstGetIcon("kst_differentiatecurves"));
  connect(_differentiateCurvesDialogAct, SIGNAL(triggered()), this, SLOT(showDifferentiateCurvesDialog()));

  _filterMultipleDialogAct = new QAction(tr("&Filter Vectors"), this);
  _filterMultipleDialogAct->setStatusTip(tr("Filter multiple vectors"));
  _filterMultipleDialogAct->setIcon(KstGetIcon("kst_filter_vectors"));
  connect(_filterMultipleDialogAct, SIGNAL(triggered()), this, SLOT(filterMultipleDialog()));

  // *********************** Settings actions ************************************** //
  _settingsDialogAct = new QAction(tr("&Configure Kst"), this);
  _settingsDialogAct->setStatusTip(tr("Show Kst's Configuration Dialog"));
  _settingsDialogAct->setIcon(KstGetIcon("configure"));
  connect(_settingsDialogAct, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

  _themeDialogAct = new QAction(tr("&Theme"), this);
  _themeDialogAct->setStatusTip(tr("Show Kst's Theme Dialog"));
  _themeDialogAct->setIcon(KstGetIcon("themes"));
  connect(_themeDialogAct, SIGNAL(triggered()), this, SLOT(showThemeDialog()));

  _clearUISettings =  new QAction(tr("Clear settings and defaults"), this);
  _clearUISettings->setStatusTip(tr("Clear sticky defaults and all settings in all dialogs."));
  //_clearUISettings->setIcon(KstGetIcon("configure"));  // FIXME: pick an icon (broom?)
  connect(_clearUISettings, SIGNAL(triggered()), this, SLOT(clearDefaults()));

  _clearDataSourceSettings =  new QAction(tr("Clear datasource settings"), this);
  _clearDataSourceSettings->setStatusTip(tr("Clear datasource settings"));
  //_clearDataSourceSettings->setIcon(KstGetIcon("configure"));   // FIXME: pick an icon (broom?)
  connect(_clearDataSourceSettings, SIGNAL(triggered()), this, SLOT(clearDataSourceSettings()));

  // *********************** Help actions ************************************** //
  _debugDialogAct = new QAction(tr("&Debug Dialog"), this);
  _debugDialogAct->setStatusTip(tr("Show the Kst debugging dialog"));
  _debugDialogAct->setIcon(KstGetIcon("text-x-log"));
  connect(_debugDialogAct, SIGNAL(triggered()), this, SLOT(showDebugDialog()));

  _bugReportWizardAct = new QAction(tr("&Bug Report Wizard"), this);
  _bugReportWizardAct->setStatusTip(tr("Show Kst's Bug Report Wizard"));
  _bugReportWizardAct->setIcon(KstGetIcon("kbugbuster"));
  connect(_bugReportWizardAct, SIGNAL(triggered()), this, SLOT(showBugReportWizard()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  _aboutAct->setIcon(KstGetIcon("dialog-information"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _video1Act = new QAction(tr("#&1: Quick Start"), this);
  _video1Act->setStatusTip(tr("Kst presentation #1: The shortest tutorial to the fastest plotting tool"));
  connect(_video1Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video1Act, 1);
  _video2Act = new QAction(tr("#&2: General UI Presentation"), this);
  _video2Act->setStatusTip(tr("Kst presentation #2: General presentation of the user interface and most important concepts"));
  connect(_video2Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video2Act, 2);
  _video3Act = new QAction(tr("#&3: Live Data / Range Tools"), this);
  _video3Act->setStatusTip(tr("Kst presentation #3: Range tools and live plots with streaming data"));
  connect(_video3Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video3Act, 3);
  _video4Act = new QAction(tr("#&4: FFT, Equations, Fits, Filters"), this);
  _video4Act->setStatusTip(tr("Kst presentation #4: FFTs, equations, filters, fits, plugins"));
  connect(_video4Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video4Act, 4);
  _video5Act = new QAction(tr("#&5: Productivity Features"), this);
  _video5Act->setStatusTip(tr("Kst presentation #5: Unique productivity features like edit multiple mode and change data file tool"));
  connect(_video5Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video5Act, 5);
  _video6Act = new QAction(tr("#&6: Advanced Layout / Export"), this);
  _video6Act->setStatusTip(tr("Kst presentation #6: Advanced layout and export options"));
  connect(_video6Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video6Act, 6);
  _video7Act = new QAction(tr("#&7: Matrices / Images / Metadata"), this);
  _video7Act->setStatusTip(tr("Kst presentation #7: Matrices, images and metadata"));
  connect(_video7Act, SIGNAL(triggered()), _videoMapper, SLOT(map()));
  _videoMapper->setMapping(_video7Act, 7);
  connect(_videoMapper, SIGNAL(mapped(int)), this, SLOT(videoTutorial(int)));
}


void MainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));
  // Session operations
  _fileMenu->addAction(_openAct);
  _fileMenu->addAction(_saveAct);
  _fileMenu->addAction(_saveAsAct);
  _fileMenu->addAction(_closeAct);
  _fileMenu->addSeparator();
  _recentKstFilesMenu = _fileMenu->addMenu(tr("&Recent Sessions"));
  _recentDataFilesMenu = _fileMenu->addMenu(tr("Recent Data &Files"));
  _fileMenu->addSeparator();
  // Reload, isolate it a bit from the other entries to avoid inadvertent triggering
  _fileMenu->addAction(_reloadAct);
  _fileMenu->addSeparator();
  // Print/export
  _fileMenu->addAction(_printAct);
  _fileMenu->addAction(_exportGraphicsAct);
  _fileMenu->addAction(_exportVectorsAct);
  _fileMenu->addAction(_logAct);
  _fileMenu->addSeparator();
  // Tabs
  _fileMenu->addAction(_newTabAct);
  _fileMenu->addAction(_closeTabAct);
  _fileMenu->addSeparator();
  // exit
  _fileMenu->addAction(_exitAct);
  // recent files
  _fileMenu->addSeparator();
  updateRecentKstFiles();


  _editMenu = menuBar()->addMenu(tr("&Edit"));
  _editMenu->addAction(_undoAct);
  _editMenu->addAction(_redoAct);
  _editMenu->addAction(_copyTabAct);
  // Cut/Copy/Paste will come here

  _viewMenu = menuBar()->addMenu(tr("&View"));
  _viewMenu->addAction(_vectorEditorAct);
  _viewMenu->addAction(_scalarEditorAct);
  _viewMenu->addAction(_matrixEditorAct);
  _viewMenu->addAction(_stringEditorAct);

  _rangeMenu = menuBar()->addMenu(tr("&Range"));
  _rangeMenu->addAction(_backAct);
  _rangeMenu->addAction(_forwardAct);
  _rangeMenu->addSeparator();
  _rangeMenu->addAction(_readFromEndAct);
  _rangeMenu->addAction(_readToEndAct);
  _rangeMenu->addAction(_pauseAct);
  _rangeMenu->addSeparator();
  _rangeMenu->addAction(_changeDataSampleDialogAct);

  _createMenu = menuBar()->addMenu(tr("&Create"));
  // Containers
  _createMenu->addAction(_insertPlotAct);
  //_createMenu->addAction(_createPlotAct);
  _createMenu->addAction(_createSharedAxisBoxAct);
  _createMenu->addSeparator();
  // Primitives
  _createMenu->addAction(_newScalarAct);
  _createMenu->addAction(_newVectorAct);
  _createMenu->addAction(_newMatrixAct);
  _createMenu->addAction(_newStringAct);
  _createMenu->addSeparator();
  // Data objects
  _createMenu->addAction(_newEquationAct);
  _createMenu->addAction(_newHistogramAct);
  _createMenu->addAction(_newPSDAct);
  _createMenu->addAction(_newSpectrogramAct);
  _createMenu->addAction(_newEventMonitorAct);
  _createMenu->addSeparator();

  // Now, create the dynamic plugin menus
  QMenu* _pluginsMenu = _createMenu->addMenu(tr("Standard P&lugin"));
  QMenu* _fitPluginsMenu = _createMenu->addMenu(tr("Fit Pl&ugin"));
  QMenu* _filterPluginsMenu = _createMenu->addMenu(tr("Fil&ter Plugin"));
  PluginMenuItemAction* action;
  foreach (const QString &pluginName, DataObject::dataObjectPluginList()) {
    action = new PluginMenuItemAction(pluginName, this);
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _pluginsMenu->addAction(action);
  }
  foreach (const QString &pluginName, DataObject::fitsPluginList()) {
    action = new PluginMenuItemAction(pluginName, this);
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _fitPluginsMenu->addAction(action);
  }
  foreach (const QString &pluginName, DataObject::filterPluginList()) {
    action = new PluginMenuItemAction(pluginName, this);
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _filterPluginsMenu->addAction(action);
  }
  _createMenu->addSeparator();
  // Relations
  _createMenu->addAction(_newCurveAct);
  _createMenu->addAction(_newImageAct);
  _createMenu->addSeparator();
  // Annotation objects
  QMenu* annotations = _createMenu->addMenu(tr("&Annotation"));
  annotations->addAction(_createLabelAct);
  annotations->addAction(_createBoxAct);
  annotations->addAction(_createCircleAct);
  annotations->addAction(_createEllipseAct);
  annotations->addAction(_createLineAct);
  annotations->addAction(_createArrowAct);
  annotations->addAction(_createPictureAct);
#ifndef KST_NO_SVG
  annotations->addAction(_createSvgAct);
#endif

  _modeMenu = menuBar()->addMenu(tr("&Mode"));
  // Interaction mode
  _modeMenu->addSeparator()->setText(tr("Interaction mode"));
  _modeMenu->addAction(_standardZoomAct);
  _modeMenu->addAction(_xOnlyZoomAct);
  _modeMenu->addAction(_yOnlyZoomAct);
  _modeMenu->addAction(_layoutModeAct);
  _standardZoomAct->setChecked(true);
  _modeMenu->addSeparator();
  // Tied zoom options
  _modeMenu->addAction(_tiedZoomAct);
  _modeMenu->addAction(_tabTiedAct);
  _modeMenu->addSeparator();
  // Data mode
  _modeMenu->addAction(_highlightPointAct);

  _toolsMenu = menuBar()->addMenu(tr("&Tools"));
  _toolsMenu->addAction(_dataManagerAct);
  _toolsMenu->addAction(_dataWizardAct);
  _toolsMenu->addAction(_changeFileDialogAct);
  _toolsMenu->addAction(_chooseColorDialogAct);
  _toolsMenu->addAction(_differentiateCurvesDialogAct);
  _toolsMenu->addAction(_filterMultipleDialogAct);
  _toolsMenu->addSeparator();
  updateRecentDataFiles();

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));
  _settingsMenu->addAction(_settingsDialogAct);
  _settingsMenu->addAction(_themeDialogAct);
  _settingsMenu->addAction(_clearUISettings);
  _settingsMenu->addAction(_clearDataSourceSettings);

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  QMenu* _videoTutorialsMenu = _helpMenu->addMenu(tr("&Video tutorials"));
  _videoTutorialsMenu->addAction(_video1Act);
  _videoTutorialsMenu->addAction(_video2Act);
  _videoTutorialsMenu->addAction(_video3Act);
  _videoTutorialsMenu->addAction(_video4Act);
  _videoTutorialsMenu->addAction(_video5Act);
  _videoTutorialsMenu->addAction(_video6Act);
  _videoTutorialsMenu->addAction(_video7Act);
  _helpMenu->addSeparator();
  _helpMenu->addAction(_debugDialogAct);
  _helpMenu->addAction(_bugReportWizardAct);
  _helpMenu->addSeparator();
  _helpMenu->addAction(_aboutAct);

}


void MainWindow::createToolBars() {
  setToolButtonStyle(Qt::ToolButtonIconOnly);

  _fileToolBar = addToolBar(tr("File"));
  _fileToolBar->setObjectName("File Toolbar");
  _fileToolBar->addAction(_openAct);
  _fileToolBar->addAction(_saveAct);
  _fileToolBar->addAction(_reloadAct);
  _fileToolBar->addAction(_printAct);
  _fileToolBar->addAction(_logAct);

  _editToolBar = addToolBar(tr("Edit"));
  _editToolBar->setObjectName("Edit Toolbar");
  _editToolBar->addAction(_undoAct);
  _editToolBar->addAction(_redoAct);
  _editToolBar->addAction(_copyTabAct);

  _toolsToolBar = addToolBar(tr("Tools"));
  _toolsToolBar->setObjectName("Tools Toolbar");
  _toolsToolBar->addAction(_dataManagerAct);
  _toolsToolBar->addAction(_dataWizardAct);
  _toolsToolBar->addAction(_changeDataSampleDialogAct);
  _toolsToolBar->addAction(_changeFileDialogAct);

  _rangeToolBar = addToolBar(tr("Data Range"));
  _rangeToolBar->setObjectName("Data Range Toolbar");
  _rangeToolBar->addAction(_backAct);
  _rangeToolBar->addAction(_forwardAct);
  _rangeToolBar->addAction(_readFromEndAct);
  _rangeToolBar->addAction(_readToEndAct);
  _rangeToolBar->addAction(_pauseAct);

  _modeToolBar = addToolBar(tr("Mode"));
  _modeToolBar->setObjectName("Mode Toolbar");
  _modeToolBar->addAction(_tiedZoomAct);
  _modeToolBar->addAction(_highlightPointAct);
  _modeToolBar->addSeparator();
  _modeToolBar->addAction(_standardZoomAct);
  _modeToolBar->addAction(_xOnlyZoomAct);
  _modeToolBar->addAction(_yOnlyZoomAct);
  _modeToolBar->addAction(_layoutModeAct);

  _plotLayoutToolBar = addToolBar(tr("Plot Layout"));
  _plotLayoutToolBar->setObjectName("Plot Layout Toolbar");
  _plotLayoutToolBar->addAction(_createSharedAxisBoxAct);
  _plotLayoutToolBar->addAction(_insertPlotAct);
  //_plotLayoutToolBar->addAction(_createPlotAct);

  _annotationToolBar = addToolBar(tr("Advanced Layout"));
  _annotationToolBar->setObjectName("Advanced Layout Toolbar");
  _annotationToolBar->addAction(_createLabelAct);
  _annotationToolBar->addAction(_createBoxAct);
  _annotationToolBar->addAction(_createCircleAct);
  _annotationToolBar->addAction(_createEllipseAct);
  _annotationToolBar->addAction(_createLineAct);
  _annotationToolBar->addAction(_createArrowAct);
  _annotationToolBar->addAction(_createPictureAct);
#ifndef KST_NO_SVG
  _annotationToolBar->addAction(_createSvgAct);
#endif
}


void MainWindow::createStatusBar() {
  _messageLabel = new QLabel(statusBar());
  setStatusMessage(tr("Ready"), 3000);

  _progressBar = new QProgressBar(statusBar());
  _progressBar->setFixedWidth(200);
  _progressBar->hide();
  statusBar()->addPermanentWidget(_progressBar);
  MemoryWidget *mw = new MemoryWidget(statusBar());
  statusBar()->addPermanentWidget(mw);
  DebugNotifier *dn = new DebugNotifier(statusBar());
  connect(dn, SIGNAL(showDebugLog()), this, SLOT(showDebugDialog()));
  connect(_debugDialog, SIGNAL(notifyOfError()), dn, SLOT(reanimate()));
  connect(_debugDialog, SIGNAL(notifyAllClear()), dn, SLOT(close()));
  statusBar()->addPermanentWidget(dn);
}

/** set the status bar message.  If you are doing this inside a view
 * object paint() call, then set delayed to true, and call ::updateStatusMessage()
 * later (after leaving paint). This is currently done for you in updateViewItems */
void MainWindow::setStatusMessage(QString message, int timeout, bool delayed) {
  _statusBarMessage = message;
  _statusBarTimeout = timeout;
  if (!delayed) {
    statusBar()->showMessage(message, timeout);
  }
}

void MainWindow::videoTutorial(int i) {
  switch (i) {
  case 1:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=d1uz5g_cWh4")); break;
  case 2:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=6z2bGNrgwL0")); break;
  case 3:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=dstOQpmfY1U")); break;
  case 4:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=YJ54X0nKnmk")); break;
  case 5:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=3YZVC-GiS_4")); break;
  case 6:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=rI7nYHlz6rs")); break;
  case 7:
    QDesktopServices::openUrl(QUrl("http://www.youtube.com/watch?v=mgP24MryyKw")); break;
  default:
    break;
  }
}

QString MainWindow::statusMessage() {
  return statusBar()->currentMessage();
}

void MainWindow::updateStatusMessage() {
  statusBar()->showMessage(_statusBarMessage, _statusBarTimeout);
}

QProgressBar *MainWindow::progressBar() const {
  return _progressBar;
}

void MainWindow::updateProgress(int percent, const QString& message)
{
  if (percent == -1) {
    _progressBar->setMaximum(0);
    _progressBar->setMinimum(0);
    _progressBar->show();
    return;
  }

  if (percent  > 0 && percent < 100) {
    _progressBar->setMaximum(100);
    _progressBar->setValue(percent);
    _progressBar->show();
  } else {
    _progressBar->hide();
  }
  setStatusMessage(message);
}

void MainWindow::readFromEnd() {
  int nf = 0;
  int skip;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();

  foreach (DataVectorPtr v, dataVectors) {
    v->readLock();
    nf = v->reqNumFrames();
    if (nf <=0 ) {
      nf = v->numFrames();
    }
    skip = v->skip();
    do_skip = v->doSkip();
    do_filter = v->doAve();
    v->unlock();

    v->writeLock();
    v->changeFrames(-1, nf, skip, do_skip, do_filter);
    v->registerChange();
    v->unlock();
  }

  DataMatrixList dataMatrices = document()->objectStore()->getObjects<DataMatrix>();
  foreach (DataMatrixPtr m, dataMatrices) {
    m->readLock();
    m->setFrame(-1);
    m->registerChange();
    m->unlock();
  }

  UpdateManager::self()->doUpdates(true);
  dialogDefaults().setValue("vector/range", nf);
  dialogDefaults().setValue("vector/countFromEnd", true);
  dialogDefaults().setValue("vector/readToEnd", false);
}

void MainWindow::readToEnd() {
  int f0 = 0;
  int skip;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();

  foreach (DataVectorPtr v, dataVectors) {
    v->readLock();
    f0 = v->startFrame();
    skip = v->skip();
    do_skip = v->doSkip();
    do_filter = v->doAve();
    v->unlock();

    v->writeLock();
    v->changeFrames(f0, -1, skip, do_skip, do_filter);
    v->registerChange();
    v->unlock();
  }

  DataMatrixList dataMatrices = document()->objectStore()->getObjects<DataMatrix>();
  foreach (DataMatrixPtr m, dataMatrices) {
    m->readLock();
    m->setFrame(-1);
    m->registerChange();
    m->unlock();
  }

  dialogDefaults().setValue("vector/start", f0);
  dialogDefaults().setValue("vector/countFromEnd", false);
  dialogDefaults().setValue("vector/readToEnd", true);
  UpdateManager::self()->doUpdates(true);
}

void MainWindow::pause(bool pause) {
  UpdateManager::self()->setPaused(pause);

  //if (!pause) {
     //foreach (DataSourcePtr s, document()->objectStore()->dataSourceList()) {
       //s->checkUpdate();
     //}
  //}
}

void MainWindow::forward() {
  int f0 = 0;
  int nf = 0;
  int lastF = -1;
  int skip;
  int filelength;
  bool count_from_end;
  bool read_to_end;
  bool do_skip;
  bool do_filter;

  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();
  DataMatrixList dataMatrices = document()->objectStore()->getObjects<DataMatrix>();

  QHash<int,int> lastframehash;

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
      lastF = f0 + nf - 1;
      ++lastframehash[lastF];

      v->writeLock();
      v->changeFrames(f0, nf, skip, do_skip, do_filter);
      v->registerChange();
      v->unlock();
    }
  }

  int most_popular_lastF = -1;
  int n_most_popular_lastF = -1;
  foreach (int key, lastframehash.keys()) {
    int n = lastframehash[key] ;
    if (n > n_most_popular_lastF) {
      n_most_popular_lastF = n;
      most_popular_lastF = key;
    }
  }

  foreach (DataMatrixPtr m, dataMatrices) {
    if (m->hasStream()) {
      int  new_frame;
      int filelength = m->fileLength();
      if (most_popular_lastF>=0) {
        if (most_popular_lastF < filelength) {
          new_frame = most_popular_lastF;
        } else {
          new_frame = filelength-1;
        }
      } else {
        if (m->frame()+1 < filelength) {
          new_frame = m->frame()+1;
        } else {
          new_frame = filelength-1;
        }
      }
      m->writeLock();
      m->setFrame(new_frame);
      m->registerChange();
      m->unlock();
    }
  }

  dialogDefaults().setValue("vector/range", nf);
  dialogDefaults().setValue("vector/start", f0);
  dialogDefaults().setValue("vector/countFromEnd", false);
  dialogDefaults().setValue("vector/readToEnd", false);

  UpdateManager::self()->doUpdates(true);
}

void MainWindow::back() {
  int f0 = 0;
  int nf = 0;
  int skip;
  int filelength;
  bool count_from_end;
  bool read_to_end;
  bool do_skip;
  bool do_filter;
  int lastF = -1;


  DataVectorList dataVectors = document()->objectStore()->getObjects<DataVector>();
  DataMatrixList dataMatrices = document()->objectStore()->getObjects<DataMatrix>();

  QHash<int,int> lastframehash;

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
      lastF = f0 + nf - 1;
      ++lastframehash[lastF];

      v->writeLock();
      v->changeFrames(f0, nf, skip, do_skip, do_filter);
      v->registerChange();
      v->unlock();
    }
  }

  int most_popular_lastF = -1;
  int n_most_popular_lastF = -1;
  foreach (int key, lastframehash.keys()) {
    int n = lastframehash[key] ;
    if (n > n_most_popular_lastF) {
      n_most_popular_lastF = n;
      most_popular_lastF = key;
    }
  }

  foreach (DataMatrixPtr m, dataMatrices) {
    if (m->hasStream()) {
      int  new_frame;
      int filelength = m->fileLength();
      if (most_popular_lastF>=0) {
        if (most_popular_lastF < filelength) {
          new_frame = most_popular_lastF;
        } else {
          new_frame = filelength-1;
        }
      } else {
        if (m->frame()-1 >= 0) {
          new_frame = m->frame()-1;
        } else {
          new_frame = filelength-2;
        }
      }
      m->writeLock();
      m->setFrame(new_frame);
      m->registerChange();
      m->unlock();
    }
  }

  dialogDefaults().setValue("vector/range", nf);
  dialogDefaults().setValue("vector/start", f0);
  dialogDefaults().setValue("vector/countFromEnd", false);
  dialogDefaults().setValue("vector/readToEnd", false);
  UpdateManager::self()->doUpdates(true);
}

void MainWindow::reload() {
  document()->objectStore()->rebuildDataSourceList();
  UpdateManager::self()->doUpdates(true);
  //document()->setChanged(true);
}

void MainWindow::showDataManager() {
  if (!_dataManager) {
    _dataManager = new DataManager(this, _doc);
  }
  if (_dataManager->isVisible()) {
    _dataManager->raise();
    _dataManager->activateWindow();
  }
  _dataManager->show();
}


void MainWindow::updateViewItems(qint64 serial) {

  QList<PlotItem *> plots = ViewItem::getItems<PlotItem>();

  bool changed = false;
  foreach (PlotItem *plot, plots) {
    changed |= plot->handleChangedInputs(serial);
  }

  QList<LabelItem*> labels = ViewItem::getItems<LabelItem>();
  foreach (LabelItem * label, labels) {
    if (label->_labelRc) {
      label->inputsChanged(serial);
    }
  }


  if (changed) {
    _tabWidget->currentView()->update();
    if (_viewVectorDialog) {
      _viewVectorDialog->update();
    }
    kstApp->mainWindow()->updateStatusMessage();
  }

  QTimer::singleShot(20, UpdateManager::self(), SLOT(viewItemUpdateFinished())); // why 20ms ???
}

void MainWindow::showVectorEditor() {
    if (!_viewVectorDialog) {
      _viewVectorDialog = new ViewVectorDialog(this, _doc);
    }
    if (_viewVectorDialog->isVisible()) {
      _viewVectorDialog->raise();
      _viewVectorDialog->activateWindow();
    }
    _viewVectorDialog->show();
}


void MainWindow::showScalarEditor() {
  ViewPrimitiveDialog *viewScalarDialog = new ViewScalarDialog(this, _doc);
  viewScalarDialog->show();
}


void MainWindow::showStringEditor() {
  ViewPrimitiveDialog *viewStringDialog = new ViewStringDialog(this, _doc);
  viewStringDialog->show();
}

void MainWindow::showMatrixEditor() {
  ViewMatrixDialog *viewMatrixDialog = new ViewMatrixDialog(this, _doc);
  viewMatrixDialog->show();
}


void MainWindow::showDebugDialog() {
  if (!_debugDialog) {
    _debugDialog = new DebugDialog(this);
  }
  _debugDialog->setObjectStore(document()->objectStore());
  if (_debugDialog->isVisible()) {
    _debugDialog->raise();
    _debugDialog->activateWindow();
  }
  _debugDialog->show();
}


void MainWindow::showExportGraphicsDialog() {
  if (!_exportGraphics) {
    _exportGraphics = new ExportGraphicsDialog(this);
    connect(_exportGraphics, SIGNAL(exportGraphics(QString,QString,int,int,int,bool,int)),
            this, SLOT(exportGraphicsFile(QString,QString,int,int,int,bool,int)));
  }
  if (_exportGraphics->isVisible()) {
    _exportGraphics->raise();
    _exportGraphics->activateWindow();
  }
  _exportGraphics->show();
}


void MainWindow::showExportVectorsDialog() {
  if (!_exportVectors) {
    _exportVectors = new ExportVectorsDialog(this);
  }
  if (_exportVectors->isVisible()) {
    _exportVectors->raise();
    _exportVectors->activateWindow();
  }
  _exportVectors->show();
}


void MainWindow::showLogDialog() {
  if (!_logDialog) {
    _logDialog = new LogDialog(this);
    //connect(_logDialog, SIGNAL(exportLog(const QString &, time_t, const QString &, int, int, int, const QString &)),
    //        this, SLOT(exportLog(const QString &, time_t, const QString &, int, int, int, const QString &)));
  }
  if (_logDialog->isVisible()) {
    _logDialog->raise();
    _logDialog->activateWindow();
  }
  _logDialog->show();
}


void MainWindow::showSettingsDialog() {
  if (!_applicationSettingsDialog) {
    _applicationSettingsDialog = new ApplicationSettingsDialog(this);
  }
  if (_applicationSettingsDialog->isVisible()) {
    _applicationSettingsDialog->raise();
    _applicationSettingsDialog->activateWindow();
  }
  _applicationSettingsDialog->show();
}


void MainWindow::showThemeDialog() {
  if (!_themeDialog) {
    _themeDialog = new ThemeDialog(this);
  }
  if (_themeDialog->isVisible()) {
    _themeDialog->raise();
    _themeDialog->activateWindow();
  }

  _themeDialog->reset();
  _themeDialog->show();
}


void MainWindow::clearDefaults() {

  QMessageBox confirmationBox;
  confirmationBox.setText(tr("Clear all settings and defaults?"));
  confirmationBox.setInformativeText(tr("You are about to clear all settings defaults in all dialogs in kst.\nThis can not be undone."));
  confirmationBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  confirmationBox.setDefaultButton(QMessageBox::Ok);
  int confirmed = confirmationBox.exec();

  switch (confirmed) {
  case QMessageBox::Ok:
    dialogDefaults().clear();
    ApplicationSettings::self()->clear();
    DataSourcePluginManager::settingsObject().clear();
    break;
  case QMessageBox::Cancel:
  default:
    // cancel: do nothing at all.
    break;
  }
}

void MainWindow::clearDataSourceSettings() {

  QMessageBox confirmationBox;
  confirmationBox.setText(tr("Clear datasource settings?"));
  confirmationBox.setInformativeText(tr("You are about to clear all\nconfiguration settings for\nall datasources/file types."));
  confirmationBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  confirmationBox.setDefaultButton(QMessageBox::Ok);
  int confirmed = confirmationBox.exec();

  switch (confirmed) {
  case QMessageBox::Ok:
    DataSourcePluginManager::settingsObject().clear();
    break;
  case QMessageBox::Cancel:
  default:
    // cancel: do nothing at all.
    break;
  }

}


void MainWindow::showDifferentiateCurvesDialog() {
  if (!_differentiateCurvesDialog) {
    _differentiateCurvesDialog = new DifferentiateCurvesDialog(this);
  }
  if (_differentiateCurvesDialog->isVisible()) {
    _differentiateCurvesDialog->raise();
    _differentiateCurvesDialog->activateWindow();
  }
  _differentiateCurvesDialog->show();
}


void MainWindow::filterMultipleDialog() {
  if (!_filterMultipleDialog) {
    _filterMultipleDialog = new FilterMultipleDialog(this);
  }
  if (_filterMultipleDialog->isVisible()) {
    _filterMultipleDialog->raise();
    _filterMultipleDialog->activateWindow();
  }
  _filterMultipleDialog->show();
}



void MainWindow::showChooseColorDialog() {
  if (!_chooseColorDialog) {
    _chooseColorDialog = new ChooseColorDialog(this);
  }
  if (_chooseColorDialog->isVisible()) {
    _chooseColorDialog->raise();
    _chooseColorDialog->activateWindow();
  }
  _chooseColorDialog->show();
}


void MainWindow::showChangeDataSampleDialog() {
  if (!_changeDataSampleDialog) {
    _changeDataSampleDialog = new ChangeDataSampleDialog(this);
  }
  if (_changeDataSampleDialog->isVisible()) {
    _changeDataSampleDialog->raise();
    _changeDataSampleDialog->activateWindow();
  }
  _changeDataSampleDialog->show();
}


void MainWindow::showDataWizard() {
  DataWizard *dataWizard = new DataWizard(this);
  connect(dataWizard, SIGNAL(dataSourceLoaded(QString)), this, SLOT(updateRecentDataFiles(QString)));
  dataWizard->show();
}

void MainWindow::showDataWizard(const QString &dataFile) {
  DataWizard *dataWizard = new DataWizard(this, dataFile);
  dataWizard->show();
}


void MainWindow::openRecentDataFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action) {
    DataWizard *dataWizard = new DataWizard(this, action->data().toString());
    connect(dataWizard, SIGNAL(dataSourceLoaded(QString)), this, SLOT(updateRecentDataFiles(QString)));
    dataWizard->show();
  }
}


void MainWindow::showBugReportWizard() {
  if (!_bugReportWizard) {
    _bugReportWizard = new BugReportWizard(this);
  }
  if (_bugReportWizard->isVisible()) {
    _bugReportWizard->raise();
    _bugReportWizard->activateWindow();
  }
  _bugReportWizard->show();
}

void MainWindow::showChangeFileDialog() {
  if (!_changeFileDialog) {
    _changeFileDialog = new ChangeFileDialog(this);
  }
  if (_changeFileDialog->isVisible()) {
    _changeFileDialog->raise();
    _changeFileDialog->activateWindow();
  }
  _changeFileDialog->show();
}

void MainWindow::showPluginDialog(QString &pluginName) {
  DialogLauncher::self()->showBasicPluginDialog(pluginName);
}

bool MainWindow::isTiedTabs() {
  return _tabTiedAct->isChecked();
}

void MainWindow::readSettings() {
#if defined(__QNX__) || defined(__ANDROID__)
  // There is only one size we want on mobile platforms - full screen!
  setWindowState(Qt::WindowFullScreen);
#else
  QByteArray geo = _settings.value("geometry").toByteArray();
  if (!geo.isEmpty()) {
      restoreGeometry(geo);
  } else {
      setGeometry(50, 50, 800, 600);
  }
#endif // defined(__QNX__) || defined(__ANDROID__)

  restoreState(_settings.value("toolbarState").toByteArray());
  _tabTiedAct->setChecked(_settings.value("tieTabs").toBool());
}


void MainWindow::writeSettings() {
  _settings.setValue("geometry", saveGeometry());
  _settings.setValue("toolbarState", saveState());
  _settings.setValue("tieTabs", _tabTiedAct->isChecked());
}

void MainWindow::setWidgetFlags(QWidget* widget)
{
  if (widget) {
    // Make sure the dialog gets maximize and minimize buttons under Windows
    widget->QWidget::setWindowFlags((Qt::WindowFlags) Qt::Dialog |     Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint
#if QT_VERSION >= 0x040500
        | Qt::WindowCloseButtonHint
#endif
);
  }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
     if (event->mimeData()->hasUrls()) {
       event->acceptProposedAction();
     }
}


void MainWindow::dropEvent(QDropEvent *event)
{
  QString path = event->mimeData()->urls().first().toLocalFile();
  if (path.endsWith(QString(".kst"))) {
     if (!promptSaveDone()) { // There are things to save => cancel
       event->ignore();
       return;
     }
     openFile(path);
   }
   else {
     showDataWizard(path); // This is not destructive: it only add data, no need to ask for confirmation
   }
   event->accept();
}

void MainWindow::cleanUpDataSourceList() {
  _doc->objectStore()->cleanUpDataSourceList();
}

#if defined(__QNX__)
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (!qApp->activeWindow()) {
    activateWindow();
  }
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key()==Qt::Key_Menu) { // i.e., bezel swipe gesture...
      qnxToggleToolbarVisibility();
    }
    return false;
  } else if (event->type() == QEvent::MouseButtonPress && obj != menuBar()) {
    if (_qnxToolbarsVisible) qnxToggleToolbarVisibility();
    update();
    return false;
  } else if (event->type() == QEvent::WindowActivate) {
    update();
    return false;
  } else {
    return QObject::eventFilter(obj, event);
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if ((event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(event)->key() == Qt::Key_Menu)) {
    qnxToggleToolbarVisibility();
  }
}

void MainWindow::qnxToggleToolbarVisibility() {
  if (_qnxLastToolbarEvent.msecsTo(QDateTime::currentDateTime()) < 100) {
    return; // HACK
  }
  _qnxLastToolbarEvent = QDateTime::currentDateTime();
  menuBar()->setVisible(!_qnxToolbarsVisible);
  statusBar()->setVisible(!_qnxToolbarsVisible);
  _fileToolBar->setVisible(!_qnxToolbarsVisible);
  _editToolBar->setVisible(!_qnxToolbarsVisible);
  _toolsToolBar->setVisible(!_qnxToolbarsVisible);
  _rangeToolBar->setVisible(!_qnxToolbarsVisible);
  _modeToolBar->setVisible(!_qnxToolbarsVisible);
  _plotLayoutToolBar->setVisible(!_qnxToolbarsVisible);
  _annotationToolBar->setVisible(!_qnxToolbarsVisible);
  _qnxToolbarsVisible = !_qnxToolbarsVisible;
}
#endif

}

// vim: ts=2 sw=2 et
