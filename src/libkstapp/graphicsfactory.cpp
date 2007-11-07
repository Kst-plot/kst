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

#include "graphicsfactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, GraphicsFactory*> *factories = 0;

void cleanupGraphics() {
  foreach (GraphicsFactory *f, *factories) {
    delete f;
  }
  delete factories;
  factories = 0;
}


GraphicsFactory::GraphicsFactory() {
}


GraphicsFactory::~GraphicsFactory() {
}


void GraphicsFactory::registerFactory(const QString& node, GraphicsFactory *factory) {
  if (!factories) {
    factories = new QMap<QString,GraphicsFactory*>;
    qAddPostRoutine(cleanupGraphics);
  }
  factories->insert(node, factory);
}


void GraphicsFactory::registerFactory(const QStringList& nodes, GraphicsFactory *factory) {
  foreach (const QString n, nodes) {
    registerFactory(n, factory);
  }
}


ViewItem *GraphicsFactory::parse(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent) {
  if (!factories) {
    return 0;
  }

  GraphicsFactory *f = factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generateGraphics(stream, store, view, parent);
}


}

// vim: ts=2 sw=2 et
