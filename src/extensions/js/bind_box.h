/***************************************************************************
                               bind_box.h
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

#ifndef BIND_BOX_H
#define BIND_BOX_H

#include "bind_viewobject.h"

#include <kstviewbox.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Box
   @inherits ViewObject
   @collection ViewObjectCollection
   @description A customizable box graphic.
*/
class KstBindBox : public KstBindViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new box in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new box and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new box in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new box and places it in the Window <i>window</i>.
    */
    KstBindBox(KJS::ExecState *exec, KstViewBoxPtr d, const char *name = 0L);
    KstBindBox(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindBox();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property number xRound
       @description The roundness of the corners in the X dimension.  Must be
                    an integer between 0 and 99.
    */
    void setXRound(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value xRound(KJS::ExecState *exec) const;
    /* @property number yRound
       @description The roundness of the corners in the Y dimension.  Must be
                    an integer between 0 and 99.
    */
    void setYRound(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value yRound(KJS::ExecState *exec) const;
    /* @property number cornerStyle
       @description The style to draw the corners of the box.
                    <ul>
                    <li>0 - Miter (default)</li>
                    <li>1 - Bevel</li>
                    <li>2 - Round</li>
                    </ul>
    */
    void setCornerStyle(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value cornerStyle(KJS::ExecState *exec) const;
    /* @property int borderWidth
       @description The width in pixels of the border.
     */
    void setBorderWidth(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value borderWidth(KJS::ExecState *exec) const;
    /* @property string borderColor
       @description The border color for this object.
     */
    void setBorderColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value borderColor(KJS::ExecState *exec) const;

  protected:
    KstBindBox(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
