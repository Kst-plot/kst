/***************************************************************************
                             datacollection.h
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

#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "datasource.h"
#include "string_kst.h"
#include "vector.h"
#include "matrix.h"
#include "kst_export.h"

class QFile;

namespace Kst {

class Relation;
class PlotItemInterface;

class KSTCORE_EXPORT Data 
{
  protected:
    static Data *_self;
    static void cleanup();
    Data();
    virtual ~Data();

  public:
    static Data *self();
    static void replaceSelf(Data *newInstance);

    virtual void removeCurveFromPlots(Relation *c); // no sharedptr here

    /** The list of plots for the current view. */
    virtual QList<PlotItemInterface*> plotList() const;

    /** Returns the number of rows in the current view's layout.
      * -1 if not in layout.
      */
    virtual int rows() const;

    /** Returns the number of columns in the given view's layout.
      * -1 if not in layout.
      */
    virtual int columns() const;
};

/** Bad choice for location - maybe move it later */
KSTCORE_EXPORT void *malloc(size_t size);
KSTCORE_EXPORT void *realloc(void *ptr, size_t size);

}

#endif
// vim: ts=2 sw=2 et
