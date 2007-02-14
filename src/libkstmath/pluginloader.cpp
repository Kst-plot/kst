/***************************************************************************
                      pluginloader.cpp  -  Part of KST
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


#include "kstdebug.h"
#include "pluginloader.h"
#include "pluginxmlparser.h"

#include <klibloader.h>
#include <klocale.h>


PluginLoader *PluginLoader::_self = 0L;
static KStaticDeleter<PluginLoader> _plSelf;

PluginLoader *PluginLoader::self() {
  if (!_self) {
    _plSelf.setObject(_self, new PluginLoader);
  }

  return _self;
}


PluginLoader::PluginLoader() {
  _parser = new PluginXMLParser;
}


PluginLoader::~PluginLoader() {
  delete _parser;
  _parser = 0L;
}


Plugin *PluginLoader::loadPlugin(const QString& xmlfile, const QString& object) {
  // First try to load the XML file.
  if (_parser->parseFile(xmlfile) != 0) {
    KstDebug::self()->log(i18n("Could not parse xml file '%1'.").arg(xmlfile), KstDebug::Error);
    return 0L;
  }

  // It parsed, create a new plugin and copy in the data
  Plugin *plug = new Plugin;
  plug->_data = _parser->data();

  // Load the plugin
  plug->_lib = KLibLoader::self()->library(object.local8Bit().data());
  if (!plug->_lib) {
    KstDebug::self()->log(i18n("Error trying to load plugin %1 [%2].").arg(object).arg(KLibLoader::self()->lastErrorMessage()), KstDebug::Error);
    delete plug;
    return 0L;
  }

  if (plug->_lib->hasSymbol(plug->_data._name.latin1())) {
    plug->_symbol = plug->_lib->symbol(plug->_data._name.latin1());
  }

  if (plug->_lib->hasSymbol("freeLocalData")) {
    plug->_freeSymbol = plug->_lib->symbol("freeLocalData");
  }

  if (plug->_lib->hasSymbol("errorCode")) {
    plug->_errorSymbol = plug->_lib->symbol("errorCode");
  }

  if (!plug->_symbol) {
    KstDebug::self()->log(i18n("Could not find symbol '%1' in plugin %2.").arg(plug->_data._name).arg(object), KstDebug::Error);
    delete plug;
    return 0L;
  }

  if (plug->_lib->hasSymbol("parameterName")) {
    plug->_parameterName = plug->_lib->symbol("parameterName");
  }

  plug->_xmlFile = xmlfile;
  plug->_soFile = object;

return plug;
}


// vim: ts=2 sw=2 et
