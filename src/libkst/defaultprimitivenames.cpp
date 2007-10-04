/***************************************************************************
                          defaultprimitivenames.cpp
                             -------------------
    begin                : July 31, 2004
    copyright            : (C) 2003 C. Barth Netterfield
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

#include "datacollection.h"
#include "object.h"
#include "kstobjectcollection.h"
#include "defaultprimitivenames.h"

#include <qregexp.h>
#include "kst_i18n.h"
#include <stdio.h>

namespace Kst {

// takes a field name and returns a unique tag name, which will be
// the field if that is unique, or field-N if there are already N
// vectors of that name
QString suggestVectorName(const QString& field) {
  int i = 0;
  QString name(field);

  while (Data::self()->vectorTagNameNotUnique(name, false)) {
    name = QString("%1-%2").arg(field).arg(++i);
  }

  return name;
}


QString suggestMatrixName(const QString& vectorName) {
  int i = 1;
  QString name(vectorName);

  while (matrixList.tagExists(name)) {
    name = QString("%1-%2").arg(vectorName).arg(++i);
  }

  return name;
}


template <class T>
ObjectTag suggestUniqueTag(const ObjectTag& baseTag, const KstObjectCollection<T>& coll) {
  int i = 0;
  ObjectTag tag = baseTag;

  while (coll.tagExists(tag)) {
    tag.setTag((QString("%1-%2").arg(baseTag.tag()).arg(++i)));
  }

  return tag;
  /*
  QString name(baseTag.tag());

  while (coll.tagExists(name)) {
    name = QString("%1-%2").arg(baseTag.tag()).arg(++i);
  }

  return KstObjectTag(name, baseTag.context());
  */
}

ObjectTag suggestUniqueMatrixTag(ObjectTag baseTag) {
  return suggestUniqueTag(baseTag, matrixList);
}

ObjectTag suggestUniqueScalarTag(ObjectTag baseTag) {
  return suggestUniqueTag(baseTag, scalarList);
}

ObjectTag suggestUniqueStringTag(ObjectTag baseTag) {
  return suggestUniqueTag(baseTag, stringList);
}

ObjectTag suggestUniqueVectorTag(ObjectTag baseTag) {
  return suggestUniqueTag(baseTag, vectorList);
}

}

// vim: ts=2 sw=2 et
