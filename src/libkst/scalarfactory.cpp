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

#include "scalarfactory.h"

#include "debug.h"
#include "scalar.h"
#include "datascalar.h"
#include "vscalar.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"

namespace Kst {

ScalarFactory::ScalarFactory()
: PrimitiveFactory() {
  registerFactory(Scalar::staticTypeTag, this);
}


ScalarFactory::~ScalarFactory() {
}


PrimitivePtr ScalarFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QString descriptiveName;

  Q_ASSERT(store);

  bool orphan, editable;
  double value=0.0;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Scalar::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        value = attrs.value("value").toString().toDouble();
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
      if (n == "scalar") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating scalar from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  ScalarPtr scalar = store->createObject<Scalar>();
  scalar->setValue(value);
  scalar->setOrphan(true);
  scalar->setEditable(true);
  scalar->setDescriptiveName(descriptiveName);

  return scalar;
}

////////////////////////////////////////

DataScalarFactory::DataScalarFactory()
: PrimitiveFactory() {
  registerFactory(DataScalar::staticTypeTag, this);
}


DataScalarFactory::~DataScalarFactory() {
}


PrimitivePtr DataScalarFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QString descriptiveName;
  Q_ASSERT(store);

  QString provider, file, field;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == DataScalar::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        provider = attrs.value("provider").toString();
        file = DataPrimitive::readFilename(attrs);
        field = attrs.value("field").toString();

        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == DataScalar::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating scalar from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  Q_ASSERT(store);
  DataSourcePtr dataSource = DataSourcePluginManager::findOrLoadSource(store, file);

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  DataScalarPtr scalar = store->createObject<DataScalar>();

  scalar->writeLock();
  scalar->change(dataSource, field);

  scalar->setDescriptiveName(descriptiveName);
  scalar->registerChange();
  scalar->unlock();

  return scalar;
}

////////////////////////////////////////

VScalarFactory::VScalarFactory()
: PrimitiveFactory() {
  registerFactory(VScalar::staticTypeTag, this);
}


VScalarFactory::~VScalarFactory() {
}


PrimitivePtr VScalarFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QString descriptiveName;
  Q_ASSERT(store);

  QString provider, file, field;
  int f0=0;

  while (!xml.atEnd()) {
    const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == VScalar::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        provider = attrs.value("provider").toString();
        file = DataPrimitive::readFilename(attrs);
        field = attrs.value("field").toString();
        f0 = attrs.value("f0").toString().toInt();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == VScalar::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating scalar from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  Q_ASSERT(store);
  DataSourcePtr dataSource = DataSourcePluginManager::findOrLoadSource(store, file);

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  VScalarPtr scalar = store->createObject<VScalar>();

  scalar->writeLock();
  scalar->change(dataSource, field, f0);

  scalar->setDescriptiveName(descriptiveName);
  scalar->registerChange();
  scalar->unlock();

  return scalar;
}

}

// vim: ts=2 sw=2 et
