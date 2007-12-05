/***************************************************************************
              objectstore.h: store of Objects
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

#ifndef OBJECTSTORE_H
#define OBJECTSTORE_H

#include <QDebug>

#include "kst_export.h"
#include "kst_i18n.h"
#include "object.h"
#include "objectlist.h"
#include "rwlock.h"
#include "datasource.h"

namespace Kst {

// Forward Declarations
//class Matrix;
//class Scalar;
//class KstString;
//class Vector;
//class DataSource;

class ObjectNameIndex;

/** Object Naming Tree */
class ObjectTreeNode
{
  public:
    ObjectTreeNode(const QString& tag = QString::null);
    ~ObjectTreeNode();

    QString nodeTag() const { return _tag; }
    QStringList fullTag() const;

    Object *object() const { return _object; }

    ObjectTreeNode *parent() const { return _parent; }
    ObjectTreeNode *child(const QString& tag) const;
    QMap<QString, ObjectTreeNode *> children() const { return _children; }

    ObjectTreeNode *descendant(const QStringList& tag);
    const ObjectTreeNode *descendant(const QStringList& tag) const;
    ObjectTreeNode *addDescendant(Object *o, ObjectNameIndex *index);
    bool removeDescendant(Object *o, ObjectNameIndex *index);

    void clear();
    bool isEmpty() const;

  private:
    QString _tag;
    QPointer<Object> _object;
    ObjectTreeNode *_parent;
    QMap<QString, ObjectTreeNode *> _children;
};


// The ObjectStore is responsible for storing all the Objects in an
// application. The store handles hierarchical naming (tags).
class KST_EXPORT ObjectStore {
  public:
    ObjectStore();
    ~ObjectStore();

    template<class T> SharedPtr<T> createObject(const ObjectTag& tag = ObjectTag::invalidTag);
    template<class T> SharedPtr<T> createObject(const QDomElement& e);

    template<class T> ObjectTag suggestObjectTag(const QString& basedOn, const ObjectTag& contextTag = ObjectTag::invalidTag) const;

    // Add an object to the store. If the object has no tag, a tag will be assigned. If the object's tag is not unique, it will be made unique.
    template<class T> bool addObject(T *o);
    bool removeObject(Object *o);

    // Change object o's tag to newTag.
    // Also rename objects below o in the naming tree appropriately.
    //
    // Returns true if successful.
    bool renameObject(Object *o, const ObjectTag& newTag);

    ObjectPtr retrieveObject(const ObjectTag& tag) const;
    bool objectExists(const ObjectTag& tag) const;

//    void setUpdateDisplayTags(bool u);

    bool isEmpty() const;
    void clear();

    // get a list containing only objects of type T in the object store
    // T must inherit from Kst::Object
    template<class T> const ObjectList<T> getObjects() const;

    // get just the data sources
    DataSourceList dataSourceList() const;

    // locking
    KstRWLock& lock() const { return _lock; }

  private:
    Q_DISABLE_COPY(ObjectStore)

    QList<ObjectTreeNode *> relatedNodes(Object *obj);
    void relatedNodesHelper(Object *o, ObjectTreeNode *n, QHash<int, ObjectTreeNode *>& nodes);

    // get the minimum number of tag components of in_tag necessary for a unique tag
    unsigned int componentsForUniqueTag(const ObjectTag& in_tag) const;
    // get the shortest unique tag in in_tag
    ObjectTag shortestUniqueTag(const ObjectTag& in_tag) const;
    // get a unique tag based on in_tag
    ObjectTag getUniqueTag(const ObjectTag& in_tag) const;

    static QString abbreviate(const QString& string);

    // must be called AFTER the object is added to the index, while holding a write lock
    void updateAllDisplayTags();
    void updateDisplayTag(Object *obj);
    void updateDisplayTags(QList<ObjectTreeNode *> nodes);

    bool _updateDisplayTags;

    mutable KstRWLock _lock;
    ObjectTreeNode *_root;
    ObjectNameIndex *_index;

    // objects are stored in these lists
    DataSourceList _dataSourceList;
    QList<ObjectPtr> _list;
};


// this is an inefficient implementation for now
template<class T>
const ObjectList<T> ObjectStore::getObjects() const {
  KstReadLocker l(&(this->_lock));
  ObjectList<T> rc;

  for (QList<ObjectPtr>::ConstIterator it = _list.begin(); it != _list.end(); ++it) {
    SharedPtr<T> x = kst_cast<T>(*it);
    if (x != 0) {
      rc.append(x);
    }
  }

  return rc;
}


template<class T>
SharedPtr<T> ObjectStore::createObject(const ObjectTag& tag) {
  KstWriteLocker l(&(this->_lock));
  T *object = new T(this, tag);
  addObject(object);

  return SharedPtr<T>(object);
}


template<class T>
SharedPtr<T> ObjectStore::createObject(const QDomElement& e) {
  KstWriteLocker l(&(this->_lock));
  T *object = new T(this, e);
  addObject(object);

  return SharedPtr<T>(object);
}


template<class T>
ObjectTag ObjectStore::suggestObjectTag(const QString& basedOn, const ObjectTag& contextTag) const {
  KstReadLocker l(&(this->_lock));
  ObjectTag tag(QString::null, contextTag, false);
  if (basedOn.isEmpty()) {
    // anonymous object
    QString name = i18n("Anonymous %1 %2").arg(T::staticTypeString);
    int i=1;
    do {
      tag.setName(name.arg(i++));
    } while (objectExists(tag));
    return tag;
  } else {
    QString name = QString("%1-%2").arg(abbreviate(T::staticTypeString)).arg(basedOn);
    tag.setName(name);
    return getUniqueTag(tag);
  }
}


// Add an object to the store. If the object has an invalid or non-unique tag, a tag will be assigned.
template<class T>
bool ObjectStore::addObject(T *o) {
  if (!o) {
    return false;
  }

  KstWriteLocker l(&this->_lock);

  o->_store = 0L;

  if (!o->tag().isValid()) {
    o->setTagName(suggestObjectTag<T>(QString::null)); // won't recurse since o->_store is NULL
  } else if (objectExists(o->tag())) {
    o->setTagName(getUniqueTag(o->tag()));  // won't recurse since o->_store is NULL
  }

  // put the object in the right place depending on its type
  if (DataSourcePtr ds = kst_cast<DataSource>(o)) {
    _dataSourceList.append(ds);
  } else {
    _list.append(o);
  }

  // add the object to the naming tree
  QList<ObjectTreeNode *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  if (_root->addDescendant(o, _index)) {
    if (_updateDisplayTags) {
      updateDisplayTag(o);
      updateDisplayTags(relNodes);
    }

    o->_store = this;

    return true;
  } else {
    // TODO: handle failed insert?
    return false;
  }
}


}
#endif

// vim: ts=2 sw=2 et
