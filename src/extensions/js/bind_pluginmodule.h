/***************************************************************************
                             bind_pluginmodule.h
                             -------------------
    begin                : Mar 29 2005
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

#ifndef BIND_PLUGINMODULE_H
#define BIND_PLUGINMODULE_H

#include "kstbinding.h"

#include <plugin.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PluginModule
   @collection PluginModuleCollection
   @description Represents a plugin module in Kst.  This is not an instance of
                a plugin, but a reference to the actual plugin itself.  It is
                typically used to instantiate a plugin object.
*/
class KstBindPluginModule : public KstBinding {
  public:
    KstBindPluginModule(KJS::ExecState *exec, const Plugin::Data& d);
    ~KstBindPluginModule();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // properties
//    KJS::Value xmlFile(KJS::ExecState *exec) const;
//    KJS::Value libraryFile(KJS::ExecState *exec) const;
    /* @property boolean usesLocalData
       @readonly
       @description If true, the plugin makes use of persistant local storage.
    */
    KJS::Value usesLocalData(KJS::ExecState *exec) const;
    /* @property boolean isFit
       @readonly
       @description If true, this plugin is a <i>fit</i> for a curve.
    */
    KJS::Value isFit(KJS::ExecState *exec) const;
    /* @property boolean isFilter
       @readonly
       @description If true, this plugin is a <i>filter</i> for a vector.
    */
    KJS::Value isFilter(KJS::ExecState *exec) const;
    /* @property string name
       @readonly
       @description Represents the short name of the plugin.
    */
    KJS::Value name(KJS::ExecState *exec) const;
    /* @property string readableName
       @readonly
       @description Contains a long name for the plugin suitable for display in
                    a user interface.
    */
    KJS::Value readableName(KJS::ExecState *exec) const;
    /* @property string author
       @readonly
       @description Contains the name of the author of the plugin.
    */
    KJS::Value author(KJS::ExecState *exec) const;
    /* @property string description
       @readonly
       @description Contains a text description of what the plugin is and/or
                    does.
    */
    KJS::Value description(KJS::ExecState *exec) const;
    /* @property string version
       @readonly
       @description Contains the version number or string of the plugin.
    */
    KJS::Value version(KJS::ExecState *exec) const;
    /* @property PluginIOCollection inputs
       @readonly
       @description A list of all the inputs for the plugin.
    */
    KJS::Value inputs(KJS::ExecState *exec) const;
    /* @property PluginIOCollection outputs
       @readonly
       @description A list of all the outputs of the plugin.
    */
    KJS::Value outputs(KJS::ExecState *exec) const;

  protected:
    friend class KstBinding;
    KstBindPluginModule(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    Plugin::Data _d;
};


#endif

// vim: ts=2 sw=2 et
