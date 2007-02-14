/***************************************************************************
                               bind_pluginio.h
                             -------------------
    begin                : Apr 18 2005
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

#ifndef BIND_PLUGINIO_H
#define BIND_PLUGINIO_H

#include "kstbinding.h"

#include <plugin.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PluginIO
   @collection PluginIOCollection
   @description An object representing an input or output for a given plugin
                module.
*/
class KstBindPluginIO : public KstBinding {
  public:
    KstBindPluginIO(KJS::ExecState *exec, Plugin::Data::IOValue d, bool input);
    ~KstBindPluginIO();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // properties
    /* @property string name
       @readonly
       @description The name of the input or output.
    */
    KJS::Value name(KJS::ExecState *exec) const;
    /* @property string type
       @readonly
       @description The data type of the input or output.
    */
    KJS::Value type(KJS::ExecState *exec) const;
    /* @property string subType
       @readonly
       @description The sub-type of the input or output, if needed.  For
                    instance, a <i>table</i> may have a subtype of <i>float</i>.
    */
    KJS::Value subType(KJS::ExecState *exec) const;
    /* @property string description
       @readonly
       @description A description of the meaning of the input or output.
    */
    KJS::Value description(KJS::ExecState *exec) const;
    /* @property string defaultValue
       @readonly
       @description The default value for this input or output, if specified by
                    the plugin.
    */
    KJS::Value defaultValue(KJS::ExecState *exec) const;

  protected:
    KstBindPluginIO(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    Plugin::Data::IOValue _d;
    bool _input;
};


#endif

// vim: ts=2 sw=2 et
