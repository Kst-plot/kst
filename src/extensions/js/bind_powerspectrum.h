/***************************************************************************
                             bind_powerspectrum.h
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

#ifndef BIND_PSD_H
#define BIND_PSD_H

#include "bind_dataobject.h"

#include <kstpsd.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

/* @class PowerSpectrum
   @inherits DataObject
   @collection PowerSpectrumCollection
   @description This class represents a power spectrum (PSD) object in Kst.
*/
class KstBindPowerSpectrum : public KstBindDataObject {
  public:
    /* @constructor
       @arg Vector vector The vector to use as input to the power spectrum.
       @arg number freq The frequency for the power spectrum.
       @optarg boolean average
       @optarg number len The base 2 logarithm of the length of the power
                          spectrum.  Should be an integer &gt;= 4.
       @optarg boolean apodize If true, sharp discontinuities are removed.
       @optarg boolean removeMean True if the mean should be removed before
                                  performing the transform.
       @description Creates a new power spectrum (PSD) object in Kst.  Units
                    are <i>V</i> and <i>Hz</i> by default.
    */
    KstBindPowerSpectrum(KJS::ExecState *exec, KstPSDPtr d);
    KstBindPowerSpectrum(KJS::ExecState *exec, KJS::Object *globalObject = 0L);
    ~KstBindPowerSpectrum();

    KJS::Object construct(KJS::ExecState *exec, const KJS::List& args);
    KJS::Value call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args);
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;
    void put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr = KJS::None);
    KJS::ReferenceList propList(KJS::ExecState *exec, bool recursive = true);
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const;

    // member functions

    /* @property number length
       @description Contains the base 2 logarithm of the length of the power
                    spectrum.  Should be an integer &gt;= 4.
    */
    void setLength(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value length(KJS::ExecState *exec) const;
    /* @property boolean removeMean
       @description True if the mean should be removed before performing the
                    transform.
    */
    void setRemoveMean(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value removeMean(KJS::ExecState *exec) const;
    /* @property number average
    */
    void setAverage(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value average(KJS::ExecState *exec) const;
    /* @property boolean apodize
       @description If true, sharp discontinuities are removed.
    */
    void setApodize(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value apodize(KJS::ExecState *exec) const;
    /* @property number frequency
       @description Contains the sampling rate of the power spectrum.
    */
    void setFrequency(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value frequency(KJS::ExecState *exec) const;
    /* @property Vector xVector
       @readonly
       @description The X-axis vector for the power spectrum.
    */
    KJS::Value xVector(KJS::ExecState *exec) const;
    /* @property Vector yVector
       @readonly
       @description The Y-axi vector for the power spectrum.
    */
    KJS::Value yVector(KJS::ExecState *exec) const;
    /* @property string vUnits
       @description A string containing the units for the vector.
    */
    void setVUnits(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value vUnits(KJS::ExecState *exec) const;
    /* @property string rUnits
       @description A string containing the units for the rate.
    */
    void setRUnits(KJS::ExecState *exec, const KJS::Value& value);
    KJS::Value rUnits(KJS::ExecState *exec) const;

  protected:
    KstBindPowerSpectrum(int id);
    void addBindings(KJS::ExecState *exec, KJS::Object& obj);
    static KstBindDataObject *bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj);
};


#endif

// vim: ts=2 sw=2 et
