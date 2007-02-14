/***************************************************************************
                           bind_timeinterpretation.h
                             -------------------
    begin                : Jan 16 2006
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

#ifndef BIND_TIMEINTERPRETATION_H
#define BIND_TIMEINTERPRETATION_H

#include "kstbinding.h"
#include "bind_axis.h"

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <qguardedptr.h>

class KstBindAxis;

/* @class TimeInterpretation
   @description A class representing a time interpretation.  It is used to
                convert values from one set of units to display in another.
*/
class KstBindTimeInterpretation : public KstBinding {
  public:
    KstBindTimeInterpretation(KJS::ExecState *exec, KstBindAxis* d);
    ~KstBindTimeInterpretation();

    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // properties
    /* @property boolean active
       @description True if the axis is being interpreted.
    */
    KJS::Value active(KJS::ExecState *exec) const;
    void setActive(KJS::ExecState *exec, const KJS::Value& value);
    /* @property string axisType
       @readonly
       @description The type of axis this interpretation applies to.
    */
    KJS::Value axisType(KJS::ExecState *exec) const;
    /* @property number input
       @description The format of the input to the interpretation.
                    Must be one of:
                    <ul>
                    <li>0 - Standard C Time</li>
                    <li>1 - TAI</li>
                    <li>2 - JD</li>
                    <li>3 - MJD</li>
                    <li>4 - RJD</li>
                    <li>5 - JY</li>
                    </ul>
     */
    void setInput(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value input(KJS::ExecState *exec) const;
    /* @property number output
       @description The format to convert the interpretation to.
                    Must be one of:
                    <ul>
                    <li>0 - DD/MM/YY HH:MM:SS</li>
                    <li>1 - YY/MM/DD HH:MM:SS</li>
                    <li>2 - JD</li>
                    <li>3 - MJD</li>
                    <li>4 - RJD</li>
                    <li>5 - JY</li>
                    <li>6 - Localized Short Date</li>
                    <li>7 - Localized Long Date</li>
                    </ul>
     */
    void setOutput(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value output(KJS::ExecState *exec) const;

  protected:
    KstBindTimeInterpretation(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    QGuardedPtr<KstBindAxis> _d;
};


#endif

// vim: ts=2 sw=2 et
