/***************************************************************************
                                 bind_plot.h
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

#ifndef BIND_PLOT_H
#define BIND_PLOT_H

#include "bind_borderedviewobject.h"

#include <kst2dplot.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Plot
   @inherits BorderedViewObject
   @collection PlotCollection
   @description This class represents a plot in Kst.
*/
class KstBindPlot : public KstBindBorderedViewObject {
  public:
    /* @constructor
       @arg Window window The window to place the new plot in.  May also be a
                          string containing the name of an existing Window.
       @description Creates a new plot and places it in the Window <i>window</i>.
    */
    KstBindPlot(KJS::ExecState *exec, Kst2DPlotPtr d);
    KstBindPlot(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindPlot();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    int methodCount() const;
    int propertyCount() const;

    // member functions

    /* @property CurveCollection curves
       @readonly
       @description A list of all the Curves used by the plot.
    */
    KJS::Value curves(KJS::ExecState *exec) const;
    /* @property Legend legend
       @readonly
       @description The Legend for this plot.  May be null.
    */
    KJS::Value legend(KJS::ExecState *exec) const;
    /* @method createLegend
       @returns Legend
       @description Creates a Legend for this plot.  If a legend already exists
                    then that one is returned.
    */
    KJS::Value createLegend(KJS::ExecState *exec, const KJS::List& args);
    /* @property string topLabel
       @description The top label for this plot.
    */
    void setTopLabel(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value topLabel(KJS::ExecState *exec) const;
    /* @property Axis xAxis
       @readonly
       @description The X-axis for this plot.
    */
    KJS::Value xAxis(KJS::ExecState *exec) const;
    /* @property Axis yAxis
       @readonly
       @description The Y-axis for this plot.
    */
    KJS::Value yAxis(KJS::ExecState *exec) const;
    /* @property boolean tied
       @description True if the plot zoom state is tied.
    */
    void setTied(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value tied(KJS::ExecState *exec) const;

  protected:
    KstBindPlot(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindViewObject *bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
