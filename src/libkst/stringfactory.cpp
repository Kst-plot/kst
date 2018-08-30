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

#include "stringfactory.h"

#include "debug.h"
#include "string_kst.h"
#include "datastring.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"


namespace Kst {

StringFactory::StringFactory()
: PrimitiveFactory() {
  registerFactory(String::staticTypeTag, this);
}


StringFactory::~StringFactory() {
}


PrimitivePtr StringFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;

  Q_ASSERT(store);

  bool orphan = true;
  bool editable = true;
  QString value, descriptiveName;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "string") {
        QXmlStreamAttributes attrs = xml.attributes();
        value = attrs.value("value").toString();
        orphan = attrs.value("orphan").toString() == "true" ? true : false;
        editable = attrs.value("editable").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "string") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating string from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  StringPtr string = store->createObject<String>();
  string->setValue(value);
  string->setOrphan(orphan);
  string->setEditable(editable);
  string->setDescriptiveName(descriptiveName);

  string->registerChange();

  return string;
}

DataStringFactory::DataStringFactory()
: PrimitiveFactory() {
  registerFactory(DataString::staticTypeTag, this);
}


DataStringFactory::~DataStringFactory() {
}


PrimitivePtr DataStringFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QString descriptiveName;
  Q_ASSERT(store);

  QString file, field;
  int frame=0;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == DataString::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();

        file = DataPrimitive::readFilename(attrs);
        field = attrs.value("field").toString();
        frame = attrs.value("frame").toString().toInt();

        if (!store->override.fileName.isEmpty()) {
          file = store->override.fileName;
        }

        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == DataString::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating DataString from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  DataSourcePtr dataSource = DataSourcePluginManager::findOrLoadSource(store, file);

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  DataStringPtr dataString = store->createObject<DataString>();

  dataString->writeLock();
  dataString->change(dataSource, field, frame);

  dataString->setDescriptiveName(descriptiveName);
  dataString->registerChange();
  dataString->unlock();


  return dataString;
}


}

// vim: ts=2 sw=2 et
