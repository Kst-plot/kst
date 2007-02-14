/***************************************************************************
                               bind_equation.h
                             -------------------
    begin                : Mar 29 2005
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

#ifndef BIND_EQUATION_H
#define BIND_EQUATION_H

#include "bind_dataobject.h"

#include <kstequation.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Equation
   @inherits DataObject
   @description  This object represents an equation object in Kst.  It is
                 interpreted using Kst's internal equation interpreter, and
                 not the JavaScript interpreter.  It should be considerably
                 faster than the JavaScript interpreter.
   @collection EquationCollection Kst.objects.equations
*/

class KstBindEquation : public KstBindDataObject {
  public:
   /* @constructor
      @description The default constructor to create an equation object.
      @arg string equation The text of the equation to interpret.
      @arg Vector xVector The X axis (input) vector to use.  The symbolic
                          variable <i>x</i>; represents the value at the
                          current index in this vector.  A text string
                          representing the name of an existing vector may
                          also be used here.
      @optarg boolean interpolate If true, the equation interpreter will
                                  interoplate to the highest resolution
                                  vector.  This is the default behavior.

    */
    /* @constructor
       @description A utility constructor which generates an X vector
                    implicitly.
       @arg string equation The text of the equation to interpret.
       @arg number x0 The first value of the generated X (input) vector.
       @arg number x1 The last value of the generate X (input) vector.
       @arg number n The number of values in the generated input vector.
    */
    KstBindEquation(KJS::ExecState *exec, KstEquationPtr d);
    KstBindEquation(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindEquation();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions

    /* @property string equation
       @description The text of the equation.  See the Kst documentation for
                    more information on what the internal equation interpreter
                    supports.
     */
    void setEquation(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value equation(KJS::ExecState *exec) const;
    /* @property boolean valid
       @readonly
       @description True if the equation object is valid.  If false, any
                    values obtained from the yVector should be considered
                    meaningless.
    */
    KJS::Value valid(KJS::ExecState *exec) const;
    /* @property Vector xVector
       @readonly
       @description The X vector (input vector) for the equation.
    */
    KJS::Value xVector(KJS::ExecState *exec) const;
    /* @property Vector yVector
       @readonly
       @description The Y vector (output vector) for the equation.
    */
    KJS::Value yVector(KJS::ExecState *exec) const;

  protected:
    KstBindEquation(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindDataObject *bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
