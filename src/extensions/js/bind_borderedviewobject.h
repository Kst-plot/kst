/***************************************************************************
                         bind_borderedviewobject.h
                             -------------------
    begin                : Dec 08 2005
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

#ifndef BIND_BORDEREDVIEWOBJECT_H
#define BIND_BORDEREDVIEWOBJECT_H

#include "bind_viewobject.h"

#include <kstborderedviewobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class BorderedViewObject
   @inherits ViewObject
   @collection ViewObjectCollection
   @description Represents some objects in a Kst window.  This is an abstract
                object and may not be instantiated directly.  It has several
                properties not in regular ViewObjects including margins,
                padding, and a border.
*/
class KstBindBorderedViewObject : public KstBindViewObject {
  public:
    KstBindBorderedViewObject(KJS::ExecState *exec, KstBorderedViewObjectPtr d, const char *name = 0L);
    KstBindBorderedViewObject(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindBorderedViewObject();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property int padding
       @description The number of pixels between the border and the contents.
    */
    void setPadding(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value padding(KJS::ExecState *exec) const;
    /* @property int margin
       @description The number of pixels between the edge of the object and the
                    border.
    */
    void setMargin(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value margin(KJS::ExecState *exec) const;
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
    KstBindBorderedViewObject(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
