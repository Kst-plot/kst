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

#include "dialogdefaults.h"
namespace Kst {
  QSettings *dialogDefaults = new QSettings("kstdialogrc", QSettings::NativeFormat);

void setDataVectorDefaults(DataVectorPtr V) {
  //FIXME Do we need a V->readLock() here?
  dialogDefaults->setValue("vector/datasource", V->filename());
  dialogDefaults->setValue("vector/range", V->reqNumFrames());
  dialogDefaults->setValue("vector/start", V->reqStartFrame());
  dialogDefaults->setValue("vector/countFromEnd", V->countFromEOF());
  dialogDefaults->setValue("vector/readToEnd", V->readToEOF());
  dialogDefaults->setValue("vector/skip", V->skip());
  dialogDefaults->setValue("vector/doSkip", V->doSkip());
  dialogDefaults->setValue("vector/doAve", V->doAve());
}

void setGenVectorDefaults(GeneratedVectorPtr V) {
  //FIXME Do we need a V->readLock() here? 
  dialogDefaults->setValue("genVector/min", V->min());
  dialogDefaults->setValue("genVector/max", V->max()); 
  dialogDefaults->setValue("genVector/length", V->length());
}

}
