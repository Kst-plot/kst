/***************************************************************************
                              dialoglauncher.h
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

#ifndef DIALOGLAUNCHER_H
#define DIALOGLAUNCHER_H
#include <qstring.h>
#include <kstaticdeleter.h>
#include "kst_export.h"

class QWidget;

KST_EXPORT class KstDialogs {
  friend class KStaticDeleter<KstDialogs>;
  protected:
    static KstDialogs *_self;
    KstDialogs();
    virtual ~KstDialogs();

  public:
    static void replaceSelf(KstDialogs *newInstance);
    static KstDialogs *self();

    virtual void showHistogramDialog(const QString& name = QString::null, bool edit = false);

    virtual void showBasicPluginDialog(const QString& name = QString::null, bool edit = false);

    virtual void showCPluginDialog(const QString& name = QString::null, bool edit = false);

    virtual void showEquationDialog(const QString& name = QString::null, bool edit = false);

    virtual void showCSDDialog(const QString& name = QString::null, bool edit = false);

    virtual void showPSDDialog(const QString& name = QString::null, bool edit = false);

    virtual void newMatrixDialog(QWidget *parent, const char *createdSlot = 0L, const char *selectedSlot = 0L, const char *updateSlot = 0L);
    virtual void showMatrixDialog(const QString& name = QString::null, bool edit = false);

    virtual void showImageDialog(const QString& name = QString::null, bool edit = false);

    virtual void showCurveDialog(const QString& name = QString::null, bool edit = false);

    virtual void newVectorDialog(QWidget *parent, const char *createdSlot = 0L, const char *selectedSlot = 0L, const char *updateSlot = 0L);
    virtual void showVectorDialog(const QString& name = QString::null, bool edit = false);
};

#endif

// vim: ts=2 sw=2 et
