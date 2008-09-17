/***************************************************************************
                   crosspowerspectrum.h
                             -------------------
    begin                : 09/08/06
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
#ifndef CROSSPOWERSPECTRUM_H
#define CROSSPOWERSPECTRUM_H

#include <kstdataobject.h>

class CrossPowerSpectrum : public KstDataObject {
  Q_OBJECT
  public:
    CrossPowerSpectrum(QObject *parent, const char *name, const QStringList &args);
    virtual ~CrossPowerSpectrum();

    //algorithm
    void crossspectrum();

    QString v1Tag() const;
    QString v2Tag() const;
    QString fftTag() const;
    QString sampleTag() const;
    QString realTag() const;
    QString imaginaryTag() const;
    QString frequencyTag() const;

    KstVectorPtr v1() const;
    KstVectorPtr v2() const;
    KstScalarPtr fft() const;
    KstScalarPtr sample() const;
    KstVectorPtr real() const;
    KstVectorPtr imaginary() const;
    KstVectorPtr frequency() const;

    void setV1(KstVectorPtr new_v1);
    void setV2(KstVectorPtr new_v2);
    void setFFT(KstScalarPtr new_fft);
    void setSample(KstScalarPtr new_sample);
    void setReal(const QString &name);
    void setImaginary(const QString &name);
    void setFrequency(const QString &name);

    //Pure virtual methods from KstDataObject
    virtual KstObject::UpdateType update(int updateCounter = -1);
    virtual QString propertyString() const;
    virtual KstDataObjectPtr makeDuplicate(KstDataObjectDataObjectMap&);

    //Regular virtual methods from KstDataObject
    virtual void load(const QDomElement &e);
    virtual void save(QTextStream& ts, const QString& indent = QString::null);

  protected slots:
    //Pure virtual slots from KstDataObject
    virtual void showNewDialog();
    virtual void showEditDialog();
};

typedef KstSharedPtr<CrossPowerSpectrum> CrossPowerSpectrumPtr;
typedef KstObjectList<CrossPowerSpectrumPtr> CrossPowerSpectrumList;

#endif
