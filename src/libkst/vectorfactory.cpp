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
    if (xml.isStartElement()) {
      const QString n = xml.name().toString();
      if (n == "vector") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = attrs.value("tag").toString();
//         QString qcs(xml.readElementText().toLatin1());
//         QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
//         data = qUncompress(qbca);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "vector") {
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

  KstVectorPtr vector = 0;
  return vector.data();
}

}

// vim: ts=2 sw=2 et
