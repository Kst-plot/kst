/***************************************************************************
                             dialogdefaults.h
                             -------------------
    begin                : November 13, 2007
    copyright            : (C) 2007 C. Barth Netterfield
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

#ifndef KSTDIALOGDEFAULTS
#define KSTDIALOGDEFAULTS

#include "datavector.h"
#include "psd.h"
#include "generatedvector.h"
#include "datamatrix.h"
#include "histogram.h"
#include "kstwidgets_export.h"

class QSettings;

namespace Kst {
  KSTWIDGETS_EXPORT extern QSettings *_dialogDefaults;

  KSTWIDGETS_EXPORT void setDataVectorDefaults(DataVectorPtr);
  KSTWIDGETS_EXPORT void setGenVectorDefaults(GeneratedVectorPtr V);
  KSTWIDGETS_EXPORT void setDataMatrixDefaults(DataMatrixPtr M);
  KSTWIDGETS_EXPORT void setHistogramDefaults(HistogramPtr H);
}

#endif

// Dialog defaults are stored and retrieved from the Kst::dialogDefaults.
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

// export/filename       QString  exportgraphicsdialog
// export/format         QString  exportgraphicsdialog
// export/xsize                   exportgraphicsdialog
// export/ysize                   exportgraphicsdialog
// export/sizeOption              exportgraphicsdialog

// picture/startdir               pictureitem.cpp

//  histogram/realTimeAutoBin    histogramdialog.cpp

//  matrix/datasource            matrixdialog.cpp
//  matrix/reqXStart             matrixdialog.cpp
//  matrix/reqYStart             matrixdialog.cpp
//  matrix/xCountFromEnd         matrixdialog.cpp
//  matrix/xNumSteps             matrixdialog.cpp
//  matrix/xReadToEnd            matrixdialog.cpp
//  matrix/yCountFromEnd         matrixdialog.cpp
//  matrix/yNumSteps             matrixdialog.cpp
//  matrix/yReadToEnd            matrixdialog.cpp

// wizard/doPSD         bool     datawizard.cpp
// wizard/doXY          bool     datawizard.cpp
// wizard/curvePlacement enum    datawizard.cpp
// wizard/plotCount     int      datawizard.cpp

// changedatafile/newFileName  QString  changefiledialog.cpp
