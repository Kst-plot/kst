/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
class ObjectMap : public QHash<QString, SharedPtr<T> >
{
public:
  typedef QHash<QString, SharedPtr<T> > HashMap;

  typename HashMap::iterator insert(const QString& key, const SharedPtr<T>& value)
  {
    addKey(key);
    return HashMap::insert(key, value);
  }

  SharedPtr<T>& operator[](const QString& key)
  {
    addKey(key);
    return HashMap::operator[](key);
  }

  const SharedPtr<T> operator[](const QString& key) const
  {
    addKey(key);
    return HashMap::operator[](key);
  }

  void addKey(const QString& key) const
  {
    if (!ordered.contains(key)) {
      ordered << key;
    }
  }

  mutable QStringList ordered;
};

}

#endif

// vim: ts=2 sw=2 et
