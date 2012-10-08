/***************************************************************************
                             datacollection.h
                             -------------------
    begin                : June 12, 2003
    copyright            : (C) 2003 The University of Toronto
    email                : netterfield@astro.utoronto.ca
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

    static double AvailableMemory();
};


/** Bad choice for location - maybe move it later */
template<class T>
bool kstrealloc(T* &ptr, size_t size)
{
  // don't overwrite old pointer when resize fails
  // it doesn't free the old pointer
  void* newptr = qRealloc(static_cast<void*>(ptr), size);
  if (!newptr)
    return false;
  ptr = static_cast<T*>(newptr);
  return true;
}

}

#endif
// vim: ts=2 sw=2 et
