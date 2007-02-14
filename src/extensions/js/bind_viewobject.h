/***************************************************************************
                              bind_viewobject.h
                             -------------------
    begin                : May 29 2005
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

#ifndef BIND_VIEWOBJECT_H
#define BIND_VIEWOBJECT_H

#include "bind_object.h"

#include <kstviewobject.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class ViewObject
   @inherits Object
   @collection ViewObjectCollection
   @description Represents any object in a Kst window.  This is an abstract
                object and may not be instantiated directly.
*/
class KstBindViewObject : public KstBindObject {
  public:
    KstBindViewObject(KJS::ExecState *exec, KstViewObjectPtr d, const char *name = 0L);
    KstBindViewObject(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindViewObject();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @method resize
       @returns ViewObject
       @arg Size size The new size for the object.
       @description Resizes the object to the given size, if possible.
    */
    KJS::Value resize(KJS::ExecState *exec, const KJS::List& args);
    /* @method move
       @returns ViewObject
       @arg Point pos The position to move the object to.
       @description Moves the object to a new position, if possible.
    */
    KJS::Value move(KJS::ExecState *exec, const KJS::List& args);
    /* @method findChild
       @returns ViewObject
       @arg Point pos The relative position to search.
       @description Finds the topmost child of this view object at the given
                    point.  Returns null if there is no child there.
    */
    KJS::Value findChild(KJS::ExecState *exec, const KJS::List& args);
    /* @method convertTo
       @returns ViewObject
       @arg string type The type to attempt to convert this object to.
       @description Attempts to convert this ViewObject to a different type.
                    The object must be derived from this type at some level.
                    Null is returned if it is not possible to convert to the
                    requested type.
    */
    KJS::Value convertTo(KJS::ExecState *exec, const KJS::List& args);

    /* @property Point position
       @description The location of the object relative to its parent.
    */
    void setPosition(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value position(KJS::ExecState *exec) const;
    /* @property Size size
       @description The size of the object in pixels.
    */
    void setSize(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value size(KJS::ExecState *exec) const;
    /* @property boolean transparent
       @description True if this object is transparent.  Not all objects
                    support transparency.
     */
    void setTransparent(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value transparent(KJS::ExecState *exec) const;
    /* @property boolean onGrid
       @description True if the children of this object are on a grid.
     */
    void setOnGrid(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value onGrid(KJS::ExecState *exec) const;
    /* @property number columns
       @description The number of columns the children are organized into.  If
                    this value is modified, <i>onGrid</i> is set to true.
     */
    void setColumns(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value columns(KJS::ExecState *exec) const;
    /* @property string color
       @description The foreground color for this object.
     */
    void setColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value color(KJS::ExecState *exec) const;
    /* @property string backgroundColor
       @description The background color for this object.
     */
    void setBackgroundColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value backgroundColor(KJS::ExecState *exec) const;
    /* @property boolean maximized
       @description If true, this object is maximized relative to its parent.
                    This is a temporary state whereby the plot uses all the
                    space available in the parent but can be restored to
                    <i>normal</i> size again.
     */
    void setMaximized(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value maximized(KJS::ExecState *exec) const;
    /* @property Size minimumSize
       @readonly
       @description The minimum size of the view object.
     */
    KJS::Value minimumSize(KJS::ExecState *exec) const;
    /* @property string type
       @readonly
       @description A string containing the type of this view object.
     */
    KJS::Value type(KJS::ExecState *exec) const;
    /* @property ViewObjectCollection children
       @description The list of all children of this view object in z-order
                    from the furthest back to the closest to the top.  This
                    is a reference to a collection which may be modified, but
                    cannot be <i>set</i>.
       @readonly
     */
    KJS::Value children(KJS::ExecState *exec) const;

    static KstBindViewObject *bind(KJS::ExecState*, KstViewObjectPtr);

  protected:
    KstBindViewObject(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);

    static void addFactory(const QString&, KstBindViewObject*(*)(KJS::ExecState*, KstViewObjectPtr));

  private:
    static QMap<QString, KstBindViewObject*(*)(KJS::ExecState*, KstViewObjectPtr)> _factoryMap;
};


#endif

// vim: ts=2 sw=2 et
