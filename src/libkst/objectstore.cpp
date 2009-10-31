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

#if 0
void ObjectStore::rebuildDataSourceList() {

  DataSourceList newDataSourceList;
  DataSourcePtr new_data_source;
  DataPrimitive* object_P;

  foreach(ObjectPtr object, _list) {
    object_P = dynamic_cast<DataPrimitive *>(object.data());
    if (object_P) {
      QString filename = object_P->filename();
      new_data_source = newDataSourceList.findReusableFileName(filename);
      if (new_data_source == 0) {
        new_data_source = DataSource::loadSource(this, filename);
        newDataSourceList.append(new_data_source);
      }
      //object->writeLock();
      object_P->changeFile(new_data_source);
      object->update();
      //object->unlock();
    }
  }

  newDataSourceList.clear();

  // clean up unused data sources
  for (DataSourceList::Iterator it = _dataSourceList.begin(); it != _dataSourceList.end(); ++it) {
    qDebug() << "Usage: " << (*it)->getUsage();
    if ((*it)->getUsage() == 0) {
      removeObject(*it);
    }
  }
}
#endif

void ObjectStore::rebuildDataSourceList() {

  DataSourceList dataSourceList;

  for (int i=0; i<_list.count(); i++) {
    DataVectorPtr object_P = kst_cast<DataVector>(_list.at(i));
    if (object_P) {
      DataSourcePtr dataSource;
      object_P->readLock();
      dataSource = object_P->dataSource();
      object_P->unlock();

      if (!dataSourceList.contains(dataSource)) {
        object_P->writeLock();
        object_P->reload();
        object_P->unlock();
        dataSourceList.append(dataSource);
      }
    }
  }

  dataSourceList.clear();
  dataSourceList.append(_dataSourceList);

  // clean up unused data sources
  for (DataSourceList::Iterator it = dataSourceList.begin(); it != dataSourceList.end(); ++it) {
    if ((*it)->getUsage() == 1) {
      removeObject(*it);
    }
  }
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


#if 0
void ObjectStore::setUpdateDisplayTags(bool u) {
  if (u && !_updateDisplayTags) {
    // turning on _updateDisplayTags, so do an update
    updateAllDisplayTags();
  }

  _updateDisplayTags = u;
}
#endif

DataSourceList ObjectStore::dataSourceList() const {
  KstReadLocker l(&_lock);
  return _dataSourceList;
}

}

// vim: ts=2 sw=2 et
