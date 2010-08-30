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

#include "ellipseitem.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

#include <debug.h>

namespace Kst {

EllipseItem::EllipseItem(View *parent)
  : ViewItem(parent) {
  setTypeName("Ellipse");
  setBrush(Qt::white);
}


EllipseItem::~EllipseItem() {
}


void EllipseItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("ellipse");
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


QPainterPath EllipseItem::itemShape() const {
  QPainterPath path;
  path.addEllipse(rect());
  return path;
}


void EllipseItem::paint(QPainter *painter) {
  const qreal w = pen().widthF();
  painter->drawEllipse(rect().adjusted(w, w, -w, -w));
}


void CreateEllipseCommand::createItem() {
  _item = new EllipseItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


EllipseItemFactory::EllipseItemFactory()
: GraphicsFactory() {
  registerFactory("ellipse", this);
}


EllipseItemFactory::~EllipseItemFactory() {
}


ViewItem* EllipseItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  EllipseItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "ellipse") {
        Q_ASSERT(!rc);
        rc = new EllipseItem(view);
        if (parent) {
          rc->setParentViewItem(parent);
        // Add any new specialized BoxItem Properties here.
        }
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "ellipse") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating ellipse object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
