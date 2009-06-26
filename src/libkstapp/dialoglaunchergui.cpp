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
#include "stringdialog.h"
#include "matrixdialog.h"
#include "powerspectrumdialog.h"
#include "csddialog.h"
#include "imagedialog.h"
#include "eventmonitordialog.h"
#include "basicplugindialog.h"
#include "filterfitdialog.h"

#include "curve.h"
#include "equation.h"
#include "vector.h"
#include "matrix.h"
#include "histogram.h"
#include "psd.h"
#include "eventmonitorentry.h"
#include "image.h"
#include "csd.h"
#include "basicplugin.h"

namespace Kst {

DialogLauncherGui::DialogLauncherGui() {
}


DialogLauncherGui::~DialogLauncherGui() {
}


void DialogLauncherGui::showVectorDialog(QString &vectorname, ObjectPtr objectPtr, bool modal) {
  VectorDialog *dialog = new VectorDialog(objectPtr, kstApp->mainWindow());
  if (!vectorname.isEmpty()) {
    dialog->setField(vectorname);
  }
  if (modal) {
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);
    dialog->exec();
    vectorname = dialog->dataObjectName();
    delete dialog;
  } else {
    dialog->show();
  }
}


void DialogLauncherGui::showMatrixDialog(QString &matrixName, ObjectPtr objectPtr, bool modal) {
  MatrixDialog *dialog = new MatrixDialog(objectPtr, kstApp->mainWindow());
  if (modal) {
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);
    dialog->exec();
    matrixName = dialog->dataObjectName();
    delete dialog;
  } else {
    dialog->show();
  }
}


void DialogLauncherGui::showScalarDialog(QString &scalarname, ObjectPtr objectPtr, bool modal) {
  ScalarDialog *dialog = new ScalarDialog(objectPtr, kstApp->mainWindow());
  if (modal) {
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);
    dialog->exec();
    scalarname = dialog->dataObjectName();
    delete dialog;
  } else {
    dialog->show();
  }
}


void DialogLauncherGui::showStringDialog(QString &stringname, ObjectPtr objectPtr, bool modal) {
  StringDialog *dialog = new StringDialog(objectPtr, kstApp->mainWindow());
  if (modal) {
    dialog->setAttribute(Qt::WA_DeleteOnClose, false);
    dialog->exec();
    stringname = dialog->dataObjectName();
    delete dialog;
  } else {
    dialog->show();
  }
}


void DialogLauncherGui::showCurveDialog(ObjectPtr objectPtr, VectorPtr vector) {
  CurveDialog *dialog = new CurveDialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog->setVector(vector);
  }
  dialog->show();
}


void DialogLauncherGui::showImageDialog(ObjectPtr objectPtr, MatrixPtr matrix) {
  ImageDialog *dialog = new ImageDialog(objectPtr, kstApp->mainWindow());
  if (matrix) {
    dialog->setMatrix(matrix);
  }
  dialog->show();
}


void DialogLauncherGui::showEquationDialog(ObjectPtr objectPtr) {
  EquationDialog *dialog = new EquationDialog(objectPtr, kstApp->mainWindow());
  dialog->show();
}


void DialogLauncherGui::showHistogramDialog(ObjectPtr objectPtr, VectorPtr vector) {
  HistogramDialog *dialog = new HistogramDialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog->setVector(vector);
  }
  dialog->show();
}


void DialogLauncherGui::showPowerSpectrumDialog(ObjectPtr objectPtr, VectorPtr vector) {
  PowerSpectrumDialog *dialog = new PowerSpectrumDialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog->setVector(vector);
  }
  dialog->show();
}


void DialogLauncherGui::showCSDDialog(ObjectPtr objectPtr, VectorPtr vector) {
  CSDDialog *dialog = new CSDDialog(objectPtr, kstApp->mainWindow());
  if (vector) {
    dialog->setVector(vector);
  }
  dialog->show();
}


void DialogLauncherGui::showEventMonitorDialog(ObjectPtr objectPtr) {
  EventMonitorDialog *dialog = new EventMonitorDialog(objectPtr, kstApp->mainWindow());
  dialog->show();
}


void DialogLauncherGui::showBasicPluginDialog(QString pluginName, ObjectPtr objectPtr, VectorPtr vectorX, VectorPtr vectorY, PlotItemInterface *plotItem) {
  if (DataObject::pluginType(pluginName) == DataObjectPluginInterface::Generic) {
    BasicPluginDialog *dialog = new BasicPluginDialog(pluginName, objectPtr, kstApp->mainWindow());
    dialog->show();
  } else {
    FilterFitDialog *dialog = new FilterFitDialog(pluginName, objectPtr, kstApp->mainWindow());
    if (!objectPtr) {
      if (vectorX) {
        dialog->setVectorX(vectorX);
      }
      if (vectorY) {
        dialog->setVectorY(vectorY);
      }
      if (plotItem) {
        dialog->setPlotMode((PlotItem*)plotItem);
      }
    }
    dialog->show();
  }
}


void DialogLauncherGui::showObjectDialog(ObjectPtr objectPtr) {
  if (CurvePtr curve = kst_cast<Curve>(objectPtr)) {
    DialogLauncher::self()->showCurveDialog(curve);
  } else if (EquationPtr equation = kst_cast<Equation>(objectPtr)) {
    DialogLauncher::self()->showEquationDialog(equation);
  } else if (HistogramPtr histogram = kst_cast<Histogram>(objectPtr)) {
    DialogLauncher::self()->showHistogramDialog(histogram);
  } else if (PSDPtr psd = kst_cast<PSD>(objectPtr)) {
    DialogLauncher::self()->showPowerSpectrumDialog(psd);
  } else if (EventMonitorEntryPtr eventMonitorEntry = kst_cast<EventMonitorEntry>(objectPtr)) {
    DialogLauncher::self()->showEventMonitorDialog(eventMonitorEntry);
  } else if (ImagePtr image = kst_cast<Image>(objectPtr)) {
    DialogLauncher::self()->showImageDialog(image);
  } else if (CSDPtr csd = kst_cast<CSD>(objectPtr)) {
    DialogLauncher::self()->showCSDDialog(csd);
  } else if (VectorPtr vector = kst_cast<Vector>(objectPtr)) {
    QString tmp;
    DialogLauncher::self()->showVectorDialog(tmp, vector);
  } else if (MatrixPtr matrix = kst_cast<Matrix>(objectPtr)) {
    QString tmp;
    DialogLauncher::self()->showMatrixDialog(tmp, matrix);
  } else if (BasicPluginPtr plugin = kst_cast<BasicPlugin>(objectPtr)) {
    DialogLauncher::self()->showBasicPluginDialog(plugin->pluginName(), plugin);
  }  else if (ScalarPtr scalar = kst_cast<Scalar>(objectPtr)) {
    QString tmp;
    DialogLauncher::self()->showScalarDialog(tmp, scalar);
  } else if (StringPtr string = kst_cast<String>(objectPtr)) {
    QString tmp;
    DialogLauncher::self()->showStringDialog(tmp, string);
  }
}

}

// vim: ts=2 sw=2 et
