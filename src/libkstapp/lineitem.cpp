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

#include "lineitem.h"
#include "view.h"
#include <kstdebug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

LineItem::LineItem(View *parent)
  : ViewItem(parent) {
  setName("LineItem");
  setAllowedGrips(RightMidGrip | LeftMidGrip);
}


LineItem::~LineItem() {
}


void LineItem::paint(QPainter *painter) {
  painter->drawLine(line());
}


QLineF LineItem::line() const {
  return QLineF(rect().left(), rect().center().y(), rect().right(), rect().center().y());
}


void LineItem::setLine(const QLineF &line_) {
  setPos(line_.p1());
  setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));

  if (!rect().isEmpty()) {
    rotateTowards(line().p2(), line_.p2());
  }

  QRectF r = rect();
  r.setSize(QSizeF(QLineF(line().p1(), line_.p2()).length(), r.height()));
  setViewRect(r);
}


QPainterPath LineItem::leftMidGrip() const {
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


QPainterPath LineItem::rightMidGrip() const {
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


QPainterPath LineItem::grips() const {
  if (gripMode() == Move)
    return QPainterPath();

  QPainterPath grips;
  grips.addPath(leftMidGrip());
  grips.addPath(rightMidGrip());
  return grips;
}


QPointF LineItem::centerOfRotation() const {
  if (activeGrip() == RightMidGrip)
    return line().p1();
  else if (activeGrip() == LeftMidGrip)
    return line().p2();

  return line().p1();
}


void LineItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));
    parentView()->scene()->addItem(this);
    setZValue(1);
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


void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseMoveEvent(event);
}


void LineItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {

//   if (parentView()->viewMode() == View::Data) {
//     event->ignore();
//     return;
//   }
// 
//   QPointF p = event->pos();
//   if (leftMidGrip().contains(p)) {
//     setActiveGrip(LeftMidGrip);
//   } else if (rightMidGrip().contains(p)) {
//     setActiveGrip(RightMidGrip);
//   } else {
//     setActiveGrip(NoGrip);
//   }

  ViewItem::mousePressEvent(event);
}


void LineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseReleaseEvent(event);
}


void LineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseDoubleClickEvent(event);
}


void CreateLineCommand::createItem() {
  _item = new LineItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

LineItemFactory::LineItemFactory()
: GraphicsFactory() {
  registerFactory("line", this);
}


LineItemFactory::~LineItemFactory() {
}


ViewItem* LineItemFactory::generateGraphics(QXmlStreamReader& xml, View *view, ViewItem *parent) {
  LineItem *rc = 0;
  double x1 = 0., y1 = 0., x2 = 10., y2 = 10.;
  while (!xml.atEnd()) {
    if (xml.isStartElement()) {
      if (xml.name().toString() == "line") {
        Q_ASSERT(!rc);
        rc = new LineItem(view);
        if (parent) {
          rc->setParentItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("thickness");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setWidthF(av.toString().toDouble());
          rc->setPen(p);
        }
        av = attrs.value("color");
        if (!av.isNull()) {
          QPen p = rc->pen();
          p.setColor(QColor(av.toString()));
          rc->setPen(p);
        }
        av = attrs.value("x1");
        if (!av.isNull()) {
          x1 = av.toString().toDouble();
        }
        av = attrs.value("y1");
        if (!av.isNull()) {
          y1 = av.toString().toDouble();
        }
        av = attrs.value("x2");
        if (!av.isNull()) {
          x2 = av.toString().toDouble();
        }
        av = attrs.value("y2");
        if (!av.isNull()) {
          y2 = av.toString().toDouble();
        }
      } else {
        Q_ASSERT(rc);
        ViewItem *i = GraphicsFactory::parse(xml, view, rc);
        if (!i) {
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "line") {
        break;
      } else {
        KstDebug::self()->log(QObject::tr("Error creating line object from Kst file."), KstDebug::Warning);
        delete rc;
        return 0;
      }
    }
    xml.readNext();
  }

  rc->setLine(QLineF(QPointF(x1, y1), QPointF(x2, y2)));
  return rc;
}

}

// vim: ts=2 sw=2 et
