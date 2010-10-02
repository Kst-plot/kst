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

#include "matrixfactory.h"

#include "debug.h"
#include "matrix.h"
#include "generatedmatrix.h"
#include "editablematrix.h"
#include "datamatrix.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"

namespace Kst {

GeneratedMatrixFactory::GeneratedMatrixFactory()
: PrimitiveFactory() {
  registerFactory(GeneratedMatrix::staticTypeTag, this);
}


GeneratedMatrixFactory::~GeneratedMatrixFactory() {
}


PrimitivePtr GeneratedMatrixFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString descriptiveName;

  Q_ASSERT(store);

  bool xDirection=1;
  double gradZMin=-1, gradZMax=1, minX=0, minY=0, nX=10, nY=10, stepX=1, stepY=1;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == GeneratedMatrix::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        gradZMin = attrs.value("gradzmin").toString().toDouble();
        gradZMax = attrs.value("gradzmax").toString().toDouble();
        minX = attrs.value("xmin").toString().toDouble();
        minY = attrs.value("ymin").toString().toDouble();
        nX = attrs.value("nx").toString().toDouble();
        nY = attrs.value("ny").toString().toDouble();
        stepX = attrs.value("xstep").toString().toDouble();
        stepY = attrs.value("ystep").toString().toDouble();
        xDirection = attrs.value("xdirection").toString() == "true" ? true : false;
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == GeneratedMatrix::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Generated Matrix from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  GeneratedMatrixPtr matrix = store->createObject<GeneratedMatrix>();
  matrix->change(uint(nX), uint(nY), minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
  matrix->setDescriptiveName(descriptiveName);

  matrix->writeLock();
  matrix->registerChange();
  matrix->unlock();

  return matrix;
}


EditableMatrixFactory::EditableMatrixFactory()
: PrimitiveFactory() {
  registerFactory(EditableMatrix::staticTypeTag, this);
}


EditableMatrixFactory::~EditableMatrixFactory() {
}


PrimitivePtr EditableMatrixFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString descriptiveName;

  Q_ASSERT(store);

  double minX=0, minY=0, nX=10, nY=10, stepX=1, stepY=1;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == EditableMatrix::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        minX = attrs.value("xmin").toString().toDouble();
        minY = attrs.value("ymin").toString().toDouble();
        nX = attrs.value("nx").toString().toDouble();
        nY = attrs.value("ny").toString().toDouble();
        stepX = attrs.value("xstep").toString().toDouble();
        stepY = attrs.value("ystep").toString().toDouble();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else if (n == "data") {

        QString qcs(xml.readElementText().toLatin1());
        QByteArray qbca = QByteArray::fromBase64(qcs.toLatin1());
        data = qUncompress(qbca);

      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == EditableMatrix::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Editable Matrix from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  EditableMatrixPtr matrix = store->createObject<EditableMatrix>();
  matrix->change(data, uint(nX), uint(nY), minX, minY, stepX, stepY);
  matrix->setDescriptiveName(descriptiveName);

  matrix->writeLock();
  matrix->registerChange();
  matrix->unlock();

  return matrix;
}



DataMatrixFactory::DataMatrixFactory()
: PrimitiveFactory() {
  registerFactory(DataMatrix::staticTypeTag, this);
}


DataMatrixFactory::~DataMatrixFactory() {
}


PrimitivePtr DataMatrixFactory::generatePrimitive(ObjectStore *store, QXmlStreamReader& xml) {
  QByteArray data;
  QString descriptiveName;

  Q_ASSERT(store);

  bool doAve=false, doSkip=false;
  int requestedXStart=0, requestedYStart=0, requestedXCount=-1, requestedYCount=-1, skip=0;
  double minX=0, minY=0, stepX=1, stepY=1;
  QString provider, file, field;

  while (!xml.atEnd()) {
      const QString n = xml.name().toString();
    if (xml.isStartElement()) {
      if (n == DataMatrix::staticTypeTag) {
        QXmlStreamAttributes attrs = xml.attributes();
        provider = attrs.value("provider").toString();
        file = DataPrimitive::readFilename(attrs);
        field = attrs.value("field").toString();
        requestedXStart = attrs.value("reqxstart").toString().toInt();
        requestedYStart = attrs.value("reqystart").toString().toInt();
        requestedXCount = attrs.value("reqnx").toString().toInt();
        requestedYCount = attrs.value("reqny").toString().toInt();
        doAve = attrs.value("doave").toString() == "true" ? true : false;
        doSkip = attrs.value("doskip").toString() == "true" ? true : false;
        skip = attrs.value("skip").toString().toInt();
        minX = attrs.value("xmin").toString().toDouble();
        minY = attrs.value("ymin").toString().toDouble();
        stepX = attrs.value("xstep").toString().toDouble();
        stepY = attrs.value("ystep").toString().toDouble();
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          descriptiveName = attrs.value("descriptiveName").toString();
        }
        Object::processShortNameIndexAttributes(attrs);
      } else {
        return 0;
      }
    } else if (xml.isEndElement()) {
      if (n == DataMatrix::staticTypeTag) {
        break;
      } else {
        Debug::self()->log(QObject::tr("Error creating Data Matrix from Kst file."), Debug::Warning);
        return 0;
      }
    }
    xml.readNext();
  }

  if (xml.hasError()) {
    return 0;
  }

  Q_ASSERT(store);
  DataSourcePtr dataSource = DataSourcePluginManager::findOrLoadSource(store, file);

  if (!dataSource) {
    return 0; //Couldn't find a suitable datasource
  }

  DataMatrixPtr matrix = store->createObject<DataMatrix>();
  matrix->change(dataSource, field, requestedXStart, requestedYStart, requestedXCount, requestedYCount, doAve, doSkip, skip, minX, minY, stepX, stepY);
  matrix->setDescriptiveName(descriptiveName);

  matrix->writeLock();
  matrix->registerChange();
  matrix->unlock();

  return matrix;
}


}

// vim: ts=2 sw=2 et
