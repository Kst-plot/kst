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

#include "relationfactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, RelationFactory*> *relation_factories = 0;

void cleanupRelations() {
  foreach (RelationFactory *f, *relation_factories) {
    delete f;
  }
  delete relation_factories;
  relation_factories = 0;
}


RelationFactory::RelationFactory() {
}


RelationFactory::~RelationFactory() {
}


void RelationFactory::registerFactory(const QString& node, RelationFactory *factory) {
  if (!relation_factories) {
    relation_factories = new QMap<QString,RelationFactory*>;
    qAddPostRoutine(cleanupRelations);
  }
  relation_factories->insert(node, factory);
}


void RelationFactory::registerFactory(const QStringList& nodes, RelationFactory *factory) {
  foreach (const QString &n, nodes) {
    registerFactory(n, factory);
  }
}


RelationPtr RelationFactory::parse(ObjectStore *store, QXmlStreamReader& stream) {
  if (!relation_factories) {
    return 0;
  }

  RelationFactory *f = relation_factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generateRelation(store, stream);
}


}

// vim: ts=2 sw=2 et
