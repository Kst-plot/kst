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
  QSettings *_dialogDefaults = new QSettings("kstdialogrc", QSettings::NativeFormat);

void setDataVectorDefaults(DataVectorPtr V) {
  //FIXME Do we need a V->readLock() here?
  _dialogDefaults->setValue("vector/datasource", V->filename());
  _dialogDefaults->setValue("vector/range", V->reqNumFrames());
  _dialogDefaults->setValue("vector/start", V->reqStartFrame());
  _dialogDefaults->setValue("vector/countFromEnd", V->countFromEOF());
  _dialogDefaults->setValue("vector/readToEnd", V->readToEOF());
  _dialogDefaults->setValue("vector/skip", V->skip());
  _dialogDefaults->setValue("vector/doSkip", V->doSkip());
  _dialogDefaults->setValue("vector/doAve", V->doAve());
}

void setGenVectorDefaults(GeneratedVectorPtr V) {
  //FIXME Do we need a V->readLock() here? 
  _dialogDefaults->setValue("genVector/min", V->min());
  _dialogDefaults->setValue("genVector/max", V->max()); 
  _dialogDefaults->setValue("genVector/length", V->length());
}

void setSpectrumDefaults(PSDPtr P) {
  //FIXME Do we need a P->readLock() here?
  //NOTE: we are storing enums as ints here... so there might be
  //      odd behavior if the enums change between recompiles.
  _dialogDefaults->setValue("spectrum/freq", P->frequency());
  _dialogDefaults->setValue("spectrum/average", P->average());
  _dialogDefaults->setValue("spectrum/len", P->length());
  _dialogDefaults->setValue("spectrum/apodize", P->apodize());
  _dialogDefaults->setValue("spectrum/removeMean", P->removeMean());
  _dialogDefaults->setValue("spectrum/vUnits", P->vectorUnits());
  _dialogDefaults->setValue("spectrum/rUnits", P->rateUnits());
  _dialogDefaults->setValue("spectrum/apodizeFxn", P->apodizeFxn());
  _dialogDefaults->setValue("spectrum/gaussianSigma", P->gaussianSigma());
  _dialogDefaults->setValue("spectrum/output", P->output());
  _dialogDefaults->setValue("spectrum/interpolateHoles", P->interpolateHoles());
}

}
