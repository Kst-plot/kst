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

#include "relationfactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, RelationFactory*> *factories = 0;

void cleanupRelations() {
  foreach (RelationFactory *f, *factories) {
    delete f;
  }
  delete factories;
  factories = 0;
}


RelationFactory::RelationFactory() {
}


RelationFactory::~RelationFactory() {
}


void RelationFactory::registerFactory(const QString& node, RelationFactory *factory) {
  if (!factories) {
    factories = new QMap<QString,RelationFactory*>;
    qAddPostRoutine(cleanupRelations);
  }
  factories->insert(node, factory);
}


void RelationFactory::registerFactory(const QStringList& nodes, RelationFactory *factory) {
  foreach (const QString n, nodes) {
    registerFactory(n, factory);
  }
}


KstDataObjectPtr RelationFactory::parse(QXmlStreamReader& stream) {
  if (!factories) {
    return 0;
  }

  RelationFactory *f = factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generateRelation(stream);
}


}

// vim: ts=2 sw=2 et
