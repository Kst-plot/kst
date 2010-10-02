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

static QMap<QString, PrimitiveFactory*> *factories = 0;

void cleanupPrimitives() {
  foreach (PrimitiveFactory *f, *factories) {
    delete f;
  }
  delete factories;
  factories = 0;
}


PrimitiveFactory::PrimitiveFactory() {
}


PrimitiveFactory::~PrimitiveFactory() {
}


void PrimitiveFactory::registerFactory(const QString& node, PrimitiveFactory *factory) {
  if (!factories) {
    factories = new QMap<QString,PrimitiveFactory*>;
    qAddPostRoutine(cleanupPrimitives);
  }
  factories->insert(node, factory);
}


void PrimitiveFactory::registerFactory(const QStringList& nodes, PrimitiveFactory *factory) {
  foreach (const QString n, nodes) {
    registerFactory(n, factory);
  }
}


PrimitivePtr PrimitiveFactory::parse(ObjectStore *store, QXmlStreamReader& stream) {
  if (!factories) {
    return 0;
  }

  PrimitiveFactory *f = factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generatePrimitive(store, stream);
}


}

// vim: ts=2 sw=2 et
