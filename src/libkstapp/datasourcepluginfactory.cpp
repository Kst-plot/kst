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

#include "datasourcepluginfactory.h"

#include "debug.h"
#include "datasource.h"
#include "datacollection.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "baddatasourcedialog.h"

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
        fileName = DataPrimitive::readFilename(attrs);
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

  if (!store->override.fileName.isEmpty()) {
    fileName = store->override.fileName;
  }

  DataSourcePtr dataSource = 0L;
  QString alternate_filename = fileName;
  do {
    dataSource = 0L;
    dataSource = DataSourcePluginManager::loadSource(store, fileName, fileType);
    if (dataSource) {
      dataSource->parseProperties(propertyAttributes);
      if (fileName != alternate_filename) {
        dataSource->setAlternateFilename(alternate_filename);
      }
      return dataSource;
    } else {
      alternate_filename = fileName;
      BadDatasourceDialog dialog(&fileName, store);
      dialog.exec();
    }
  } while (!fileName.isEmpty());

  return NULL;
}

}

// vim: ts=2 sw=2 et
