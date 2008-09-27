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

#ifndef DIALOGLAUNCHERGUI_H
#define DIALOGLAUNCHERGUI_H

#include "dialoglauncher.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT DialogLauncherGui : public DialogLauncher {
  public:
    DialogLauncherGui();
    virtual ~DialogLauncherGui();

    //primitives
    virtual void showVectorDialog(QString &vectorname, ObjectPtr objectPtr = 0);

    virtual void showMatrixDialog(QString &matrixName, ObjectPtr objectPtr = 0);

    virtual void showScalarDialog(QString &scalarname, ObjectPtr objectPtr = 0);

    virtual void showStringDialog(QString &stringname, ObjectPtr objectPtr = 0);

    //standard objects
    virtual void showCurveDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);

    virtual void showImageDialog(ObjectPtr objectPtr = 0, MatrixPtr matrix = 0);

    //standard data objects
    virtual void showEquationDialog(ObjectPtr objectPtr = 0);

    virtual void showHistogramDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);

    virtual void showPowerSpectrumDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);

    virtual void showCSDDialog(ObjectPtr objectPtr = 0, VectorPtr vector = 0);

    virtual void showEventMonitorDialog(ObjectPtr objectPtr = 0);

    //plugins
    virtual void showBasicPluginDialog(QString pluginName, ObjectPtr objectPtr = 0, VectorPtr vectorX = 0, VectorPtr vectorY = 0, PlotItemInterface *plotItem = 0 );
};

}

#endif

// vim: ts=2 sw=2 et
