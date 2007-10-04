/***************************************************************************
                  dataobject.cpp: base class for data objects
                             -------------------
    begin                : May 20, 2003
    copyright            : (C) 2003 by C. Barth Netterfield
                           (C) 2003 The University of Toronto
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

#include "dataobject.h"

#include "datacollection.h"
#include "kstdataobjectcollection.h"
#include "dataplugin.h"
#include "debug.h"
#include "kst_i18n.h"

#include <qdebug.h>
#include <qtimer.h>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <assert.h>

//#define LOCKTRACE

namespace Kst {

DataObject::DataObject() : Object() {
  //qDebug() << "+++ CREATING DATA OBJECT: " << (void*)this << endl;
  _curveHints = new KstCurveHintList;
  _isInputLoaded = false;
}

DataObject::DataObject(const QDomElement& e) : Object() {
  Q_UNUSED(e)
  //qDebug() << "+++ CREATING DATA OBJECT: " << (void*)this << endl;
  _curveHints = new KstCurveHintList;
  _isInputLoaded = false;
}


DataObject::~DataObject() {
  // Remove our slave vectors, scalars, and strings, and matrices
  stringList.lock().writeLock();
  for (KstStringMap::Iterator it = _outputStrings.begin();
                               it != _outputStrings.end();
                                                      ++it) {
    stringList.remove(it.value());
  }
  stringList.lock().unlock();

  scalarList.lock().writeLock();
  for (ScalarMap::Iterator it = _outputScalars.begin();
                               it != _outputScalars.end();
                                                      ++it) {
    scalarList.remove(it.value());
  }
  scalarList.lock().unlock();

  vectorList.lock().writeLock();
  for (VectorMap::Iterator it = _outputVectors.begin();
                               it != _outputVectors.end();
                                                      ++it) {
    vectorList.remove(it.value());
  }
  vectorList.lock().unlock();
  
  matrixList.lock().writeLock();
  for (KstMatrixMap::Iterator it = _outputMatrices.begin();
       it != _outputMatrices.end();
       ++it) {
    matrixList.remove(it.value());
  }
  matrixList.lock().unlock();
//  qDebug() << "Destroying Data Object: " << tag().displayString() << endl;
  delete _curveHints;
}

static QMap<QString, DataObjectPtr> pluginInfo;
void DataObject::cleanupForExit() {
  pluginInfo.clear(); //FIXME?
}


void DataObject::attach() {
}


DataObjectPtr DataObject::createPlugin() {
#if 0
  int err = 0;
  DataObject *object = KService::createInstance<DataObject>(service, 0, QStringList(), &err);

  KstSharedPtr<Plugin> p = new DataObjectPlugin(service);

  if (object && p->key()) {
    const QString name = service->property("Name").toString();
    const QString description = service->property("Comment").toString();
    const QString author = service->property("X-Kst-Plugin-Author").toString();
    const QString version = service->property("X-Kst-Plugin-Version").toString();
    const QString library = service->library();
    Q_ASSERT( !name.isEmpty() );
    Q_ASSERT( !library.isEmpty() );
    object->setName(name);
    object->setAuthor(author);
    object->setDescription(description);
    object->setVersion(version);
    object->setLibrary(library);

    Debug::self()->log(i18n("Loaded data-object plugin %1.").arg(service->name()));
    return object;
  }

  Debug::self()->log(i18n("Could not load data-object plugin %1.").arg(service->name()), Debug::Error);
#endif
  return 0L;
}


// Scans for plugins and stores the information for them
void DataObject::scanPlugins() {
  Debug::self()->log(i18n("Scanning for data-object plugins."));

  pluginInfo.clear(); //FIXME?

#if 0
  KService::List sl = KServiceTypeTrader::self()->query("Kst Data Object");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    if (DataObjectPtr object = createPlugin(*it)) {
      pluginInfo.insert((*it)->name(), DataObjectPtr(object));
    }
  }
#endif
}


KstPluginInfoList DataObject::pluginInfoList() {
  if (pluginInfo.isEmpty()) {
    scanPlugins();
  }

  KstPluginInfoList list;
  QMap<QString, DataObjectPtr>::ConstIterator it = pluginInfo.begin();
  for (; it != pluginInfo.end(); ++it) {
    list.insert(it.key(), it.value()->kind());
  }
  return list;
}


DataObjectPtr DataObject::plugin(const QString& name) {
    if (pluginInfo.contains(name)) {
        return pluginInfo[name];
    }
    return 0L;
}


DataObjectPtr DataObject::createPlugin(const QString& name) {
#if 0
  KService::List sl = KServiceTypeTrader::self()->query("Kst Data Object");
  for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
    if ((*it)->name() != name) {
      continue;
    } else if (DataObjectPtr object = createPlugin(*it)) {
      return object;
    }
  }
#endif
  return 0L;
}


double *DataObject::vectorRealloced(VectorPtr v, double *memptr, int newSize) const {
  if (!v) {
    return 0L;
  }

  // One would think this needs special locking, but it results in deadlock
  // in complicated object hierarchies such as filtered vectors.  Therefore if
  // you call vectorRealloced() and v is not locked by you already, you'd
  // better lock it!
  return v->realloced(memptr, newSize);
}


void DataObject::load(const QXmlStreamReader &e) {
  qDebug() << QString("FIXME! Loading of %1 is not implemented yet.").arg(typeString()) << endl;
  Q_UNUSED(e)
}


void DataObject::save(QXmlStreamWriter& ts) {
  qDebug() << QString("FIXME! Saving of %1 is not implemented yet.").arg(typeString()) << endl;
  Q_UNUSED(ts)
}


bool DataObject::loadInputs() {
  bool rc = true;
  QList<QPair<QString,QString> >::Iterator i;
  
  vectorList.lock().readLock();
  for (i = _inputVectorLoadQueue.begin(); i != _inputVectorLoadQueue.end(); ++i) {
    VectorList::Iterator it = vectorList.findTag((*i).second);
    if (it != vectorList.end()) {
      assert(*it);
      _inputVectors.insert((*i).first, *it);
    } else {
      Debug::self()->log(i18n("Unable to find required vector [%1] for data object %2.").arg((*i).second).arg(tagName()), Debug::Error);
      rc = false;
    }
  }
  vectorList.lock().unlock();

  scalarList.lock().readLock();
  for (i = _inputScalarLoadQueue.begin(); i != _inputScalarLoadQueue.end(); ++i) {
    ScalarList::Iterator it = scalarList.findTag((*i).second);
    if (it != scalarList.end()) {
      _inputScalars.insert((*i).first, *it);
    } else {
      Debug::self()->log(i18n("Unable to find required scalar [%1] for data object %2.").arg((*i).second).arg(tagName()), Debug::Error);
      rc = false;
    }
  }
  scalarList.lock().unlock();

  stringList.lock().readLock();
  for (i = _inputStringLoadQueue.begin(); i != _inputStringLoadQueue.end(); ++i) {
    KstStringList::Iterator it = stringList.findTag((*i).second);
    if (it != stringList.end()) {
      _inputStrings.insert((*i).first, *it);
    } else {
      Debug::self()->log(i18n("Unable to find required string [%1] for data object %2.").arg((*i).second).arg(tagName()), Debug::Error);
      rc = false;
    }
  }
  stringList.lock().unlock();
  
  matrixList.lock().readLock();
  for (i = _inputMatrixLoadQueue.begin(); i != _inputMatrixLoadQueue.end(); ++i) {
    KstMatrixList::Iterator it = matrixList.findTag((*i).second);
    if (it != matrixList.end()) {
      _inputMatrices.insert((*i).first, *it);
    } else {
      Debug::self()->log(i18n("Unable to find required matrix [%1] for data object %2.").arg((*i).second).arg(tagName()), Debug::Error);
      rc = false;
    }
  }
  matrixList.lock().unlock();

  _inputVectorLoadQueue.clear();
  _inputScalarLoadQueue.clear();
  _inputStringLoadQueue.clear();
  _inputMatrixLoadQueue.clear();
  
  setDirty();

  _isInputLoaded = true;
  return rc;
}


int DataObject::getUsage() const {
  int rc = 0;

  for (VectorMap::ConstIterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    if (i.value().data()) {
      rc += i.value()->getUsage() - 1;
    }
  }

  for (ScalarMap::ConstIterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    if (i.value().data()) {
      rc += i.value()->getUsage() - 1;
    }
  }

  for (KstStringMap::ConstIterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    if (i.value().data()) {
      rc += i.value()->getUsage() - 1;
    }
  }
  
  for (KstMatrixMap::ConstIterator i = _outputMatrices.begin(); i != _outputMatrices.end(); ++i) {
    if (i.value().data()) {
      rc += i.value()->getUsage() - 1;  
    }  
  }

  return Object::getUsage() + rc;
}


void DataObject::showDialog(bool isNew) {
  if (isNew) {
    QTimer::singleShot(0, this, SLOT(showNewDialog()));
  } else {
    QTimer::singleShot(0, this, SLOT(showEditDialog()));
  }
}


void DataObject::readLock() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::readLock() by tid=" << (int)QThread::currentThread() << ": read locking myself" << endl;
  #endif

  Object::readLock();
}


void DataObject::writeLock() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::writeLock() by tid=" << (int)QThread::currentThread() << ": write locking myself" << endl;
  #endif

  Object::writeLock();
}


void DataObject::unlock() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlock() by tid=" << (int)QThread::currentThread() << ": unlocking myself" << endl;
  #endif

  Object::unlock();
}


void DataObject::writeLockInputsAndOutputs() const {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  QList<KstPrimitivePtr> inputs;
  QList<KstPrimitivePtr> outputs;

  QList<KstStringPtr> sl = _inputStrings.values();
  for (QList<KstStringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
    inputs += (*i).data();
  }
  sl = _outputStrings.values();
  for (QList<KstStringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
    outputs += (*i).data();
  }
  
  QList<ScalarPtr> sc = _inputScalars.values();
  for (QList<ScalarPtr>::Iterator i = sc.begin(); i != sc.end(); ++i) {
    inputs += (*i).data();
  }
  sc = _outputScalars.values();
  for (QList<ScalarPtr>::Iterator i = sc.begin(); i != sc.end(); ++i) {
    outputs += (*i).data();
  }
  
  QList<VectorPtr> vl = _inputVectors.values();
  for (QList<VectorPtr>::Iterator i = vl.begin(); i != vl.end(); ++i) {
    inputs += (*i).data();
  }
  vl = _outputVectors.values();
  for (QList<VectorPtr>::Iterator i = vl.begin(); i != vl.end(); ++i) {
    outputs += (*i).data();
  }
  
  QList<KstMatrixPtr> ml = _inputMatrices.values();
  for (QList<KstMatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    inputs += (*i).data();
  }
  ml = _outputMatrices.values();
  for (QList<KstMatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    outputs += (*i).data();
  }

  qSort(inputs);
  qSort(outputs);

  QList<KstPrimitivePtr>::ConstIterator inputIt = inputs.begin();
  QList<KstPrimitivePtr>::ConstIterator outputIt = outputs.begin();

  while (inputIt != inputs.end() || outputIt != outputs.end()) {
    if (inputIt != inputs.end() && (outputIt == outputs.end() || (void*)(*inputIt) < (void*)(*outputIt))) {
      // do input
      if (!(*inputIt)) {
        qWarning() << "Input for data object " << this->tag().displayString() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking input \"" << (*inputIt)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*inputIt) << ")" << endl;
#endif
      (*inputIt)->writeLock();
      ++inputIt;
    } else {
      // do output
      if (!(*outputIt)) {
        qWarning() << "Output for data object " << this->tag().displayString() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking output \"" << (*outputIt)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*outputIt) << ")" << endl;
#endif
      if ((*outputIt)->provider() != this) {
        Debug::self()->log(i18n("(%1) DataObject::writeLockInputsAndOutputs() by tid=%2: write locking output %3 (not provider) -- this is probably an error. Please email kst@kde.org with details.").arg(this->type()).arg(reinterpret_cast<qint64>(QThread::currentThread())).arg((*outputIt)->tagName()), Debug::Error);
      }
      (*outputIt)->writeLock();
      ++outputIt;
    }
  }
}


void DataObject::unlockInputsAndOutputs() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  for (KstMatrixMap::ConstIterator i = _outputMatrices.begin(); i != _outputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output matrix for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output matrix \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (KstMatrixMap::ConstIterator i = _inputMatrices.begin(); i != _inputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input matrix for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input matrix \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output vector for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output vector \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input vector for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input vector \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output scalar for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output scalar \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input scalar for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input scalar \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (KstStringMap::ConstIterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output string for data object " << this->tag().displayString() << " is invalid." << endl;
    }
   #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output string \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (KstStringMap::ConstIterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input string for data object " << this->tag().displayString() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->tag().tagString() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input string \"" << (*i)->tag().tagString() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }
}


bool DataObject::isValid() const {
  return true;
}


const KstCurveHintList* DataObject::curveHints() const {
  return _curveHints;
}


bool DataObject::deleteDependents() {
  KST::dataObjectList.lock().readLock();
  DataObjectList dol = KST::dataObjectList;
  KST::dataObjectList.lock().unlock();
  for (DataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
    bool user = (*i)->uses(this);
    if (!user) {
      for (VectorMap::Iterator j = _outputVectors.begin(); !user && j != _outputVectors.end(); ++j) {
        user = (*i)->uses(j.value().data());
      }
      for (ScalarMap::Iterator j = _outputScalars.begin(); !user && j != _outputScalars.end(); ++j) {
        user = (*i)->uses(j.value().data());
      }
      for (KstStringMap::Iterator j = _outputStrings.begin(); !user && j != _outputStrings.end(); ++j) {
        user = (*i)->uses(j.value().data());
      }
    }
    if (user) {
      DataObjectPtr dop = *i;
      KST::dataObjectList.lock().writeLock();
      KST::dataObjectList.removeAll(dop);
      KST::dataObjectList.lock().unlock();
      dop->deleteDependents();
    }
  } 

  return true;
}


bool DataObject::duplicateDependents(QMap<DataObjectPtr, DataObjectPtr> &duplicatedMap) {
  // work with a copy of the data object list
  KST::dataObjectList.lock().readLock();
  DataObjectList dol = KST::dataObjectList;
  KST::dataObjectList.lock().unlock();
  
  for (DataObjectList::Iterator i = dol.begin(); i != dol.end(); ++i) {
    if ((*i)->uses(this)) {
      if (duplicatedMap.contains(*i)) {
        (duplicatedMap[*i])->replaceDependency(this, duplicatedMap[this]);
      } else {
        DataObjectPtr newObject = (*i)->makeDuplicate(duplicatedMap);
        KST::dataObjectList.lock().writeLock();
        KST::dataObjectList.append(newObject.data());
        KST::dataObjectList.lock().unlock();
        (duplicatedMap[*i])->replaceDependency(this, duplicatedMap[this]);
        (*i)->duplicateDependents(duplicatedMap);
      }
    }
  }
  return true;
}


void DataObject::replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject) {
  
  // find all connections from this object to old object
  
  // vectors
  for (VectorMap::Iterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    for (VectorMap::Iterator k = _inputVectors.begin(); k != _inputVectors.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputVectors[k.key()] = (newObject->outputVectors())[j.key()]; 
      }
    }
    // also replace dependencies on vector stats
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    for (ScalarMap::Iterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == k.value()) {
          _inputScalars[k.key()] = (((newObject->outputVectors())[j.key()])->scalars())[scalarDictIter.key()];
        }  
      }
    }
  }
  
  // matrices
  for (KstMatrixMap::Iterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    for (KstMatrixMap::Iterator k = _inputMatrices.begin(); k != _inputMatrices.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputMatrices[k.key()] = (newObject->outputMatrices())[j.key()]; 
      }
    }
    // also replace dependencies on matrix stats
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    for (ScalarMap::Iterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == k.value()) {
          _inputScalars[k.key()] = (((newObject->outputMatrices())[j.key()])->scalars())[scalarDictIter.key()];
        }  
      }
    }
  }

  // scalars
  for (ScalarMap::Iterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    for (ScalarMap::Iterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputScalars[k.key()] = (newObject->outputScalars())[j.key()];  
      }
    } 
  }
  
  // strings 
  for (KstStringMap::Iterator j = oldObject->outputStrings().begin(); j != oldObject->outputStrings().end(); ++j) {
    for (KstStringMap::Iterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputStrings[k.key()] = (newObject->outputStrings())[j.key()];  
      }
    }
  }
}


void DataObject::replaceDependency(VectorPtr oldVector, VectorPtr newVector) {
  for (VectorMap::Iterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
    if (j.value() == oldVector) {
      _inputVectors[j.key()] = newVector;  
    }      
  }
  
  QHashIterator<QString, Scalar*> scalarDictIter(oldVector->scalars());
  for (ScalarMap::Iterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      if (scalarDictIter.value() == j.value()) {
        _inputScalars[j.key()] = (newVector->scalars())[scalarDictIter.key()];
      }  
    }
  }
}


void DataObject::replaceDependency(KstMatrixPtr oldMatrix, KstMatrixPtr newMatrix) {
  for (KstMatrixMap::Iterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
    if (j.value() == oldMatrix) {
      _inputMatrices[j.key()] = newMatrix;  
    }      
  }
  
  QHashIterator<QString, Scalar*> scalarDictIter(oldMatrix->scalars());
  for (ScalarMap::Iterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      if (scalarDictIter.value() == j.value()) {
        _inputScalars[j.key()] = (newMatrix->scalars())[scalarDictIter.key()];
      }  
    }
  }
}


bool DataObject::uses(ObjectPtr p) const {
  VectorPtr v = kst_cast<Vector>(p);
  if (v) {
    for (VectorMap::ConstIterator j = _inputVectors.begin(); j != _inputVectors.end(); ++j) {
      if (j.value() == v) {
        return true;
      }
    }
    QHashIterator<QString, Scalar*> scalarDictIter(v->scalars());
    for (ScalarMap::ConstIterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == j.value()) {
          return true;  
        }  
      }
    }
  } else if (KstMatrixPtr matrix = kst_cast<KstMatrix>(p)) {
    for (KstMatrixMap::ConstIterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
      if (j.value() == matrix) {
        return true;
      }
    }
    QHashIterator<QString, Scalar*> scalarDictIter(matrix->scalars());
    for (ScalarMap::ConstIterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == j.value()) {
          return true;  
        }  
      }
    }
  } else if (DataObjectPtr obj = kst_cast<DataObject>(p) ) {
    // check all connections from this object to p
    for (VectorMap::Iterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (VectorMap::ConstIterator k = _inputVectors.begin(); k != _inputVectors.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
      // also check dependencies on vector stats
      QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        while (scalarDictIter.hasNext()) {
          scalarDictIter.next();
          if (scalarDictIter.value() == k.value()) {
            return true;
          }  
        }
      }
    }
  
    for (KstMatrixMap::Iterator j = obj->outputMatrices().begin(); j != obj->outputMatrices().end(); ++j) {
      for (KstMatrixMap::ConstIterator k = _inputMatrices.begin(); k != _inputMatrices.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
      // also check dependencies on vector stats
      QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        while (scalarDictIter.hasNext()) {
          scalarDictIter.next();
          if (scalarDictIter.value() == k.value()) {
            return true;
          }  
        }
      }
    }
    
    for (ScalarMap::Iterator j = obj->outputScalars().begin(); j != obj->outputScalars().end(); ++j) {
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      } 
    }
  
    for (KstStringMap::Iterator j = obj->outputStrings().begin(); j != obj->outputStrings().end(); ++j) {
      for (KstStringMap::ConstIterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return false;
}

}
// vim: ts=2 sw=2 et
