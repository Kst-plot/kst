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

#ifndef DATAPLUGIN_H
#define DATAPLUGIN_H

#include <QtPlugin>
#include <QSettings>
#include <QDomElement>

#include "sharedptr.h"
#include "datasource.h"

namespace Kst {

class ObjectStore;
class DataSource;
class DataSourceConfigWidget;


KSTCORE_EXPORT QStringList pluginSearchPaths();

class PluginInterface : public Shared {
  public:
    PluginInterface() {}

    virtual ~PluginInterface() {}

    virtual QString pluginName() const { return QString(); }
    virtual QString pluginDescription() const { return QString(); }

    virtual bool hasConfigWidget() const { return false; }
};


class DataSourcePluginInterface : public PluginInterface {
  public:
    virtual ~DataSourcePluginInterface() {}

    virtual DataSource *create(ObjectStore *store,
                            QSettings *cfg,
                                  const QString &filename,
                                  const QString &type,
                                  const QDomElement &element) const = 0;

    virtual QStringList matrixList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList scalarList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList stringList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual QStringList fieldList(QSettings *cfg,
                                  const QString& filename,
                                  const QString& type,
                                  QString *typeSuggestion,
                                  bool *complete) const = 0;

    virtual int understands(QSettings *cfg, const QString& filename) const = 0;

    virtual bool supportsTime(QSettings *cfg, const QString& filename) const = 0;

    virtual QStringList provides() const = 0;

    bool provides(const QString& type) const { return provides().contains(type); }

    virtual DataSourceConfigWidget *configWidget(QSettings *cfg, const QString& filename) const = 0;
};

// Optional capability for datasource plugins whose identifiers are not paths.
// This is separate from DataSourcePluginInterface to preserve its plugin ABI.
class NonFileDataSourcePluginInterface {
  public:
    virtual ~NonFileDataSourcePluginInterface() {}

    // Return true only when this plugin accepts the identifier as a non-file source.
    virtual bool isNonFileSource(const QString &identifier) const = 0;
};


}


// Data source plugin ABI identifier. Plugins use it in Q_PLUGIN_METADATA, and
// Qt refuses to load a plugin whose IID differs. Bump it (a plain counter,
// independent of the app version) whenever a change would break plugins built
// against the old headers: adding, removing or reordering virtual methods in
// DataSource, DataSourcePluginInterface or the classes they use, changing class
// layouts, or changing signatures plugins call or override. Keep the
// kst-datasource-abi-N Provides in debian/control in sync.
#define KST_DATASOURCE_IID "com.kst.DataSourcePluginInterface/3"

Q_DECLARE_INTERFACE(Kst::PluginInterface, "com.kst.PluginInterface/2.0")
Q_DECLARE_INTERFACE(Kst::DataSourcePluginInterface, KST_DATASOURCE_IID)
Q_DECLARE_INTERFACE(Kst::NonFileDataSourcePluginInterface, "com.kst.NonFileDataSourcePluginInterface/1.0")


#endif
