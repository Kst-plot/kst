/***************************************************************************
                         bind_pluginmoduleollection.h
                             -------------------
    begin                : Apr 12 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef BIND_PLUGINMODULECOLLECTION_H
#define BIND_PLUGINMODULECOLLECTION_H

#include "bind_collection.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PluginModuleCollection
   @inherits Collection
   @description An array of PluginModules.
*/
class KstBindPluginModuleCollection : public KstBindCollection {
  public:
    KstBindPluginModuleCollection(KJS::ExecState *exec);
    ~KstBindPluginModuleCollection();

    virtual KJS::Value length(KJS::ExecState *exec) const;

    virtual QStringList collection(KJS::ExecState *exec) const;
    virtual KJS::Value extract(KJS::ExecState *exec, const KJS::Identifier& item) const;
    virtual KJS::Value extract(KJS::ExecState *exec, unsigned item) const;
};


#endif

// vim: ts=2 sw=2 et
