/***************************************************************************
                              bind_extension.h
                             -------------------
    begin                : Apr 11 2005
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

#ifndef BIND_EXTENSION_H
#define BIND_EXTENSION_H

#include "kstbinding.h"

#include <kstextension.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Extension
   @collection ExtensionCollection
   @description A class representing an extension in Kst.
*/
class KstBindExtension : public KstBinding {
  public:
    KstBindExtension(KJS::ExecState *exec, const QString& extension);
    ~KstBindExtension();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions
    /* @method load
       @returns boolean
       @description Loads this extension into Kst.
    */
    KJS::Value load(KJS::ExecState *exec, const KJS::List& args);
    /* @method unload
       @description Unloads this extension if it is loaded.
    */
    KJS::Value unload(KJS::ExecState *exec, const KJS::List& args);

    // properties
    /* @property string name
       @readonly
       @description The name of this extension.
    */
    KJS::Value name(KJS::ExecState *exec) const;
    /* @property boolean loaded
       @readonly
       @description True if this extension is presently loaded.
    */
    KJS::Value loaded(KJS::ExecState *exec) const;

  protected:
    KstBindExtension(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    QString _d;
};


#endif

// vim: ts=2 sw=2 et
