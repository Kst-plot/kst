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

#include "arrowitem.h"

#include "view.h"
#include "viewitemzorder.h"
#include "arrowitemdialog.h"

#include "math_kst.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ArrowItem::ArrowItem(View *parent)
  : LineItem(parent),
  _startArrowHead(false),
  _endArrowHead(true),
  _startArrowScale(1.0),
  _endArrowScale(1.0) {
  setName("Arrow");
  setZValue(ARROW_ZVALUE);
  QBrush b = brush();
  b.setStyle(Qt::SolidPattern);
  setBrush(b);
}


ArrowItem::~ArrowItem() {
}


void ArrowItem::paint(QPainter *painter) {
  painter->drawLine(line());

  if (_startArrowHead) {
    double deltax = (parentView()->defaultFont().pixelSize() / 2) * _startArrowScale;
    double theta = atan2(double(line().y2() - line().y1()), double(line().x2() - line().x1())) - M_PI / 2.0;
    double sina = sin(theta);
    double cosa = cos(theta);
    double yin = sqrt(3.0) * deltax;
    double x1, y1, x2, y2;
    QMatrix m(cosa, sina, -sina, cosa, 0.0, 0.0);

    m.map( deltax, yin, &x1, &y1);
    m.map(-deltax, yin, &x2, &y2);

    QPolygonF pts;
    pts.append(line().p1());
    pts.append(line().p1() + QPointF(x1, y1));
    pts.append(line().p1() + QPointF(x2, y2));
    painter->drawPolygon(pts);
  }

  if (_endArrowHead) {
    double deltax = (parentView()->defaultFont().pixelSize() / 2) * _endArrowScale;
    double theta = atan2(double(line().y1() - line().y2()), double(line().x1() - line().x2())) - M_PI / 2.0;
    double sina = sin(theta);
    double cosa = cos(theta);
    double yin = sqrt(3.0) * deltax;
    double x1, y1, x2, y2;
    QMatrix m(cosa, sina, -sina, cosa, 0.0, 0.0);

    m.map( deltax, yin, &x1, &y1);
    m.map(-deltax, yin, &x2, &y2);

    QPolygonF pts;
    pts.append(line().p2());
    pts.append(line().p2() + QPointF(x1, y1));
    pts.append(line().p2() + QPointF(x2, y2));
    painter->drawPolygon(pts);
  }
}


void ArrowItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("arrow");
  xml.writeAttribute("startarrowhead", QVariant(_startArrowHead).toString());
  xml.writeAttribute("endarrowhead", QVariant(_startArrowHead).toString());
  xml.writeAttribute("startarrowheadscale", QVariant(_startArrowScale).toString());
  xml.writeAttribute("endarrowheadscale", QVariant(_endArrowScale).toString());
  ViewItem::save(xml);
  xml.writeEndElement();
}


void ArrowItem::edit() {
  ArrowItemDialog editDialog(this);
  editDialog.exec();
}


void CreateArrowCommand::createItem() {
  _item = new ArrowItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

ArrowItemFactory::ArrowItemFactory()
: GraphicsFactory() {
  registerFactory("arrow", this);
}


ArrowItemFactory::~ArrowItemFactory() {
}


ViewItem* ArrowItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  ArrowItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "arrow") {
        Q_ASSERT(!rc);
        rc = new ArrowItem(view);
        if (parent) {
          rc->setParent(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("startarrowhead");
        if (!av.isNull()) {
          rc->setStartArrowHead(QVariant(av.toString()).toBool());
        }
        av = attrs.value("endarrowhead");
        if (!av.isNull()) {
          rc->setEndArrowHead(QVariant(av.toString()).toBool());
        }
        av = attrs.value("startarrowheadscale");
        if (!av.isNull()) {
          rc->setStartArrowScale(QVariant(av.toString()).toDouble());
        }
        av = attrs.value("endarrowheadscale");
        if (!av.isNull()) {
          rc->setEndArrowScale(QVariant(av.toString()).toDouble());
        }
        // Add any new specialized ArrowItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "arrow") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating arrow object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }

  return rc;
}

}

// vim: ts=2 sw=2 et
