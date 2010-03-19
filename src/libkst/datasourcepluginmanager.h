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

#ifndef KST_DATA_SOURCE_PLUGIN_MANAGER_H
#define KST_DATA_SOURCE_PLUGIN_MANAGER_H


#include "dataplugin.h"
#include "sharedptr.h"
#include "datasource.h"

namespace Kst {

class ObjectStore;

typedef ObjectList<PluginInterface> PluginList;


class KST_EXPORT DataSourcePluginManager
{
  public:

    static const QMap<QString, QString> urlMap();

    static void init();
    static void cleanupForExit();
    static void initPlugins();

    /** Returns a list of plugins found on the system. */
    static QStringList pluginList();

    static SharedPtr<DataSource> loadSource(ObjectStore *store, const QString& filename, const QString& type = QString::null);
    static SharedPtr<DataSource> loadSource(ObjectStore *store, QDomElement& e);
    static SharedPtr<DataSource> findOrLoadSource(ObjectStore *store, const QString& filename);
    static bool validSource(const QString& filename);

    static bool sourceHasConfigWidget(const QString& filename, const QString& type = QString());
    static DataSourceConfigWidget *configWidgetForSource(const QString& filename, const QString& type = QString());

    static bool pluginHasConfigWidget(const QString& plugin);
    static DataSourceConfigWidget *configWidgetForPlugin(const QString& plugin);

    //static QStringList matrixListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);
    static QStringList scalarListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);
    static QStringList stringListForSource(const QString& filename, const QString& type = QString(), QString *outType = 0L, bool *complete = 0L);


};

}


#endif
