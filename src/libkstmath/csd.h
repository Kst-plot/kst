/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005 by University of British Columbia                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** A class for handling cumulative spectral decay for kst
 */

#ifndef CSD_H
#define CSD_H

#include "dataobject.h"
#include "psdcalculator.h"
#include "kst_export.h"

namespace Kst {

class KST_EXPORT CSD : public DataObject {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }
    static const QString staticTypeTag;

    virtual UpdateType update();

    virtual void save(QXmlStreamWriter &s);
    virtual QString propertyString() const;

    void setVector(VectorPtr);
    VectorPtr vector() const;

    virtual bool slaveVectorsUsed() const;

    virtual void showNewDialog();
    virtual void showEditDialog();

    bool apodize() const;
    void setApodize(bool in_apodize);

    bool removeMean() const;
    void setRemoveMean(bool in_removeMean);

    bool average() const;
    void setAverage(bool in_average);

    double frequency() const;
    void setFrequency(double in_frequency);

    ApodizeFunction apodizeFxn() const;
    void setApodizeFxn(ApodizeFunction in_fxn);

    double gaussianSigma() const;
    void setGaussianSigma(double in_sigma);

    int windowSize() const;
    void setWindowSize(int in_size);

    int length() const;
    void setLength(int in_length);

    const QString& vectorUnits() const;
    void setVectorUnits(const QString& units);

    const QString& rateUnits() const;
    void setRateUnits(const QString& units);

    PSDType output() const;
    void setOutput(PSDType in_outputType);

    MatrixPtr outputMatrix() const;

    virtual DataObjectPtr makeDuplicate();

    void change(VectorPtr in_V, double in_freq, bool in_average,
        bool in_removeMean, bool in_apodize, ApodizeFunction in_apodizeFxn,
        int in_windowSize, int in_length, double in_gaussianSigma,
        PSDType in_outputType, const QString& in_vectorUnits,
        const QString& in_rateUnits);
  protected:
    CSD(ObjectStore *store, const ObjectTag &in_tag);
    CSD(ObjectStore *store, const ObjectTag &in_tag, VectorPtr in_V, double in_freq, bool in_average, bool in_removeMean,
           bool in_apodize, ApodizeFunction in_apodizeFxn, int in_windowSize, int in_length,
           double in_gaussianSigma, PSDType in_outputType, const QString& in_vectorUnits = QString::null,
           const QString& in_rateUnits = QString::null);
    CSD(ObjectStore *store, const QDomElement& e);
    virtual ~CSD();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

  private:
    void commonConstructor(ObjectStore *store, VectorPtr in_V, double in_freq, bool in_average,
                            bool in_removeMean, bool in_apodize, ApodizeFunction in_apodizeFxn,
                            int in_windowSize, int in_length, double in_gaussianSigma,
                            const QString& in_vectorUnits, const QString& in_rateUnits,
                            PSDType in_outputType, const QString& vecName);

    void updateMatrixLabels();

    double _frequency;
    bool _average;
    bool _removeMean;
    bool _apodize;
    ApodizeFunction _apodizeFxn;
    PSDType _outputType;
    double _gaussianSigma;
    int _windowSize;
    int _averageLength;
    int _length;
    QString _vectorUnits;
    QString _rateUnits;

    PSDCalculator _psdCalculator;

    // output matrix
    MatrixPtr _outMatrix;
};

typedef SharedPtr<CSD> CSDPtr;
typedef ObjectList<CSD> CSDList;


}

#endif
// vim: ts=2 sw=2 et
