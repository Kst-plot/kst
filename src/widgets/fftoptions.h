/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FFTOPTIONS_H
#define FFTOPTIONS_H

#include <QWidget>
#include "ui_fftoptions.h"

#include "psd.h"

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT FFTOptions : public QWidget, public Ui::FFTOptions 
{
    Q_OBJECT

  public:
    FFTOptions(QWidget *parent = 0);
    ~FFTOptions();

    double sampleRate() const;
    bool sampleRateDirty() const;
    void setSampleRate(const double sampleRate);

    double sigma() const;
    bool sigmaDirty() const;
    void setSigma(const double sigma);

    bool interleavedAverage() const;
    bool interleavedAverageDirty() const;
    void setInterleavedAverage(const bool interleavedAverage);

    int FFTLength() const;
    bool FFTLengthDirty() const;
    void setFFTLength(const int FFTLength);

    bool apodize() const;
    bool apodizeDirty() const;
    void setApodize(const bool apodize);

    bool removeMean() const;
    bool removeMeanDirty() const;
    void setRemoveMean(const bool removeMean);

    QString vectorUnits() const;
    bool vectorUnitsDirty() const;
    void setVectorUnits(const QString vectorUnits);

    QString rateUnits() const;
    bool rateUnitsDirty() const;
    void setRateUnits(const QString rateUnits);

    ApodizeFunction apodizeFunction() const;
    bool apodizeFunctionDirty() const;
    void setApodizeFunction(const ApodizeFunction apodizeFunction);

    PSDType output() const;
    bool outputDirty() const;
    void setOutput(const PSDType output);

    bool interpolateOverHoles() const;
    bool interpolateOverHolesDirty() const;
    void setInterpolateOverHoles(const bool interpolateOverHoles);

    void disableInterpolateOverHoles();
    void clearValues();

    void setWidgetDefaults();
    void loadWidgetDefaults();

  Q_SIGNALS:
    void modified();

  public slots:
    void changedApodizeFxn();
    void clickedInterleaved();
    void clickedApodize();
    void synch();
    bool checkValues();
    bool checkGivenValues(double sampRate, int FFTLen);

  private:
    void init();
};

}
#endif
// vim: ts=2 sw=2 et
