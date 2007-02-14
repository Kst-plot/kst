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

#ifndef _PLUGINLOADER_H
#define _PLUGINLOADER_H


#include "plugin.h"

class PluginXMLParser;

class PluginLoader {
friend class KStaticDeleter<PluginLoader>;
public:

  static PluginLoader *self();

  Plugin *loadPlugin(const QString& xmlfile, const QString& object);

protected:
  PluginLoader();
  ~PluginLoader();

  static PluginLoader *_self;
  PluginXMLParser *_parser;
};


#endif

// vim: ts=2 sw=2 et
