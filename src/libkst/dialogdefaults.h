/***************************************************************************
                             dialogdefaults.h
                             -------------------
    begin                : November 13, 2007
    copyright            : (C) 2007 C. Barth Netterfield
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

#ifndef KSTDIALOGDEFAULTS
#define KSTDIALOGDEFAULTS

#include "datavector.h"
#include "../libkstmath/psd.h"
#include "generatedvector.h"

class QSettings;

namespace Kst {
  extern QSettings *dialogDefaults;

  void setDataVectorDefaults(DataVectorPtr);
  void setGenVectorDefaults(GeneratedVectorPtr V);
  void setSpectrumDefaults(PSDPtr P);
}

#endif

// Dialog defaults are stored and retreived from the Kst::dialogDefaults.
// As you add defaults, add them here!
// Name                   Type    Where used
// vector/datasource      QString vectordialog datawizard
// vector/range           int     vectordialog datawizard
// vector/start           int     vectordialog datawizard
// vector/countFromEnd    bool    vectordialog datawizard
// vector/readToEnd       bool    vectordialog datawizard
// vector/skip            int     vectordialog datawizard
// vector/doSkip          bool    vectordialog datawizard
// vector/doAve           bool    vectordialog datawizard
//
// genVector/min"         int     vectordialog
// genVector/max"         int     vectordialog
// genVector/length       int     vectordialog
// spectrum/freq          double  powerspectrumdailog datawizard
// spectrum/average       bool    powerspectrumdailog datawizard
// spectrum/len           int     powerspectrumdailog datawizard
// spectrum/apodize       bool    powerspectrumdailog datawizard
// spectrum/removeMean    bool    powerspectrumdailog datawizard
// spectrum/vUnits       QString  powerspectrumdailog datawizard
// spectrum/rUnits       QString  powerspectrumdailog datawizard
// spectrum/apodizeFxn   int      powerspectrumdailog datawizard
// spectrum/gaussianSigma double  powerspectrumdailog datawizard
// spectrum/output       int      powerspectrumdailog datawizard
// spectrum/interpolateHoles bool powerspectrumdailog datawizard

