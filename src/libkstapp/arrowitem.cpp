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

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

ArrowItem::ArrowItem(View *parent)
  : ViewItem(parent) {
  setName("Arrow");
  setZValue(LINE_ZVALUE);
  setAllowedGrips(RightMidGrip | LeftMidGrip);
  QBrush b = brush();
  b.setStyle(Qt::SolidPattern);
  setBrush(b);
}


ArrowItem::~ArrowItem() {
}


void ArrowItem::paint(QPainter *painter) {
  painter->drawLine(line());
  double deltax = 4.0;
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


void ArrowItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("arrow");
  ViewItem::save(xml);
  xml.writeEndElement();
}


QLineF ArrowItem::line() const {
  return QLineF(rect().left(), rect().center().y(), rect().right(), rect().center().y());
}


void ArrowItem::setLine(const QLineF &line_) {
  setPos(line_.p1());
  setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));

  if (!rect().isEmpty()) {
    rotateTowards(line().p2(), line_.p2());
  }

  QRectF r = rect();
  r.setSize(QSizeF(QLineF(line().p1(), line_.p2()).length(), r.height()));
  setViewRect(r);
}


QPainterPath ArrowItem::leftMidGrip() const {
  if (gripMode() == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));
  QPainterPath path;
  if (gripMode() == Resize || gripMode() == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ArrowItem::rightMidGrip() const {
  if (gripMode() == Move)
    return QPainterPath();

  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));
  QPainterPath path;
  if (gripMode() == Resize || gripMode() == Scale)
    path.addRect(grip);
  else
    path.addEllipse(grip);

  return path;
}


QPainterPath ArrowItem::grips() const {
  if (gripMode() == Move)
    return QPainterPath();

  QPainterPath grips;
  grips.addPath(leftMidGrip());
  grips.addPath(rightMidGrip());
  return grips;
}


QPointF ArrowItem::centerOfRotation() const {
  if (activeGrip() == RightMidGrip)
    return line().p1();
  else if (activeGrip() == LeftMidGrip)
    return line().p2();

  return line().p1();
}


void ArrowItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));
    parentView()->scene()->addItem(this);
    //setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    if (!rect().isEmpty()) {
      rotateTowards(line().p2(), poly.last());
    }
    QRectF r = rect();
    r.setSize(QSizeF(QLineF(line().p1(), poly.last()).length(), r.height()));
    setViewRect(r);
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    maybeReparent();
    emit creationComplete();
    return;
  }
}


void ArrowItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseMoveEvent(event);
}


void ArrowItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mousePressEvent(event);
}


void ArrowItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseReleaseEvent(event);
}


void ArrowItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseDoubleClickEvent(event);
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
          rc->setParentItem(parent);
        }
        // TODO add any specialized ArrowItem Properties here.
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
