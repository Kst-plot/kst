/***************************************************************************
                          kstpsd.h: Power Spectra for KST
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

#ifndef KSTPSD_H
#define KSTPSD_H

#include "dataobject.h"
#include "psdcalculator.h"
#include "kst_export.h"

class KST_EXPORT KstPSD : public Kst::DataObject {
  public:
    KstPSD(const QString& in_tag, Kst::VectorPtr in_V, double freq,
        bool average, int average_len,
        bool in_apodize, bool in_removeMean,
        const QString& VUnits, const QString& RUnits, ApodizeFunction in_apodizeFxn = WindowOriginal, double in_gaussianSigma = 3.0, PSDType in_output = PSDAmplitudeSpectralDensity);
    KstPSD(const QDomElement& e);
    virtual ~KstPSD();

    virtual UpdateType update(int update_counter = -1);

    virtual void save(QTextStream& ts, const QString& indent = QString::null);
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
    void setVector(Kst::VectorPtr);

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

    virtual QString xVTag() const { return (*_fVector)->tagName(); }
    virtual QString yVTag() const { return (*_sVector)->tagName(); }

    Kst::VectorPtr vX() const { return *_fVector; }
    Kst::VectorPtr vY() const { return *_sVector; }

    const KstCurveHintList *curveHints() const;
    
    virtual Kst::DataObjectPtr makeDuplicate(Kst::DataObjectDataObjectMap& duplicatedMap);

  private:
    void commonConstructor(const QString& in_tag, Kst::VectorPtr in_V,
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

    Kst::VectorMap::Iterator _sVector, _fVector;
};

typedef KstSharedPtr<KstPSD> KstPSDPtr;
typedef KstObjectList<KstPSDPtr> KstPSDList;

#endif
// vim: ts=2 sw=2 et
