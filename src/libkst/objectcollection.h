/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2006 The University of Toronto                        *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

// NAMEDEBUG: 0 for no debug, 1 for some debug, 2 for more debug, 3 for all debug
#define NAMEDEBUG 0

#include <qhash.h>
#include <qdebug.h>

#include "kst_export.h"
#include "object.h"
#include "objectlist.h"

class KstString;


namespace Kst {

// Forward Declarations
template <class T>
class ObjectTreeNode;

class Scalar;
class Vector;

// Typedefs
template <class T>
class ObjectNameIndex : public QHash<QString, QList<ObjectTreeNode<T> *>* > {
};


/** KstObject Naming Tree */
template <class T>
class ObjectTreeNode {
  public:
    ObjectTreeNode(const QString& tag = QString::null);
    ~ObjectTreeNode();

    QString nodeTag() const { return _tag; }
    QStringList fullTag() const;

    T *object() const { return _object; }

    ObjectTreeNode<T> *parent() const { return _parent; }
    ObjectTreeNode<T> *child(const QString& tag) const;
    QMap<QString, ObjectTreeNode<T> *> children() const { return _children; }

    ObjectTreeNode<T> *descendant(const QStringList& tag);
    const ObjectTreeNode<T> *descendant(const QStringList& tag) const;
    ObjectTreeNode<T> *addDescendant(T *o, ObjectNameIndex<T> *index = NULL);
    bool removeDescendant(T *o, ObjectNameIndex<T> *index = NULL);

    void clear();

  private:
    QString _tag;
    QPointer<T> _object;
    ObjectTreeNode<T> *_parent;
    QMap<QString, ObjectTreeNode<T> *> _children;
};


template <class T>
class ObjectCollection {
  public:
    ObjectCollection();
    ObjectCollection(const ObjectCollection& copy) { qDebug() << "CRASH! Deep copy required."; }
    ~ObjectCollection();

    typedef typename ObjectList<KstSharedPtr<T> >::const_iterator const_iterator;
    typedef typename ObjectList<KstSharedPtr<T> >::iterator iterator;
    bool addObject(T *o);
    bool removeObject(T *o);
    void doRename(T *o, const Kst::ObjectTag& newTag);

    KstSharedPtr<T> retrieveObject(QStringList tag) const;
    KstSharedPtr<T> retrieveObject(const Kst::ObjectTag& tag) const;
    bool tagExists(const QString& tag) const;
    bool tagExists(const Kst::ObjectTag& tag) const;

    // get the shortest unique tag in in_tag
    Kst::ObjectTag shortestUniqueTag(const Kst::ObjectTag& in_tag) const;
    // get the minimum number of tag components of in_tag necessary for a unique tag
    unsigned int componentsForUniqueTag(const Kst::ObjectTag& in_tag) const;

    void setUpdateDisplayTags(bool u);

    ObjectTreeNode<T> *nameTreeRoot() { return &_root; }

    // QValueList compatibility
    bool isEmpty() const { return _list.isEmpty(); }
    typename ObjectList<KstSharedPtr<T> >::size_type count() const { return _list.count(); }
    void append(T *o);
    void remove(T *o);
    void clear();
    QStringList tagNames() const;
    typename ObjectList<KstSharedPtr<T> >::Iterator removeTag(const QString& x);
    typename ObjectList<KstSharedPtr<T> >::Iterator findTag(const QString& x);
    typename ObjectList<KstSharedPtr<T> >::ConstIterator findTag(const QString& x) const;
    typename ObjectList<KstSharedPtr<T> >::Iterator findTag(const Kst::ObjectTag& x);
    typename ObjectList<KstSharedPtr<T> >::ConstIterator findTag(const Kst::ObjectTag& x) const;
    typename ObjectList<KstSharedPtr<T> >::Iterator begin();
    typename ObjectList<KstSharedPtr<T> >::ConstIterator begin() const;
    typename ObjectList<KstSharedPtr<T> >::Iterator end();
    typename ObjectList<KstSharedPtr<T> >::ConstIterator end() const;
    const ObjectList<KstSharedPtr<T> >& list() const { return _list; }
    KstSharedPtr<T>& operator[](int i) { return _list.at(i); }
    const KstSharedPtr<T>& operator[](int i) const { return _list.at(i); }

    // locking
    KstRWLock& lock() const { return _list.lock(); }

  private:
    QList<ObjectTreeNode<T> *> relatedNodes(T *obj);
    void relatedNodesHelper(T *o, ObjectTreeNode<T> *n, QHash<int, ObjectTreeNode<T>* >& nodes);

    // must be called AFTER the object is added to the index, while holding a write lock
    void updateAllDisplayTags();
    void updateDisplayTag(T *obj);
    void updateDisplayTags(QList<ObjectTreeNode<T> *> nodes);

    bool _updateDisplayTags;

    ObjectTreeNode<T> _root;
    ObjectNameIndex<T> _index;
    ObjectList<KstSharedPtr<T> > _list; // owns the objects
};

// FIXME: this should probably return a ObjectCollection
template<class T, class S>
ObjectList<KstSharedPtr<S> > ObjectSubList(ObjectCollection<T>& coll) {
  ObjectList<KstSharedPtr<T> > list = coll.list();
  list.lock().readLock();
  ObjectList<KstSharedPtr<S> > rc;
  typename ObjectList<KstSharedPtr<T> >::Iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    S *x = dynamic_cast<S*>((*it).data());
    if (x != 0L) {
      rc.append(x);
    }
  }

  list.lock().unlock();
  return rc;
}

template <class T>
ObjectTreeNode<T>::ObjectTreeNode(const QString& tag) : _tag(tag),
                                                           _object(NULL),
                                                           _parent(NULL)
{
}


template <class T>
ObjectTreeNode<T>::~ObjectTreeNode() {
  qDeleteAll(_children);
}


template <class T>
QStringList ObjectTreeNode<T>::fullTag() const {
  QStringList tag;
  const ObjectTreeNode *p = this;

  while (p) {
    if (!p->_tag.isEmpty()) {
      tag.prepend(p->_tag);
    }
    p = p->_parent;
  }

  return tag;
}


template <class T>
ObjectTreeNode<T> *ObjectTreeNode<T>::child(const QString& tag) const {
  typename QMap<QString, ObjectTreeNode<T> *>::ConstIterator i = _children.find(tag);
  if (i != _children.end()) {
    return *i;
  }
  return 0;
}


template <class T>
const ObjectTreeNode<T> *ObjectTreeNode<T>::descendant(const QStringList& tag) const {
  const ObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


template <class T>
ObjectTreeNode<T> *ObjectTreeNode<T>::descendant(const QStringList& tag) {
  ObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


template <class T>
ObjectTreeNode<T> *ObjectTreeNode<T>::addDescendant(T *o, ObjectNameIndex<T> *index) {
  if (!o) {
    return NULL;
  }

  QStringList tag = o->tag().fullTag();

  ObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    ObjectTreeNode<T> *nextNode = currNode->child(*i);
    if (!nextNode) {
      nextNode = new ObjectTreeNode<T>(*i);
      nextNode->_parent = currNode;
      currNode->_children[*i] = nextNode;
      if (index) {
        QList<ObjectTreeNode<T> *> *l = 0;
        typename ObjectNameIndex<T>::Iterator it = index->find(*i);
        if (it == index->end()) {
          l = new QList<ObjectTreeNode<T> *>;
          index->insert(*i, l);
        } else {
          l = *it;
        }
        l->append(nextNode);
      }
    }
    currNode = nextNode;
  }

  if (currNode->_object) {
#if NAMEDEBUG > 0
    qDebug() << "Tried to add KstObject to naming tree " << (void*)this << o->tag().tagString() << ", but there's already an object with that name";
#endif
    return NULL;
  } else {
    currNode->_object = o;
#if NAMEDEBUG > 0
    qDebug() << "Added KstObject to naming tree " << (void*)this << o->tag().tagString();
#endif
    return currNode;
  }
}


template <class T>
bool ObjectTreeNode<T>::removeDescendant(T *o, ObjectNameIndex<T> *index) {
  if (!o) {
    return false;
  }

  QStringList tag = o->tag().fullTag();

  ObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    ObjectTreeNode<T> *nextNode = currNode->child(*i);
    if (!nextNode) {
#if NAMEDEBUG > 0
      qDebug() << "Tried to remove KstObject from naming tree" << o->tag().tagString() << ", but the node is not in the tree";
#endif
      return false;
    }
    currNode = nextNode;
  }

  if (currNode->_object != QPointer<Kst::Object>(o)) {
#if NAMEDEBUG > 0
    qDebug() << "Tried to remove KstObject from naming tree" << o->tag().tagString() << ", but the object is not in the tree";
#endif
    return false;
  } else {
    currNode->_object = NULL;
    QStringList::ConstIterator i = tag.end();
    while (i != tag.begin() && currNode->_object.isNull() && currNode->_children.isEmpty()) {
      --i;
      ObjectTreeNode<T> *lastNode = currNode->_parent;
      lastNode->_children.remove(*i);
#if NAMEDEBUG > 1
      qDebug() << "Removed naming tree node" << currNode->fullTag().join(Kst::ObjectTag::tagSeparator);
#endif
      if (index) {
        QList<ObjectTreeNode<T> *> *l = index->take(*i);
        if (l) {
          l->removeAll(currNode);
          index->insert(*i, l);
        }
      }
      delete currNode;
      currNode = lastNode;
    }
#if NAMEDEBUG > 0
    qDebug() << "Removed KstObject from naming tree" << o->tag().tagString();
#endif
    return true;
  }
}

template <class T>
void ObjectTreeNode<T>::clear() {
  _tag = QString::null;
  _parent = 0;
  _object = 0;
  qDeleteAll(_children);
  _children.clear();
}


template <class T>
ObjectCollection<T>::ObjectCollection() : _updateDisplayTags(true)
{
}


template <class T>
ObjectCollection<T>::~ObjectCollection()
{
  qDeleteAll(_index);
}


template <class T>
bool ObjectCollection<T>::addObject(T *o) {
  if (!o) {
    return false;
  }

  _list.append(o);

  QList<ObjectTreeNode<T> *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  ObjectTreeNode<T> *n = _root.addDescendant(o, &_index);

  if (n) {
    if (_updateDisplayTags) {
      updateDisplayTag(o);
      updateDisplayTags(relNodes);
    }
    return true;
  } else {
    // TODO: handle failed insert?
    return false;
  }
}


template <class T>
bool ObjectCollection<T>::removeObject(T *o) {
  if (!o) {
    return false;
  }

  if (!_list.contains(o)) {
#if NAMEDEBUG > 1
    qDebug() << "Trying to delete a non-existant object from the collection: " << o->tag().tagString();
#endif
    return false;
  }

#if NAMEDEBUG > 1
  qDebug() << "Removing object from the collection: " << o->tag().tagString();
#endif

  QList<ObjectTreeNode<T> *> relNodes;
  if (_updateDisplayTags) {
#if NAMEDEBUG > 2
    qDebug() << "  fetching related nodes";
#endif
    relNodes = relatedNodes(o);
  }

#if NAMEDEBUG > 2
  qDebug() << "  removing object from tree";
#endif
  bool ok = _root.removeDescendant(o, &_index);

  if (ok) {
    if (_updateDisplayTags) {
#if NAMEDEBUG > 2
      qDebug() << "  updating display components";
#endif
      updateDisplayTags(relNodes);
    }

#if NAMEDEBUG > 2
    qDebug() << "  removing object from list";
#endif
    _list.removeAll(o);
  }

  return ok;
}


// Rename a KstObject in the collection.
//
// Updates the display components of all related objects. This can be somewhat
// expensive, but it shouldn't happen very often.
template <class T>
void ObjectCollection<T>::doRename(T *o, const Kst::ObjectTag& newTag) {
  if (!o) {
    return;
  }

  QList<ObjectTreeNode<T> *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  _root.removeDescendant(o, &_index);

  o->Kst::Object::setTagName(newTag);

  if (_root.addDescendant(o, &_index)) {
    if (_updateDisplayTags) {
      relNodes += relatedNodes(o);  // TODO: remove duplicates
      updateDisplayTag(o);
      updateDisplayTags(relNodes);
    }
  } else {
    // TODO: handle failed insert
  }
}


template <class T>
KstSharedPtr<T> ObjectCollection<T>::retrieveObject(QStringList tag) const {
#if NAMEDEBUG > 1
  qDebug() << "Retrieving object with tag" << tag.join(Kst::ObjectTag::tagSeparator);
#endif

  if (tag.isEmpty()) {
    return NULL;
  }

  typename ObjectNameIndex<T>::ConstIterator ni = _index.find(tag.first());
  if (ni != _index.end() && (*ni)->count() == 1) {
    // the first tag element is unique, so use the index
#if NAMEDEBUG > 2
    qDebug() << "  first tag element" << tag.first() << "is unique in index";
#endif

    ObjectTreeNode<T> *n = (*ni)->first();
    if (n) {
      tag.pop_front();
      n = n->descendant(tag);
    }
    if (n) {
#if NAMEDEBUG > 1
      qDebug() << "  found node, returning object " << (void*) n->object();
#endif
      return n->object();
    }
  }

  // search through the tree
  const ObjectTreeNode<T> *n = _root.descendant(tag);
  if (n) {
#if NAMEDEBUG > 1
    qDebug() << "  found node, returning object " << (void*) n->object();
#endif
    return n->object();
  } else {
#if NAMEDEBUG > 1
    qDebug() << "  node not found";
#endif
    return NULL;
  }
}

template <class T>
KstSharedPtr<T> ObjectCollection<T>::retrieveObject(const Kst::ObjectTag& tag) const {
  if (!tag.isValid()) {
    return NULL;
  }

  return retrieveObject(tag.fullTag());
}

template <class T>
bool ObjectCollection<T>::tagExists(const QString& tag) const {
  typename ObjectNameIndex<T>::ConstIterator ni = _index.find(tag);
  return ni != _index.end() && (*ni)->count() > 0;
}

template <class T>
bool ObjectCollection<T>::tagExists(const Kst::ObjectTag& tag) const {
  return 0 != _root.descendant(tag.fullTag());
}

template <class T>
Kst::ObjectTag ObjectCollection<T>::shortestUniqueTag(const Kst::ObjectTag& tag) const {
  QStringList in_tag = tag.fullTag();
  QStringList out_tag;

  QStringList::ConstIterator it = in_tag.end();
  if (it == in_tag.begin()) {
    return Kst::ObjectTag::invalidTag;
  }

  // add components starting from the end until a unique tag is found
  do {
    --it;
    out_tag.prepend(*it);
    typename ObjectNameIndex<T>::ConstIterator ni = _index.find(*it);
    if (ni != _index.end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return Kst::ObjectTag(out_tag);
}

template <class T>
unsigned int ObjectCollection<T>::componentsForUniqueTag(const Kst::ObjectTag& tag) const {
  QStringList in_tag = tag.fullTag();
  unsigned int components = 0;

  QStringList::ConstIterator it = in_tag.end();
  if (it == in_tag.begin()) {
    // tag is empty
    return components;
  }

  // add components starting from the end until a unique tag is found
  do {
    --it;
    components++;
    typename ObjectNameIndex<T>::ConstIterator ni = _index.find(*it);
    if (ni != _index.end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return components;
}


// ObjectList compatibility
template <class T>
void ObjectCollection<T>::append(T *o) {
  addObject(o);
}

template <class T>
void ObjectCollection<T>::remove(T *o) {
  removeObject(o);
}

template <class T>
void ObjectCollection<T>::clear() {
#if NAMEDEBUG > 0
  qDebug () << "Clearing object collection " << (void*) this;
#endif
  _root.clear();
  _index.clear();
  _list.clear();
}

template <class T>
QStringList ObjectCollection<T>::tagNames() const {
  return _list.tagNames();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::Iterator ObjectCollection<T>::removeTag(const QString& x) {
  // find object in tree
  T *obj = retrieveObject(Kst::ObjectTag::fromString(x));

  if (obj) {
    // remove object from tree
    _root.removeDescendant(obj, &_index);

    // remove object from list
    typename ObjectList<KstSharedPtr<T> >::Iterator it = _list.find(obj);
    if (it != _list.end()) {
      return _list.remove(it);
    }
  }
  return _list.end();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::Iterator ObjectCollection<T>::findTag(const Kst::ObjectTag& x) {
  T *obj = retrieveObject(x);
  if (obj) {
    // Can't use qFind() due to ambiguity.  Also have to do ugly implementation.
    typename ObjectList<KstSharedPtr<T> >::Iterator i = _list.begin();
    typename ObjectList<KstSharedPtr<T> >::Iterator j = _list.end();
    while (i != j) {
      if ((T*)(*i) == obj) {
        break;
      }
      ++i;
    }
    return i;
  } else {
    // For historical compatibility:
    // previously, output vectors of equations, PSDs, etc. were named PSD1-ABCDE-freq
    // now, they are PSD1-ABCDE/freq
    QString newTag = x.tagString();
    newTag.replace(newTag.lastIndexOf('-'), 1, Kst::ObjectTag::tagSeparator);
    obj = retrieveObject(Kst::ObjectTag::fromString(newTag));
    if (obj) {
      // Can't use qFind() due to ambiguity.
      typename ObjectList<KstSharedPtr<T> >::Iterator i = _list.begin();
      typename ObjectList<KstSharedPtr<T> >::Iterator j = _list.end();
      while (i != j) {
        if ((T*)(*i) == obj) {
          break;
        }
        ++i;
      }
      return i;
    }
  }
  return _list.end();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::Iterator ObjectCollection<T>::findTag(const QString& x) {
  return findTag(Kst::ObjectTag::fromString(x));
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::ConstIterator ObjectCollection<T>::findTag(const Kst::ObjectTag& x) const {
  T *obj = retrieveObject(x);
  if (obj) {
    return _list.find(obj);
  } else {
    // For historical compatibility:
    // previously, output vectors of equations, PSDs, etc. were named PSD1-ABCDE-freq
    // now, they are PSD1-ABCDE/freq
    QString newTag = x.tagString();
    newTag.replace(newTag.lastIndexOf('-'), 1, Kst::ObjectTag::tagSeparator);
    obj = retrieveObject(Kst::ObjectTag::fromString(newTag));
    if (obj) {
      return _list.find(obj);
    }
  }
  return _list.end();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::ConstIterator ObjectCollection<T>::findTag(const QString& x) const {
  return findTag(Kst::ObjectTag::fromString(x));
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::Iterator ObjectCollection<T>::begin() {
  return _list.begin();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::ConstIterator ObjectCollection<T>::begin() const {
  return _list.begin();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::Iterator ObjectCollection<T>::end() {
  return _list.end();
}

template <class T>
typename ObjectList<KstSharedPtr<T> >::ConstIterator ObjectCollection<T>::end() const {
  return _list.end();
}


template <class T>
void ObjectCollection<T>::setUpdateDisplayTags(bool u) {
  if (u && !_updateDisplayTags) {
    // turning on _updateDisplayTags, so do an update
    updateAllDisplayTags();
  }

  _updateDisplayTags = u;
}


// update the display tags for all the objects in the collection
template <class T>
void ObjectCollection<T>::updateAllDisplayTags() {
  Q_ASSERT(lock().myLockStatus() == KstRWLock::WRITELOCKED);

  for (typename ObjectList<KstSharedPtr<T> >::Iterator i = _list.begin(); i != _list.end(); ++i) {
    updateDisplayTag(*i);
  }
}

// must be called AFTER the object is added to the index, while holding a write lock
template <class T>
void ObjectCollection<T>::updateDisplayTag(T *obj) {
  if (!obj) {
    return;
  }

  Kst::ObjectTag tag = obj->tag();

  if (!_index.contains(tag.tag())) {
    return;
  }

  unsigned int nc = componentsForUniqueTag(tag);
  if (tag.uniqueDisplayComponents() != nc) {
#if NAMEDEBUG > 2
    qDebug() << "Changing display components on" << tag.tagString() << "from" << tag.uniqueDisplayComponents() << "to" << nc;
#endif
    obj->tag().setUniqueDisplayComponents(nc);
  }
}

template <class T>
void ObjectCollection<T>::updateDisplayTags(QList<ObjectTreeNode<T> *> nodes) {
  for (typename QList<ObjectTreeNode<T> *>::Iterator i = nodes.begin(); i != nodes.end(); ++i) {
    updateDisplayTag((*i)->object());
  }
}


// recursion helper
template <class T>
void ObjectCollection<T>::relatedNodesHelper(T *o, ObjectTreeNode<T> *n, QHash<int, ObjectTreeNode<T>* >& nodes) {
  Q_ASSERT(o);
  Q_ASSERT(n);
  if (n->object() && n->object() != o && !nodes.contains((long)n)) {
#if NAMEDEBUG > 2
          qDebug() << "Found related node to" << o->tag().tagString() << ":" << n->object()->tag().tagString(); 
#endif
    nodes.insert((long)n, n);
  }

  if (!n->children().isEmpty()) {
    // non-leaf node, so recurse
    QMap<QString, ObjectTreeNode<T> *> children = n->children();
    for (typename QMap<QString, ObjectTreeNode<T> *>::ConstIterator i = children.begin(); i != children.end(); ++i) {
      relatedNodesHelper(o, *i, nodes);
    }
  }
}

// Find the nodes with KstObjects which are affected by the addition or removal
// of an object with the given tag.
//
// There should not be any duplicates in the returned list.
template <class T>
QList<ObjectTreeNode<T> *> ObjectCollection<T>::relatedNodes(T *o) {
  QHash<int, ObjectTreeNode<T>* > nodes;
  QList<ObjectTreeNode<T> *> outNodes;

  if (!o) {
    return outNodes;
  }

#if NAMEDEBUG > 2
  qDebug() << "Looking for related nodes to" << o->tag().tagString(); 
  qDebug() << "index contains" << _index.keys();
#endif

  QStringList ft = o->tag().fullTag();

  for (QStringList::ConstIterator i = ft.begin(); i != ft.end(); ++i) {
    typename ObjectNameIndex<T>::ConstIterator ni = _index.find(*i);
    if (ni != _index.end()) {
      QList<ObjectTreeNode<T> *> *nodeList = *ni;
      for (typename QList<ObjectTreeNode<T> *>::ConstIterator i2 = nodeList->begin(); i2 != nodeList->end(); ++i2) {
        relatedNodesHelper(o, *i2, nodes);
      }
    }
  }

  QHashIterator<int, ObjectTreeNode<T>* > j(nodes);
  while (j.hasNext()) {
    j.next();
    outNodes << j.value();
  }
  return outNodes;
}

}
#endif

// vim: ts=2 sw=2 et
