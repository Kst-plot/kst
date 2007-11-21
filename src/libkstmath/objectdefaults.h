/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OBJECTDEFAULTS_H
#define OBJECTDEFAULTS_H

#include <QString>

#include "kst_export.h"
#include "psd.h"

class QSettings;

namespace Kst {

class ObjectStore;

class KST_EXPORT ObjectDefaults {
  public:
    ObjectDefaults();
    void sync();
    double psdFreq() const;
    int fftLen() const;

    const QString& vUnits() const { return _vUnits; }
    const QString& rUnits() const { return _rUnits; }
    bool apodize() const { return _apodize; }
    bool removeMean() const { return _removeMean; }
    bool psdAverage() const { return _psd_average; }
    int apodizeFxn() const { return _apodizeFxn; }
    int output() const { return _output; }
    int interpolateHoles() const { return _interpolateHoles; }
    void setSpectrumDefaults(PSDPtr P);

    void readConfig(QSettings *config);
    void writeConfig(QSettings *config);

  private:
    double _psd_freq;
    int _fft_len;
    QString _vUnits;
    QString _rUnits;
    bool _apodize;
    bool _removeMean;
    bool _psd_average;
    int _apodizeFxn;
    int _output;
    bool _interpolateHoles;
    ObjectStore *_store;  // FIXME: this must get initialized
};

extern KST_EXPORT ObjectDefaults objectDefaults;

}
#endif
// vim: ts=2 sw=2 et
