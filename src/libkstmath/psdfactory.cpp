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

#include "psdfactory.h"

#include "debug.h"
#include "psd.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

PSDFactory::PSDFactory()
: ObjectFactory() {
  registerFactory(PSD::staticTypeTag, this);
}


PSDFactory::~PSDFactory() {
}


DataObjectPtr PSDFactory::generateObject(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;

  Q_ASSERT(store);

  double frequency, gaussianSigma;
  int length, apodizeFunction, outputType;
  QString vectorTag, vectorUnits, rateUnits;
  bool average, removeMean, apodize, interpolateHoles;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == PSD::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
        vectorTag = attrs.value("vector").toString();
        vectorUnits = attrs.value("vectorunits").toString();
        rateUnits = attrs.value("rateunits").toString();

        frequency = attrs.value("samplerate").toString().toDouble();
        gaussianSigma = attrs.value("gaussiansigma").toString().toDouble();

        length = attrs.value("fftlength").toString().toInt();
        apodizeFunction = attrs.value("apodizefunction").toString().toInt();
        outputType = attrs.value("outputtype").toString().toInt();

        average = attrs.value("average").toString() == "true" ? true : false;
        interpolateHoles = attrs.value("interpolateholes").toString() == "true" ? true : false;
        removeMean = attrs.value("removemean").toString() == "true" ? true : false;
        apodize = attrs.value("apodize").toString() == "true" ? true : false;
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == PSD::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating PSD from Kst file."), Debug::Warning);
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
    Debug::self()->log(QObject::tr("Error creating PSD from Kst file.  Could not find Vector."), Debug::Warning);
    return 0;
  }

  PSDPtr powerspectrum = store->createObject<PSD>(tag);
  Q_ASSERT(powerspectrum);

  powerspectrum->writeLock();
  powerspectrum->setVector(vector);
  powerspectrum->setFrequency(frequency);
  powerspectrum->setAverage(average);
  powerspectrum->setLength(length);
  powerspectrum->setApodize(apodize);
  powerspectrum->setRemoveMean(removeMean);
  powerspectrum->setVectorUnits(vectorUnits);
  powerspectrum->setRateUnits(rateUnits);
  powerspectrum->setApodizeFxn((ApodizeFunction)apodizeFunction);
  powerspectrum->setGaussianSigma(gaussianSigma);
  powerspectrum->setOutput((PSDType)outputType);
  powerspectrum->setInterpolateHoles(interpolateHoles);

  powerspectrum->update(0);
  powerspectrum->unlock();

  return powerspectrum;
}

}

// vim: ts=2 sw=2 et
