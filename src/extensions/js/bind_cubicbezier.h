/***************************************************************************
                           bind_cubicbezier.h
                             --------------
    begin                : Jun 14 2005
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

#ifndef BIND_CUBICBEZIER_H
#define BIND_CUBICBEZIER_H

#include "bind_viewobject.h"

#include <kstviewbezier.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class CubicBezier
   @inherits ViewObject
   @collection ViewObjectCollection
   @description A customizable cubic bezier graphic.
*/
class KstBindCubicBezier : public KstBindViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new curve in.  May also be
                              a string containing the name of an existing
                              ViewObject.
       @description Creates a new curve and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new curve in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new cubic bezier and places it in the Window <i>window</i>.
    */
    KstBindCubicBezier(KJS::ExecState *exec, KstViewBezierPtr d, const char *name = 0L);
    KstBindCubicBezier(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindCubicBezier();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property Point a
       @description The first control point for the curve.
    */
    void setA(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value a(KJS::ExecState *exec) const;
    /* @property Point b
       @description The second control point for the curve.
    */
    void setB(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value b(KJS::ExecState *exec) const;
    /* @property Point c
       @description The third control point for the curve.
    */
    void setC(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value c(KJS::ExecState *exec) const;
    /* @property Point d
       @description The fourth control point for the curve.
    */
    void setD(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value d(KJS::ExecState *exec) const;
    /* @property number width
       @description The width of the curve line.
    */
    void setWidth(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value width(KJS::ExecState *exec) const;
    /* @property number capStyle
       @description The cap style for the curve.
                    <ul>
                    <li>0 - Flat - may not cover the line ends (default)</li>
                    <li>1 - Box - may extend past line ends</li>
                    <li>2 - Rounded</li>
                    </ul>
    */
    void setCapStyle(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value capStyle(KJS::ExecState *exec) const;
    /* @property number lineStyle
       @description The style for the curve.
                    <ul>
                    <li>0 - Solid line(default)</li>
                    <li>1 - Dashed line</li>
                    <li>2 - Dotted line</li>
                    <li>3 - Dash - dot line</li>
                    <li>4 - Dash - dot - dot line</li>
                    </ul>
    */
    void setLineStyle(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value lineStyle(KJS::ExecState *exec) const;

  protected:
    KstBindCubicBezier(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
