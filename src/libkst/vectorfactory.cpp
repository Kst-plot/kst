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

  return new Vector(store, tag, data);
}


GeneratedVectorFactory::GeneratedVectorFactory()
: PrimitiveFactory() {
  registerFactory(GeneratedVector::staticTypeTag, this);
}


GeneratedVectorFactory::~GeneratedVectorFactory() {
}


PrimitivePtr GeneratedVectorFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  QByteArray data;
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

  return new GeneratedVector(store, tag, data, min, max, count);
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

  return new EditableVector(store, tag, data);
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
  int start, count, skip;
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

  return new DataVector(store, tag, data, provider, file, field, start, count, skip, doAve);
}

}

// vim: ts=2 sw=2 et
