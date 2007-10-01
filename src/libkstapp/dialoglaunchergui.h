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
    virtual void showVectorDialog(KstObjectPtr objectPtr = 0);

    virtual void showMatrixDialog(KstObjectPtr objectPtr = 0);

    virtual void showScalarDialog(KstObjectPtr objectPtr = 0);

    virtual void showStringDialog(KstObjectPtr objectPtr = 0);

    //standard objects
    virtual void showCurveDialog(KstObjectPtr objectPtr = 0);

    virtual void showImageDialog(KstObjectPtr objectPtr = 0);

    //standard data objects
    virtual void showEquationDialog(KstObjectPtr objectPtr = 0);

    virtual void showHistogramDialog(KstObjectPtr objectPtr = 0);

    virtual void showPSDDialog(KstObjectPtr objectPtr = 0);

    virtual void showCSDDialog(KstObjectPtr objectPtr = 0);

    //plugins
    virtual void showBasicPluginDialog(KstObjectPtr objectPtr = 0);
};

}

#endif

// vim: ts=2 sw=2 et
