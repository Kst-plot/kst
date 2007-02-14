/***************************************************************************
                             kstdatacollection.h
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

#ifndef KSTDATACOLLECTION_H
#define KSTDATACOLLECTION_H

#include <kstaticdeleter.h>
#include "kstdatasource.h"
#include "kststring.h"
#include "kstvector.h"
#include "kstmatrix.h"
#include "kst_export.h"
#include "kstobjectcollection.h"

class QFile;
class KstBaseCurve;

KST_EXPORT class KstData {
  friend class KStaticDeleter<KstData>;
  protected:
    static KstData *_self;
    KstData();
    virtual ~KstData();

  public:
    static KstData *self();
    static void replaceSelf(KstData *newInstance);

    /** check that a tag has not been used by any other tags */
    virtual bool tagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    virtual bool dataTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    virtual bool vectorTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    virtual bool vectorTagNameNotUniqueInternal(const QString& tag);
    virtual bool matrixTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);
    virtual bool matrixTagNameNotUniqueInternal(const QString& tag);
    virtual bool dataSourceTagNameNotUnique(const QString& tag, bool warn = true, void *parent = 0L);

    virtual bool viewObjectNameNotUnique(const QString& tag);

    virtual void removeCurveFromPlots(KstBaseCurve *c); // no sharedptr here

    /** Save a vector to a file */
    virtual int vectorToFile(KstVectorPtr v, QFile *f);
    virtual int vectorsToFile(const KstVectorList& l, QFile *f, bool interpolate);

    /** The list of plots for the given window.  Returns all plots if
        the window is empty/null. */
    virtual QStringList plotList(const QString& window = QString::null);
    /** FIXME: move these to a new class in 1.3 */
    /** Returns the number of columns in the given window.  -1 if not on grid */
    virtual int columns(const QString& window);
    /** Triggers creation of a new window. */
    virtual void newWindow(QWidget *dialogParent = 0L);
    /** Returns the names of all windows. */
    virtual QStringList windowList();
    /** Returns the name of the current window. */
    virtual QString currentWindow();
};


namespace KST {
    /** The list of data sources (files) */
    KST_EXPORT extern KstDataSourceList dataSourceList;

    /** The list of vectors that are being read */
    KST_EXPORT extern KstVectorCollection vectorList;

    /** The list of Scalars which have been generated */
    KST_EXPORT extern KstScalarCollection scalarList;

    /** The list of Strings */
    KST_EXPORT extern KstStringCollection stringList;

    /** The list of matrices that are being read */
    KST_EXPORT extern KstMatrixCollection matrixList;

    /** Bad choice for location - maybe move it later */
    KST_EXPORT void *malloc(size_t size);
    KST_EXPORT void *realloc(void *ptr, size_t size);
}

#endif
// vim: ts=2 sw=2 et
