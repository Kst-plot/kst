/***************************************************************************
                              bind_arrow.h
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

#ifndef BIND_ARROW_H
#define BIND_ARROW_H

#include "bind_line.h"

#include <kstviewarrow.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Arrow
   @inherits Line
   @collection ViewObjectCollection
   @description A customizable arrow graphic.
*/
class KstBindArrow : public KstBindLine {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new arrow in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new arrow and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new arrow in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new arrow and places it in the Window <i>window</i>.
    */
    KstBindArrow(KJS::ExecState *exec, KstViewArrowPtr d, const char *name = 0L);
    KstBindArrow(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindArrow();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

  protected:
    KstBindArrow(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
