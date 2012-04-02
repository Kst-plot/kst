/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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

  Q_ASSERT(store);

  int lineStyle=0, lineWidth=0, pointType=0, pointDensity=0, headType=0;
  QString xVectorName, yVectorName, legend, errorXVectorName, errorYVectorName, errorXMinusVectorName;
  QString errorYMinusVectorName, color, headColor;
  QString barFillColor;
  QString descriptiveName;
  bool hasLines=true, hasPoints=false, hasBars=false, ignoreAutoScale=false, hasHead=false;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Curve::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        xVectorName = attrs.value("xvector").toString();
        yVectorName = attrs.value("yvector").toString();
        legend = attrs.value("legend").toString();
        color = attrs.value("color").toString();
        headColor = attrs.value("headcolor").toString();
        barFillColor = attrs.value("barfillcolor").toString();

        errorXVectorName = attrs.value("errorxvector").toString();
        errorYVectorName = attrs.value("erroryvector").toString();
        errorXMinusVectorName = attrs.value("errorxminusvector").toString();
        errorYMinusVectorName = attrs.value("erroryminusvector").toString();

        hasLines = attrs.value("haslines").toString() == "true" ? true : false;
        lineWidth = attrs.value("linewidth").toString().toInt();
        lineStyle = attrs.value("linestyle").toString().toInt();

        hasPoints = attrs.value("haspoints").toString() == "true" ? true : false;
        pointType = attrs.value("pointtype").toString().toInt();
        pointDensity = attrs.value("pointdensity").toString().toInt();

        hasHead = attrs.value("hashead").toString() == "true" ? true : false;
        headType = attrs.value("headtype").toString().toInt();

        hasBars = attrs.value("hasbars").toString() == "true" ? true : false;

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
  if (store && !xVectorName.isEmpty()) {
    xVector = kst_cast<Vector>(store->retrieveObject(xVectorName));
  }

  if (!xVector) {
    Debug::self()->log(QObject::tr("Error creating Curve from Kst file.  Could not find xVector."), Debug::Warning);
    return 0;
  }

  VectorPtr yVector = 0;
  if (store && !yVectorName.isEmpty()) {
    yVector = kst_cast<Vector>(store->retrieveObject(yVectorName));
  }

  if (!yVector) {
    Debug::self()->log(QObject::tr("Error creating Curve from Kst file.  Could not find yVector."), Debug::Warning);
    return 0;
  }

  VectorPtr errorXVector = 0;
  if (store && !errorXVectorName.isEmpty()) {
    errorXVector = kst_cast<Vector>(store->retrieveObject(errorXVectorName));
  }

  VectorPtr errorYVector = 0;
  if (store && !errorYVectorName.isEmpty()) {
    errorYVector = kst_cast<Vector>(store->retrieveObject(errorYVectorName));
  }

  VectorPtr errorXMinusVector = 0;
  if (store && !errorXMinusVectorName.isEmpty()) {
    errorXMinusVector = kst_cast<Vector>(store->retrieveObject(errorXMinusVectorName));
  }

  VectorPtr errorYMinusVector = 0;
  if (store && !errorYMinusVectorName.isEmpty()) {
    errorYMinusVector = kst_cast<Vector>(store->retrieveObject(errorYMinusVectorName));
  }

  CurvePtr curve = store->createObject<Curve>();

  curve->setXVector(xVector);
  curve->setYVector(yVector);
  curve->setXError(errorXVector);
  curve->setYError(errorYVector);
  curve->setXMinusError(errorXMinusVector);
  curve->setYMinusError(errorYMinusVector);
  curve->setColor(QColor(color));
  curve->setHeadColor(QColor(headColor));
  if (barFillColor.isEmpty()) {
    curve->setBarFillColor(curve->color());
  } else {
    curve->setBarFillColor(QColor(barFillColor));
  }
  curve->setHasPoints(hasPoints);
  curve->setHasLines(hasLines);
  curve->setHasBars(hasBars);
  curve->setHasHead(hasHead);
  curve->setLineWidth(lineWidth);
  curve->setLineStyle(lineStyle);
  curve->setPointType(pointType);
  curve->setHeadType(headType);
  curve->setPointDensity(pointDensity);
  curve->setIgnoreAutoScale(ignoreAutoScale);

  curve->setDescriptiveName(descriptiveName);

  curve->writeLock();
  curve->registerChange();
  curve->unlock();

  return curve;
}

}

// vim: ts=2 sw=2 et
