/***************************************************************************
                             bind_jsdataobject.h
                             -------------------
    begin                : Apr 23 2005
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

#ifndef BIND_JSDATAOBJECT_H
#define BIND_JSDATAOBJECT_H

#include "kstbinding.h"

#include "js_dataobject.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

class KstBindJSDataObject : public KstBinding {
  public:
    KstBindJSDataObject(KJS::ExecState *exec, JSDataObjectPtr d);
    KstBindJSDataObject(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindJSDataObject();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions

    // properties
    void setTagName(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value tagName(KJS::ExecState *exec) const;
    void setScript(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value script(KJS::ExecState *exec) const;
    void setProperty(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value property(KJS::ExecState *exec) const;
    KJS::Value valid(KJS::ExecState *exec) const;

  protected:
    KstBindJSDataObject(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    JSDataObjectPtr _d;
};


#endif

// vim: ts=2 sw=2 et
