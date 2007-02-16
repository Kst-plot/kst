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

void KST::addDataObjectToList(KstDataObjectPtr d) {
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d);
  KST::dataObjectList.lock().unlock();
}

#if 0
bool KstData::dataTagNameNotUnique(const QString& tag, bool warn, void *p) {
  Q_UNUSED(p)
  Q_UNUSED(warn)
  /* verify that the tag name is not empty */
  if (tag.trimmed().isEmpty()) {
      return true;
  }

  /* verify that the tag name is not used by a data object */
  KstReadLocker ml(&KST::dataObjectList.lock());
  if (KST::dataObjectList.findTag(tag) != KST::dataObjectList.end()) {
      return true;
  }

  return false;
}
#endif

// vim: ts=2 sw=2 et
