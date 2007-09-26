/***************************************************************************
                   kstdatasource.cpp  -  abstract data source
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

#include "kstdatasource.h"

#include <assert.h>

#include <qapplication.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlibraryinfo.h>
#include <qpluginloader.h>
#include <qtextdocument.h>
#include <qurl.h>
#include <QXmlStreamWriter>

#include "kst_i18n.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include "kstscalar.h"
#include "stdinsource.h"

#include "kstdataplugin.h"

static QSettings *settingsObject = 0L;
static QMap<QString,QString> urlMap;
void KstDataSource::setupOnStartup(QSettings *cfg) {
  settingsObject = cfg;
}


static KstPluginList _pluginList;
void KstDataSource::cleanupForExit() {
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
  KstPluginList tmpList;

  KstDebug::self()->log(i18n("Scanning for data-source plugins."));

  foreach (QObject *plugin, QPluginLoader::staticInstances()) {
    //try a cast
    if (KstDataSourcePluginInterface *ds = qobject_cast<KstDataSourcePluginInterface*>(plugin)) {
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
          if (KstDataSourcePluginInterface *ds = qobject_cast<KstDataSourcePluginInterface*>(plugin)) {
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


QStringList KstDataSource::pluginList() {
  QStringList plugins;

  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

  for (KstPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    plugins += (*it)->pluginName();
  }

  return plugins;
}


namespace {
class PluginSortContainer {
  public:
    KstSharedPtr<KstDataSourcePluginInterface> plugin;
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

  KstPluginList info = _pluginList;

  if (!type.isEmpty()) {
    for (KstPluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
      if (KstDataSourcePluginInterface *p = kst_cast<KstDataSourcePluginInterface>(*it)) {
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

  for (KstPluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    PluginSortContainer psc;
    if (KstDataSourcePluginInterface *p = kst_cast<KstDataSourcePluginInterface>(*it)) {
      if ((psc.match = p->understands(settingsObject, filename)) > 0) {
        psc.plugin = p;
        bestPlugins.append(psc);
      }
    }
  }

  qSort(bestPlugins);
  return bestPlugins;
}


static KstDataSourcePtr findPluginFor(const QString& filename, const QString& type, const QDomElement& e = QDomElement()) {

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(filename, type);

  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    KstDataSourcePtr plugin = (*i).plugin->create(settingsObject, filename, QString::null, e);
    if (plugin) {
      // restore tag if present
      QDomNodeList l = e.elementsByTagName("tag");
      if (l.count() > 0) {
        QDomElement e2 = l.item(0).toElement();
        if (!e2.isNull()) {
          qDebug() << "Restoring tag " << e2.text() << " to KstDataSource" << endl;
          plugin->setTagName(KstObjectTag::fromString(e2.text()));
        }
      }
      return plugin;
    }
  }

  return 0L;
}


KstDataSourcePtr KstDataSource::loadSource(const QString& filename, const QString& type) {
  if (filename == "stdin" || filename == "-") {
    return new KstStdinSource(settingsObject);
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  return findPluginFor(fn, type);
}


bool KstDataSource::hasConfigWidget() const {
  return sourceHasConfigWidget(_filename, fileType());
}


KstDataSourceConfigWidget* KstDataSource::configWidget() {
  if (!hasConfigWidget())
    return 0;

  KstDataSourceConfigWidget *w = configWidgetForSource(_filename, fileType());
  Q_ASSERT(w);

  //This is still ugly to me...
  w->_instance = this;

  return w;
}


bool KstDataSource::pluginHasConfigWidget(const QString& plugin) {
  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

  KstPluginList info = _pluginList;

  for (KstPluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    if ((*it)->pluginName() == plugin) {
      return (*it)->hasConfigWidget();
    }
  }

  return false;
}


KstDataSourceConfigWidget* KstDataSource::configWidgetForPlugin(const QString& plugin) {
  if (_pluginList.isEmpty()) {
    scanPlugins();
  }

  KstPluginList info = _pluginList;

  for (KstPluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    if (KstDataSourcePluginInterface *p = kst_cast<KstDataSourcePluginInterface>(*it)) {
      if (p->pluginName() == plugin) {
        return p->configWidget(settingsObject, QString::null);
      }
    }
  }

  return 0L;
}


bool KstDataSource::sourceHasConfigWidget(const QString& filename, const QString& type) {
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

  KstDebug::self()->log(i18n("Could not find a datasource for '%1'(%2), but we found one just prior.  Something is wrong with Kst.", filename, type), KstDebug::Error);
  return false;
}


KstDataSourceConfigWidget* KstDataSource::configWidgetForSource(const QString& filename, const QString& type) {
  if (filename == "stdin" || filename == "-") {
    return 0L;
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    KstDataSourceConfigWidget *w = (*i).plugin->configWidget(settingsObject, fn);
    // Don't iterate.
    return w;
  }

  KstDebug::self()->log(i18n("Could not find a datasource for '%1'(%2), but we found one just prior.  Something is wrong with Kst.", filename, type), KstDebug::Error);
  return 0L;
}


bool KstDataSource::supportsTime(const QString& filename, const QString& type) {
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


QStringList KstDataSource::fieldListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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


QStringList KstDataSource::matrixListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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


KstDataSourcePtr KstDataSource::loadSource(QDomElement& e) {
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

  if (filename == "stdin" || filename == "-") {
    return new KstStdinSource(settingsObject);
  }

  return findPluginFor(filename, type, e);
}


KstDataSource::KstDataSource(QSettings *cfg, const QString& filename, const QString& type)
: KstObject(), _filename(filename), _cfg(cfg) {
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
  int count = 1;

  KstObject::setTagName(KstObjectTag(tn, KstObjectTag::globalTagContext));  // are DataSources always top-level?
  while (KstData::self()->dataSourceTagNameNotUnique(tagName(), false)) {
    KstObject::setTagName(KstObjectTag(tn + QString::number(-(count++)), KstObjectTag::globalTagContext));  // are DataSources always top-level?
  }

  _numFramesScalar = new KstScalar(KstObjectTag("frames", tag()));
  // Don't set provider - this is always up-to-date
}


KstDataSource::~KstDataSource() {
//  qDebug() << "KstDataSource destructor: " << tag().tagString() << endl;
  KST::scalarList.lock().writeLock();
//  qDebug() << "  removing numFrames scalar" << endl;
  KST::scalarList.remove(_numFramesScalar);
  KST::scalarList.lock().unlock();

//  qDebug() << "  removing metadata strings" << endl;
  KST::stringList.lock().writeLock();
  KST::stringList.setUpdateDisplayTags(false);
  for (QHash<QString, KstString*>::Iterator it = _metaData.begin(); it != _metaData.end(); ++it) {
//    qDebug() << "    removing " << it.current()->tag().tagString() << endl;
    KST::stringList.remove(it.value());
  }
  KST::stringList.setUpdateDisplayTags(true);
  KST::stringList.lock().unlock();

  _numFramesScalar = 0L;
}


void KstDataSource::setTagName(const KstObjectTag& in_tag) {
  if (in_tag == tag()) {
    return;
  }

  KstObject::setTagName(in_tag);
  _numFramesScalar->setTagName(KstObjectTag("frames", tag()));
  for (QHash<QString, KstString*>::Iterator it = _metaData.begin(); it != _metaData.end(); ++it) {
    KstObjectTag stag = it.value()->tag();
    stag.setContext(tag().fullTag());
    it.value()->setTagName(stag);
  }
}


KstObject::UpdateType KstDataSource::update(int u) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  Q_UNUSED(u)
  return KstObject::NO_CHANGE;
}


void KstDataSource::updateNumFramesScalar() {
  _numFramesScalar->setValue(frameCount());
}


int KstDataSource::readField(double *v, const QString& field, int s, int n, int skip, int *lastFrameRead) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  Q_UNUSED(skip)
  Q_UNUSED(lastFrameRead)
  return -9999; // unsupported
}


int KstDataSource::readField(double *v, const QString& field, int s, int n) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  return -1;
}

bool KstDataSource::isWritable() const {
  return _writable;
}

int KstDataSource::writeField(const double *v, const QString& field, int s, int n) {
  Q_UNUSED(v)
  Q_UNUSED(field)
  Q_UNUSED(s)
  Q_UNUSED(n)
  return -1;
}

int KstDataSource::readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip) {
  Q_UNUSED(data)
  Q_UNUSED(matrix)
  Q_UNUSED(xStart)
  Q_UNUSED(yStart)
  Q_UNUSED(xNumSteps)
  Q_UNUSED(yNumSteps)
  Q_UNUSED(skip)
  return -9999;
}
 
  
int KstDataSource::readMatrix(KstMatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps) {
  Q_UNUSED(data)
  Q_UNUSED(matrix)
  Q_UNUSED(xStart)
  Q_UNUSED(yStart)
  Q_UNUSED(xNumSteps)
  Q_UNUSED(yNumSteps)
  return -1;
}


bool KstDataSource::matrixDimensions(const QString& matrix, int* xDim, int* yDim) {
  Q_UNUSED(matrix)
  Q_UNUSED(xDim)
  Q_UNUSED(yDim)
  return false;  
}


bool KstDataSource::isValid() const {
  return _valid;
}


bool KstDataSource::isValidField(const QString& field) const {
  Q_UNUSED(field)
  return false;
}


bool KstDataSource::isValidMatrix(const QString& field) const {
  Q_UNUSED(field)
  return false;  
}


int KstDataSource::samplesPerFrame(const QString &field) {
  Q_UNUSED(field)
  return 0;
}


int KstDataSource::frameCount(const QString& field) const {
  Q_UNUSED(field)
  return 0;
}


QString KstDataSource::fileName() const {
  // Look to see if it was a URL and save the URL instead
  for (QMap<QString,QString>::ConstIterator i = urlMap.begin(); i != urlMap.end(); ++i) {
    if (i.value() == _filename) {
      return i.key();
    }
  }
  return _filename;
}


QStringList KstDataSource::fieldList() const {
  return _fieldList;
}


QStringList KstDataSource::matrixList() const { 
  return _matrixList;  
}


QString KstDataSource::fileType() const {
  return QString::null;
}


void KstDataSource::save(QXmlStreamWriter &s) {
}


void KstDataSource::saveSource(QXmlStreamWriter &s) {
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


void *KstDataSource::bufferMalloc(size_t size) {
  return KST::malloc(size);
}


void KstDataSource::bufferFree(void *ptr) {
  return ::free(ptr);
}


void *KstDataSource::bufferRealloc(void *ptr, size_t size) {
  return KST::realloc(ptr, size);
}


bool KstDataSource::fieldListIsComplete() const {
  return true;
}


bool KstDataSource::isEmpty() const {
  return true;
}


bool KstDataSource::reset() {
  return false;
}


const QHash<QString, KstString*>& KstDataSource::metaData() const {
  return _metaData;
}


QString KstDataSource::metaData(const QString& key) const {
  if (_metaData[key]) {
    return _metaData[key]->value();
  } else {
    return QString::null;
  }
}


bool KstDataSource::hasMetaData() const {
  return !_metaData.isEmpty();
}


bool KstDataSource::hasMetaData(const QString& key) const {
  return (_metaData[key] != NULL);
}


bool KstDataSource::supportsTimeConversions() const {
  return false;
}


int KstDataSource::sampleForTime(const QDateTime& time, bool *ok) {
  Q_UNUSED(time)
  if (ok) {
    *ok = false;
  }
  return 0;
}



int KstDataSource::sampleForTime(double ms, bool *ok) {
  Q_UNUSED(ms)
  if (ok) {
    *ok = false;
  }
  return 0;
}



QDateTime KstDataSource::timeForSample(int sample, bool *ok) {
  Q_UNUSED(sample)
  if (ok) {
    *ok = false;
  }
  return QDateTime::currentDateTime();
}



double KstDataSource::relativeTimeForSample(int sample, bool *ok) {
  Q_UNUSED(sample)
  if (ok) {
    *ok = false;
  }
  return 0;
}


bool KstDataSource::reusable() const {
  return _reusable;
}


void KstDataSource::disableReuse() {
  _reusable = false;
}


/////////////////////////////////////////////////////////////////////////////
KstDataSourceConfigWidget::KstDataSourceConfigWidget()
: QWidget(0L), _cfg(0L) {
}


KstDataSourceConfigWidget::~KstDataSourceConfigWidget() {
}


void KstDataSourceConfigWidget::save() {
}


void KstDataSourceConfigWidget::load() {
}


void KstDataSourceConfigWidget::setConfig(QSettings *cfg) {
  _cfg = cfg;
}


void KstDataSourceConfigWidget::setInstance(KstDataSourcePtr inst) {
  _instance = inst;
}


KstDataSourcePtr KstDataSourceConfigWidget::instance() const {
  return _instance;
}

// vim: ts=2 sw=2 et
