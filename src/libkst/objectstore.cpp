/***************************************************************************
              objectstore.cpp: store of Objects
                             -------------------
    begin                : November 22, 2006
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

#include <QHash>
#include <QList>
#include <QString>

#include "object.h"
#include "objectstore.h"
#include "datavector.h"
#include "datastring.h"
#include "datascalar.h"
#include "datamatrix.h"

// NAMEDEBUG: 0 for no debug, 1 for some debug, 2 for more debug, 3 for all debug
#define NAMEDEBUG 0

namespace Kst {

ObjectStore::ObjectStore() 
{
}


ObjectStore::~ObjectStore()
{
}


bool ObjectStore::removeObject(Object *o) {
  if (!this) {
    qDebug() << "tried to remove object in store, but there is no store?\n";
    return false;
  }

  if (!o) {
    return false;
  }

  KstWriteLocker l(&this->_lock);

  DataSourcePtr ds = kst_cast<DataSource>(o);
  if (ds) {
    if (!_dataSourceList.contains(ds)) {
#if NAMEDEBUG > 1
      qDebug() << "Trying to delete a non-existant data source from the store: " << ds->tag().tagString();
#endif
      return false;
    }
  } else if (!_list.contains(o)) {
#if NAMEDEBUG > 1
    qDebug() << "Trying to delete a non-existant object from the store: " << o->tag().tagString();
#endif
    return false;
  }

  if (ds) {
    ds->deleteDependents();
    _dataSourceList.removeAll(ds);
  } else {
    o->deleteDependents();
    _list.removeAll(o);
  }

  o->_store = 0;

  return true;
}

ObjectPtr ObjectStore::retrieveObject(const QString name) const {

  int match = -1;

  if (name.isEmpty()) {
    return NULL;
  }

  QString shortName;
  QRegExp rx("(\\(|^)([A-Z]\\d+)(\\)$|$)");
  rx.indexIn(name);
  shortName = rx.cap(2);

  // 1) search for short names
  for (int i = 0; i < _list.size(); ++i) {
    if (_list.at(i)->shortName()==shortName) 
      return _list.at(i);
  }
  // 3) search for descriptive names: must be unique
  for (int i = 0; i < _list.size(); ++i) {
    if (_list.at(i)->descriptiveName() == name) {
      if (match != -1) 
        return NULL; // not unique, so... no match
      match = i;
    }
  }

  if (match >-1) 
    return _list.at(match);

  return NULL;
}

void ObjectStore::rebuildDataSourceList() {
  cleanUpDataSourceList();
  foreach (DataSourcePtr ds, _dataSourceList) {
    ds->writeLock();
    ds->reset();
    ds->unlock();
  }
  foreach (ObjectPtr object, _list) {
    object->writeLock();
    object->reset();
    object->unlock();
  }
}

#if 0
void ObjectStore::rebuildDataSourceList() {
  DataSourceList dataSourceList;

  for (int i=0; i<_list.count(); i++) {
    if (DataVectorPtr object_V = kst_cast<DataVector>(_list.at(i))) {
      DataSourcePtr dataSource;
      object_V->readLock();
      dataSource = object_V->dataSource();
      object_V->unlock();

      if (!dataSourceList.contains(dataSource)) {
        object_V->writeLock();
        object_V->reload();
        object_V->unlock();
        dataSourceList.append(dataSource);
      }
    } else if (DataStringPtr object_T = kst_cast<DataString>(_list.at(i))) {
      DataSourcePtr dataSource;
      object_T->readLock();
      dataSource = object_T->dataSource();
      object_T->unlock();

      if (!dataSourceList.contains(dataSource)) {
        object_T->writeLock();
        object_T->reload();
        object_T->unlock();
        dataSourceList.append(dataSource);
      }
    } else if (DataScalarPtr object_X = kst_cast<DataScalar>(_list.at(i))) {
      DataSourcePtr dataSource;
      object_X->readLock();
      dataSource = object_X->dataSource();
      object_X->unlock();

      if (!dataSourceList.contains(dataSource)) {
        object_X->writeLock();
        object_X->reload();
        object_X->unlock();
        dataSourceList.append(dataSource);
      }
    } else if (DataMatrixPtr object_M = kst_cast<DataMatrix>(_list.at(i))) {
      DataSourcePtr dataSource;
      object_M->readLock();
      dataSource = object_M->dataSource();
      object_M->unlock();

      if (!dataSourceList.contains(dataSource)) {
        object_M->writeLock();
        object_M->reload();
        object_M->unlock();
        dataSourceList.append(dataSource);
      }
    }
  }

  dataSourceList.clear();

  cleanUpDataSourceList();
}
#endif

void ObjectStore::cleanUpDataSourceList() {
  DataSourceList currentSourceList;

  currentSourceList.clear();
  currentSourceList.append(_dataSourceList);
  // clean up unused data sources
  for (DataSourceList::Iterator it = currentSourceList.begin(); it != currentSourceList.end(); ++it) {
    if ((*it)->getUsage() <= 1) {
      removeObject(*it);
    }
  }
  currentSourceList.clear();
}

bool ObjectStore::isEmpty() const {
  KstReadLocker l(&_lock);
  return _list.isEmpty();
}


void ObjectStore::clear() {
  KstWriteLocker l(&_lock);
#if NAMEDEBUG > 0
  qDebug () << "Clearing object store " << (void*) this;
#endif
  foreach(DataSource *ds, _dataSourceList) {
    removeObject(ds);
  }
  foreach(Object *o, _list) {
    removeObject(o);
  }

  // Reset the named objects id's.
  NamedObject::resetNameIndex();
}


DataSourceList ObjectStore::dataSourceList() const {
  KstReadLocker l(&_lock);
  return _dataSourceList;
}

QList<ObjectPtr> ObjectStore::objectList() {
  KstReadLocker l(&_lock);
  return _list;
}

}
// vim: ts=2 sw=2 et
