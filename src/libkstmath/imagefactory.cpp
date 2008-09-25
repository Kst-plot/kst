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

#include "imagefactory.h"

#include "debug.h"
#include "image.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

ImageFactory::ImageFactory()
: RelationFactory() {
  registerFactory(Image::staticTypeTag, this);
}


ImageFactory::~ImageFactory() {
}


RelationPtr ImageFactory::generateRelation(ObjectStore *store, QXmlStreamReader& xml) {
  Q_ASSERT(store);

  double lowerThreshold=0, upperThreshold=1;
  int numberOfContourLines=4, contourWeight=0;
  QString matrixName, paletteName, legend, contourColor, descriptiveName;
  bool hasColorMap=true, hasContourMap=false, autoThreshold=true;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == Image::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();

        matrixName = attrs.value("matrix").toString();
        legend = attrs.value("legend").toString();

        paletteName = attrs.value("palettename").toString();

        hasColorMap = attrs.value("hascolormap").toString() == "true" ? true : false;
        lowerThreshold = attrs.value("lowerthreshold").toString().toDouble();
        upperThreshold = attrs.value("upperthreshold").toString().toDouble();

        hasContourMap = attrs.value("hascontourmap").toString() == "true" ? true : false;
        numberOfContourLines = attrs.value("numcontourlines").toString().toInt();
        contourWeight = attrs.value("contourweight").toString().toInt();
        contourColor = attrs.value("contourcolor").toString();

        autoThreshold = attrs.value("autothreshold").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == Image::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Image from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  MatrixPtr matrix = 0;
  if (store && !matrixName.isEmpty()) {
    matrix = kst_cast<Matrix>(store->retrieveObject(matrixName));
  }

  if (!matrix) {
    Debug::self()->log(QObject::tr("Error creating Image from Kst file.  Could not find matrix."), Debug::Warning);
    return 0;
  }

  ImagePtr image = store->createObject<Image>();

  if (hasColorMap && hasContourMap) {
    image->changeToColorAndContour(matrix,
        lowerThreshold,
        upperThreshold,
        autoThreshold,
        paletteName,
        numberOfContourLines,
        QColor(contourColor),
        contourWeight);
  } else if (hasContourMap) {
    image->changeToContourOnly(matrix,
        numberOfContourLines,
        QColor(contourColor),
        contourWeight);
  } else {
    image->changeToColorOnly(matrix,
        lowerThreshold,
        upperThreshold,
        autoThreshold,
        paletteName);
  }

  image->setDescriptiveName(descriptiveName);

  image->writeLock();
  image->update();
  image->unlock();

  return image;
}

}

// vim: ts=2 sw=2 et
