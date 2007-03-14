/***************************************************************************
                            dialoglauncher-gui.h
                             -------------------
    begin                : Nov. 24, 2004
    copyright            : (C) 2004 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
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

class KstGuiDialogs : public KstDialogs {
  public:
    KstGuiDialogs();
    ~KstGuiDialogs();

    void showHistogramDialog(const QString& name = QString::null, bool edit = false);

//     void showCPluginDialog(const QString& name = QString::null, bool edit = false);

    void showBasicPluginDialog(const QString& name = QString::null, bool edit = false);

    void showEquationDialog(const QString& name = QString::null, bool edit = false);

    void showCSDDialog(const QString& name = QString::null, bool edit = false);

    void showPSDDialog(const QString& name = QString::null, bool edit = false);

    void newMatrixDialog(QWidget *parent, const char *createdSlot = 0L, const char *selectedSlot = 0L, const char *updateSlot = 0L);
    void showMatrixDialog(const QString& name = QString::null, bool edit = false);

    void showImageDialog(const QString& name = QString::null, bool edit = false);

    void showCurveDialog(const QString& name = QString::null, bool edit = false);

    void newVectorDialog(QWidget *parent, const char *createdSlot = 0L, const char *selectedSlot = 0L, const char *updateSlot = 0L);
    void showVectorDialog(const QString& name = QString::null, bool edit = false);
} KST_EXPORT;

#endif

// vim: ts=2 sw=2 et
