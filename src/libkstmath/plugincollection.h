/***************************************************************************
                       pluginloader.h  -  Part of KST
                             -------------------
    begin                : Mon May 12 2003
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

#ifndef _PLUGINCOLLECTION_H
#define _PLUGINCOLLECTION_H


#include "plugin.h"
#include "kst_export.h"

#include <kstaticdeleter.h>

class PluginXMLParser;

class KST_EXPORT PluginCollection : public QObject {
  Q_OBJECT
  friend class KStaticDeleter<PluginCollection>;
  public:

  static PluginCollection *self();

  void loadAllPlugins();
  void unloadAllPlugins();

  /**
   *  Returns 0 on success, nonzero on error.
   */
  int loadPlugin(const QString& xmlfile);

  /**
   *  Returns 0 on success, nonzero on error.
   */
  int unloadPlugin(const KstSharedPtr<Plugin> p);
  int unloadPlugin(const QString& name);

  int count() const;

  /**
   *  Returns 0L on error.  Will load the plugin if it is not already
   *  loaded.
   */
  KstSharedPtr<Plugin> plugin(const QString& name);

  /**
   *  Returns true if the plugin is loaded.
   */
  bool isLoaded(const QString& name) const;

  /**
   *  Returns the map of all plugins available (cheap).
   *  Map is <PluginFileName, PluginData>
   */
  const QMap<QString, Plugin::Data>& pluginList() const;

  /**
   *  Returns the map of all plugins available (cheap).
   *  Map is <PluginName, PluginFileName>
   */
  const QMap<QString, QString>& pluginNameList() const;

  /**
   *  Returns the map of all plugins available (cheap).
   *  Map is <ReadableName, PluginName>
   */
  const QMap<QString, QString>& readableNameList() const;

  /**
   *  Returns the list of all plugins loaded (cheap).
   */
  QStringList loadedPluginList() const;

  /**
   *  Rescan the plugin directories (expensive).
   */
  void rescan();

  /**
   *  Delete a plugin from the system and the collection.
   */
  int deletePlugin(const QString& xmlfile, const QString& object = QString::null);


  signals:
    void pluginLoaded(const QString& name);
    void pluginUnloaded(const QString& name);
    void pluginInstalled(const QString& name);
    void pluginRemoved(const QString& name);
    void pluginListChanged();

  protected:
    PluginCollection();
    ~PluginCollection();

    static PluginCollection *_self;

  private:
    PluginXMLParser *_parser;
    QMap<QString, KstSharedPtr<Plugin> > _plugins;

    // Map: XMLFILE -> Plugin data
    mutable QMap<QString, Plugin::Data> _installedPlugins;
    // Map: Plugin Name -> XMLFILE
    mutable QMap<QString, QString> _installedPluginNames;
    // Map: Readable Name -> Plugin Name
    mutable QMap<QString, QString> _installedReadablePluginNames;
    void scanPlugins() ;
    void loadPluginsFor(const QString& path);
};


#endif

// vim: ts=2 sw=2 et
