/***************************************************************************
                        kstdataobjectcollection.cpp
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

#include "kstdataobjectcollection.h"

/** The list of data objects which are in use */
KstDataObjectList KST::dataObjectList;

KstRelationList KST::relationList;

void KST::addDataObjectToList(KstDataObjectPtr d) {
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d);
  KST::dataObjectList.lock().unlock();
}

// vim: ts=2 sw=2 et
