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

#include "primitivefactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, PrimitiveFactory*> *primitive_factories = 0;

void cleanupPrimitives() {
  foreach (PrimitiveFactory *f, *primitive_factories) {
    delete f;
  }
  delete primitive_factories;
  primitive_factories = 0;
}


PrimitiveFactory::PrimitiveFactory() {
}


PrimitiveFactory::~PrimitiveFactory() {
}


void PrimitiveFactory::registerFactory(const QString& node, PrimitiveFactory *factory) {
  if (!primitive_factories) {
    primitive_factories = new QMap<QString,PrimitiveFactory*>;
    qAddPostRoutine(cleanupPrimitives);
  }
  primitive_factories->insert(node, factory);
}


void PrimitiveFactory::registerFactory(const QStringList& nodes, PrimitiveFactory *factory) {
  foreach (const QString n, nodes) {
    registerFactory(n, factory);
  }
}


PrimitivePtr PrimitiveFactory::parse(ObjectStore *store, QXmlStreamReader& stream) {
  if (!primitive_factories) {
    return 0;
  }

  PrimitiveFactory *f = primitive_factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generatePrimitive(store, stream);
}


}

// vim: ts=2 sw=2 et
