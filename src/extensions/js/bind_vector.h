/***************************************************************************
                                 bind_vector.h
                             -------------------
    begin                : Mar 23 2005
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

#ifndef BIND_VECTOR_H
#define BIND_VECTOR_H

#include "bind_object.h"

#include <kstvector.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class Vector
   @inherits Object
   @collection VectorCollection Kst.vectors
   @description Represents a vector of any type in Kst.  Some vectors are
                editable, while others are not.  This class behaves just as a
                JavaScript array, but has additional properties and methods.
*/
class KstBindVector : public KstBindObject {
  public:
    /* @constructor
       @description Default constructor creates an empty, editable vector.
    */
    KstBindVector(KJS::ExecState *exec, KstVectorPtr v, const char *name = 0L);
    KstBindVector(KJS::ExecState *exec, KJS::Object *globalObject = 0L, const char *name = 0L);
    ~KstBindVector();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    KJS::Value getPropertyByIndex(KJS::ExecState *exec, unsigned propertyName) const;
    void putPropertyByIndex(KJS::ExecState *exec, unsigned propertyName, const KJS::Value &value, int attr = KJS::None);
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    int methodCount() const;
    int propertyCount() const;

    /* @method resize
       @description Resizes the vector.
       @arg number size The new size of the vector.  Should be &gt;= 2.
       @exception GeneralError Throws this exception if the vector is not
                               editable.
    */
    KJS::Value resize(KJS::ExecState *exec, const KJS::List& args);
    /* @method interpolate
       @returns number
       @description Interpolates the vector to <i>n</i> samples and
                    returns the interpolated value of sample <i>i</i>.
       @arg number i The sample number to obtain the interpolated value of.
       @arg number n The number of samples to interpolate to.
    */
    KJS::Value interpolate(KJS::ExecState *exec, const KJS::List& args);
    /* @method zero
       @description Sets all values in the vector to zero.
       @exception GeneralError Throws this exception if the vector is not editable.
    */
    KJS::Value zero(KJS::ExecState *exec, const KJS::List& args);
    /* @property number length
       @readonly
       @description The number of samples in the vector.
    */
    KJS::Value length(KJS::ExecState *exec) const;
    /* @property number min
       @readonly
       @description The value of the smallest sample in the vector.
    */
    KJS::Value min(KJS::ExecState *exec) const;
    /* @property number max
       @readonly
       @description The value of the largest sample in the vector.
    */
    KJS::Value max(KJS::ExecState *exec) const;
    /* @property number mean
       @readonly
       @description The mean value of all samples in the vector.
    */
    KJS::Value mean(KJS::ExecState *exec) const;
    /* @property number numNew
       @readonly
    */
    KJS::Value numNew(KJS::ExecState *exec) const;
    /* @property number numShifted
       @readonly
    */
    KJS::Value numShifted(KJS::ExecState *exec) const;
    /* @property number editable
       @readonly
       @description True if the vector is editable by the user.  This should be
                    checked before attempting to modify a vector in any way.
    */
    KJS::Value editable(KJS::ExecState *exec) const;

  protected:
    KstBindVector(int id, const char *name = 0L);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
};


#endif

// vim: ts=2 sw=2 et
