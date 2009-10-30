/***************************************************************************
                   datasource.cpp  -  abstract data source
                             -------------------
    begin                : Thu Oct 16 2003
    copyright            : (C) 2003 The University of Toronto
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

#include "datasource.h"

#include <assert.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QPluginLoader>
#include <QTextDocument>
#include <QUrl>
#include <QXmlStreamWriter>
#include <QTimer>
#include <QFileSystemWatcher>

#include "kst_i18n.h"
#include "datacollection.h"
#include "debug.h"
#include "objectstore.h"
#include "scalar.h"
#include "string.h"
#include "stdinsource.h"
#include "updatemanager.h"

#include "dataplugin.h"

#define DATASOURCE_UPDATE_TIMER_LENGTH 1000

namespace Kst {

const QString DataSource::staticTypeString = I18N_NOOP("Data Source");
const QString DataSource::staticTypeTag = I18N_NOOP("source");

static QSettings *settingsObject = 0L;
static QMap<QString,QString> urlMap;
void DataSource::init() {
  if (!settingsObject) {
    QSettings *settingsObj = new QSettings("kst", "data");
    settingsObject = settingsObj;
  }
  initPlugins();
}

void DataSource::_initializeShortName() {
}

static PluginList _pluginList;
void DataSource::cleanupForExit() {
  _pluginList.clear();
  delete settingsObject;
  settingsObject = 0L;
//   for (QMap<QString,QString>::Iterator i = urlMap.begin(); i != urlMap.end(); ++i) {
//     KIO::NetAccess::removeTempFile(i.value());
//   }
  urlMap.clear();
}


static QString obtainFile(const QString& source) {
  QUrl url;

  if (QFile::exists(source) && QFileInfo(source).isRelative()) {
    url.setPath(source);
  } else {
    url = QUrl(source);
  }

//   if (url.isLocalFile() || url.protocol().isEmpty() || url.protocol().toLower() == "nad") {
    return source;
//   }

  if (urlMap.contains(source)) {
    return urlMap[source];
  }

  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
//   if (!KIO::NetAccess::exists(url, true, 0L)) {
//     return QString::null;
//   }

  QString tmpFile;
  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
//   if (!KIO::NetAccess::download(url, tmpFile, 0L)) {
//     return QString::null;
//   }

  urlMap[source] = tmpFile;

  return tmpFile;
}


// Scans for plugins and stores the information for them in "_pluginList"
static void scanPlugins() {
  PluginList tmpList;

  Debug::self()->log(i18n("Scanning for data-source plugins."));

  foreach (QObject *plugin, QPluginLoader::staticInstances()) {
    //try a cast
    if (DataSourcePluginInterface *ds = dynamic_cast<DataSourcePluginInterface*>(plugin)) {
      tmpList.append(ds);
    } 
  }

  QStringList pluginPaths;
  pluginPaths << QLibraryInfo::location(QLibraryInfo::PluginsPath);
  pluginPaths << QString(qApp->applicationDirPath()).replace("bin", "plugin");

  QDir rootDir = QApplication::applicationDirPath();
  rootDir.cdUp();
  QString pluginPath = rootDir.canonicalPath();
  pluginPath += QDir::separator();
  pluginPath += QLatin1String(INSTALL_LIBDIR);
  pluginPath += QDir::separator();
  pluginPath += QLatin1String("kst");
  pluginPaths << pluginPath;

  foreach (QString pluginPath, pluginPaths) {
    QDir d(pluginPath);
    foreach (QString fileName, d.entryList(QDir::Files)) {
        QPluginLoader loader(d.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
          if (DataSourcePluginInterface *ds = dynamic_cast<DataSourcePluginInterface*>(plugin)) {
            tmpList.append(ds);
          }
        } else {
              Debug::self()->log(QString("instance failed for %1 (%2)").arg(fileName).arg(loader.errorString()));
        }
    }
  }

  // This cleans up plugins that have been uninstalled and adds in new ones.
  // Since it is a shared pointer it can't dangle anywhere.
  _pluginList.clear();
  _pluginList = tmpList;
}

void DataSource::initPlugins() {
  if (_pluginList.isEmpty()) {
      scanPlugins();
  }
}


QStringList DataSource::pluginList() {
  QStringList plugins;

  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (PluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    plugins += (*it)->pluginName();
  }

  return plugins;
}


namespace {
class PluginSortContainer {
  public:
    SharedPtr<DataSourcePluginInterface> plugin;
    int match;
    int operator<(const PluginSortContainer& x) const {
      return match > x.match; // yes, this is by design.  biggest go first
    }
    int operator==(const PluginSortContainer& x) const {
      return match == x.match;
    }
};
}


static QList<PluginSortContainer> bestPluginsForSource(const QString& filename, const QString& type) {
  QList<PluginSortContainer> bestPlugins;
  DataSource::init();

  PluginList info = _pluginList;

  if (!type.isEmpty()) {
    for (PluginList::Iterator it = info.begin(); it != info.end(); ++it) {
      if (DataSourcePluginInterface *p = dynamic_cast<DataSourcePluginInterface*>((*it).data())) {
        if (p->provides(type)) {
          PluginSortContainer psc;
          psc.match = 100;
          psc.plugin = p;
          bestPlugins.append(psc);
          return bestPlugins;
        }
      }
    }
  }

  for (PluginList::Iterator it = info.begin(); it != info.end(); ++it) {
    PluginSortContainer psc;
    if (DataSourcePluginInterface *p = dynamic_cast<DataSourcePluginInterface*>((*it).data())) {
      if ((psc.match = p->understands(settingsObject, filename)) > 0) {
        psc.plugin = p;
        bestPlugins.append(psc);
      }
    }
  }

  qSort(bestPlugins);
  return bestPlugins;
}


static DataSourcePtr findPluginFor(ObjectStore *store, const QString& filename, const QString& type, const QDomElement& e = QDomElement()) {

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(filename, type);

  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    DataSourcePtr plugin = (*i).plugin->create(store, settingsObject, filename, QString::null, e);
    if (plugin) {
      return plugin;
    }
  }
  return 0L;
}


DataSourcePtr DataSource::loadSource(ObjectStore *store, const QString& filename, const QString& type) {
#ifndef Q_WS_WIN32
  if (filename == "stdin" || filename == "-") {
    // FIXME: what store do we put this in?
    return new StdinSource(0, settingsObject);
  }
#endif
  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  DataSourcePtr dataSource = findPluginFor(store, fn, type);
  if (dataSource) {
    store->addObject<DataSource>(dataSource);
  }

  return dataSource;

}


DataSourcePtr DataSource::findOrLoadSource(ObjectStore *store, const QString& filename) {
  Q_ASSERT(store);
  DataSourcePtr dataSource = store->dataSourceList().findReusableFileName(filename);

  if (!dataSource) {
    dataSource = DataSource::loadSource(store, filename);
  }

  return dataSource;
}


bool DataSource::validSource(const QString& filename) {
#ifndef Q_WS_WIN32
  if (filename == "stdin" || filename == "-") {
    return true;
  }
#endif
  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return false;
  }

  // Use a local version of the store, we don't want to save this.
  ObjectStore store;
  if (DataSourcePtr dataSource = findPluginFor(&store, fn, QString::null)) {
    return true;
  }
  return false;
}


bool DataSource::hasConfigWidget() const {
  return sourceHasConfigWidget(_filename, fileType());
}


DataSourceConfigWidget* DataSource::configWidget() {
  if (!hasConfigWidget())
    return 0;

  DataSourceConfigWidget *w = configWidgetForSource(_filename, fileType());
  Q_ASSERT(w);

  //This is still ugly to me...
  w->_instance = this;
  w->load();
  return w;
}


bool DataSource::pluginHasConfigWidget(const QString& plugin) {
  initPlugins();

  PluginList info = _pluginList;

  for (PluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    if ((*it)->pluginName() == plugin) {
      return (*it)->hasConfigWidget();
    }
  }

  return false;
}


DataSourceConfigWidget* DataSource::configWidgetForPlugin(const QString& plugin) {
  initPlugins();

  PluginList info = _pluginList;

  for (PluginList::Iterator it = info.begin(); it != info.end(); ++it) {
    if (DataSourcePluginInterface *p = dynamic_cast<DataSourcePluginInterface*>((*it).data())) {
      if (p->pluginName() == plugin) {
        return p->configWidget(settingsObject, QString::null);
      }
    }
  }

  return 0L;
}


bool DataSource::sourceHasConfigWidget(const QString& filename, const QString& type) {
  if (filename == "stdin" || filename == "-") {
    return 0L;
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    return (*i).plugin->hasConfigWidget();
  }

  Debug::self()->log(i18n("Could not find a datasource for '%1'(%2), but we found one just prior.  Something is wrong with Kst.", filename, type), Debug::Error);
  return false;
}


DataSourceConfigWidget* DataSource::configWidgetForSource(const QString& filename, const QString& type) {
  if (filename == "stdin" || filename == "-") {
    return 0L;
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    DataSourceConfigWidget *w = (*i).plugin->configWidget(settingsObject, fn);
    // Don't iterate.
    return w;
  }

  Debug::self()->log(i18n("Could not find a datasource for '%1'(%2), but we found one just prior.  Something is wrong with Kst.", filename, type), Debug::Error);
  return 0L;
}


bool DataSource::supportsTime(const QString& filename, const QString& type) {
  if (filename.isEmpty() || filename == "stdin" || filename == "-") {
    return false;
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return false;
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  if (bestPlugins.isEmpty()) {
    return false;
  }
  return (*bestPlugins.begin()).plugin->supportsTime(settingsObject, fn);
}


QStringList DataSource::fieldListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
  if (filename == "stdin" || filename == "-") {
    return QStringList();
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return QStringList();
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  QStringList rc;
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    QString typeSuggestion;
    rc = (*i).plugin->fieldList(settingsObject, fn, QString::null, &typeSuggestion, complete);
    if (!rc.isEmpty()) {
      if (outType) {
        if (typeSuggestion.isEmpty()) {
          *outType = (*i).plugin->provides()[0];
        } else {
          *outType = typeSuggestion;
        }
      }
      break;
    }
  }

  return rc;
}


QStringList DataSource::matrixListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
  if (filename == "stdin" || filename == "-") {
    return QStringList();
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return QStringList();
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  QStringList rc;
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    QString typeSuggestion;
    rc = (*i).plugin->matrixList(settingsObject, fn, QString::null, &typeSuggestion, complete);
    if (!rc.isEmpty()) {
      if (outType) {
        if (typeSuggestion.isEmpty()) {
          *outType = (*i).plugin->provides()[0];
        } else {
          *outType = typeSuggestion;
        }
      }
      break;
    }
  }

  return rc;
}

QStringList DataSource::scalarListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
  if (filename == "stdin" || filename == "-") {
    return QStringList();
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return QStringList();
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  QStringList rc;
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    QString typeSuggestion;
    rc = (*i).plugin->scalarList(settingsObject, fn, QString::null, &typeSuggestion, complete);
    if (!rc.isEmpty()) {
      if (outType) {
        if (typeSuggestion.isEmpty()) {
          *outType = (*i).plugin->provides()[0];
        } else {
          *outType = typeSuggestion;
        }
      }
      break;
    }
  }

  return rc;
}


QStringList DataSource::stringListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
  if (filename == "stdin" || filename == "-") {
    return QStringList();
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return QStringList();
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  QStringList rc;
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    QString typeSuggestion;
    rc = (*i).plugin->stringList(settingsObject, fn, QString::null, &typeSuggestion, complete);
    if (!rc.isEmpty()) {
      if (outType) {
        if (typeSuggestion.isEmpty()) {
          *outType = (*i).plugin->provides()[0];
        } else {
          *outType = typeSuggestion;
        }
      }
      break;
    }
  }

  return rc;
}


DataSourcePtr DataSource::loadSource(ObjectStore *store, QDomElement& e) {
  QString filename, type, tag;

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "filename") {
        filename = obtainFile(e.text());
      } else if (e.tagName() == "type") {
        type = e.text();
      }
    }
    n = n.nextSibling();
  }

  if (filename.isEmpty()) {
    return 0L;
  }

#ifndef Q_WS_WIN32
  if (filename == "stdin" || filename == "-") {
    // FIXME: what store do we put this in?
    return new StdinSource(0, settingsObject);
  }
#endif

  return findPluginFor(store, filename, type, e);
}

DataSource::DataSource(ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type, const UpdateCheckType updateType)
    : Object(), _filename(filename), _cfg(cfg), _updateCheckType(updateType) {
  Q_UNUSED(type)
  Q_UNUSED(store)
  _valid = false;
  _reusable = true;
  _writable = false;

  QString shortFilename = filename;
  while (shortFilename.at(shortFilename.length() - 1) == '/') {
    shortFilename.truncate(shortFilename.length() - 1);
  }
  shortFilename = shortFilename.section('/', -1);
  QString tn = i18n("DS-%1", shortFilename);
  _shortName = tn;

  if (_updateCheckType == Timer) {
    QTimer::singleShot(UpdateManager::self()->minimumUpdatePeriod()-1, this, SLOT(checkUpdate()));
  } else if (_updateCheckType == File) {
    QFileSystemWatcher *watcher = new QFileSystemWatcher();
    watcher->addPath(_filename);
    connect(watcher, SIGNAL(fileChanged ( const QString & )), this, SLOT(checkUpdate()));
    connect(watcher, SIGNAL(directoryChanged ( const QString & )), this, SLOT(checkUpdate()));
  }
}


DataSource::~DataSource() {
//    qDebug() << "DataSource destructor: " << Name() << endl;
}


void DataSource::checkUpdate() {
  if (!UpdateManager::self()->paused()) {
    if (update()) {
#if DEBUG_UPDATE_CYCLE > 1
      qDebug() << "UP - DataSource update ready for" << shortName();
#endif
      UpdateManager::self()->requestUpdate(this);
    }

    if (_updateCheckType == Timer) {
      QTimer::singleShot(UpdateManager::self()->minimumUpdatePeriod()-1, this, SLOT(checkUpdate()));
    }
  }
}


void DataSource::processUpdate(ObjectPtr object) {
  Q_UNUSED(object);
  UpdateManager::self()->updateStarted(this, this);
  emit sourceUpdated(this);
  UpdateManager::self()->updateFinished(this, this);
}


void DataSource::deleteDependents() {
}


const QString& DataSource::typeString() const {
  return staticTypeString;
}


Object::UpdateType DataSource::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  return Object::NO_CHANGE;
}


int DataSource::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  Q_UNUSED(skip)
  Q_UNUSED(lastFrameRead)
  return -9999; // unsupported
}


int DataSource::readField(double *v, const QString& field, int s, int n) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  return -1;
}

bool DataSource::isWritable() const {
  return _writable;
}

int DataSource::writeField(const double *v, const QString& field, int s, int n) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  return -1;
}

int DataSource::readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip) {
  Q_UNUSED(data)
  Q_UNUSED(matrix)
  Q_UNUSED(xStart)
  Q_UNUSED(yStart)
  Q_UNUSED(xNumSteps)
  Q_UNUSED(yNumSteps)
  Q_UNUSED(skip)
  return -9999;
}

int DataSource::readScalar(double &S, const QString& scalar) {
  Q_UNUSED(scalar)
  S = -9999;
  return 1;
}

int DataSource::readString(QString &S, const QString& field) {
  Q_UNUSED(field)
  S = QString();
  return 1;
}

int DataSource::readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps) {
  Q_UNUSED(data)
  Q_UNUSED(matrix)
  Q_UNUSED(xStart)
  Q_UNUSED(yStart)
  Q_UNUSED(xNumSteps)
  Q_UNUSED(yNumSteps)
  return -1;
}


bool DataSource::matrixDimensions(const QString& matrix, int* xDim, int* yDim) {
  Q_UNUSED(matrix)
  Q_UNUSED(xDim)
  Q_UNUSED(yDim)
  return false;
}


bool DataSource::isValid() const {
  return _valid;
}


bool DataSource::isValidField(const QString& field) const {
  return fieldList().contains(field);
}


bool DataSource::isValidMatrix(const QString& field) const {
  return matrixList().contains(field);
}


bool DataSource::isValidScalar(const QString& field) const {
  return scalarList().contains(field);
}


bool DataSource::isValidString(const QString& field) const {
  return stringList().contains(field);
}


int DataSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 0;
}


int DataSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return 0;
}


QString DataSource::fileName() const {
  // Look to see if it was a URL and save the URL instead
  for (QMap<QString,QString>::ConstIterator i = urlMap.begin(); i != urlMap.end(); ++i) {
    if (i.value() == _filename) {
      return i.key();
    }
  }
  return _filename;
}


QStringList DataSource::fieldList() const {
  return _fieldList;
}


QStringList DataSource::fieldScalars(const QString& field) {
  Q_UNUSED(field)
  return QStringList();
}

int DataSource::readFieldScalars(QList<double> &v, const QString& field, bool init) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(init)
  return (0);
}


QStringList DataSource::fieldStrings(const QString& field) {
  Q_UNUSED(field)
  return QStringList();
}


int DataSource::readFieldStrings(QStringList &v, const QString& field, bool init) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(init)
  return (0);
}


QStringList DataSource::matrixList() const {
  return _matrixList;
}

QStringList DataSource::scalarList() const {
  return _scalarList;
}

QStringList DataSource::stringList() const {
  return _stringList;
}

QString DataSource::fileType() const {
  return QString::null;
}


void DataSource::save(QXmlStreamWriter &s) {
  Q_UNUSED(s)
}


void DataSource::saveSource(QXmlStreamWriter &s) {
  QString name = _filename;
  // Look to see if it was a URL and save the URL instead
  for (QMap<QString,QString>::ConstIterator i = urlMap.begin(); i != urlMap.end(); ++i) {
    if (i.value() == _filename) {
      name = i.key();
      break;
    }
  }
  s.writeStartElement("source");
  s.writeAttribute("reader", fileType());
  s.writeAttribute("file", name);
  save(s);
  s.writeEndElement();
}


void DataSource::parseProperties(QXmlStreamAttributes &properties) {
  Q_UNUSED(properties);
}


void *DataSource::bufferMalloc(size_t size) {
  return malloc(size);
}


void DataSource::bufferFree(void *ptr) {
  return ::free(ptr);
}


void *DataSource::bufferRealloc(void *ptr, size_t size) {
  return realloc(ptr, size);
}


bool DataSource::fieldListIsComplete() const {
  return true;
}


bool DataSource::scalarListIsComplete() const {
  return true;
}


bool DataSource::stringListIsComplete() const {
  return true;
}


bool DataSource::isEmpty() const {
  return true;
}


bool DataSource::reset() {
  return false;
}


bool DataSource::supportsTimeConversions() const {
  return false;
}


int DataSource::sampleForTime(const QDateTime& time, bool *ok) {
  Q_UNUSED(time)
  if (ok) {
    *ok = false;
  }
  return 0;
}



int DataSource::sampleForTime(double ms, bool *ok) {
  Q_UNUSED(ms)
  if (ok) {
    *ok = false;
  }
  return 0;
}



QDateTime DataSource::timeForSample(int sample, bool *ok) {
  Q_UNUSED(sample)
  if (ok) {
    *ok = false;
  }
  return QDateTime::currentDateTime();
}



double DataSource::relativeTimeForSample(int sample, bool *ok) {
  Q_UNUSED(sample)
  if (ok) {
    *ok = false;
  }
  return 0;
}


bool DataSource::reusable() const {
  return _reusable;
}


void DataSource::disableReuse() {
  _reusable = false;
}

QString DataSource::_automaticDescriptiveName() const {
  return fileName();
}

QString DataSource::descriptionTip() const {
  return fileName();
}

/////////////////////////////////////////////////////////////////////////////
DataSourceConfigWidget::DataSourceConfigWidget()
: QWidget(0L), _cfg(0L) {
}


DataSourceConfigWidget::~DataSourceConfigWidget() {
}


void DataSourceConfigWidget::save() {
}


void DataSourceConfigWidget::load() {
}


void DataSourceConfigWidget::setConfig(QSettings *cfg) {
  _cfg = cfg;
}


void DataSourceConfigWidget::setInstance(DataSourcePtr inst) {
  _instance = inst;
}


DataSourcePtr DataSourceConfigWidget::instance() const {
  return _instance;
}


ValidateDataSourceThread::ValidateDataSourceThread(const QString& file, const int requestID) : QRunnable(), 
  _file(file),
  _requestID(requestID) {
}


void ValidateDataSourceThread::run() {
  QFileInfo info(_file);
  if (!info.exists()) {
    emit dataSourceInvalid(_requestID);
    return;
  }

  if (!DataSource::validSource(_file)) {
    emit dataSourceInvalid(_requestID);
    return;
  }

  emit dataSourceValid(_file, _requestID);
}

}
// vim: ts=2 sw=2 et
