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
#include "objectstore.h"

namespace Kst {

ScalarFactory::ScalarFactory()
: PrimitiveFactory() {
  registerFactory(Scalar::staticTypeTag, this);
}


ScalarFactory::~ScalarFactory() {
}


PrimitivePtr ScalarFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;
  QByteArray data;

  Q_ASSERT(store);

  bool orphan, editable;
  double value;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == "scalar") {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
        value = attrs.value("value").toString().toDouble();
        orphan = attrs.value("orphan").toString() == "true" ? true : false;
        editable = attrs.value("editable").toString() == "true" ? true : false;
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

  ScalarPtr scalar = store->createObject<Scalar>(tag);
  scalar->setValue(value);
  scalar->setOrphan(true);
  scalar->setEditable(true);

  return scalar;
}


}

// vim: ts=2 sw=2 et
