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
#include "vectordialog.h"
#include "scalardialog.h"
#include "matrixdialog.h"

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


void DialogLauncherGui::showCurveDialog(ObjectPtr objectPtr) {
  CurveDialog dialog(objectPtr, kstApp->mainWindow());
  dialog.exec();
}


void DialogLauncherGui::showImageDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncherGui::showEquationDialog(ObjectPtr objectPtr) {
  EquationDialog(objectPtr, kstApp->mainWindow()).exec();
}


void DialogLauncherGui::showHistogramDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncherGui::showPSDDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncherGui::showCSDDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}


void DialogLauncherGui::showBasicPluginDialog(ObjectPtr objectPtr) {
  Q_UNUSED(objectPtr);
}

}

// vim: ts=2 sw=2 et
