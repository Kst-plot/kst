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

class ObjectNameIndex : public QHash<QString, QList<ObjectTreeNode *>*>
{
};


ObjectTreeNode::ObjectTreeNode(const QString& tag) : _tag(tag),
  _object(NULL), _parent(NULL)
{
}


ObjectTreeNode::~ObjectTreeNode() {
  qDeleteAll(_children);
}


QStringList ObjectTreeNode::fullTag() const {
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


ObjectTreeNode *ObjectTreeNode::child(const QString& tag) const {
  QMap<QString, ObjectTreeNode *>::ConstIterator i = _children.find(tag);
  if (i != _children.end()) {
    return *i;
  }
  return 0;
}


const ObjectTreeNode *ObjectTreeNode::descendant(const QStringList& tag) const {
  const ObjectTreeNode *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


ObjectTreeNode *ObjectTreeNode::descendant(const QStringList& tag) {
  ObjectTreeNode *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    currNode = currNode->child(*i);
    if (!currNode) {
      return NULL;
    }
  }

  return currNode;
}


ObjectTreeNode *ObjectTreeNode::addDescendant(Object *o, ObjectNameIndex *index) {
  if (!o) {
    return NULL;
  }

  QStringList tag = o->tag().fullTag();

  ObjectTreeNode *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    ObjectTreeNode *nextNode = currNode->child(*i);
    if (!nextNode) {
      nextNode = new ObjectTreeNode(*i);
      nextNode->_parent = currNode;
      currNode->_children[*i] = nextNode;
      if (index) {
        QList<ObjectTreeNode *> *l = 0;
        ObjectNameIndex::Iterator it = index->find(*i);
        if (it == index->end()) {
          l = new QList<ObjectTreeNode *>;
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
    qDebug() << "Tried to add Object (" << o->typeString() << ") to naming tree " << (void*)this << o->tag().tagString() << ", but there's already an object with that name";
#endif
    return NULL;
  } else {
    currNode->_object = o;
#if NAMEDEBUG > 0
    qDebug() << "Added Object (" << o->typeString() << ") to naming tree " << (void*)this << o->tag().tagString();
#endif
    return currNode;
  }
}


bool ObjectTreeNode::removeDescendant(Object *o, ObjectNameIndex *index) {
  if (!o) {
    return false;
  }

  QStringList tag = o->tag().fullTag();

  ObjectTreeNode *currNode = this;
  for (QStringList::ConstIterator i = tag.begin(); i != tag.end(); ++i) {
    ObjectTreeNode *nextNode = currNode->child(*i);
    if (!nextNode) {
#if NAMEDEBUG > 0
      qDebug() << "Tried to remove Object from naming tree" << o->tag().tagString() << ", but the node is not in the tree";
#endif
      return false;
    }
    currNode = nextNode;
  }

  if (currNode->_object != QPointer<Object>(o)) {
#if NAMEDEBUG > 0
    qDebug() << "Tried to remove Object from naming tree" << o->tag().tagString() << ", but the object is not in the tree";
#endif
    return false;
  } else {
    currNode->_object = NULL;
    QStringList::ConstIterator i = tag.end();
    while (i != tag.begin() && currNode->_object.isNull() && currNode->_children.isEmpty()) {
      --i;
      ObjectTreeNode *lastNode = currNode->_parent;
      lastNode->_children.remove(*i);
#if NAMEDEBUG > 1
      qDebug() << "Removed naming tree node" << currNode->fullTag().join(ObjectTag::tagSeparator);
#endif
      if (index) {
        QList<ObjectTreeNode *> *l = index->take(*i);
        if (l) {
          l->removeAll(currNode);
          if (!l->isEmpty()) {
            index->insert(*i, l);
          }
        }
      }
      delete currNode;
      currNode = lastNode;
    }
#if NAMEDEBUG > 0
    qDebug() << "Removed Object from naming tree" << o->tag().tagString();
#endif
    return true;
  }
}


void ObjectTreeNode::clear() {
  _tag = QString::null;
  _parent = 0;
  _object = 0;
  qDeleteAll(_children);
  _children.clear();
}


bool ObjectTreeNode::isEmpty() const {
  return _tag.isEmpty() && !_object && !_parent && _children.isEmpty();
}


//==============================================================================


ObjectStore::ObjectStore() : _updateDisplayTags(true), _root(new ObjectTreeNode), _index(new ObjectNameIndex)
{
}


ObjectStore::~ObjectStore()
{
  qDeleteAll(*_index);
  delete _root;
  delete _index;
}


bool ObjectStore::removeObject(Object *o) {
  if (!o) {
    return false;
  }

  KstWriteLocker(&this->_lock);

  if (!_list.contains(o)) {
#if NAMEDEBUG > 1
    qDebug() << "Trying to delete a non-existant object from the store: " << o->tag().tagString();
#endif
    return false;
  }

#if NAMEDEBUG > 1
  qDebug() << "Removing object from the store: " << o->tag().tagString();
#endif

  QList<ObjectTreeNode *> relNodes;
  if (_updateDisplayTags) {
#if NAMEDEBUG > 2
    qDebug() << "  fetching related nodes";
#endif
    relNodes = relatedNodes(o);
  }

#if NAMEDEBUG > 2
  qDebug() << "  removing object from tree";
#endif
  bool ok = _root->removeDescendant(o, _index);

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

  o->_store = 0;

  return ok;
}


// Rename a Object in the store. Also renames objects lower in the naming tree.
//
// Updates the display components of all related objects. This can be somewhat
// expensive, but it shouldn't happen very often.
bool ObjectStore::renameObject(Object *o, const ObjectTag& newTag) {
  if (!o) {
    return false;
  }

  KstWriteLocker(&this->_lock);

  if (objectExists(newTag)) {
    // TODO: what to do if tag exists?
    qWarning() << "Trying to rename object " << o->tag().tagString() << " to " << newTag.tagString() << ", which already exists";
    return false;
  }

  QList<ObjectTreeNode *> relNodes;
  if (_updateDisplayTags) {
    relNodes = relatedNodes(o);
  }

  _root->removeDescendant(o, _index);

  o->_tag = newTag;
  o->setObjectName(newTag.tagString().toLocal8Bit().data());

  if (_root->addDescendant(o, _index)) {
    if (_updateDisplayTags) {
      relNodes += relatedNodes(o);  // TODO: remove duplicates
      updateDisplayTag(o);
      updateDisplayTags(relNodes);
    }
  } else {
    // TODO: handle failed insert
	return false;
  }

  return true;
}


ObjectPtr ObjectStore::retrieveObject(const ObjectTag& tag) const {
  if (!tag.isValid()) {
    return NULL;
  }

  KstReadLocker(&this->_lock);

  QStringList fullTag = tag.fullTag();

#if NAMEDEBUG > 1
  qDebug() << "Retrieving object with tag" << fullTag.join(ObjectTag::tagSeparator);
#endif

  if (fullTag.isEmpty()) {
    return NULL;
  }

  ObjectNameIndex::ConstIterator ni = _index->find(fullTag.first());
  if (ni != _index->end() && (*ni)->count() == 1) {
    // the first fullTag element is unique, so use the index
#if NAMEDEBUG > 2
    qDebug() << "  first tag element" << fullTag.first() << "is unique in index";
#endif

    ObjectTreeNode *n = (*ni)->first();
    if (n) {
      fullTag.pop_front();
      n = n->descendant(fullTag);
    }
    if (n) {
#if NAMEDEBUG > 1
      qDebug() << "  found node, returning object " << (void*) n->object();
#endif
      return n->object();
    }
  }

  // search through the tree
  const ObjectTreeNode *n = _root->descendant(fullTag);
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


bool ObjectStore::objectExists(const ObjectTag& tag) const {
  KstReadLocker(&this->_lock);
  ObjectTreeNode *n = _root->descendant(tag.fullTag());
  return n && n->object();
}


ObjectTag ObjectStore::shortestUniqueTag(const ObjectTag& tag) const {
  QStringList in_tag = tag.fullTag();
  QStringList out_tag;

  QStringList::ConstIterator it = in_tag.end();
  if (it == in_tag.begin()) {
    return ObjectTag::invalidTag;
  }

  // add components starting from the end until a unique tag is found
  do {
    --it;
    out_tag.prepend(*it);
    ObjectNameIndex::ConstIterator ni = _index->find(*it);
    if (ni != _index->end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return ObjectTag(out_tag);
}

unsigned int ObjectStore::componentsForUniqueTag(const ObjectTag& tag) const {
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
    ObjectNameIndex::ConstIterator ni = _index->find(*it);
    if (ni != _index->end() && (*ni)->count() == 1) {
      // found unique tag
      break;
    }
  } while (it != in_tag.begin());

  return components;
}


// get a unique tag based on in_tag
ObjectTag ObjectStore::getUniqueTag(const ObjectTag& in_tag) const {
  if (!objectExists(in_tag)) {
    return in_tag;
  } else {
    // append an integer to the name to make it unique
    ObjectTag uniqueTag = in_tag;
    QString uniqueName = QString("%1 %2").arg(uniqueTag.name());
    int i=2;
    do {
      uniqueTag.setName(uniqueName.arg(i++));
    } while (objectExists(uniqueTag));
    return uniqueTag;
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

  Q_ASSERT(_index->isEmpty());
  Q_ASSERT(_root->isEmpty());
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


// update the display tags for all the objects in the store
void ObjectStore::updateAllDisplayTags() {
  Q_ASSERT(lock().myLockStatus() == KstRWLock::WRITELOCKED);

  for (QList<ObjectPtr>::Iterator i = _list.begin(); i != _list.end(); ++i) {
    updateDisplayTag(*i);
  }
}

// must be called AFTER the object is added to the index, while holding a write lock
void ObjectStore::updateDisplayTag(Object *obj) {
  if (!obj) {
    return;
  }

  ObjectTag tag = obj->tag();

  if (!_index->contains(tag.name())) {
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

void ObjectStore::updateDisplayTags(QList<ObjectTreeNode *> nodes) {
  for (QList<ObjectTreeNode *>::Iterator i = nodes.begin(); i != nodes.end(); ++i) {
    updateDisplayTag((*i)->object());
  }
}


// recursion helper
void ObjectStore::relatedNodesHelper(Object *o, ObjectTreeNode *n, QHash<int, ObjectTreeNode* >& nodes) {
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
    QMap<QString, ObjectTreeNode *> children = n->children();
    for (QMap<QString, ObjectTreeNode *>::ConstIterator i = children.begin(); i != children.end(); ++i) {
      relatedNodesHelper(o, *i, nodes);
    }
  }
}


// Find the nodes with Objects which are affected by the addition or removal
// of an object with the given tag.
//
// There should not be any duplicates in the returned list.
QList<ObjectTreeNode *> ObjectStore::relatedNodes(Object *o) {
  QHash<int, ObjectTreeNode* > nodes;
  QList<ObjectTreeNode *> outNodes;

  if (!o) {
    return outNodes;
  }

#if NAMEDEBUG > 2
  qDebug() << "Looking for related nodes to" << o->tag().tagString();
  qDebug() << "index contains" << _index->keys();
#endif

  QStringList ft = o->tag().fullTag();

  for (QStringList::ConstIterator i = ft.begin(); i != ft.end(); ++i) {
    ObjectNameIndex::ConstIterator ni = _index->find(*i);
    if (ni != _index->end()) {
      QList<ObjectTreeNode *> *nodeList = *ni;
      for (QList<ObjectTreeNode *>::ConstIterator i2 = nodeList->begin(); i2 != nodeList->end(); ++i2) {
        relatedNodesHelper(o, *i2, nodes);
      }
    }
  }

  QHashIterator<int, ObjectTreeNode*> hi(nodes);
  while (hi.hasNext()) {
    hi.next();
    outNodes << hi.value();
  }
  return outNodes;
}


DataSourceList ObjectStore::dataSourceList() const {
  KstReadLocker l(&_lock);
  return _dataSourceList;
}


QString ObjectStore::abbreviate(const QString& string) {
  QString abbrev = string.simplified();
  int goodChars = 1;
  int spaceIndex;
  while ((spaceIndex = abbrev.indexOf(' ')) != -1) {
    abbrev.remove(goodChars, spaceIndex - goodChars + 1);
    goodChars++;
  }
  abbrev.truncate(goodChars);
  return abbrev;
}


}

// vim: ts=2 sw=2 et
