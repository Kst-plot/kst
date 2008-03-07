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
#include "viewitemzorder.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

LineItem::LineItem(View *parent)
  : ViewItem(parent) {
  setName("Line");
  setZValue(LINE_ZVALUE);
  setAllowedGrips(RightMidGrip | LeftMidGrip);
  setAllowedGripModes(Resize);
  QPen p = pen();
  p.setWidthF(1);
  setPen(p);
}


LineItem::~LineItem() {
}


void LineItem::paint(QPainter *painter) {
  painter->drawLine(line());
}


void LineItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("line");
  ViewItem::save(xml);
  xml.writeEndElement();
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
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topLeft(), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));
  QPainterPath path;
  path.addEllipse(grip);

  return path;
}


QPainterPath LineItem::rightMidGrip() const {
  QRectF bound = gripBoundingRect();
  QRectF grip = QRectF(bound.topRight() - QPointF(sizeOfGrip().width(), 0), sizeOfGrip());
  grip.moveCenter(QPointF(grip.center().x(), bound.center().y()));
  QPainterPath path;
  path.addEllipse(grip);

  return path;
}


QPainterPath LineItem::grips() const {
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


void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (parentView()->viewMode() == View::Data || itemInLayout()) {
    event->ignore();
    return;
  }

  if (parentView()->mouseMode() == View::Default) {
    if (gripMode() == ViewItem::Resize) {
      parentView()->setMouseMode(View::Resize);
      parentView()->undoStack()->beginMacro(tr("Resize"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  QPointF l = event->lastPos();
  QPointF s = event->scenePos();

  if (gripMode() == ViewItem::Resize) {
    switch(activeGrip()) {
    case RightMidGrip:
      resizeRight(p.x() - l.x());
      rotateTowards(rightMidGrip().controlPointRect().center(), p);
      break;
    case LeftMidGrip:
      resizeLeft(p.x() - l.x());
      rotateTowards(leftMidGrip().controlPointRect().center(), p);
      break;
    default:
      break;
    }
  }
}


void LineItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mousePressEvent(event);
}


void LineItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseReleaseEvent(event);
}


void LineItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
  ViewItem::mouseDoubleClickEvent(event);
}


void LineItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
  QGraphicsRectItem::hoverMoveEvent(event);
  if (isSelected()) {
    QPointF p = event->pos();
    if (isAllowed(RightMidGrip) && rightMidGrip().contains(p) || isAllowed(LeftMidGrip) && leftMidGrip().contains(p)) {
      parentView()->setCursor(Qt::CrossCursor);
    } else {
      parentView()->setCursor(Qt::SizeAllCursor);
    }
  } else {
    parentView()->setCursor(Qt::SizeAllCursor);
  }
}


void LineItem::updateRelativeSize() {
    QPointF topLeft = mapToParent(rect().topLeft());
    QPointF bottomRight = mapToParent(rect().bottomRight());

  if (parentViewItem()) {
    QPointF topLeftOffset = topLeft - parentViewItem()->rect().topLeft();
    QPointF bottomRightOffset = bottomRight - parentViewItem()->rect().topLeft();

    _xTopLeftRelation = topLeftOffset.x() / parentViewItem()->width();
    _yTopLeftRelation = topLeftOffset.y() / parentViewItem()->height();
    _xBottomRightRelation = bottomRightOffset.x() / parentViewItem()->width();
    _yBottomRightRelation = bottomRightOffset.y() / parentViewItem()->height();
  } else if (parentView()) {
    QPointF topLeftOffset = topLeft - parentView()->rect().topLeft();
    QPointF bottomRightOffset = bottomRight - parentView()->rect().topLeft();

    _xTopLeftRelation = topLeftOffset.x() / parentView()->width();
    _yTopLeftRelation = topLeftOffset.y() / parentView()->height();
    _xBottomRightRelation = bottomRightOffset.x() / parentView()->width();
    _yBottomRightRelation = bottomRightOffset.y() / parentView()->height();
  }
  ViewItem::updateRelativeSize();
}


void LineItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
//   qDebug() << "LineItem::updateChildGeometry" << oldParentRect << newParentRect << endl;

  QRectF itemRect = rect();

  QPointF newTopLeft = newParentRect.topLeft() + QPointF(newParentRect.width() * _xTopLeftRelation, newParentRect.height() * _yTopLeftRelation);
  QPointF newBottomRight = newParentRect.topLeft() + QPointF(newParentRect.width() * _xBottomRightRelation, newParentRect.height() * _yBottomRightRelation);

  QPointF posOffset = newTopLeft - mapToParent(rect().topLeft());

  itemRect.setRight(itemRect.left() + mapFromParent(newBottomRight).x() - mapFromParent(newTopLeft).x());
  setViewRect(itemRect, true);

  rotateTowards(rightMidGrip().controlPointRect().center(), mapFromParent(pos() + (newBottomRight - newTopLeft)));
  setPos(pos() + posOffset);
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


ViewItem* LineItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  LineItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "line") {
        Q_ASSERT(!rc);
        rc = new LineItem(view);
        if (parent) {
          rc->setParent(parent);
        }
        // TODO add any specialized LineItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "line") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating line object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }

  return rc;
}

}

// vim: ts=2 sw=2 et
