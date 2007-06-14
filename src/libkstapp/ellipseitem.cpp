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

namespace Kst {

EllipseItem::EllipseItem(View *parent)
  : ViewItem(parent) {
}


EllipseItem::~EllipseItem() {
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

}

#include "ellipseitem.moc"

// vim: ts=2 sw=2 et
