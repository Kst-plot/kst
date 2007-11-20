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

#include "dialoglaunchergui.h"

#include "application.h"
#include "curvedialog.h"
#include "equationdialog.h"
#include "histogramdialog.h"
#include "vectordialog.h"
#include "scalardialog.h"
#include "matrixdialog.h"
#include "powerspectrumdialog.h"
#include "csddialog.h"
#include "imagedialog.h"
#include "eventmonitordialog.h"
#include "basicplugindialog.h"

namespace Kst {

DialogLauncherGui::DialogLauncherGui() {
}


DialogLauncherGui::~DialogLauncherGui() {
}


void DialogLauncherGui::showVectorDialog(ObjectPtr objectPtr) {
  VectorDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}


void DialogLauncherGui::showMatrixDialog(ObjectPtr objectPtr) {
  MatrixDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}


void DialogLauncherGui::showScalarDialog(ObjectPtr objectPtr) {
  ScalarDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}


void DialogLauncherGui::showStringDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncherGui::showCurveDialog(ObjectPtr objectPtr, VectorPtr vector) {
  CurveDialog dialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog.setVector(vector);
  }
  dialog.exec();
}


void DialogLauncherGui::showImageDialog(ObjectPtr objectPtr, MatrixPtr matrix) {
  ImageDialog dialog(objectPtr, kstApp->mainWindow());
  if (matrix) {
    dialog.setMatrix(matrix);
  }
  dialog.exec();
}


void DialogLauncherGui::showEquationDialog(ObjectPtr objectPtr) {
  EquationDialog(objectPtr, kstApp->mainWindow()).exec();
}


void DialogLauncherGui::showHistogramDialog(ObjectPtr objectPtr, VectorPtr vector) {
  HistogramDialog dialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog.setVector(vector);
  }
  dialog.exec();
}


void DialogLauncherGui::showPowerSpectrumDialog(ObjectPtr objectPtr, VectorPtr vector) {
  PowerSpectrumDialog dialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog.setVector(vector);
  }
  dialog.exec();
}


void DialogLauncherGui::showCSDDialog(ObjectPtr objectPtr, VectorPtr vector) {
  CSDDialog dialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog.setVector(vector);
  }
  dialog.exec();
}


void DialogLauncherGui::showEventMonitorDialog(ObjectPtr objectPtr) {
  EventMonitorDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}


void DialogLauncherGui::showBasicPluginDialog(ObjectPtr objectPtr) {
  BasicPluginDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}

}

// vim: ts=2 sw=2 et
