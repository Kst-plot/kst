/***************************************************************************
                               bind_ellipse.h
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

#ifndef BIND_ELLIPSE_H
#define BIND_ELLIPSE_H

#include "bind_viewobject.h"

#include <kstviewellipse.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Ellipse
   @inherits ViewObject
   @collection ViewObjectCollection
   @description A customizable ellipse graphic.
*/
class KstBindEllipse : public KstBindViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new ellipse in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new ellipse and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new ellipse in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new ellipse and places it in the Window <i>window</i>.
    */
    KstBindEllipse(KJS::ExecState *exec, KstViewEllipsePtr d, const char *name = 0L);
    KstBindEllipse(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindEllipse();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property number borderWidth
       @description The width of the border of the ellipse.
    */
    void setBorderWidth(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value borderWidth(KJS::ExecState *exec) const;
    /* @property string borderColor
       @description The color of the border of the ellipse.
    */
    void setBorderColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value borderColor(KJS::ExecState *exec) const;

  protected:
    KstBindEllipse(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
