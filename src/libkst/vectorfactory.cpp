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

#include "kstdebug.h"
#include "kstvector.h"
#include "generatedvector.h"
#include "editablevector.h"
#include "datavector.h"
#include "kstdatacollection.h"

namespace Kst {

VectorFactory::VectorFactory()
: PrimitiveFactory() {
  registerFactory("vector", this);
}


VectorFactory::~VectorFactory() {
}


KstPrimitivePtr VectorFactory::generatePrimitive(QXmlStreamReader& xml) {

  QString tag;
  QByteArray data;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "vector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();
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
        KstDebug::self()->log(QObject::tr("Error creating vector from Kst file."), KstDebug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  KstVectorPtr vector = new KstVector(tag, data);
  return vector.data();
}


SVectorFactory::SVectorFactory()
: PrimitiveFactory() {
  registerFactory("svector", this);
}


SVectorFactory::~SVectorFactory() {
}


KstPrimitivePtr SVectorFactory::generatePrimitive(QXmlStreamReader& xml) {

  QString tag;
  QByteArray data;
  double min, max;
  int count;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "svector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();
        min = attrs.value("min").toString().toDouble();
        max = attrs.value("max").toString().toDouble();
        count = attrs.value("count").toString().toInt();
      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "svector") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating vector from Kst file."), KstDebug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  KstVectorPtr vector = new GeneratedVector(tag, data, min, max, count);
  return vector.data();
}

AVectorFactory::AVectorFactory()
: PrimitiveFactory() {
  registerFactory("avector", this);
}


AVectorFactory::~AVectorFactory() {
}


KstPrimitivePtr AVectorFactory::generatePrimitive(QXmlStreamReader& xml) {

  QString tag;
  QByteArray data;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "avector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();
      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == "avector") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating vector from Kst file."), KstDebug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  KstVectorPtr vector = new EditableVector(tag, data);
  return vector.data();
}


RVectorFactory::RVectorFactory()
: PrimitiveFactory() {
  registerFactory("rvector", this);
}


RVectorFactory::~RVectorFactory() {
}


KstPrimitivePtr RVectorFactory::generatePrimitive(QXmlStreamReader& xml) {

  QString tag;
  QByteArray data;
  QString provider, file, field;
  int start, count, skip;
  bool doAve;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "rvector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();

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
      if (n == "rvector") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating vector from Kst file."), KstDebug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  KstVectorPtr vector = new Kst::DataVector(tag, data, provider, file, field, start, count, skip, doAve);
  return vector.data();
}

}

// vim: ts=2 sw=2 et
