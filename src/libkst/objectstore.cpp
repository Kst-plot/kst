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
  //QRegExp rx("\\(([A-Z]\\d+)\\)$|^(\\d+)$");
  rx.indexIn(name);
  shortName = rx.cap(2);

qDebug() << "---- short name retrieved: " << shortName << "from" << name;

  // 1) search for exact names
/*  for (int i = 0; i < _list.size(); ++i) {
    if (_list.at(i)->Name()==name)
      return _list.at(i);
  }*/
  // 2) search for short names
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
