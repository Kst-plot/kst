/***************************************************************************
                                bind_curve.h
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

#ifndef BIND_CURVE_H
#define BIND_CURVE_H

#include "bind_dataobject.h"

#include <kstvcurve.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Curve
   @inherits DataObject
   @collection CurveCollection Kst.objects.curves
   @description Represents a curve formed from an X and a Y vector.
*/
class KstBindCurve : public KstBindDataObject {
  public:
    /* @constructor
       @description Main constructor for Curve class.  Constructing a new
                    curve automatically adds it to the data collection of Kst.
       @arg Vector x The X vector for the curve.  Can be specified as a string
                     containing the tag name of an existing vector, or as a
                     vector object.
       @arg Vector y The Y vector for the curve.  Can be specified as a string
                     containing the tag name of an existing vector, or as a
                     vector object.
       @optarg Vector xError The X error bar vector for the curve.  Can be
                             specified as a string containing the tag name of
                             an existing vector, or as a vector object.
       @optarg Vector yError The Y error bar vector for the curve.  Can be
                             specified as a string containing the tag name of
                             an existing vector, or as a vector object.
       @optarg Vector xMinusError The X minus error bar vector for the curve.
                                  Can be specified as a string containing the
                                  tag name of an existing vector, or as a
                                  vector object.
       @optarg Vector yMinusError The Y minus error bar vector for the curve.
                                  Can be specified as a string containing the
                                  tag name of an existing vector, or as a
                                  vector object.
    */
    KstBindCurve(KJS::ExecState *exec, KstVCurvePtr d);
    KstBindCurve(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindCurve();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    /* @method point
       @returns Point
       @description Retrieve the coordinates for a given point in the curve.
                    Returns an invalid point if that index does not exist.
       @arg number index The index of the point to retrieve the coordinates
                         for. Starts at 0.
    */
    KJS::Value point(KJS::ExecState *exec, const KJS::List& args);
    /* @method xErrorPoint
       @returns Point
       @description Retrieve the coordinates for a given point in the X error
                    bars.  Returns an invalid point if that index does not
                    exist.
       @arg number index The index of the point to retrieve the coordinates
                         for. Starts at 0.
    */
    KJS::Value xErrorPoint(KJS::ExecState *exec, const KJS::List& args);
    /* @method yErrorPoint
       @returns Point
       @description Retrieve the coordinates for a given point in the Y error
                    bars.  Returns an invalid point if that index does not
                    exist.
       @arg number index The index of the point to retrieve the coordinates
                         for. Starts at 0.
    */
    KJS::Value yErrorPoint(KJS::ExecState *exec, const KJS::List& args);
    /* @method xMinusErrorPoint
       @returns Point
       @description Retrieve the coordinates for a given point in the X minus
                    error bars.  Returns an invalid point if that index does
                    not exist.
       @arg number index The index of the point to retrieve the coordinates
                         for. Starts at 0.
    */
    KJS::Value xMinusErrorPoint(KJS::ExecState *exec, const KJS::List& args);
    /* @method yMinusErrorPoint
       @returns Point
       @description Retrieve the coordinates for a given point in the Y minus
                    error bars.  Returns an invalid point if that index does
                    not exist.
       @arg number index The index of the point to retrieve the coordinates
                         for. Starts at 0.
    */
    KJS::Value yMinusErrorPoint(KJS::ExecState *exec, const KJS::List& args);


    /* @property string color
       @description The color of the curve.  Stored in the form "#RRGGBB" as
                    hex values.  This property can also be set with English
                    strings such as "blue".
    */
    void setColor(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value color(KJS::ExecState *exec) const;
    /* @property Vector xVector
       @description The X axis vector for the curve.
    */
    void setXVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value xVector(KJS::ExecState *exec) const;
    /* @property Vector yVector
       @description The Y axis vector for the curve.
    */
    void setYVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value yVector(KJS::ExecState *exec) const;
    /* @property Vector xErrorVector
       @description The X axis error vector for the curve.
    */
    void setXErrorVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value xErrorVector(KJS::ExecState *exec) const;
    /* @property Vector yErrorVector
       @description The Y axis error vector for the curve.
    */
    void setYErrorVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value yErrorVector(KJS::ExecState *exec) const;
    /* @property Vector xMinusErrorVector
       @description The X axis negative error vector for the curve.
    */
    void setXMinusErrorVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value xMinusErrorVector(KJS::ExecState *exec) const;
    /* @property Vector xMinusErrorVector
       @description The X axis negative error vector for the curve.
    */
    void setYMinusErrorVector(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value yMinusErrorVector(KJS::ExecState *exec) const;
    /* @property number samplesPerFrame
       @readonly
       @description The number of samples per frame for the curve.
    */
    KJS::Value samplesPerFrame(KJS::ExecState *exec) const;
    /* @property boolean ignoreAutoScale
       @description If true, this curve ignores auto scale.
    */
    void setIgnoreAutoScale(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value ignoreAutoScale(KJS::ExecState *exec) const;
    /* @property boolean hasPoints
       @description If true, points are plotted for this curve.
    */
    void setHasPoints(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value hasPoints(KJS::ExecState *exec) const;
    /* @property boolean hasLines
       @description If true, lines are drawn for this curve.
    */
    void setHasLines(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value hasLines(KJS::ExecState *exec) const;
    /* @property boolean hasBars
       @description If true, bars are drawn for this curve.
    */
    void setHasBars(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value hasBars(KJS::ExecState *exec) const;
    /* @property number lineWidth
       @description Sets the line width for this curve when lines are drawn.
    */
    void setLineWidth(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value lineWidth(KJS::ExecState *exec) const;
    /* @property number lineStyle
       @description Sets the line style for this curve when lines are drawn.
                    <ul>
                    <li>0 - Cross</li>
                    <li>1 - Hollow box</li>
                    <li>2 - Hollow circle</li>
                    <li>3 - Filled circle</li>
                    <li>4 - Inverted hollow triangle</li>
                    <li>5 - Hollow triangle</li>
                    <li>6 - Filled box</li>
                    <li>7 - Plus</li>
                    <li>8 - Asterisk</li>
                    <li>9 - Inverted filled triangle</li>
                    <li>10 - Filled triangle</li>
                    <li>11 - Hollow diamond</li>
                    <li>12 - Filled diamond</li>
                    </ul>
    */
    void setLineStyle(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value lineStyle(KJS::ExecState *exec) const;
    /* @property number barStyle
       @description Sets the bar style for this curve when bars are drawn.
                    <ul>
                    <li>0 - Lines</li>
                    <li>1 - Solid</li>
                    </ul>
    */
    void setBarStyle(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value barStyle(KJS::ExecState *exec) const;
    /* @property number pointDensity
       @description Sets the point density for this curve when points are plotted.
                    <ul>
                    <li>0 - Draw all points</li>
                    <li>1 - High density</li>
                    <li>2 - Medium density</li>
                    <li>3 - Low density</li>
                    </ul>
    */
    void setPointDensity(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value pointDensity(KJS::ExecState *exec) const;
    /* @property string topLabel
       @readonly
       @description The top label suggestion for this curve.
    */
    KJS::Value topLabel(KJS::ExecState *exec) const;
    /* @property string xLabel
       @readonly
       @description The X-axis label suggestion for this curve.
    */
    KJS::Value xLabel(KJS::ExecState *exec) const;
    /* @property string yLabel
       @readonly
       @description The Y-axis label suggestion for this curve.
    */
    KJS::Value yLabel(KJS::ExecState *exec) const;

  protected:
    KstBindCurve(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindDataObject *bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
