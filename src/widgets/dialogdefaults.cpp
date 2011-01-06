/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dialogdefaults.h"
#include "datasource.h"

namespace Kst {
  QSettings *_dialogDefaults;

void setDataVectorDefaults(DataVectorPtr V) {
  _dialogDefaults->setValue("vector/datasource", V->filename());
}

void setGenVectorDefaults(GeneratedVectorPtr V) {
  _dialogDefaults->setValue("genVector/min", V->min());
  _dialogDefaults->setValue("genVector/max", V->max()); 
  _dialogDefaults->setValue("genVector/length", V->length());
}

void setDataMatrixDefaults(DataMatrixPtr M) {
  //qDebug() << "M...filename: " << M->dataSource()->fileName();

  // FIXME: data source filename isn't valid...
  _dialogDefaults->setValue("matrix/datasource",M->dataSource()->fileName());

  _dialogDefaults->setValue("matrix/xCountFromEnd",M->xCountFromEnd());
  _dialogDefaults->setValue("matrix/yCountFromEnd",M->yCountFromEnd());
  _dialogDefaults->setValue("matrix/xReadToEnd",M->xReadToEnd());
  _dialogDefaults->setValue("matrix/yReadToEnd",M->yReadToEnd());

  _dialogDefaults->setValue("matrix/xNumSteps",M->xNumSteps());
  _dialogDefaults->setValue("matrix/yNumSteps",M->yNumSteps());
  _dialogDefaults->setValue("matrix/reqXStart",M->reqXStart());
  _dialogDefaults->setValue("matrix/reqYStart",M->reqYStart());
}

void setHistogramDefaults(HistogramPtr H) {  
  _dialogDefaults->setValue("histogram/realTimeAutoBin", H->realTimeAutoBin());
  _dialogDefaults->setValue("histogram/normalizationType",H->normalizationType());
}

}
