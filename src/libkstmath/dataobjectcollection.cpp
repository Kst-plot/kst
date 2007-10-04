/***************************************************************************
                        dataobjectcollection.cpp
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

#include "dataobjectcollection.h"

namespace Kst {

/** The list of data objects which are in use */
DataObjectList dataObjectList;

KstRelationList relationList;

void addDataObjectToList(DataObjectPtr d) {
  dataObjectList.lock().writeLock();
  dataObjectList.append(d);
  dataObjectList.lock().unlock();
}

}
// vim: ts=2 sw=2 et
