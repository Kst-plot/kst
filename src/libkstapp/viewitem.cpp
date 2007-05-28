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

#include "viewitem.h"

#include "kstplotcommands.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

ViewItem::ViewItem(KstPlotView *parent)
    : QObject(parent) {
#ifdef DEBUG_GEOMETRY
  QColor semiRed(QColor(255, 0, 0, 50));
  _rectItem = new QGraphicsRectItem;
  _rectItem->setZValue(0);
  _rectItem->setPen(semiRed);
  _rectItem->setBrush(semiRed);
  parent->scene()->addItem(_rectItem);
#endif
}


ViewItem::~ViewItem() {
}


KstPlotView *ViewItem::parentView() const {
  return qobject_cast<KstPlotView*>(parent());
}


#ifdef DEBUG_GEOMETRY
void ViewItem::debugGeometry() {
  _rectItem->setRect(graphicsItem()->boundingRect());
}
#endif

}

#include "viewitem.moc"

// vim: ts=2 sw=2 et
