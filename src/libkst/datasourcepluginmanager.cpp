/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "datasourcepluginmanager.h"


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
//#include <QFileSystemWatcher>

#include "kst_i18n.h"
#include "datacollection.h"
#include "debug.h"
#include "objectstore.h"
#include "scalar.h"
#include "string.h"
#include "updatemanager.h"

#include "dataplugin.h"
#include "datasourcepluginmanager.h"

#define DATASOURCE_UPDATE_TIMER_LENGTH 1000

using namespace Kst;


QSettings DataSourcePluginManager::settingsObject("kst", "data");
QMap<QString,QString> DataSourcePluginManager::url_map;


const QMap<QString,QString> DataSourcePluginManager::urlMap() {
  return url_map;
}

void DataSourcePluginManager::init() {
  initPlugins();
}



static PluginList _pluginList;
void DataSourcePluginManager::cleanupForExit() {
  _pluginList.clear();
  qDebug() << "cleaning up for exit in datasource";
//   for (QMap<QString,QString>::Iterator i = urlMap.begin(); i != urlMap.end(); ++i) {
//     KIO::NetAccess::removeTempFile(i.value());
//   }
  url_map.clear();
}


QString DataSourcePluginManager::obtainFile(const QString& source) {
  QUrl url;

  if (QFile::exists(source) && QFileInfo(source).isRelative()) {
    url.setPath(source);
  } else {
    url = QUrl(source);
  }

//   if (url.isLocalFile() || url.protocol().isEmpty() || url.protocol().toLower() == "nad") {
    return source;
//   }

  if (url_map.contains(source)) {
    return url_map[source];
  }

  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
//   if (!KIO::NetAccess::exists(url, true, 0L)) {
//     return QString();
//   }

  QString tmpFile;
  // FIXME: come up with a way to indicate the "widget" and fill it in here so
  //        that KIO dialogs are associated with the proper window
//   if (!KIO::NetAccess::download(url, tmpFile, 0L)) {
//     return QString();
//   }

  url_map[source] = tmpFile;

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
#ifdef Q_OS_WIN
        if (!fileName.endsWith(".dll"))
            continue;
#endif
        QPluginLoader loader(d.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
          if (DataSourcePluginInterface *ds = dynamic_cast<DataSourcePluginInterface*>(plugin)) {
            tmpList.append(ds);
            Debug::self()->log(QString("Plugin loaded: %1").arg(fileName));
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

void DataSourcePluginManager::initPlugins() {
  if (_pluginList.isEmpty()) {
      scanPlugins();
  }
}


QStringList DataSourcePluginManager::pluginList() {
  QStringList plugins;

  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (PluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    plugins += (*it)->pluginName();
  }

  return plugins;
}



int DataSourcePluginManager::PluginSortContainer::operator<(const PluginSortContainer& x) const {
  return match > x.match; // yes, this is by design.  biggest go first
}
int DataSourcePluginManager::PluginSortContainer::operator==(const PluginSortContainer& x) const {
  return match == x.match;
}



QList<DataSourcePluginManager::PluginSortContainer> DataSourcePluginManager::bestPluginsForSource(const QString& filename, const QString& type) {

  QList<PluginSortContainer> bestPlugins;
  DataSourcePluginManager::init();

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
      if ((psc.match = p->understands(&settingsObject, filename)) > 0) {
        psc.plugin = p;
        bestPlugins.append(psc);
      }
    }
  }

  qSort(bestPlugins);

  return bestPlugins;
}


DataSourcePtr DataSourcePluginManager::findPluginFor(ObjectStore *store, const QString& filename, const QString& type, const QDomElement& e) {

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(filename, type);

  // we don't actually iterate here, unless the first plugin fails.  (Not sure this helps at all.)
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    DataSourcePtr plugin = (*i).plugin->create(store, &settingsObject, filename, QString(), e);
    if (plugin) {
      return plugin;
    }
  }
  return 0L;
}


DataSourcePtr DataSourcePluginManager::loadSource(ObjectStore *store, const QString& filename, const QString& type) {

#ifndef Q_WS_WIN32
  //if (filename == "stdin" || filename == "-") {
    // FIXME: what store do we put this in?
  //  return new StdinSource(0, settingsObject);
  //}
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


DataSourcePtr DataSourcePluginManager::findOrLoadSource(ObjectStore *store, const QString& filename) {
  Q_ASSERT(store);

  DataSourcePtr dataSource = store->dataSourceList().findReusableFileName(filename);

  if (!dataSource) {
    dataSource = DataSourcePluginManager::loadSource(store, filename);
  }

  return dataSource;
}


bool DataSourcePluginManager::validSource(const QString& filename) {
#ifndef Q_WS_WIN32
//  if (filename == "stdin" || filename == "-") {
//    return true;
//  }
#endif
  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return false;
  }

  DataSourcePluginManager::init();

  PluginList info = _pluginList;

  for (PluginList::Iterator it = info.begin(); it != info.end(); ++it) {
    if (DataSourcePluginInterface *p = dynamic_cast<DataSourcePluginInterface*>((*it).data())) {
      if ((p->understands(&settingsObject, filename)) > 0) {
        return true;
      }
    }
  }

  return false;
}



bool DataSourcePluginManager::pluginHasConfigWidget(const QString& plugin) {
  initPlugins();

  PluginList info = _pluginList;

  for (PluginList::ConstIterator it = info.begin(); it != info.end(); ++it) {
    if ((*it)->pluginName() == plugin) {
      return (*it)->hasConfigWidget();
    }
  }

  return false;
}


DataSourceConfigWidget* DataSourcePluginManager::configWidgetForPlugin(const QString& plugin) {
  initPlugins();

  PluginList info = _pluginList;

  for (PluginList::Iterator it = info.begin(); it != info.end(); ++it) {
    if (DataSourcePluginInterface *p = dynamic_cast<DataSourcePluginInterface*>((*it).data())) {
      if (p->pluginName() == plugin) {
        return p->configWidget(&settingsObject, QString());
      }
    }
  }

  return 0L;
}


bool DataSourcePluginManager::sourceHasConfigWidget(const QString& filename, const QString& type) {
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


DataSourceConfigWidget* DataSourcePluginManager::configWidgetForSource(const QString& filename, const QString& type) {
  if (filename == "stdin" || filename == "-") {
    return 0L;
  }

  QString fn = obtainFile(filename);
  if (fn.isEmpty()) {
    return 0L;
  }

  QList<PluginSortContainer> bestPlugins = bestPluginsForSource(fn, type);
  for (QList<PluginSortContainer>::Iterator i = bestPlugins.begin(); i != bestPlugins.end(); ++i) {
    DataSourceConfigWidget *w = (*i).plugin->configWidget(&settingsObject, fn);
    // Don't iterate.
    return w;
  }

  Debug::self()->log(i18n("Could not find a datasource for '%1'(%2), but we found one just prior.  Something is wrong with Kst.", filename, type), Debug::Error);
  return 0L;
}




/*
QStringList DataSourcePluginManager::fieldListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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
    rc = (*i).plugin->fieldList(settingsObject, fn, QString(), &typeSuggestion, complete);
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
*/

# if 0
QStringList DataSourcePluginManager::matrixListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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
    rc = (*i).plugin->matrixList(settingsObject, fn, QString(), &typeSuggestion, complete);
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
#endif


QStringList DataSourcePluginManager::scalarListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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
    rc = (*i).plugin->scalarList(&settingsObject, fn, QString(), &typeSuggestion, complete);
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


QStringList DataSourcePluginManager::stringListForSource(const QString& filename, const QString& type, QString *outType, bool *complete) {
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
    rc = (*i).plugin->stringList(&settingsObject, fn, QString(), &typeSuggestion, complete);
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

