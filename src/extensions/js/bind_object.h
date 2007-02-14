/***************************************************************************
                                bind_object.h
                             -------------------
    begin                : May 31 2005
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

#ifndef BIND_OBJECT_H
#define BIND_OBJECT_H

#include "kstbinding.h"

#include <kstobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Object
   @collection ObjectCollection
   @description Represents any object in Kst.  This is an abstract
                object and may not be instantiated directly.
*/
class KstBindObject : public KstBinding {
  public:
    KstBindObject(KJS::ExecState *exec, KstObjectPtr d, const char *name = 0L);
    KstBindObject(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindObject();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property string tagName
       @description A unique identifier for this data object.
     */
    void setTagName(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value tagName(KJS::ExecState *exec) const;

  protected:
    KstBindObject(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
  public:
    KstObjectPtr _d;
};


#endif

// vim: ts=2 sw=2 et
