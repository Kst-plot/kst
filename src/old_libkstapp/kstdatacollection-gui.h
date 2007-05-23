/***************************************************************************
                           kstdatacollection-gui.h
                             -------------------
    begin                : June 12, 2003
    copyright            : (C) 2003 The University of Toronto
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

#ifndef KSTDATACOLLECTIONGUI_H
#define KSTDATACOLLECTIONGUI_H

#include "kstdatacollection.h"

#include "kst_export.h"

class KstGuiData : public KstData {
  public:
    KstGuiData();
    ~KstGuiData();

    /** check that a tag has not been used by any other tags */
    bool dataTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    bool vectorTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    bool matrixTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);

    bool viewObjectNameNotUnique(const QString& tag);

    void removeCurveFromPlots(KstBaseCurve *c); // no sharedptr here

    /** Save a vector to a file */
    int vectorToFile(KstVectorPtr v, QFile *f);
    int vectorsToFile(const KstVectorList& l, QFile *f, bool interpolate);

    QStringList plotList(const QString& window = QString::null);
    int columns(const QString& window);
    void newWindow(QWidget *dialogParent = 0L);
    QStringList windowList();
    QString currentWindow();
} KST_EXPORT;

#endif
// vim: ts=2 sw=2 et
