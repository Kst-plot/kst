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
  Q_ASSERT(store);

  double frequency=1.0, gaussianSigma=1.0;
  int length=8, windowSize=8, apodizeFunction=0, outputType=0;
  QString vectorName, vectorUnits, rateUnits, descriptiveName;
  bool average=false, removeMean=false, apodize=false;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == CSD::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        vectorName = attrs.value("vector").toString();
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
  if (store && !vectorName.isEmpty()) {
    vector = kst_cast<Vector>(store->retrieveObject(vectorName));
  }

  if (!vector) {
    Debug::self()->log(QObject::tr("Error creating CSD from Kst file.  Could not find Vector."), Debug::Warning);
    return 0;
  }

  CSDPtr csd = store->createObject<CSD>();
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
