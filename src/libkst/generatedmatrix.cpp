/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005 University of British Columbia                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QTextStream>
#include <QTextDocument>

#include "kst_i18n.h"

#include "generatedmatrix.h"
#include <QXmlStreamWriter>
#include <QVariant>

namespace Kst {

const QString GeneratedMatrix::staticTypeString = I18N_NOOP("Generated Matrix");
const QString GeneratedMatrix::staticTypeTag = I18N_NOOP("generatedmatrix");

GeneratedMatrix::GeneratedMatrix(ObjectStore *store, const QDomElement &e) : Matrix(store) {
  double in_xMin = 0, in_yMin = 0, in_xStep = 1, in_yStep = 1;
  double in_gradZMin = 0, in_gradZMax = 1;
  bool in_xDirection = true;
  int in_nX = 2, in_nY = 2;
  QString in_tag = QString::null;

  /* parse the DOM tree */
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "nx") {
        in_nX = e.text().toInt();
      } else if (e.tagName() == "ny") {
        in_nY = e.text().toInt();
      } else if (e.tagName() == "xmin") {
        in_xMin = e.text().toDouble();
      } else if (e.tagName() == "ymin") {
        in_yMin = e.text().toDouble();
      } else if (e.tagName() == "xstep") {
        in_xStep = e.text().toDouble();
      } else if (e.tagName() == "ystep") {
        in_yStep = e.text().toDouble();
      } else if (e.tagName() == "gradzmin") {
        in_gradZMin = e.text().toDouble();
      } else if (e.tagName() == "gradzmax") {
        in_gradZMax = e.text().toDouble();
      } else if (e.tagName() == "xdirection") {
        in_xDirection = (e.text() != "0");
      }
    }
    n = n.nextSibling();
  }

  setTagName(ObjectTag::fromString(in_tag));

  _saveable = true;
  _editable = true;
  _zSize = 0;
  change(in_nX, in_nY, in_xMin, in_yMin, in_xStep, in_yStep, in_gradZMin, in_gradZMax, in_xDirection);
}

GeneratedMatrix::GeneratedMatrix(ObjectStore *store, const ObjectTag& tag,
                       uint nX, uint nY, double minX, double minY,
                       double stepX, double stepY,
                       double gradZMin, double gradZMax,
                       bool xDirection) : Matrix(store, tag) {
  _saveable = true;
  _editable = true;
  _zSize = 0;
  change(nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
}

const QString& GeneratedMatrix::typeString() const {
  return staticTypeString;
}

void GeneratedMatrix::save(QXmlStreamWriter &xml) {
  xml.writeStartElement(staticTypeTag);
  xml.writeAttribute("tag", tag().tagString());
  xml.writeAttribute("xmin", QString::number(minX()));
  xml.writeAttribute("ymin", QString::number(minY()));
  xml.writeAttribute("nx", QString::number(xNumSteps()));
  xml.writeAttribute("ny", QString::number(yNumSteps()));
  xml.writeAttribute("xstep", QString::number(xStepSize()));
  xml.writeAttribute("ystep", QString::number(yStepSize()));
  xml.writeAttribute("gradzmin", QString::number(_gradZMin));
  xml.writeAttribute("gradzmax", QString::number(_gradZMax));
  xml.writeAttribute("xdirection", QVariant(_xDirection).toString());
  saveNameInfo(xml, VNUM|MNUM|XNUM);

  xml.writeEndElement();

}

void GeneratedMatrix::change(uint nX, uint nY, double minX, double minY,
                             double stepX, double stepY,
                             double gradZMin, double gradZMax,
                             bool xDirection) {
  // some checks on parameters
  if (nX < 1) {
    nX = 1;
  }
  if (nY < 1) {
    nY = 1;
  }
  if (stepX <= 0) {
    stepX = 0.1;
  }
  if (stepY <= 0) {
    stepY = 0.1;
  }

  _nX = nX;
  _nY = nY;
  _minX = minX;
  _minY = minY;
  _stepX = stepX;
  _stepY = stepY;
  _gradZMin = gradZMin;
  _gradZMax = gradZMax;
  _xDirection = xDirection;

  if (_nX*_nY != _zSize) {
    resizeZ(_nX*_nY, false);
  }

  // zIncrement can be negative, to reverse gradient direction
  double zIncrement;
  if (_xDirection) {
    if (_nX > 1) {
      zIncrement = (_gradZMax - _gradZMin) / (_nX - 1);
    } else {
      zIncrement = 0;
    }
  } else {
    if (_nY > 1) {
      zIncrement = (_gradZMax - _gradZMin) / (_nY - 1);
    } else {
      zIncrement = 0;
    }
  }

  // fill in the matrix with the gradient
  for (int i = 0; i < _nX; i++) {
    for (int j = 0; j < _nY; j++) {
      if (_xDirection) {
        _z[i*nY + j] = _gradZMin + i*zIncrement;
      } else {
        _z[i*nY + j] = _gradZMin + j*zIncrement;
      }
    }
  }
  setDirty(true);
}

}
// vim: ts=2 sw=2 et
