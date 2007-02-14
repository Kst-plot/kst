/***************************************************************************
                              bind_axis.h
                             -------------------
    begin                : Jan 13 2006
    copyright            : (C) 2006 The University of Toronto
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

#ifndef BIND_AXIS_H
#define BIND_AXIS_H

#include "kstbinding.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <qguardedptr.h>

#include <kst2dplot.h>

/* @class Axis
   @description A class representing a plot axis.
*/
class KstBindAxis : public QObject, public KstBinding {
  public:
    KstBindAxis(KJS::ExecState *exec, Kst2DPlotPtr d, bool isX);
    ~KstBindAxis();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions
    /* @method scaleAuto
       @description Sets the scale mode to Auto.
    */
    KJS::Value scaleAuto(KJS::ExecState *exec, const KJS::List& args);
    /* @method scaleAutoSpikeInsensitive
       @description Sets the scale mode to Auto Spike Insensitive.
    */
    KJS::Value scaleAutoSpikeInsensitive(KJS::ExecState *exec, const KJS::List& args);
    /* @method scaleExpression
       @description Sets the scale mode to Expression.
       @arg string minExp The expression for the minimum of the scale.
       @arg string maxExp The expression for the maximum of the scale.
    */
    KJS::Value scaleExpression(KJS::ExecState *exec, const KJS::List& args);
    /* @method scaleRange
       @description Sets the scale mode to Fixed and sets the range.
       @arg number min The value for the minimum of the scale.
       @arg number max The value for the maximum of the scale.
    */
    KJS::Value scaleRange(KJS::ExecState *exec, const KJS::List& args);

    // properties
    /* @property boolean log
       @description True if the axis is in logarithm mode.
    */
    KJS::Value log(KJS::ExecState *exec) const;
    void setLog(KJS::ExecState *exec, const KJS::Value& value);
    /* @property boolean suppressed
       @description True if this axis is suppressed.
    */
    void setSuppressed(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value suppressed(KJS::ExecState *exec) const;
    /* @property boolean oppositeSuppressed
       @description True if the opposite axis is suppressed.  (Right X or top Y)
    */
    void setOppositeSuppressed(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value oppositeSuppressed(KJS::ExecState *exec) const;
    /* @property boolean offsetMode
       @description True if this axis is in base + offset mode.
    */
    void setOffsetMode(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value offsetMode(KJS::ExecState *exec) const;
    /* @property boolean reversed
       @description True if this axis is reversed.
    */
    void setReversed(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value reversed(KJS::ExecState *exec) const;
    /* @property boolean majorGridLines
       @description True if this axis shows major grid lines.
    */
    void setMajorGridLines(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value majorGridLines(KJS::ExecState *exec) const;
    /* @property boolean minorGridLines
       @description True if this axis shows minor grid lines.
    */
    void setMinorGridLines(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value minorGridLines(KJS::ExecState *exec) const;
    /* @property string transformation
       @description The transformation expression for this axis.
    */
    void setTransformation(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value transformation(KJS::ExecState *exec) const;
    /* @property boolean innerTicks
       @description True if tick marks are displayed inside the plot.
    */
    void setInnerTicks(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value innerTicks(KJS::ExecState *exec) const;
    /* @property boolean outerTicks
       @description True if tick marks are displayed outside the plot.
    */
    void setOuterTicks(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value outerTicks(KJS::ExecState *exec) const;
    /* @property string label
       @description The label for this axis.
    */
    void setLabel(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value label(KJS::ExecState *exec) const;
    /* @property string type
       @readonly
       @description The type of axis - X or Y presently.
    */
    KJS::Value type(KJS::ExecState *exec) const;
    /* @property string majorGridColor
       @description The color for the major grid lines.
     */
    void setMajorGridColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value majorGridColor(KJS::ExecState *exec) const;
    /* @property string minorGridColor
       @description The color for the minor grid lines.
     */
    void setMinorGridColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value minorGridColor(KJS::ExecState *exec) const;
    /* @property number minorTickCount
       @description The number of minor ticks between two major ticks.  The
                    special value -1 forces Kst to determine this automatically.
     */
    void setMinorTickCount(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value minorTickCount(KJS::ExecState *exec) const;
    /* @property number majorTickDensity
       @description The density of major tick markers along this axis.  Value
                    must be one of:
                    <ul>
                    <li>0 - Coarse</li>
                    <li>1 - Default</li>
                    <li>2 - Fine</li>
                    <li>3 - Very Fine</li>
                    </ul>
     */
    void setMajorTickDensity(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value majorTickDensity(KJS::ExecState *exec) const;
    /* @property number scaleMode
       @readonly
       @description The type of scaling done on the axis.
                    Value is one of:
                    <ul>
                    <li>0 - Auto</li>
                    <li>1 - All Curves (By Midpoint)</li>
                    <li>2 - Fixed</li>
                    <li>3 - Auto Up (Only Scales Upward)</li>
                    <li>4 - Auto (Spike Insensitive)</li>
                    <li>5 - Auto (obsolete, same as 0)</li>
                    <li>6 - Expression Based</li>
                    </ul>
     */
    KJS::Value scaleMode(KJS::ExecState *exec) const;
    /* @property TimeInterpretation interpretation
       @readonly
       @description The method with which the axis is interpreted and times are
                    displayed.
     */
    KJS::Value interpretation(KJS::ExecState *exec) const;

  protected:
    friend class KstBindTimeInterpretation;
    KstBindAxis(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    QGuardedPtr<Kst2DPlot> _d;
    bool _xAxis;
};


#endif

// vim: ts=2 sw=2 et
