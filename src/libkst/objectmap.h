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

#ifndef OBJECTMAP_H
#define OBJECTMAP_H

#include <QHash>
#include <QStringList>

#include "sharedptr.h"

namespace Kst {

template<class T>
class ObjectMap : public QHash<QString, SharedPtr<T> > {
#if 0
  public:
    ObjectMap() : QMap<QString,T>() {}
    virtual ~ObjectMap() {}

    virtual QStringList tagNames() {
      QStringList rc;
      for (typename QMap<QString,T>::ConstIterator it = QMap<QString,T>::begin(); it != QMap<QString,T>::end(); ++it) {
        rc << it.value()->tagName();
      }
      return rc;
    }

    // @since 1.1.0
    QStringList tagNames() const {
      QStringList rc;
      for (typename QMap<QString,T>::ConstIterator it = QMap<QString,T>::begin(); it != QMap<QString,T>::end(); ++it) {
        rc << it.value()->tagName();
      }
      return rc;
    }

    // Careful - sets key(value) == value->tagName();
    typename QMap<QString,T>::iterator insertObject(const T& value) {
      return QMap<QString,T>::insert(value->tagName(), value);
    }

    typename QMap<QString,T>::iterator findTag(const QString& tag) {
      typename QMap<QString,T>::iterator i;
      for (i = QMap<QString,T>::begin(); i != QMap<QString,T>::end(); ++i) {
        if (i.value()->tagName() == tag) {
          break;
        }
      }
      return i;
    }

/*
These are wrong.  We should not assume that key(x) == x->tagName().
    bool contains(const T& value) {
      return QMap<QString,T>::contains(value->tagName());
    }

    typename QMap<QString,T>::iterator find(const T& value) {
      return QMap<QString,T>::find(value->tagName());
    }

    void remove(const T& value) {
      QMap<QString,T>::remove(value->tagName());
    }

*/
#endif
};

}

#endif

// vim: ts=2 sw=2 et
