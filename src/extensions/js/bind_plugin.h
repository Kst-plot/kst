/***************************************************************************
                                bind_plugin.h
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

#ifndef BIND_PLUGIN_H
#define BIND_PLUGIN_H

#include "bind_dataobject.h"

#include <kstcplugin.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Plugin
   @inherits DataObject
   @description This class represents a data plugin in Kst.  It can be
                instantiated to create a new instance of a plugin in the
                Kst data collection.
   @collection PluginCollectoin
 */

class KstBindPlugin : public KstBindDataObject {
  public:
    /* @constructor
       @description Main constructor for the Plugini class.  New plugins are
                    typically invalid until various properties have been set.
       @optarg PluginModule module The plugin module to use for this plugin.
    */
    KstBindPlugin(KJS::ExecState *exec, KstCPluginPtr d);
    KstBindPlugin(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindPlugin();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions

    /* @property PluginModule module
       @description The library or module that is used for data processing by
                    this plugin object.
     */
    void setModule(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value module(KJS::ExecState *exec) const;
    /* @property string lastError
       @description A string containing details of the last error that
                    occurred while running the plugin.
       @readonly
     */
    KJS::Value lastError(KJS::ExecState *exec) const;
    /* @property boolean valid
       @description True if this plugin object is valid.  If false, there is
                    probably an invalid setting somewhere that needs to be
                    corrected before it can be used.  Some errors include
                    invalid inputs or outputs, or an invalid or missing
                    PluginModule.
       @readonly
     */
    KJS::Value valid(KJS::ExecState *exec) const;

  protected:
    KstBindPlugin(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindDataObject *bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
