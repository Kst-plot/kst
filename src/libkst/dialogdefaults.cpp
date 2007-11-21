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
}

void Kst::setDataVectorDefaults(DataVectorPtr V) {
  //FIXME Do we need a V->readLock() here?
  Kst::dialogDefaults->setValue("vector/datasource", V->filename());
  Kst::dialogDefaults->setValue("vector/range", V->reqNumFrames());
  Kst::dialogDefaults->setValue("vector/start", V->reqStartFrame());
  Kst::dialogDefaults->setValue("vector/countFromEnd", V->countFromEOF());
  Kst::dialogDefaults->setValue("vector/readToEnd", V->readToEOF());
  Kst::dialogDefaults->setValue("vector/skip", V->skip());
  Kst::dialogDefaults->setValue("vector/doSkip", V->doSkip());
  Kst::dialogDefaults->setValue("vector/doAve", V->doAve());
}

void Kst::setGenVectorDefaults(GeneratedVectorPtr V) {
  //FIXME Do we need a V->readLock() here? 
  Kst::dialogDefaults->setValue("genVector/min", V->min());
  Kst::dialogDefaults->setValue("genVector/max", V->max()); 
  Kst::dialogDefaults->setValue("genVector/length", V->length());
}

