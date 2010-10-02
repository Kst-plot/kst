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

#include "arrowitem.h"

#include "view.h"
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
  _startArrowScale(12.0),
  _endArrowScale(12.0) {
  setTypeName("Arrow");
  QBrush b = brush();
  b.setStyle(Qt::SolidPattern);
  setBrush(b);
}


ArrowItem::~ArrowItem() {
}


void ArrowItem::paint(QPainter *painter) {
  painter->drawLine(line());

  start.clear();
  end.clear();
  if (_startArrowHead) {
    //FIXME: this assumes that pixelSize == pointSize, which it only might...
    double deltax = parentView()->defaultFont(_startArrowScale).pointSizeF()*0.5;
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
    start = pts;
  }

  if (_endArrowHead) {
    //FIXME: this assumes that pixelSize == pointSize, which it only might...
    double deltax = parentView()->defaultFont(_endArrowScale).pointSizeF()*0.5;
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
    end = pts;
  }
}


QPainterPath ArrowItem::shape() const {
  QPainterPath selectPath;
  selectPath.setFillRule(Qt::WindingFill);
  selectPath.addPolygon(rect());
  selectPath.addPolygon(start);
  selectPath.addPolygon(end);
  if ((!isSelected() && !isHovering()) || (parentView()->mouseMode() == View::Create)) {
  } else {
    selectPath.addPath(grips());
  }
  return selectPath;
}


void ArrowItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("arrow");
    xml.writeAttribute("startarrowhead", QVariant(_startArrowHead).toString());
    xml.writeAttribute("endarrowhead", QVariant(_endArrowHead).toString());
    xml.writeAttribute("startarrowheadscale", QVariant(_startArrowScale).toString());
    xml.writeAttribute("endarrowheadscale", QVariant(_endArrowScale).toString());
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


void ArrowItem::edit() {
  ArrowItemDialog *editDialog = new ArrowItemDialog(this);
  editDialog->show();
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
      if (!rc && xml.name().toString() == "arrow") {
        Q_ASSERT(!rc);
        rc = new ArrowItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
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
