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

#include "circleitem.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>

namespace Kst {

CircleItem::CircleItem(View *parent)
    : ViewItem(parent) {
  setTypeName("Circle");
  setBrush(Qt::white);
  setLockAspectRatio(true);
  setLockAspectRatioFixed(true);
}


CircleItem::~CircleItem() {
}


void CircleItem::paint(QPainter *painter) {
  const qreal w = pen().widthF();
  painter->drawEllipse(rect().adjusted(w, w, -w, -w));
}


void CircleItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("circle");
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


void CircleItem::creationPolygonChanged(View::CreationEvent event) {
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
    qreal size = (fabs(poly.last().x() - rect().x()) < fabs(poly.last().y() - rect().y())) ? fabs(poly.last().x() - rect().x()) : fabs(poly.last().y() - rect().y());
    qreal width = (poly.last().x() - rect().x()) < 0 ? size * -1.0 : size;
    qreal height = (poly.last().y() - rect().y()) < 0 ? size * -1.0 : size;
    QRectF newRect(rect().x(), rect().y(),
                   width,
                   height);
    setViewRect(newRect);

    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(view()->creationPolygon(View::MouseRelease));
    view()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    view()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    view()->setMouseMode(View::Default);
    updateViewItemParent();
    emit creationComplete();
    return;
  }
}


void CreateCircleCommand::createItem() {
  _item = new CircleItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


CircleItemFactory::CircleItemFactory()
: GraphicsFactory() {
  registerFactory("circle", this);
}


CircleItemFactory::~CircleItemFactory() {
}


ViewItem* CircleItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  CircleItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "circle") {
        Q_ASSERT(!rc);
        rc = new CircleItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        }
        // Add any new specialized CircleItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "circle") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating circle object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
