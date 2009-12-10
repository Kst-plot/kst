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

#include "datasourcefactory.h"

#include <QCoreApplication>
#include <QMap>

namespace Kst {

static QMap<QString, DataSourceFactory*> *factories = 0;

void cleanupDataSources() {
  foreach (DataSourceFactory *f, *factories) {
    delete f;
  }
  delete factories;
  factories = 0;
}


DataSourceFactory::DataSourceFactory() {
}


DataSourceFactory::~DataSourceFactory() {
}


void DataSourceFactory::registerFactory(const QString& node, DataSourceFactory *factory) {
  if (!factories) {
    factories = new QMap<QString,DataSourceFactory*>;
    qAddPostRoutine(cleanupDataSources);
  }
  factories->insert(node, factory);
}


void DataSourceFactory::registerFactory(const QStringList& nodes, DataSourceFactory *factory) {
  foreach (const QString n, nodes) {
    registerFactory(n, factory);
  }
}


DataSourcePtr DataSourceFactory::parse(ObjectStore *store, QXmlStreamReader& stream) {
  if (!factories) {
    return 0;
  }
  DataSourceFactory *f = factories->value(stream.name().toString());
  if (!f) {
    return 0;
  }

  return f->generateDataSource(store, stream);
}


}

// vim: ts=2 sw=2 et
