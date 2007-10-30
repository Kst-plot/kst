/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QList>
#include <QStringList>

#include "rwlock.h"
#include "sharedptr.h"

namespace Kst {

template<class T>
class ObjectList : public QList<SharedPtr<T> > {
};

}

#if 0
template<class T>
class ObjectList : public QList<T> {
  public:
    ObjectList() : QList<T>() {}
    ObjectList(const ObjectList<T>& x) : QList<T>(x) {}
    virtual ~ObjectList() { }

    ObjectList& operator=(const ObjectList& l) {
      this->QList<T>::operator=(l);
      return *this;
    }

    virtual QStringList tagNames() {
      QStringList rc;
      for (typename QList<T>::ConstIterator it = QList<T>::begin(); it != QList<T>::end(); ++it) {
        rc << (*it)->tagName();
      }
      return rc;
    }

    // @since 1.1.0
    QStringList tagNames() const {
      QStringList rc;
      for (typename QList<T>::ConstIterator it = QList<T>::begin(); it != QList<T>::end(); ++it) {
        rc << (*it)->tagName();
      }
      return rc;
    }

    virtual typename QList<T>::Iterator findTag(const QString& x) {
      for (typename QList<T>::Iterator it = QList<T>::begin(); it != QList<T>::end(); ++it) {
        if (*(*it) == x) {
          return it;
        }
      }
      return QList<T>::end();
    }

    virtual typename QList<T>::ConstIterator findTag(const QString& x) const {
      for (typename QList<T>::ConstIterator it = QList<T>::begin(); it != QList<T>::end(); ++it) {
        if (*(*it) == x) {
          return it;
        }
      }
      return QList<T>::end();
    }

    virtual int findIndexTag(const QString& x) const {
      int i = 0;
      for (typename QList<T>::ConstIterator it = QList<T>::begin(); it != QList<T>::end(); ++it) {
        if (*(*it) == x) {
          return i;
        }
        i++;
      }
      return -1;
    }

    virtual typename QList<T>::Iterator removeTag(const QString& x) {
      typename QList<T>::Iterator it = findTag(x);
      if (it != QList<T>::end()) {
        return QList<T>::erase(it);
      }
      return it;
    }

    KstRWLock& lock() const { return _lock; }

  private:
    mutable KstRWLock _lock;
};

/* Does locking for you automatically. */
template<class T, class S>
ObjectList<SharedPtr<S> > kstObjectSubList(ObjectList<SharedPtr<T> >& list) {
  list.lock().readLock();
  ObjectList<SharedPtr<S> > rc;
  typename ObjectList<SharedPtr<T> >::Iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    S *x = dynamic_cast<S*>((*it).data());
    if (x != 0L) {
      rc.append(x);
    }
  }

  list.lock().unlock();
  return rc;
}


/* Does locking for you automatically. */
template<class T, class S>
void kstObjectSplitList(ObjectList<SharedPtr<T> >& list, ObjectList<SharedPtr<S> >& inclusive, ObjectList<SharedPtr<T> >& exclusive) {
  list.lock().readLock();
  typename ObjectList<SharedPtr<T> >::Iterator it;

  for (it = list.begin(); it != list.end(); ++it) {
    S *x = dynamic_cast<S*>((*it).data());
    if (x != 0L) {
      inclusive.append(x);
    } else {
      exclusive.append(*it);
    }
  }

  list.lock().unlock();
}

#endif
#endif

// vim: ts=2 sw=2 et
