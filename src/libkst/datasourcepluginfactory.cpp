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

#include "datasourcepluginfactory.h"

#include "debug.h"
#include "datasource.h"
#include "datacollection.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"

namespace Kst {

DataSourcePluginFactory::DataSourcePluginFactory()
: DataSourceFactory() {
  registerFactory(DataSource::staticTypeTag, this);
}


DataSourcePluginFactory::~DataSourcePluginFactory() {
}


DataSourcePtr DataSourcePluginFactory::generateDataSource(ObjectStore *store, QXmlStreamReader& xml) {

  Q_ASSERT(store);

  QString fileType, fileName;
  QXmlStreamAttributes propertyAttributes;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == DataSource::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        fileType = attrs.value("reader").toString();
        fileName = attrs.value("file").toString();
      } else if (n == "properties") {
        propertyAttributes = xml.attributes();
        xml.readElementText();
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == DataSource::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating data source from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  DataSourcePtr dataSource = DataSourcePluginManager::loadSource(store, fileName, fileType);
  if (dataSource) {
    dataSource->parseProperties(propertyAttributes);
  }

  return dataSource;
}

}

// vim: ts=2 sw=2 et
