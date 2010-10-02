/***************************************************************************
                  dataobject.cpp: base class for data objects
                             -------------------
    begin                : May 20, 2003
    copyright            : (C) 2003 by C. Barth Netterfield
                           (C) 2003 The University of Toronto
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

#include "dataobject.h"

#include "datacollection.h"
#include "dataobjectplugin.h"
#include "debug.h"
#include "kst_i18n.h"
#include "objectstore.h"
#include "relation.h"

#include <QApplication>
#include <QDir>
#include <qdebug.h>
#include <qtimer.h>
#include <QPluginLoader>
#include <QLibraryInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <limits.h>
#include <assert.h>

//#define LOCKTRACE

namespace Kst {

QSettings DataObject::settingsObject("kst", "data");
QMap<QString,QString> DataObject::url_map;


void DataObject::init() {
  initPlugins();
}


DataObject::DataObject(ObjectStore *store) : Object() {
  Q_UNUSED(store);
  _curveHints = new CurveHintList;
}


DataObject::~DataObject() {
  delete _curveHints;
}


static DataObjectPluginList _pluginList;
void DataObject::cleanupForExit() {
  _pluginList.clear(); //FIXME?
}


void DataObject::attach() {
}


// Scans for plugins and stores the information for them
void DataObject::scanPlugins() {
  Debug::self()->log(i18n("Scanning for data-object plugins."));

  _pluginList.clear(); //FIXME?

  DataObjectPluginList tmpList;

  Debug::self()->log(i18n("Scanning for data-object plugins."));

  foreach (QObject *plugin, QPluginLoader::staticInstances()) {
    //try a cast
    if (DataObjectPluginInterface *basicPlugin = dynamic_cast<DataObjectPluginInterface*>(plugin)) {
      tmpList.append(basicPlugin);
    }
  }

  QStringList pluginPaths;
  pluginPaths << QLibraryInfo::location(QLibraryInfo::PluginsPath);
  pluginPaths << QString(qApp->applicationDirPath()).replace("bin", "plugin");

  QDir rootDir = QApplication::applicationDirPath();
  rootDir.cdUp();
  QString pluginPath = rootDir.canonicalPath();
  pluginPath += QDir::separator();
  pluginPath += QLatin1String(INSTALL_LIBDIR);
  pluginPath += QDir::separator();
  pluginPath += QLatin1String("kst");
  pluginPaths << pluginPath;

  foreach (QString pluginPath, pluginPaths) {
    QDir d(pluginPath);
    foreach (QString fileName, d.entryList(QDir::Files)) {
        QPluginLoader loader(d.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
          if (DataObjectPluginInterface *dataObjectPlugin = dynamic_cast<DataObjectPluginInterface*>(plugin)) {
            tmpList.append(dataObjectPlugin);
          }
        }
    }
  }

  // This cleans up plugins that have been uninstalled and adds in new ones.
  // Since it is a shared pointer it can't dangle anywhere.
  _pluginList.clear();
  _pluginList = tmpList;
}


void DataObject::initPlugins() {
  if (_pluginList.isEmpty()) {
      scanPlugins();
  }
}


QStringList DataObject::pluginList() {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  QStringList plugins;

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    plugins += (*it)->pluginName();
  }

  return plugins;
}


QStringList DataObject::dataObjectPluginList() {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  QStringList plugins;

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginType() == DataObjectPluginInterface::Generic) {
      plugins += (*it)->pluginName();
    }
  }

  plugins.sort();
  return plugins;
}


QStringList DataObject::filterPluginList() {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  QStringList plugins;

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginType() == DataObjectPluginInterface::Filter) {
      plugins += (*it)->pluginName();
    }
  }

  plugins.sort();
  return plugins;
}


QStringList DataObject::fitsPluginList() {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  QStringList plugins;

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginType() == DataObjectPluginInterface::Fit) {
      plugins += (*it)->pluginName();
    }
  }

  plugins.sort();
  return plugins;
}


DataObjectConfigWidget* DataObject::pluginWidget(const QString& name) {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginName() == name) {
      if ((*it)->hasConfigWidget()) {
        return (*it)->configWidget(&settingsObject);
      }
      break;
    }
  }
  return 0L;
}


QString DataObject::pluginDescription(const QString& name) {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginName() == name) {
      return (*it)->pluginDescription();
    }
  }
  return QString();
}


int DataObject::pluginType(const QString& name) {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginName() == name) {
      return (*it)->pluginType();
    }
  }
  return -1;
}


DataObjectPtr DataObject::createPlugin(const QString& name, ObjectStore *store, DataObjectConfigWidget *configWidget, bool setupInputsOutputs) {
  // Ensure state.  When using kstapp MainWindow calls init.
  init();

  for (DataObjectPluginList::ConstIterator it = _pluginList.begin(); it != _pluginList.end(); ++it) {
    if ((*it)->pluginName() == name) {
      if (DataObjectPtr object = (*it)->create(store, configWidget, setupInputsOutputs)) {
        return object;
      }
    }
  }

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

  for (StringMap::ConstIterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    if (i.value().data()) {
      rc += i.value()->getUsage() - 1;
    }
  }

  for (MatrixMap::ConstIterator i = _outputMatrices.begin(); i != _outputMatrices.end(); ++i) {
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
  qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::readLock() by tid=" << (int)QThread::currentThread() << ": read locking myself" << endl;
  #endif

  Object::readLock();
}


void DataObject::writeLock() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::writeLock() by tid=" << (int)QThread::currentThread() << ": write locking myself" << endl;
  #endif

  Object::writeLock();
}


void DataObject::unlock() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlock() by tid=" << (int)QThread::currentThread() << ": unlocking myself" << endl;
  #endif

  Object::unlock();
}


void DataObject::writeLockInputsAndOutputs() const {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  QList<PrimitivePtr> inputs;
  QList<PrimitivePtr> outputs;

  QList<StringPtr> sl = _inputStrings.values();
  for (QList<StringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
    inputs += (*i).data();
  }
  sl = _outputStrings.values();
  for (QList<StringPtr>::Iterator i = sl.begin(); i != sl.end(); ++i) {
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

  QList<MatrixPtr> ml = _inputMatrices.values();
  for (QList<MatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    inputs += (*i).data();
  }
  ml = _outputMatrices.values();
  for (QList<MatrixPtr>::Iterator i = ml.begin(); i != ml.end(); ++i) {
    outputs += (*i).data();
  }

  qSort(inputs);
  qSort(outputs);

  QList<PrimitivePtr>::ConstIterator inputIt = inputs.begin();
  QList<PrimitivePtr>::ConstIterator outputIt = outputs.begin();

  while (inputIt != inputs.end() || outputIt != outputs.end()) {
    if (inputIt != inputs.end() && (outputIt == outputs.end() || (void*)(*inputIt) < (void*)(*outputIt))) {
      // do input
      if (!(*inputIt)) {
        qWarning() << "Input for data object " << this->Name() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking input \"" << (*inputIt)->Name() << "\" (" << (void*)((KstRWLock*)*inputIt) << ")" << endl;
#endif
      (*inputIt)->writeLock();
      ++inputIt;
    } else {
      // do output
      if (!(*outputIt)) {
        qWarning() << "Output for data object " << this->Name() << " is invalid." << endl;
      }
#ifdef LOCKTRACE
      qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::writeLockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": write locking output \"" << (*outputIt)->Name() << "\" (" << (void*)((KstRWLock*)*outputIt) << ")" << endl;
#endif
      if ((*outputIt)->provider() != this) {
        Debug::self()->log(i18n("(%1) DataObject::writeLockInputsAndOutputs() by tid=%2: write locking output %3 (not provider) -- this is probably an error. Please email kst@kde.org with details.").arg(this->type()).arg(reinterpret_cast<qint64>(QThread::currentThread())).arg((*outputIt)->Name()), Debug::Error);
      }

      (*outputIt)->writeLock();
      ++outputIt;
    }
  }
}


void DataObject::unlockInputsAndOutputs() const {
  #ifdef LOCKTRACE
  qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << endl;
  #endif

  for (MatrixMap::ConstIterator i = _outputMatrices.begin(); i != _outputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output matrix for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output matrix \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (MatrixMap::ConstIterator i = _inputMatrices.begin(); i != _inputMatrices.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input matrix for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input matrix \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _outputVectors.begin(); i != _outputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output vector for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output vector \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (VectorMap::ConstIterator i = _inputVectors.begin(); i != _inputVectors.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input vector for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input vector \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _outputScalars.begin(); i != _outputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output scalar for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output scalar \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (ScalarMap::ConstIterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input scalar for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input scalar \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (StringMap::ConstIterator i = _outputStrings.begin(); i != _outputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Output string for data object " << this->Name() << " is invalid." << endl;
    }
   #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking output string \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }

  for (StringMap::ConstIterator i = _inputStrings.begin(); i != _inputStrings.end(); ++i) {
    if (!(*i)) {
      qWarning() << "Input string for data object " << this->Name() << " is invalid." << endl;
    }
    #ifdef LOCKTRACE
    qDebug() << (void*)this << " (" << this->type() << ": " << this->Name() << ") DataObject::unlockInputsAndOutputs() by tid=" << (int)QThread::currentThread() << ": unlocking input string \"" << (*i)->Name() << "\" (" << (void*)((KstRWLock*)*i) << ")" << endl;
    #endif
    (*i)->unlock();
  }
}


bool DataObject::isValid() const {
  return true;
}


const CurveHintList* DataObject::curveHints() const {
  return _curveHints;
}


void DataObject::deleteDependents() {
  DataObjectList dataObjects = _store->getObjects<DataObject>();
  foreach (DataObjectPtr object, dataObjects) {
    bool usesObject = object->uses(this);
    if (!usesObject) {
      for (VectorMap::Iterator j = _outputVectors.begin(); !usesObject && j != _outputVectors.end(); ++j) {
        usesObject = object->uses(j.value().data());
      }
      for (ScalarMap::Iterator j = _outputScalars.begin(); !usesObject && j != _outputScalars.end(); ++j) {
        usesObject = object->uses(j.value().data());
      }
      for (StringMap::Iterator j = _outputStrings.begin(); !usesObject && j != _outputStrings.end(); ++j) {
        usesObject = object->uses(j.value().data());
      }
      for (MatrixMap::Iterator j = _outputMatrices.begin(); !usesObject && j != _outputMatrices.end(); ++j) {
        usesObject = object->uses(j.value().data());
      }
    }
    if (usesObject) {
      _store->removeObject(object);
    }
  }

  RelationList relations = _store->getObjects<Relation>();
  foreach (RelationPtr relation, relations) {
    bool usesRelation = relation->uses(this);
    if (!usesRelation) {
      for (VectorMap::Iterator j = _outputVectors.begin(); !usesRelation && j != _outputVectors.end(); ++j) {
        usesRelation = relation->uses(j.value().data());
      }
      for (ScalarMap::Iterator j = _outputScalars.begin(); !usesRelation && j != _outputScalars.end(); ++j) {
        usesRelation = relation->uses(j.value().data());
      }
      for (StringMap::Iterator j = _outputStrings.begin(); !usesRelation && j != _outputStrings.end(); ++j) {
        usesRelation = relation->uses(j.value().data());
      }
      for (MatrixMap::Iterator j = _outputMatrices.begin(); !usesRelation && j != _outputMatrices.end(); ++j) {
        usesRelation = relation->uses(j.value().data());
      }
    }
    if (usesRelation) {
      _store->removeObject(relation);
    }
  }

  foreach (VectorPtr vector, _outputVectors) {
    _store->removeObject(vector);
  }
  foreach (MatrixPtr matrix, _outputMatrices) {
    _store->removeObject(matrix);
  }
  foreach (ScalarPtr scalar, _outputScalars) {
    _store->removeObject(scalar);
  }
  foreach (StringPtr string, _outputStrings) {
    _store->removeObject(string);
  }
}


bool DataObject::duplicateDependents(DataObjectPtr newObject, QMap< SharedPtr<Relation>, SharedPtr<Relation> > &duplicatedRelations) {
  RelationList relations = _store->getObjects<Relation>();
  foreach (RelationPtr relation, relations) {
    if (relation->uses(this)) {
      RelationPtr newRelation = relation->makeDuplicate(duplicatedRelations);
      newRelation->replaceDependency(this, newObject);
    }
  }

  DataObjectList dataObjects = _store->getObjects<DataObject>();
  foreach (DataObjectPtr object, dataObjects) {
    if (object->uses(this)) {
      DataObjectPtr newDataObject = object->makeDuplicate();
      newDataObject->replaceDependency(this, newObject);
      object->duplicateDependents(newDataObject, duplicatedRelations);
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
    QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
    for (ScalarMap::Iterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == k.value()) {
          _inputScalars[k.key()] = (((newObject->outputVectors())[j.key()])->scalars())[scalarDictIter.key()];
        }
      }
    }
    // also replace dependencies on vector strings
    QHashIterator<QString, StringPtr> stringDictIter(j.value()->strings());
    for (StringMap::Iterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
      while (stringDictIter.hasNext()) {
        stringDictIter.next();
        if (stringDictIter.value() == k.value()) {
          _inputStrings[k.key()] = (((newObject->outputVectors())[j.key()])->strings())[stringDictIter.key()];
        }
      }
    }
  }

  // matrices
  for (MatrixMap::Iterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    for (MatrixMap::Iterator k = _inputMatrices.begin(); k != _inputMatrices.end(); ++k) {
      if (j.value().data() == k.value().data()) {
        // replace input with the output from newObject
        _inputMatrices[k.key()] = (newObject->outputMatrices())[j.key()];
      }
    }
    // also replace dependencies on matrix stats
    QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
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
  for (StringMap::Iterator j = oldObject->outputStrings().begin(); j != oldObject->outputStrings().end(); ++j) {
    for (StringMap::Iterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
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

  // slave scalars
  QHashIterator<QString, ScalarPtr> scalarDictIter(oldVector->scalars());
  for (ScalarMap::Iterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
    while (scalarDictIter.hasNext()) {
      scalarDictIter.next();
      if (scalarDictIter.value() == j.value()) {
        _inputScalars[j.key()] = (newVector->scalars())[scalarDictIter.key()];
      }
    }
  }

  // slave strings
  QHashIterator<QString, StringPtr> stringDictIter(oldVector->strings());
  for (StringMap::Iterator j = _inputStrings.begin(); j != _inputStrings.end(); ++j) {
    while (stringDictIter.hasNext()) {
      stringDictIter.next();
      if (stringDictIter.value() == j.value()) {
        _inputStrings[j.key()] = (newVector->strings())[stringDictIter.key()];
      }
    }
  }
}


void DataObject::replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix) {
  for (MatrixMap::Iterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
    if (j.value() == oldMatrix) {
      _inputMatrices[j.key()] = newMatrix;
    }
  }

  QHashIterator<QString, ScalarPtr> scalarDictIter(oldMatrix->scalars());
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
    QHashIterator<QString, ScalarPtr> scalarDictIter(v->scalars());
    for (ScalarMap::ConstIterator j = _inputScalars.begin(); j != _inputScalars.end(); ++j) {
      while (scalarDictIter.hasNext()) {
        scalarDictIter.next();
        if (scalarDictIter.value() == j.value()) {
          return true;
        }
      }
    }
    QHashIterator<QString, StringPtr> stringDictIter(v->strings());
    for (StringMap::ConstIterator j = _inputStrings.begin(); j != _inputStrings.end(); ++j) {
      while (stringDictIter.hasNext()) {
        stringDictIter.next();
        if (stringDictIter.value() == j.value()) {
          return true;
        }
      }
    }
  } else if (MatrixPtr matrix = kst_cast<Matrix>(p)) {
    for (MatrixMap::ConstIterator j = _inputMatrices.begin(); j != _inputMatrices.end(); ++j) {
      if (j.value() == matrix) {
        return true;
      }
    }
    QHashIterator<QString, ScalarPtr> scalarDictIter(matrix->scalars());
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
      QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
      for (ScalarMap::ConstIterator k = _inputScalars.begin(); k != _inputScalars.end(); ++k) {
        while (scalarDictIter.hasNext()) {
          scalarDictIter.next();
          if (scalarDictIter.value() == k.value()) {
            return true;
          }
        }
      }
      // also check dependencies on vector strings
      QHashIterator<QString, StringPtr> stringDictIter(j.value()->strings());
      for (StringMap::ConstIterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
        while (stringDictIter.hasNext()) {
          stringDictIter.next();
          if (stringDictIter.value() == k.value()) {
            return true;
          }
        }
      }

    }

    for (MatrixMap::Iterator j = obj->outputMatrices().begin(); j != obj->outputMatrices().end(); ++j) {
      for (MatrixMap::ConstIterator k = _inputMatrices.begin(); k != _inputMatrices.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
      // also check dependencies on matrix stats
      QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
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

    for (StringMap::Iterator j = obj->outputStrings().begin(); j != obj->outputStrings().end(); ++j) {
      for (StringMap::ConstIterator k = _inputStrings.begin(); k != _inputStrings.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return false;
}

qint64 DataObject::minInputSerial() const{
  qint64 minSerial = LLONG_MAX;

  foreach (VectorPtr P, _inputVectors) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (ScalarPtr P, _inputScalars) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (MatrixPtr P, _inputMatrices) {
    minSerial = qMin(minSerial, P->serial());
  }
  foreach (StringPtr P, _inputStrings) {
    minSerial = qMin(minSerial, P->serial());
  }
  return minSerial;
}

qint64 DataObject::minInputSerialOfLastChange() const {
  qint64 minSerial = LLONG_MAX;

  foreach (VectorPtr P, _inputVectors) {
    minSerial = qMin(minSerial, P->serialOfLastChange());
  }
  foreach (ScalarPtr P, _inputScalars) {
    minSerial = qMin(minSerial, P->serialOfLastChange());
  }
  foreach (MatrixPtr P, _inputMatrices) {
    minSerial = qMin(minSerial, P->serialOfLastChange());
  }
  foreach (StringPtr P, _inputStrings) {
    minSerial = qMin(minSerial, P->serialOfLastChange());
  }
  return minSerial;
}


/////////////////////////////////////////////////////////////////////////////
DataObjectConfigWidget::DataObjectConfigWidget(QSettings *cfg)
: QWidget(0L), _cfg(cfg) {
}


DataObjectConfigWidget::~DataObjectConfigWidget() {
}


void DataObjectConfigWidget::save() {
}


void DataObjectConfigWidget::load() {
}


void DataObjectConfigWidget::setObjectStore(ObjectStore* store) {
  Q_UNUSED(store);
}


void DataObjectConfigWidget::setupFromObject(Object* dataObject) {
  Q_UNUSED(dataObject);
}


void DataObjectConfigWidget::setVectorX(VectorPtr vector) {
  Q_UNUSED(vector);
}


void DataObjectConfigWidget::setVectorY(VectorPtr vector) {
  Q_UNUSED(vector);
}


void DataObjectConfigWidget::setupSlots(QWidget* dialog) {
  Q_UNUSED(dialog);
}


void DataObjectConfigWidget::setVectorsLocked(bool locked) {
  Q_UNUSED(locked);
}


bool DataObjectConfigWidget::configurePropertiesFromXml(ObjectStore *store, QXmlStreamAttributes& attrs) {
  Q_UNUSED(store);
  Q_UNUSED(attrs);
  return true;
}

}
// vim: ts=2 sw=2 et
