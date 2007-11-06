/***************************************************************************
                          psd.h: Power Spectra for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

/** A class for handling power spectra for kst
 *@author C. Barth Netterfield
 */

#ifndef PSD_H
#define PSD_H

#include "dataobject.h"
#include "psdcalculator.h"
#include "kst_export.h"

namespace Kst {

class ObjectStore;

class KST_EXPORT PSD : public DataObject {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    static const QString staticTypeTag;
    const QString& typeString() const { return staticTypeString; }

    virtual UpdateType update(int update_counter = -1);

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

    double freq() const;
    void setFreq(double in_freq);

    int len() const;
    void setLen(int in_len);

    QString vTag() const;
    void setVector(VectorPtr);

    const QString& vUnits() const;
    void setVUnits(const QString& units);

    const QString& rUnits() const;
    void setRUnits(const QString& units);

    PSDType output() const;
    void setOutput(PSDType in_output);

    bool interpolateHoles() const;
    void setInterpolateHoles(bool interpolate);

    virtual bool slaveVectorsUsed() const;

    virtual void showNewDialog();
    virtual void showEditDialog();

    virtual ObjectTag xVTag() const { return _fVector->tag(); }
    virtual ObjectTag yVTag() const { return _sVector->tag(); }

    VectorPtr vX() const { return _fVector; }
    VectorPtr vY() const { return _sVector; }

    const CurveHintList *curveHints() const;

    virtual DataObjectPtr makeDuplicate(DataObjectDataObjectMap& duplicatedMap);

  protected:
    PSD(ObjectStore *store, const ObjectTag& in_tag);
    PSD(ObjectStore *store, const ObjectTag& in_tag, VectorPtr in_V, double freq,
        bool average, int average_len,
        bool in_apodize, bool in_removeMean,
        const QString& VUnits, const QString& RUnits, ApodizeFunction in_apodizeFxn = WindowOriginal, double in_gaussianSigma = 3.0, PSDType in_output = PSDAmplitudeSpectralDensity);
    PSD(ObjectStore *store, const QDomElement& e);
    virtual ~PSD();

    friend class ObjectStore;

  private:
    void commonConstructor(ObjectStore *store, VectorPtr in_V,
        double freq, bool average, int average_len, bool apodize, bool removeMean,
        const QString& VUnits, const QString& RUnits, ApodizeFunction in_apodizeFxn,
        double in_gaussianSigma, PSDType in_output, bool interpolateHoles);
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
    double _Freq;

    int _PSDLen;
    int _averageLen;

    PSDCalculator _psdCalculator;

    QString _vUnits;
    QString _rUnits;

    VectorPtr _sVector, _fVector;
};

typedef SharedPtr<PSD> PSDPtr;
typedef ObjectList<PSD> PSDList;


}

#endif
// vim: ts=2 sw=2 et
