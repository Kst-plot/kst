/***************************************************************************
                               bind_template.h
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

#ifndef BIND_ TEMPLATE _H
#define BIND_ TEMPLATE _H

#include "kstbinding.h"

#include <ksttemplate.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Template
   @description .....
*/
class KstBindTemplate : public KstBinding {
  public:
    /* @constructor
       @arg type name description
       @optarg type name description
       @description ....
    */
    KstBindTemplate(KJS::ExecState *exec, KstTemplatePtr d);
    KstBindTemplate(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindTemplate();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    int methodCount() const;
    int propertyCount() const;

    // member functions
    /* @method name
       @returns type
       @arg type name description
       @optarg type name description
       @description ...
    */
    KJS::Value doIt(KJS::ExecState *exec, const KJS::List& args);

    // properties
    /* @property type name
       @description ....
    */
    void setValue(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value value(KJS::ExecState *exec) const;

  protected:
    KstBindTemplate(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    KstTemplatePtr _d;
};


#endif

// vim: ts=2 sw=2 et
