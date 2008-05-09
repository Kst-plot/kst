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

#include "csdfactory.h"

#include "debug.h"
#include "csd.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

CSDFactory::CSDFactory()
: ObjectFactory() {
  registerFactory(CSD::staticTypeTag, this);
}


CSDFactory::~CSDFactory() {
}


DataObjectPtr CSDFactory::generateObject(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;

  Q_ASSERT(store);

  double frequency, gaussianSigma;
  int length, windowSize, apodizeFunction, outputType;
  QString vectorTag, vectorUnits, rateUnits, descriptiveName;
  bool average, removeMean, apodize;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == CSD::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
        vectorTag = attrs.value("vector").toString();
        vectorUnits = attrs.value("vectorunits").toString();
        rateUnits = attrs.value("rateunits").toString();

        frequency = attrs.value("samplerate").toString().toDouble();
        gaussianSigma = attrs.value("gaussiansigma").toString().toDouble();

        length = attrs.value("fftlength").toString().toInt();
        windowSize = attrs.value("windowsize").toString().toInt();
        apodizeFunction = attrs.value("apodizefunction").toString().toInt();
        outputType = attrs.value("outputtype").toString().toInt();

        average = attrs.value("average").toString() == "true" ? true : false;
        removeMean = attrs.value("removemean").toString() == "true" ? true : false;
        apodize = attrs.value("apodize").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == CSD::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating CSD from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  VectorPtr vector = 0;
  if (store && !vectorTag.isEmpty()) {
    vector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(vectorTag)));
  }

  if (!vector) {
    Debug::self()->log(QObject::tr("Error creating CSD from Kst file.  Could not find Vector."), Debug::Warning);
    return 0;
  }

  CSDPtr csd = store->createObject<CSD>(tag);
  csd->change(vector,
              frequency,
              average,
              removeMean,
              apodize,
              (ApodizeFunction)apodizeFunction,
              windowSize,
              length,
              gaussianSigma,
              (PSDType)outputType,
              vectorUnits,
              rateUnits);

  csd->setDescriptiveName(descriptiveName);
  csd->writeLock();
  csd->update();
  csd->unlock();

  return csd;
}

}

// vim: ts=2 sw=2 et
