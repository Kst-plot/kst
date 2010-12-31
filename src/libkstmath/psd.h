/***************************************************************************
                          psd.h: Power Spectra for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** A class for handling power spectra for kst
 *@author C. Barth Netterfield
 */

#ifndef PSD_H
#define PSD_H

#include "dataobject.h"
#include "psdcalculator.h"
#include "kstmath_export.h"

namespace Kst {

class ObjectStore;

class KSTMATH_EXPORT PSD : public DataObject {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    static const QString staticTypeTag;
    const QString& typeString() const { return staticTypeString; }

    virtual void save(QXmlStreamWriter &s);
    virtual QString propertyString() const;

    bool apodize() const;
    void setApodize(bool in_apodize);

    ApodizeFunction apodizeFxn() const;
    void setApodizeFxn(ApodizeFunction in_apodizeFxn);

    double gaussianSigma() const;
    void setGaussianSigma(double in_gaussianSigma);

    bool removeMean() const;
    void setRemoveMean(bool in_removeMean);

    bool average() const;
    void setAverage(bool in_average);

    double frequency() const;
    void setFrequency(double in_frequency);

    int length() const;
    void setLength(int in_length);

    void setVector(VectorPtr);
    VectorPtr vector() const;

    const QString& vectorUnits() const;
    void setVectorUnits(const QString& units);

    const QString& rateUnits() const;
    void setRateUnits(const QString& units);

    PSDType output() const;
    void setOutput(PSDType in_output);

    bool interpolateHoles() const;
    void setInterpolateHoles(bool interpolate);

    virtual bool slaveVectorsUsed() const;

    virtual void showNewDialog();
    virtual void showEditDialog();

    VectorPtr vX() const { return _fVector; }
    VectorPtr vY() const { return _sVector; }

    const CurveHintList *curveHints() const;

    virtual DataObjectPtr makeDuplicate() const;

    virtual QString descriptionTip() const;

    void change(VectorPtr in_V,
        double freq, bool average, int average_len, bool apodize, bool removeMean,
        const QString& VUnits, const QString& RUnits, ApodizeFunction in_apodizeFxn = WindowOriginal, 
        double in_gaussianSigma = 3.0, PSDType in_output = PSDAmplitudeSpectralDensity, bool interpolateHoles = false);
    virtual void internalUpdate();

  protected:

    PSD(ObjectStore *store);

#ifdef KST_USE_QSHAREDPOINTER
  public:
#endif
    virtual ~PSD();

  protected:

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;
    virtual void _initializeShortName();

  private:
    void updateVectorLabels();

    void _adjustLengths();
    ApodizeFunction _apodizeFxn;
    double _gaussianSigma;
    bool _Apodize;
    bool _RemoveMean;
    bool _Average;
    bool _interpolateHoles;
    PSDType _Output;
    PSDType _prevOutput;
    int _last_n_subsets;
    int _last_n_new;
    double _Frequency;

    int _PSDLength;
    int _averageLength;

    PSDCalculator _psdCalculator;

    QString _vectorUnits;
    QString _rateUnits;

    VectorPtr _sVector, _fVector;
};

typedef SharedPtr<PSD> PSDPtr;
typedef ObjectList<PSD> PSDList;


}

#endif
// vim: ts=2 sw=2 et
