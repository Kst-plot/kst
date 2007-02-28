/***************************************************************************
                                treetools.h
                              ----------------
    begin                : Feb 11 2006
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

#ifndef TREETOOLS_H
#define TREETOOLS_H

#include <q3deepcopy.h>

#include <kstdatacollection.h>

namespace KST {
  template<class T>
    bool deleteDependents(T objectFor) {
      KST::dataObjectList.lock().readLock();
      KstDataObjectList dol = Q3DeepCopy<KstDataObjectList>(KST::dataObjectList);
      KST::dataObjectList.lock().unlock();
      for (KstDataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
        bool user = (*i)->uses(objectFor.data());
        if (user) {
          KstDataObjectPtr dop = *i;
          KST::dataObjectList.lock().writeLock();
          KST::dataObjectList.remove(dop);
          KST::dataObjectList.lock().unlock();
          dop->deleteDependents();
        }
      }

      return true;
    }


  template<class T>
    bool duplicateDependents(T objectFor, KstDataObjectDataObjectMap& duplicatedMap, QMap<T, T>& duplicatedVectors) {
      // work with a copy of the data object list
      KST::dataObjectList.lock().readLock();
      KstDataObjectList dol = Q3DeepCopy<KstDataObjectList>(KST::dataObjectList);
      KST::dataObjectList.lock().unlock();

      for (KstDataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
        if ((*i)->uses(objectFor.data())) {
          if (duplicatedMap.contains(*i)) {
            (duplicatedMap[*i])->replaceDependency(objectFor, duplicatedVectors[objectFor]);  
          } else {
            KstDataObjectPtr newObject = (*i)->makeDuplicate(duplicatedMap);
            newObject->replaceDependency(objectFor, duplicatedVectors[objectFor]);
            if (newObject) {
              KST::dataObjectList.lock().writeLock();
              KST::dataObjectList.append(newObject.data());
              KST::dataObjectList.lock().unlock();
              (*i)->duplicateDependents(duplicatedMap);
            }
          }
        }
      }

      return true;
    }
}

#endif
// vim: ts=2 sw=2 et
