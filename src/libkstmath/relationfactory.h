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

#ifndef RELATIONFACTORY_H
#define RELATIONFACTORY_H

#include <QXmlStreamReader>
#include <QStringList>

#include "relation.h"
#include "kst_export.h"

namespace Kst {

class ObjectStore;

class RelationFactory {
  public:
    RelationFactory();
    virtual ~RelationFactory();

    // This takes ownership
    static void registerFactory(const QString& node, RelationFactory *factory);
    static void registerFactory(const QStringList& nodes, RelationFactory *factory);
    KST_EXPORT static RelationPtr parse(ObjectStore *store, QXmlStreamReader& stream);
    virtual RelationPtr generateRelation(ObjectStore *store, QXmlStreamReader& stream) = 0;
};

}

#endif

// vim: ts=2 sw=2 et
