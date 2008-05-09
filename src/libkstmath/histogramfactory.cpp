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

#include "histogramfactory.h"

#include "debug.h"
#include "histogram.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

HistogramFactory::HistogramFactory()
: ObjectFactory() {
  registerFactory(Histogram::staticTypeTag, this);
}


HistogramFactory::~HistogramFactory() {
}


DataObjectPtr HistogramFactory::generateObject(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;

  Q_ASSERT(store);

  double min, max;
  int numberOfBins, normalizationMode;
  QString vectorTag, descriptiveName;
  bool realTimeAutoBin;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Histogram::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());
        vectorTag = attrs.value("vector").toString();

        min = attrs.value("min").toString().toDouble();
        max = attrs.value("max").toString().toDouble();
        numberOfBins = attrs.value("numberofbins").toString().toInt();
        normalizationMode = attrs.value("normalizationmode").toString().toInt();
        realTimeAutoBin = attrs.value("realtimeautobin").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == Histogram::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Histogram from Kst file."), Debug::Warning);
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
    Debug::self()->log(QObject::tr("Error creating Histogram from Kst file.  Could not find Vector."), Debug::Warning);
    return 0;
  }

  HistogramPtr histogram = store->createObject<Histogram>(tag);

  histogram->setVector(vector);
  histogram->setXRange(min, max);
  histogram->setNumberOfBins(numberOfBins);
  histogram->setNormalizationType((Histogram::NormalizationType)normalizationMode);
  histogram->setDescriptiveName(descriptiveName);

  histogram->writeLock();
  histogram->update();
  histogram->unlock();

  return histogram;
}

}

// vim: ts=2 sw=2 et
