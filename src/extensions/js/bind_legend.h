/***************************************************************************
                              bind_legend.h
                             ---------------
    begin                : Nov 08 2005
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

#ifndef BIND_LEGEND_H
#define BIND_LEGEND_H

#include "bind_borderedviewobject.h"

#include <kstviewlegend.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Legend
   @inherits BorderedViewObject
   @collection ViewObjectCollection
   @description A plot legend.
*/
class KstBindLegend : public KstBindBorderedViewObject {
  public:
    /* @constructor
       @arg ViewObject parent The parent to place the new legend in.  May also
                              be a string containing the name of an existing
                              ViewObject.
       @description Creates a new legend and places it in the ViewObject <i>parent</i>.
    */
    /* @constructor
       @arg Window window The window to place the new legend in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new legend and places it in the Window <i>window</i>.
    */
    KstBindLegend(KJS::ExecState *exec, KstViewLegendPtr d, const char *name = 0L);
    KstBindLegend(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindLegend();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    int methodCount() const;
    int propertyCount() const;

    /* @property string font
       @description Used to set or get the current font used for the legend.
    */
    void setFont(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value font(KJS::ExecState *exec) const;
    /* @property number fontSize
       @description Contains the size of the font used to draw the legend.
    */
    void setFontSize(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value fontSize(KJS::ExecState *exec) const;
    /* @property string textColor
       @description Contains the color of the text used to draw the legend.
    */
    void setTextColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value textColor(KJS::ExecState *exec) const;
    /* @property boolean vertical
       @description True if the legend entries are stacked vertically.
    */
    void setVertical(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value vertical(KJS::ExecState *exec) const;
    /* @property CurveCollection curves
       @description The list of curves shown in the legend.
       @readonly
    */
    KJS::Value curves(KJS::ExecState *exec) const;

  protected:
    KstBindLegend(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
