/***************************************************************************
                                bind_point.h
                             -------------------
    begin                : Mar 30 2005
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

#ifndef BIND_POINT_H
#define BIND_POINT_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Point
   @description Represents a cartesian co-ordinate of the form (x,y).
*/
class KstBindPoint : public KstBinding {
  public:
    /* @constructor
       @description Creates a new point object with value (0.0, 0.0).
    */
    /* @constructor
       @arg number x The X value.
       @arg number y The Y value.
       @description Creates a new point object with value (x, y).
    */
    KstBindPoint(KJS::ExecState *exec, double x, double y);
    KstBindPoint(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindPoint();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::UString toString(KJS::ExecState *exec) const;

    /* @property number x
       @description The value of the x co-ordinate.
    */
    void setX(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value x(KJS::ExecState *exec) const;
    /* @property number y
       @description The value of the y co-ordinate.
    */
    void setY(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value y(KJS::ExecState *exec) const;

  protected:
    KstBindPoint(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

  public:
    double _x, _y;
};


#endif

// vim: ts=2 sw=2 et
