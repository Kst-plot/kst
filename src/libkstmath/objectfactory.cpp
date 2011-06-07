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

#include "objectfactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, ObjectFactory*> *factories = 0;

void cleanupObjects() {
  foreach (ObjectFactory *f, *factories) {
    delete f;
  }
  delete factories;
  factories = 0;
}


ObjectFactory::ObjectFactory() {
}


ObjectFactory::~ObjectFactory() {
}


void ObjectFactory::registerFactory(const QString& node, ObjectFactory *factory) {
  if (!factories) {
    factories = new QMap<QString,ObjectFactory*>;
    qAddPostRoutine(cleanupObjects);
  }
  factories->insert(node, factory);
}


void ObjectFactory::registerFactory(const QStringList& nodes, ObjectFactory *factory) {
  foreach (const QString &n, nodes) {
    registerFactory(n, factory);
  }
}


DataObjectPtr ObjectFactory::parse(ObjectStore *store, QXmlStreamReader& stream) {
  if (!factories) {
    return 0;
  }

  ObjectFactory *f = factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generateObject(store, stream);
}


}

// vim: ts=2 sw=2 et
