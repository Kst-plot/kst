/***************************************************************************
                                 bind_size.h
                             -------------------
    begin                : May 28 2005
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

#ifndef BIND_SIZE_H
#define BIND_SIZE_H

#include "kstbinding.h"

#include <qsize.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Size
   @description Represents a two-dimensional size of the form (w, h).
*/
class KstBindSize : public KstBinding {
  public:
    /* @constructor
       @description Creates a new size object with value (0.0, 0.0).
    */
    /* @constructor
       @arg number w The width.
       @arg number h The height.
       @description Creates a new size object with value (w, h).
    */
    KstBindSize(KJS::ExecState *exec, int w, int h);
    KstBindSize(KJS::ExecState *exec, const QSize& sz);
    KstBindSize(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindSize();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::UString toString(KJS::ExecState *exec) const;

    /* @property number w
       @description The width.
    */
    void setW(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value w(KJS::ExecState *exec) const;
    /* @property number h
       @description The height.
    */
    void setH(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value h(KJS::ExecState *exec) const;

  protected:
    KstBindSize(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

  public:
    QSize _sz;
};


#endif

// vim: ts=2 sw=2 et
