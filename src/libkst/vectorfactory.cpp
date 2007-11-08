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

#include "vectorfactory.h"

#include "debug.h"
#include "vector.h"
#include "generatedvector.h"
#include "editablevector.h"
#include "datavector.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

VectorFactory::VectorFactory()
: PrimitiveFactory() {
  registerFactory(Vector::staticTypeTag, this);
}


VectorFactory::~VectorFactory() {
}


PrimitivePtr VectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  QByteArray data;

  Q_ASSERT(store);

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "vector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "vector") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  VectorPtr vector = store->createObject<Vector>(tag);
  vector->change(data);

  vector->writeLock();
  vector->update(0);
  vector->unlock();

  return vector;
}


GeneratedVectorFactory::GeneratedVectorFactory()
: PrimitiveFactory() {
  registerFactory(GeneratedVector::staticTypeTag, this);
}


GeneratedVectorFactory::~GeneratedVectorFactory() {
}


PrimitivePtr GeneratedVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  double min, max;
  int count;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "generatedvector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
        min = attrs.value("min").toString().toDouble();
        max = attrs.value("max").toString().toDouble();
        count = attrs.value("count").toString().toInt();
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "generatedvector") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating generated vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  GeneratedVectorPtr vector = store->createObject<GeneratedVector>(tag);
  vector->changeRange(min, max, count);

  vector->writeLock();
  vector->update(0);
  vector->unlock();

  return vector;
}

EditableVectorFactory::EditableVectorFactory()
: PrimitiveFactory() {
  registerFactory(EditableVector::staticTypeTag, this);
}


EditableVectorFactory::~EditableVectorFactory() {
}


PrimitivePtr EditableVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  QByteArray data;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "editablevector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
      } else if (n == "data") {
        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "editablevector") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  EditableVectorPtr vector = store->createObject<EditableVector>(tag);
  vector->change(data);

  vector->writeLock();
  vector->update(0);
  vector->unlock();

  return vector;
}


DataVectorFactory::DataVectorFactory()
: PrimitiveFactory() {
  registerFactory(DataVector::staticTypeTag, this);
}


DataVectorFactory::~DataVectorFactory() {
}


PrimitivePtr DataVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  QByteArray data;
  QString provider, file, field;
  int start, count, skip = -1;
  bool doAve;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "datavector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());

        provider = attrs.value("provider").toString();
        file = attrs.value("file").toString();
        field = attrs.value("field").toString();
        start = attrs.value("start").toString().toInt();
        count = attrs.value("count").toString().toInt();
        skip = attrs.value("skip").toString().toInt();
        doAve = attrs.value("doAve").toString() == "true" ? true : false;

      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "datavector") {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating vector from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  Q_ASSERT(store);
  DataSourcePtr dataSource = store->dataSourceList().findReusableFileName(file);

  if (!dataSource) {
    dataSource = DataSource::loadSource(store, file, QString());
  }

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  DataVectorPtr vector = store->createObject<DataVector>(tag);

  vector->writeLock();
  vector->change(dataSource, field,
      start,
      count,
      skip,
      (skip != -1),
      doAve);

  vector->update(0);
  vector->unlock();

  return vector;

  return 0;
}

}

// vim: ts=2 sw=2 et
