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

#include "plotlayoutitem.h"

#include "plotitem.h"

#include <QDebug>

namespace Kst {

PlotLayoutItem::PlotLayoutItem(View *parent)
  : ViewItem(parent) {
  connect(this, SIGNAL(geometryChanged()), this, SLOT(updateGeometry()));
}


PlotLayoutItem::~PlotLayoutItem() {
}


void PlotLayoutItem::updateGeometry() {

  QList<QGraphicsItem*> plotItems = QGraphicsItem::children();
  QList<QGraphicsItem*>::Iterator it = plotItems.begin();

  qreal itemHeight = 0.0;

  for (; it != plotItems.end(); ++it) {
    PlotItem *plotItem = qgraphicsitem_cast<PlotItem*>(*it);
    Q_ASSERT(plotItem);

    QPointF topLeft(0, itemHeight);
    plotItem->resetTransform();
    plotItem->setPos(topLeft);
    plotItem->setViewRect(QRectF(QPoint(0,0), QSizeF(width(), height() / plotItems.count())));
    itemHeight += height() / plotItems.count();
  }
}

}

// vim: ts=2 sw=2 et
