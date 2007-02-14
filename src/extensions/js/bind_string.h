/***************************************************************************
                                bind_string.h
                             -------------------
    begin                : Mar 28 2005
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

#ifndef BIND_STRING_H
#define BIND_STRING_H

#include "bind_object.h"

#include <kststring.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class String
   @inherits Object
   @collection StringCollection
   @description Represents a string in Kst.  Note that this is <i>not</i>
                analagous to a string in JavaScript, and may not be directly
                interchanged as such.  A Kst String is an object internal to
                Kst and is subject to update scheduling, sharing, and other
                policies of the Kst core.  However the <i>contents</i> of a
                String may be manipulated with JavaScript string manipulation
                functions.
*/
class KstBindString : public KstBindObject {
  public:
    /* @constructor
       @description Default constructor - creates an empty String.
    */
    KstBindString(KJS::ExecState *exec, KstStringPtr s);
    KstBindString(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindString();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property string value
       @description The value of the String.
     */
    void setValue(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value value(KJS::ExecState *exec) const;

  protected:
    KstBindString(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
