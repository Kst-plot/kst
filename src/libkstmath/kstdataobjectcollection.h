/***************************************************************************
                          kstdataobjectcollection.h
                             -------------------
    begin                : Feb 10, 2006
    copyright            : (C) 2006 The University of Toronto
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

#ifndef KSTDATAOBJECTCOLLECTION_H
#define KSTDATAOBJECTCOLLECTION_H

#include "kstdataobject.h"
#include <kst_export.h>

namespace KST {
    /** The list of data objects which are in use */
    KST_EXPORT extern KstDataObjectList dataObjectList;
    KST_EXPORT extern void addDataObjectToList(KstDataObjectPtr d);
}

#endif
// vim: ts=2 sw=2 et
