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

#include "curvefactory.h"

#include "debug.h"
#include "curve.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

CurveFactory::CurveFactory()
: RelationFactory() {
  registerFactory(Curve::staticTypeTag, this);
}


CurveFactory::~CurveFactory() {
}


RelationPtr CurveFactory::generateRelation(ObjectStore *store, QXmlStreamReader& xml) {
  ObjectTag tag;

  Q_ASSERT(store);

  int lineStyle, lineWidth, pointType, pointDensity, barStyle;
  QString xVectorTag, yVectorTag, legend, errorXVectorTag, errorYVectorTag, errorXMinusVectorTag, errorYMinusVectorTag, color;
  QString descriptiveName;
  bool hasLines, hasPoints, hasBars, ignoreAutoScale;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Curve::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        tag = ObjectTag::fromString(attrs.value("tag").toString());

        xVectorTag = attrs.value("xvector").toString();
        yVectorTag = attrs.value("yvector").toString();
        legend = attrs.value("legend").toString();
        color = attrs.value("color").toString();

        errorXVectorTag = attrs.value("errorxvector").toString();
        errorYVectorTag = attrs.value("erroryvector").toString();
        errorXMinusVectorTag = attrs.value("errorxminusvector").toString();
        errorYMinusVectorTag = attrs.value("erroryminusvector").toString();

        hasLines = attrs.value("haslines").toString() == "true" ? true : false;
        lineWidth = attrs.value("linewidth").toString().toInt();
        lineStyle = attrs.value("linestyle").toString().toInt();

        hasPoints = attrs.value("haspoints").toString() == "true" ? true : false;
        pointType = attrs.value("pointtype").toString().toInt();
        pointDensity = attrs.value("pointdensity").toString().toInt();

        hasBars = attrs.value("hasbars").toString() == "true" ? true : false;
        barStyle = attrs.value("barstyle").toString().toInt();

        ignoreAutoScale = attrs.value("ignoreautoscale").toString() == "true" ? true : false;

        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == Curve::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Curve from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  VectorPtr xVector = 0;
  if (store && !xVectorTag.isEmpty()) {
    xVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(xVectorTag)));
  }

  if (!xVector) {
    Debug::self()->log(QObject::tr("Error creating Curve from Kst file.  Could not find xVector."), Debug::Warning);
    return 0;
  }

  VectorPtr yVector = 0;
  if (store && !yVectorTag.isEmpty()) {
    yVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(yVectorTag)));
  }

  if (!yVector) {
    Debug::self()->log(QObject::tr("Error creating Curve from Kst file.  Could not find yVector."), Debug::Warning);
    return 0;
  }

  VectorPtr errorXVector = 0;
  if (store && !errorXVectorTag.isEmpty()) {
    errorXVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(errorXVectorTag)));
  }

  VectorPtr errorYVector = 0;
  if (store && !errorYVectorTag.isEmpty()) {
    errorYVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(errorYVectorTag)));
  }

  VectorPtr errorXMinusVector = 0;
  if (store && !errorXMinusVectorTag.isEmpty()) {
    errorXMinusVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(errorXMinusVectorTag)));
  }

  VectorPtr errorYMinusVector = 0;
  if (store && !errorYMinusVectorTag.isEmpty()) {
    errorYMinusVector = kst_cast<Vector>(store->retrieveObject(ObjectTag::fromString(errorYMinusVectorTag)));
  }

  CurvePtr curve = store->createObject<Curve>(tag);

  curve->setXVector(xVector);
  curve->setYVector(yVector);
  curve->setXError(errorXVector);
  curve->setYError(errorYVector);
  curve->setXMinusError(errorXMinusVector);
  curve->setYMinusError(errorYMinusVector);
  curve->setColor(QColor(color));
  curve->setHasPoints(hasPoints);
  curve->setHasLines(hasLines);
  curve->setHasBars(hasBars);
  curve->setLineWidth(lineWidth);
  curve->setLineStyle(lineStyle);
  curve->setPointType(pointType);
  curve->setPointDensity(pointDensity);
  curve->setBarStyle(barStyle);

  curve->setDescriptiveName(descriptiveName);

  curve->writeLock();
  curve->update();
  curve->unlock();

  return curve;
}

}

// vim: ts=2 sw=2 et
