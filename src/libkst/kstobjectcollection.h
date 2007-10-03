/***************************************************************************
              kstobjectcollection.h: collection of KstObjects
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

#ifndef KSTOBJECTCOLLECTION_H
#define KSTOBJECTCOLLECTION_H

// NAMEDEBUG: 0 for no debug, 1 for some debug, 2 for more debug, 3 for all debug
#define NAMEDEBUG 0

#include <qhash.h>
#include <qdebug.h>

#include "kst_export.h"
#include "kstobject.h"
#include "kstobjectlist.h"

// Forward Declarations
template <class T>
class KstObjectTreeNode;

class KstMatrix;
class Scalar;
class KstString;
class Vector;

// Typedefs
template <class T>
class KstObjectNameIndex : public QHash<QString, QList<KstObjectTreeNode<T> *>* > {
};


/** KstObject Naming Tree */
template <class T>
class KstObjectTreeNode {
  public:
    KstObjectTreeNode(const QString& tag = QString::null);
    ~KstObjectTreeNode();

    QString nodeTag() const { return _tag; }
    QStringList fullTag() const;

    T *object() const { return _object; }

    KstObjectTreeNode<T> *parent() const { return _parent; }
    KstObjectTreeNode<T> *child(const QString& tag) const;
    QMap<QString, KstObjectTreeNode<T> *> children() const { return _children; }

    KstObjectTreeNode<T> *descendant(const QStringList& tag);
    const KstObjectTreeNode<T> *descendant(const QStringList& tag) const;
    KstObjectTreeNode<T> *addDescendant(T *o, KstObjectNameIndex<T> *index = NULL);
    bool removeDescendant(T *o, KstObjectNameIndex<T> *index = NULL);

    void clear();

  private:
    QString _tag;
    QPointer<T> _object;
    KstObjectTreeNode<T> *_parent;
    QMap<QString, KstObjectTreeNode<T> *> _children;
};


template <class T>
class KstObjectCollection {
  public:
    KstObjectCollection();
    KstObjectCollection(const KstObjectCollection& copy) { qDebug() << "CRASH! Deep copy required."; }
    ~KstObjectCollection();

    typedef typename KstObjectList<KstSharedPtr<T> >::const_iterator const_iterator;
    typedef typename KstObjectList<KstSharedPtr<T> >::iterator iterator;
    bool addObject(T *o);
    bool removeObject(T *o);
    void doRename(T *o, const KstObjectTag& newTag);

    KstSharedPtr<T> retrieveObject(QStringList tag) const;
    KstSharedPtr<T> retrieveObject(const KstObjectTag& tag) const;
    bool tagExists(const QString& tag) const;
    bool tagExists(const KstObjectTag& tag) const;

    // get the shortest unique tag in in_tag
    KstObjectTag shortestUniqueTag(const KstObjectTag& in_tag) const;
    // get the minimum number of tag components of in_tag necessary for a unique tag
    unsigned int componentsForUniqueTag(const KstObjectTag& in_tag) const;

    void setUpdateDisplayTags(bool u);

    KstObjectTreeNode<T> *nameTreeRoot() { return &_root; }

    // QValueList compatibility
    bool isEmpty() const { return _list.isEmpty(); }
    typename KstObjectList<KstSharedPtr<T> >::size_type count() const { return _list.count(); }
    void append(T *o);
    void remove(T *o);
    void clear();
    QStringList tagNames() const;
    typename KstObjectList<KstSharedPtr<T> >::Iterator removeTag(const QString& x);
    typename KstObjectList<KstSharedPtr<T> >::Iterator findTag(const QString& x);
    typename KstObjectList<KstSharedPtr<T> >::ConstIterator findTag(const QString& x) const;
    typename KstObjectList<KstSharedPtr<T> >::Iterator findTag(const KstObjectTag& x);
    typename KstObjectList<KstSharedPtr<T> >::ConstIterator findTag(const KstObjectTag& x) const;
    typename KstObjectList<KstSharedPtr<T> >::Iterator begin();
    typename KstObjectList<KstSharedPtr<T> >::ConstIterator begin() const;
    typename KstObjectList<KstSharedPtr<T> >::Iterator end();
    typename KstObjectList<KstSharedPtr<T> >::ConstIterator end() const;
    const KstObjectList<KstSharedPtr<T> >& list() const { return _list; }
    KstSharedPtr<T>& operator[](int i) { return _list.at(i); }
    const KstSharedPtr<T>& operator[](int i) const { return _list.at(i); }

    // locking
    KstRWLock& lock() const { return _list.lock(); }

  private:
    QList<KstObjectTreeNode<T> *> relatedNodes(T *obj);
    void relatedNodesHelper(T *o, KstObjectTreeNode<T> *n, QHash<int, KstObjectTreeNode<T>* >& nodes);

    // must be called AFTER the object is added to the index, while holding a write lock
    void updateAllDisplayTags();
    void updateDisplayTag(T *obj);
    void updateDisplayTags(QList<KstObjectTreeNode<T> *> nodes);

    bool _updateDisplayTags;

    KstObjectTreeNode<T> _root;
    KstObjectNameIndex<T> _index;
    KstObjectList<KstSharedPtr<T> > _list; // owns the objects
};

// FIXME: this should probably return a KstObjectCollection
template<class T, class S>
KstObjectList<KstSharedPtr<S> > kstObjectSubList(KstObjectCollection<T>& coll) {
  KstObjectList<KstSharedPtr<T> > list = coll.list();
  list.lock().readLock();
  KstObjectList<KstSharedPtr<S> > rc;
  typename KstObjectList<KstSharedPtr<T> >::Iterator it;

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
KstObjectTreeNode<T>::KstObjectTreeNode(const QString& tag) : _tag(tag),
                                                           _object(NULL),
                                                           _parent(NULL)
{
}


template <class T>
KstObjectTreeNode<T>::~KstObjectTreeNode() {
  qDeleteAll(_children);
}


template <class T>
QStringList KstObjectTreeNode<T>::fullTag() const {
  QStringList tag;
  const KstObjectTreeNode *p = this;

  while (p) {
    if (!p->_tag.isEmpty()) {
      tag.prepend(p->_tag);
    }
    p = p->_parent;
  }

  return tag;
}


template <class T>
KstObjectTreeNode<T> *KstObjectTreeNode<T>::child(const QString& tag) const {
  typename QMap<QString, KstObjectTreeNode<T> *>::ConstIterator i = _children.find(tag);
  if (i != _children.end()) {
    return *i;
  }
  return 0;
}


template <class T>
const KstObjectTreeNode<T> *KstObjectTreeNode<T>::descendant(const QStringList& tag) const {
  const KstObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


template <class T>
KstObjectTreeNode<T> *KstObjectTreeNode<T>::descendant(const QStringList& tag) {
  KstObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


template <class T>
KstObjectTreeNode<T> *KstObjectTreeNode<T>::addDescendant(T *o, KstObjectNameIndex<T> *index) {
  if (!o) {
    return NULL;
  }

  QStringList tag = o->tag().fullTag();

  KstObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    KstObjectTreeNode<T> *nextNode = currNode->child(*i);
    if (!nextNode) {
      nextNode = new KstObjectTreeNode<T>(*i);
      nextNode->_parent = currNode;
      currNode->_children[*i] = nextNode;
      if (index) {
        QList<KstObjectTreeNode<T> *> *l = 0;
        typename KstObjectNameIndex<T>::Iterator it = index->find(*i);
        if (it == index->end()) {
          l = new QList<KstObjectTreeNode<T> *>;
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
bool KstObjectTreeNode<T>::removeDescendant(T *o, KstObjectNameIndex<T> *index) {
  if (!o) {
    return false;
  }

  QStringList tag = o->tag().fullTag();

  KstObjectTreeNode<T> *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    KstObjectTreeNode<T> *nextNode = currNode->child(*i);
    if (!nextNode) {
#if NAMEDEBUG > 0
      qDebug() << "Tried to remove KstObject from naming tree" << o->tag().tagString() << ", but the node is not in the tree";
#endif
      return false;
    }
    currNode = nextNode;
  }

  if (currNode->_object != QPointer<KstObject>(o)) {
#if NAMEDEBUG > 0
    qDebug() << "Tried to remove KstObject from naming tree" << o->tag().tagString() << ", but the object is not in the tree";
#endif
    return false;
  } else {
    currNode->_object = NULL;
    QStringList::ConstIterator i = tag.end();
    while (i != tag.begin() && currNode->_object.isNull() && currNode->_children.isEmpty()) {
      --i;
      KstObjectTreeNode<T> *lastNode = currNode->_parent;
      lastNode->_children.remove(*i);
#if NAMEDEBUG > 1
      qDebug() << "Removed naming tree node" << currNode->fullTag().join(KstObjectTag::tagSeparator);
#endif
      if (index) {
        QList<KstObjectTreeNode<T> *> *l = index->take(*i);
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
void KstObjectTreeNode<T>::clear() {
  _tag = QString::null;
  _parent = 0;
  _object = 0;
  qDeleteAll(_children);
  _children.clear();
}


template <class T>
KstObjectCollection<T>::KstObjectCollection() : _updateDisplayTags(true)
{
}


template <class T>
KstObjectCollection<T>::~KstObjectCollection()
{
  qDeleteAll(_index);
}


template <class T>
bool KstObjectCollection<T>::addObject(T *o) {
  if (!o) {
    return false;
  }

  _list.append(o);

  QList<KstObjectTreeNode<T> *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  KstObjectTreeNode<T> *n = _root.addDescendant(o, &_index);

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
bool KstObjectCollection<T>::removeObject(T *o) {
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

  QList<KstObjectTreeNode<T> *> relNodes;
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
void KstObjectCollection<T>::doRename(T *o, const KstObjectTag& newTag) {
  if (!o) {
    return;
  }

  QList<KstObjectTreeNode<T> *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  _root.removeDescendant(o, &_index);

  o->KstObject::setTagName(newTag);

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
KstSharedPtr<T> KstObjectCollection<T>::retrieveObject(QStringList tag) const {
#if NAMEDEBUG > 1
  qDebug() << "Retrieving object with tag" << tag.join(KstObjectTag::tagSeparator);
#endif

  if (tag.isEmpty()) {
    return NULL;
  }

  typename KstObjectNameIndex<T>::ConstIterator ni = _index.find(tag.first());
  if (ni != _index.end() && (*ni)->count() == 1) {
    // the first tag element is unique, so use the index
#if NAMEDEBUG > 2
    qDebug() << "  first tag element" << tag.first() << "is unique in index";
#endif

    KstObjectTreeNode<T> *n = (*ni)->first();
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
  const KstObjectTreeNode<T> *n = _root.descendant(tag);
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
KstSharedPtr<T> KstObjectCollection<T>::retrieveObject(const KstObjectTag& tag) const {
  if (!tag.isValid()) {
    return NULL;
  }

  return retrieveObject(tag.fullTag());
}

template <class T>
bool KstObjectCollection<T>::tagExists(const QString& tag) const {
  typename KstObjectNameIndex<T>::ConstIterator ni = _index.find(tag);
  return ni != _index.end() && (*ni)->count() > 0;
}

template <class T>
bool KstObjectCollection<T>::tagExists(const KstObjectTag& tag) const {
  return 0 != _root.descendant(tag.fullTag());
}

template <class T>
KstObjectTag KstObjectCollection<T>::shortestUniqueTag(const KstObjectTag& tag) const {
  QStringList in_tag = tag.fullTag();
  QStringList out_tag;

  QStringList::ConstIterator it = in_tag.end();
  if (it == in_tag.begin()) {
    return KstObjectTag::invalidTag;
  }

  // add components starting from the end until a unique tag is found
  do {
    --it;
    out_tag.prepend(*it);
    typename KstObjectNameIndex<T>::ConstIterator ni = _index.find(*it);
    if (ni != _index.end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return KstObjectTag(out_tag);
}

template <class T>
unsigned int KstObjectCollection<T>::componentsForUniqueTag(const KstObjectTag& tag) const {
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
    typename KstObjectNameIndex<T>::ConstIterator ni = _index.find(*it);
    if (ni != _index.end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return components;
}


// KstObjectList compatibility
template <class T>
void KstObjectCollection<T>::append(T *o) {
  addObject(o);
}

template <class T>
void KstObjectCollection<T>::remove(T *o) {
  removeObject(o);
}

template <class T>
void KstObjectCollection<T>::clear() {
#if NAMEDEBUG > 0
  qDebug () << "Clearing object collection " << (void*) this;
#endif
  _root.clear();
  _index.clear();
  _list.clear();
}

template <class T>
QStringList KstObjectCollection<T>::tagNames() const {
  return _list.tagNames();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::Iterator KstObjectCollection<T>::removeTag(const QString& x) {
  // find object in tree
  T *obj = retrieveObject(KstObjectTag::fromString(x));

  if (obj) {
    // remove object from tree
    _root.removeDescendant(obj, &_index);

    // remove object from list
    typename KstObjectList<KstSharedPtr<T> >::Iterator it = _list.find(obj);
    if (it != _list.end()) {
      return _list.remove(it);
    }
  }
  return _list.end();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::Iterator KstObjectCollection<T>::findTag(const KstObjectTag& x) {
  T *obj = retrieveObject(x);
  if (obj) {
    // Can't use qFind() due to ambiguity.  Also have to do ugly implementation.
    typename KstObjectList<KstSharedPtr<T> >::Iterator i = _list.begin();
    typename KstObjectList<KstSharedPtr<T> >::Iterator j = _list.end();
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
    newTag.replace(newTag.lastIndexOf('-'), 1, KstObjectTag::tagSeparator);
    obj = retrieveObject(KstObjectTag::fromString(newTag));
    if (obj) {
      // Can't use qFind() due to ambiguity.
      typename KstObjectList<KstSharedPtr<T> >::Iterator i = _list.begin();
      typename KstObjectList<KstSharedPtr<T> >::Iterator j = _list.end();
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
typename KstObjectList<KstSharedPtr<T> >::Iterator KstObjectCollection<T>::findTag(const QString& x) {
  return findTag(KstObjectTag::fromString(x));
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::ConstIterator KstObjectCollection<T>::findTag(const KstObjectTag& x) const {
  T *obj = retrieveObject(x);
  if (obj) {
    return _list.find(obj);
  } else {
    // For historical compatibility:
    // previously, output vectors of equations, PSDs, etc. were named PSD1-ABCDE-freq
    // now, they are PSD1-ABCDE/freq
    QString newTag = x.tagString();
    newTag.replace(newTag.lastIndexOf('-'), 1, KstObjectTag::tagSeparator);
    obj = retrieveObject(KstObjectTag::fromString(newTag));
    if (obj) {
      return _list.find(obj);
    }
  }
  return _list.end();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::ConstIterator KstObjectCollection<T>::findTag(const QString& x) const {
  return findTag(KstObjectTag::fromString(x));
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::Iterator KstObjectCollection<T>::begin() {
  return _list.begin();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::ConstIterator KstObjectCollection<T>::begin() const {
  return _list.begin();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::Iterator KstObjectCollection<T>::end() {
  return _list.end();
}

template <class T>
typename KstObjectList<KstSharedPtr<T> >::ConstIterator KstObjectCollection<T>::end() const {
  return _list.end();
}


template <class T>
void KstObjectCollection<T>::setUpdateDisplayTags(bool u) {
  if (u && !_updateDisplayTags) {
    // turning on _updateDisplayTags, so do an update
    updateAllDisplayTags();
  }

  _updateDisplayTags = u;
}


// update the display tags for all the objects in the collection
template <class T>
void KstObjectCollection<T>::updateAllDisplayTags() {
  Q_ASSERT(lock().myLockStatus() == KstRWLock::WRITELOCKED);

  for (typename KstObjectList<KstSharedPtr<T> >::Iterator i = _list.begin(); i != _list.end(); ++i) {
    updateDisplayTag(*i);
  }
}

// must be called AFTER the object is added to the index, while holding a write lock
template <class T>
void KstObjectCollection<T>::updateDisplayTag(T *obj) {
  if (!obj) {
    return;
  }

  KstObjectTag tag = obj->tag();

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
void KstObjectCollection<T>::updateDisplayTags(QList<KstObjectTreeNode<T> *> nodes) {
  for (typename QList<KstObjectTreeNode<T> *>::Iterator i = nodes.begin(); i != nodes.end(); ++i) {
    updateDisplayTag((*i)->object());
  }
}


// recursion helper
template <class T>
void KstObjectCollection<T>::relatedNodesHelper(T *o, KstObjectTreeNode<T> *n, QHash<int, KstObjectTreeNode<T>* >& nodes) {
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
    QMap<QString, KstObjectTreeNode<T> *> children = n->children();
    for (typename QMap<QString, KstObjectTreeNode<T> *>::ConstIterator i = children.begin(); i != children.end(); ++i) {
      relatedNodesHelper(o, *i, nodes);
    }
  }
}

// Find the nodes with KstObjects which are affected by the addition or removal
// of an object with the given tag.
//
// There should not be any duplicates in the returned list.
template <class T>
QList<KstObjectTreeNode<T> *> KstObjectCollection<T>::relatedNodes(T *o) {
  QHash<int, KstObjectTreeNode<T>* > nodes;
  QList<KstObjectTreeNode<T> *> outNodes;

  if (!o) {
    return outNodes;
  }

#if NAMEDEBUG > 2
  qDebug() << "Looking for related nodes to" << o->tag().tagString(); 
  qDebug() << "index contains" << _index.keys();
#endif

  QStringList ft = o->tag().fullTag();

  for (QStringList::ConstIterator i = ft.begin(); i != ft.end(); ++i) {
    typename KstObjectNameIndex<T>::ConstIterator ni = _index.find(*i);
    if (ni != _index.end()) {
      QList<KstObjectTreeNode<T> *> *nodeList = *ni;
      for (typename QList<KstObjectTreeNode<T> *>::ConstIterator i2 = nodeList->begin(); i2 != nodeList->end(); ++i2) {
        relatedNodesHelper(o, *i2, nodes);
      }
    }
  }

  QHashIterator<int, KstObjectTreeNode<T>* > j(nodes);
  while (j.hasNext()) {
    j.next();
    outNodes << j.value();
  }
  return outNodes;
}

#endif

// vim: ts=2 sw=2 et
