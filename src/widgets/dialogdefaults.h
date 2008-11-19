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
#include "psd.h"
#include "generatedvector.h"
#include "datamatrix.h"
#include "histogram.h"

class QSettings;

namespace Kst {
  extern QSettings *_dialogDefaults;

  void setDataVectorDefaults(DataVectorPtr);
  void setGenVectorDefaults(GeneratedVectorPtr V);
  void setDataMatrixDefaults(DataMatrixPtr M);
  void setHistogramDefaults(HistogramPtr H);
}

#endif

// Dialog defaults are stored and retreived from the Kst::dialogDefaults.
// As you add defaults, add them here!
// Name                   Type    Where used
// vector/datasource      QString vectordialog datawizard
// vector/range           int     datarange 
// vector/start           int     datarange  
// vector/countFromEnd    bool    datarange 
// vector/readToEnd       bool    datarange 
// vector/skip            int     datarange 
// vector/doSkip          bool    datarange 
// vector/doAve           bool    datarange 
//
// genVector/min"         int     vectordialog
// genVector/max"         int     vectordialog
// genVector/length       int     vectordialog
// spectrum/freq          double  FFToptions 
// spectrum/average       bool    FFToptions 
// spectrum/len           int     FFToptions 
// spectrum/apodize       bool    FFToptions 
// spectrum/removeMean    bool    FFToptions 
// spectrum/vUnits       QString  FFToptions 
// spectrum/rUnits       QString  FFToptions 
// spectrum/apodizeFxn   int      FFToptions 
// spectrum/gaussianSigma double  FFToptions 
// spectrum/output       int      FFToptions 
// spectrum/interpolateHoles bool FFToptions 

// curve/xvectorfield    QString  datawizard, curvedialog
