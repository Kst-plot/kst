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

#include "kst_i18n.h"
#include "datacollection.h"
#include "debug.h"
#include "objectstore.h"
#include "scalar.h"
#include "string.h"
#include "stdinsource.h"

#include "dataplugin.h"

namespace Kst {

const QString DataSource::staticTypeString = I18N_NOOP("Data Source");
const QString DataSource::staticTypeTag = I18N_NOOP("source");

static QSettings *settingsObject = 0L;
static QMap<QString,QString> urlMap;
void DataSource::setupOnStartup(QSettings *cfg) {
  settingsObject = cfg;
}


static PluginList _pluginList;
void DataSource::cleanupForExit() {
  _pluginList.clear();
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

  foreach (QString pluginPath, pluginPaths) {
    QDir d(pluginPath);
    foreach (QString fileName, d.entryList(QDir::Files)) {
        QPluginLoader loader(d.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
          if (DataSourcePluginInterface *ds = dynamic_cast<DataSourcePluginInterface*>(plugin)) {
            tmpList.append(ds);
          }
        }
    }
  }

  // This cleans up plugins that have been uninstalled and adds in new ones.
  // Since it is a shared pointer it can't dangle anywhere.
  _pluginList.clear();
  _pluginList = tmpList;
}


QStringList DataSource::pluginList() {
  QStringList plugins;

  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

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
  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

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
      // restore tag if present
      QDomNodeList l = e.elementsByTagName("tag");
      if (l.count() > 0) {
        QDomElement e2 = l.item(0).toElement();
        if (!e2.isNull()) {
          qDebug() << "Restoring tag " << e2.text() << " to DataSource" << endl;
          // FIXME
          //plugin->setTagName(ObjectTag::fromString(e2.text()));
        }
      }
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

  return w;
}


bool DataSource::pluginHasConfigWidget(const QString& plugin) {
  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

  PluginList info = _pluginList;

  for (PluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    if ((*it)->pluginName() == plugin) {
      return (*it)->hasConfigWidget();
    }
  }

  return false;
}


DataSourceConfigWidget* DataSource::configWidgetForPlugin(const QString& plugin) {
  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

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


DataSource::DataSource(ObjectStore *store, QSettings *cfg, const QString& filename, const QString& type)
    : Object(), _filename(filename), _cfg(cfg), _dataSourceVersion(0) {
  Q_UNUSED(type)
  _valid = false;
  _reusable = true;
  _writable = false;

  QString shortFilename = filename;
  while (shortFilename.at(shortFilename.length() - 1) == '/') {
    shortFilename.truncate(shortFilename.length() - 1);
  }
  shortFilename = shortFilename.section('/', -1);
  QString tn = i18n("DS-%1", shortFilename);
//  int count = 1;

  Object::setTagName(ObjectTag(tn, ObjectTag::globalTagContext));  // are DataSources always top-level?
// FIXME: put this back in
//  while (Data::self()->dataSourceTagNameNotUnique(tagName(), false)) {
//    Object::setTagName(ObjectTag(tn + QString::number(-(count++)), ObjectTag::globalTagContext));  // are DataSources always top-level?
//  }

  Q_ASSERT(store);
  _numFramesScalar = store->createObject<Scalar>(ObjectTag("frames", tag()));
  // Don't set provider - this is always up-to-date

  QTimer::singleShot(1000, this, SLOT(checkUpdate()));
}


DataSource::~DataSource() {
  //  qDebug() << "DataSource destructor: " << tag().tagString() << endl;
}


void DataSource::checkUpdate() {
  if (update()) {
#if DEBUG_UPDATE_CYCLE
    qDebug() << "DataSource" << shortName() << "updated to verison " << _dataSourceVersion;
#endif
    emit dataSourceUpdated(shortName(), _dataSourceVersion);
  }

  QTimer::singleShot(1000, this, SLOT(checkUpdate()));
}


void DataSource::deleteDependents() {
  _store->removeObject(_numFramesScalar);
  _numFramesScalar = 0L;

  for (QHash<QString, String*>::Iterator it = _metaData.begin(); it != _metaData.end(); ++it) {
    _store->removeObject(it.value());
  }
}


const QString& DataSource::typeString() const {
  return staticTypeString;
}


Object::UpdateType DataSource::update(int u) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  Q_UNUSED(u)
  return Object::NO_CHANGE;
}


void DataSource::updateNumFramesScalar() {
  if (_numFramesScalar->value() != frameCount()) {
    _dataSourceVersion++;
  }
  _numFramesScalar->setValue(frameCount());
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
  Q_UNUSED(field)
  return false;
}


bool DataSource::isValidMatrix(const QString& field) const {
  Q_UNUSED(field)
  return false;
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


QStringList DataSource::matrixList() const {
  return _matrixList;
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
  s.writeAttribute("tag", tag().tagString());
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


bool DataSource::isEmpty() const {
  return true;
}


bool DataSource::reset() {
  return false;
}


const QHash<QString, String*>& DataSource::metaData() const {
  return _metaData;
}


QString DataSource::metaData(const QString& key) const {
  if (_metaData[key]) {
    return _metaData[key]->value();
  } else {
    return QString::null;
  }
}


bool DataSource::hasMetaData() const {
  return !_metaData.isEmpty();
}


bool DataSource::hasMetaData(const QString& key) const {
  return (_metaData[key] != NULL);
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

QString DataSource::_automaticDescriptiveName() {
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

}
// vim: ts=2 sw=2 et
