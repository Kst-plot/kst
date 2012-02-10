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


#include "lineitem.h"

#include "view.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

LineItem::LineItem(View *parent)
  : ViewItem(parent) {
  _created = false;
  setTypeName("Line");
  setAllowedGrips(RightMidGrip | LeftMidGrip);
  setAllowedGripModes(Resize);
  setAllowsLayout(false);
  QPen p = pen();
  p.setWidthF(1);
  setPen(p);
  applyDialogDefaultsStroke();
  applyDialogDefaultsLockPosToData();
}


LineItem::~LineItem() {
}


void LineItem::paint(QPainter *painter) {
  painter->drawLine(line());
}


void LineItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("line");
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


QLineF LineItem::line() const {
  return QLineF(rect().left(), rect().center().y(), rect().right(), rect().center().y());
}


//void LineItem::setLine(const QLineF &line_) {
//  setPos(line_.p1());
//  setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));
//
//  if (!rect().isEmpty()) {
//    rotateTowards(line().p2(), line_.p2());
//  }
//
//  QRectF r = rect();
//  r.setSize(QSizeF(QLineF(line().p1(), line_.p2()).length(), r.height()));
//  setViewRect(r);
//}


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

  // when creating the object with the mouse, use p1 as the rotation point.
  // otherwise, use the center
  if (_created) {
      return QPointF((line().x1()+line().x2())*0.5, (line().y1()+line().y2())*0.5);
  } else {
      return QPointF(line().p1());
  }
}


void LineItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::EscapeEvent) {
    ViewItem::creationPolygonChanged(event);
    return;
  }

  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MousePress));
    setPos(poly.first().x(), poly.first().y());
    setViewRect(QRectF(0.0, 0.0, 0.0, sizeOfGrip().height()));
    view()->scene()->addItem(this);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseMove));
    if (!rect().isEmpty()) {
      rotateTowards(line().p2(), poly.last());
    }
    QRectF r = rect();
    r.setSize(QSizeF(QLineF(line().p1(), poly.last()).length(), r.height()));
    setViewRect(r);
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseRelease));
    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);
    updateViewItemParent();
    _created = true;
    emit creationComplete();
    return;
  }
}


void LineItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

  if (view()->viewMode() == View::Data) {
    event->ignore();
    return;
  }

  if (!dragStartPosition.isNull() && event->buttons() & Qt::LeftButton) {
    if (view()->mouseMode() == View::Move) {
      startDragging(event->widget(), dragStartPosition.toPoint());
      return;
    }
  }

  if (view()->mouseMode() == View::Default) {
    if (gripMode() == ViewItem::Move || activeGrip() == NoGrip) {
      view()->setMouseMode(View::Move);
      view()->undoStack()->beginMacro(tr("Move"));
    } else if (gripMode() == ViewItem::Resize) {
      view()->setMouseMode(View::Resize);
      view()->undoStack()->beginMacro(tr("Resize"));
    }
  }

  if (activeGrip() == NoGrip)
    return QGraphicsRectItem::mouseMoveEvent(event);

  QPointF p = event->pos();
  //QPointF s = event->scenePos();

  if (gripMode() == ViewItem::Resize) {
    switch(activeGrip()) {
    case RightMidGrip:
      resizeRight(p.x() - rightMidGrip().controlPointRect().center().x());
      rotateTowards(rightMidGrip().controlPointRect().center(), p);
      break;
    case LeftMidGrip:
      resizeLeft(p.x() - leftMidGrip().controlPointRect().center().x());
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
    if ((isAllowed(RightMidGrip) && rightMidGrip().contains(p)) || (isAllowed(LeftMidGrip) && leftMidGrip().contains(p))) {
      view()->setCursor(Qt::CrossCursor);
    } else {
      view()->setCursor(Qt::SizeAllCursor);
    }
  } else {
    //view()->setCursor(Qt::SizeAllCursor);
  }
}


void LineItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
  // parent has been resized: update the line's dimensions:

  // we would like to have lines in terms of relative endpoint locations,
  // but instead they are in terms of length (relative to parent width)
  // and angle, relative to the parent.
  qreal theta = rotationAngle()*M_PI/180.0;
  qreal oldL = relativeWidth()*oldParentRect.width();

  // we want to keep the endpoints fixed relative to the parent, so
  // we need to calculate new lengths and angles.
  qreal newDx = cos(theta)*oldL*newParentRect.width()/oldParentRect.width();
  qreal newDy = sin(theta)*oldL*newParentRect.height()/oldParentRect.height();
  qreal newWidth = sqrt(newDx*newDx + newDy*newDy);
  QTransform transform;
  transform.rotate(atan2(newDy, newDx)*180.0/M_PI);

  // my brain hurts less for rotations when we center the object at 0,0
  QRectF itemRect(-newWidth*0.5, -rect().height()*0.5,
                  newWidth, rect().height());

  // we don't now what the parents's origin is, so, add .x() and .y()
  setPos(relativeCenter().x() * newParentRect.width() + newParentRect.x(),
         relativeCenter().y() * newParentRect.height()+ newParentRect.y());
  setViewRect(itemRect, true);

  setTransform(transform);
  setRelativeWidth(newWidth / newParentRect.width());
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
      if (!rc && xml.name().toString() == "line") {
        Q_ASSERT(!rc);
        rc = new LineItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        // Add any new specialized LineItem Properties here.
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
