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

// application specific includes
#include "plotmarkers.h"
#include "objectstore.h"
#include <QDebug>
#include <QVariant>
#include <QXmlStreamWriter>

namespace Kst {

PlotMarkers::PlotMarkers(bool xAxis) :
  _xAxis(xAxis),
  _lineStyle(Qt::SolidLine),
  _lineWidth(1.0),
  _vector(0), 
  _curve(0) {
}


PlotMarkers::~PlotMarkers() {
}


QList<double> PlotMarkers::markers() {
  QList<double> calculatedMarkers = _manualMarkers;
  if (_vector != 0) {
    _vector->readLock();
    for (int i = 0; i < _vector->length(); ++i) {
      calculatedMarkers << _vector->value(i);
    }
    _vector->unlock();
  }
  if (_curve != 0) {
    _curve->readLock();

    int count = _curve->sampleCount();

    if (count > 0) {
      double prevX, prevY;
      double curX, curY;

      // scan through the whole curve
      _curve->point(0, prevX, prevY);
      for (int i = 1; i < count; i++) {
        _curve->point(i, curX, curY);
        if (_xAxis) {
          if ((_curveMode == RisingEdge || _curveMode == BothEdges) && prevY == 0.0 && curY > 0.0) {
            calculatedMarkers << curX;
          }
          if ((_curveMode == FallingEdge || _curveMode == BothEdges) && prevY > 0.0 && curY == 0.0) {
            calculatedMarkers << prevX;
          }
        } else {
          if ((_curveMode == RisingEdge || _curveMode == BothEdges) && prevX == 0.0 && curX > 0.0) {
            calculatedMarkers << curY;
          }
          if ((_curveMode == FallingEdge || _curveMode == BothEdges) && prevX > 0.0 && curX == 0.0) {
            calculatedMarkers << prevY;
          }
        }
        prevX = curX;
        prevY = curY;
      }
    }
    _curve->unlock();
  }
  return calculatedMarkers;
}


void PlotMarkers::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeStartElement("plotmarkers");
  xml.writeAttribute("xaxis", QVariant(_xAxis).toString());
  xml.writeAttribute("linecolor", QVariant(_lineColor).toString());
  xml.writeAttribute("linestyle", QVariant(_lineStyle).toString());
  xml.writeAttribute("linewidth", QVariant(_lineWidth).toString());
  xml.writeAttribute("curvemode", QVariant(_curveMode).toString());
  if (_vector != 0) {
    xml.writeStartElement("vector");
    xml.writeAttribute("tag", _vector->Name());
    xml.writeEndElement();
  }
  if (_curve != 0) {
    xml.writeStartElement("curve");
    xml.writeAttribute("tag", _curve->Name());
    xml.writeEndElement();
  }
  foreach (double marker, _manualMarkers) {
    xml.writeStartElement("manualmarker");
    xml.writeAttribute("location", QVariant(marker).toString());
    xml.writeEndElement();
  }
  xml.writeEndElement();
}


bool PlotMarkers::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av;
  av = attrs.value("xaxis");
  if (!av.isNull()) {
    _xAxis = QVariant(av.toString()).toBool();
  }
  av = attrs.value("linecolor");
  if (!av.isNull()) {
    _lineColor = QColor(av.toString());
  }
  av = attrs.value("linestyle");
  if (!av.isNull()) {
    _lineStyle = (Qt::PenStyle)QVariant(av.toString()).toInt();
  }
  av = attrs.value("linewidth");
  if (!av.isNull()) {
    _lineWidth = QVariant(av.toString()).toDouble();
  }
  av = attrs.value("curvemode");
  if (!av.isNull()) {
    _curveMode = (CurveMarkerMode)QVariant(av.toString()).toInt();
  }

  QString expectedEnd;
  while (!(xml.isEndElement() && (xml.name().toString() == primaryTag))) {
   if (xml.isStartElement() && xml.name().toString() == "vector") {
      expectedEnd = xml.name().toString();
      attrs = xml.attributes();
      QString tagName = attrs.value("tag").toString();
      VectorPtr vector = kst_cast<Vector>(store->retrieveObject(tagName));
      if (vector) {
        setVector(vector);
      }
   } else if (xml.isStartElement() && xml.name().toString() == "curve") {
      expectedEnd = xml.name().toString();
      attrs = xml.attributes();
      QString tagName = attrs.value("tag").toString();
      CurvePtr curve = kst_cast<Curve>(store->retrieveObject(tagName));
      if (curve) {
        setCurve(curve);
      }
   } else if (xml.isStartElement() && xml.name().toString() == "manualmarker") {
      expectedEnd = xml.name().toString();
      attrs = xml.attributes();
      av = attrs.value("location");
      if (!av.isNull()) {
        _manualMarkers << av.toString().toDouble();
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() != expectedEnd) {
        validTag = false;
        break;
      }
    }
    xml.readNext();
  }

  return validTag;
}

}
// vim: ts=2 sw=2 et
