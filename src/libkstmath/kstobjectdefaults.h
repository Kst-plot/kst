/***************************************************************************
                             kstobjectdefaults.h
                             -------------------
    begin                : May 28, 2004
    copyright            : (C) 2004 The University of Toronto
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

#ifndef KSTOBJECTDEFAULTS_H
#define KSTOBJECTDEFAULTS_H

#include <qstring.h>
#include "kst_export.h"

class QSettings;

class KST_EXPORT KstObjectDefaults {
  public:
    KstObjectDefaults();
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
};

namespace KST {
  extern KST_EXPORT KstObjectDefaults objectDefaults;
}

#endif
// vim: ts=2 sw=2 et
