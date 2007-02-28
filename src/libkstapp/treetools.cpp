

bool KST::deleteDependents(KstVectorPtr vectorFor) {
  KST::dataObjectList.lock().readLock();
  KstDataObjectList dol = Q3DeepCopy<KstDataObjectList>(KST::dataObjectList);
  KST::dataObjectList.lock().unlock();
  for (KstDataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
    bool user = (*i)->uses(vectorFor);
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


bool KST::duplicateDependents(KstVectorPtr vectorFor, KstDataObjectDataObjectMap& duplicatedMap, QMap<KstVectorPtr, KstVectorPtr> &duplicatedVectors) {
  // work with a copy of the data object list
  KST::dataObjectList.lock().readLock();
  KstDataObjectList dol = Q3DeepCopy<KstDataObjectList>(KST::dataObjectList);
  KST::dataObjectList.lock().unlock();
  
  for (KstDataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
    if ((*i)->uses(vectorFor)) {
      if (duplicatedMap.contains(*i)) {
        (duplicatedMap[*i])->replaceDependency(vectorFor, duplicatedVectors[vectorFor]);  
      } else {
        KstDataObjectPtr newObject = (*i)->makeDuplicate(duplicatedMap);
        newObject->replaceDependency(vectorFor, duplicatedVectors[vectorFor]);
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


// vim: ts=2 sw=2 et
