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

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

LineItem::LineItem(KstPlotView *parent)
    : ViewItem(parent) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  parent->setMouseMode(KstPlotView::Create);
  parent->setCursor(Qt::CrossCursor);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(deleteLater()));

  connect(parent, SIGNAL(creationPolygonChanged(KstPlotView::CreationEvent)),
          this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
}


LineItem::~LineItem() {
}


void LineItem::creationPolygonChanged(KstPlotView::CreationEvent event) {
  if (event == KstPlotView::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(KstPlotView::MousePress));
    setLine(QLineF(poly[0], poly[0])); //start and end
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == KstPlotView::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(KstPlotView::MouseMove));
    setLine(QLineF(line().p1(), poly.last())); //start and end
    return;
  }

  if (event == KstPlotView::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(KstPlotView::MouseRelease));
    setLine(QLineF(line().p1(), poly.last())); //start and end

#ifdef DEBUG_GEOMETRY
    debugGeometry();
#endif

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
    parentView()->setMouseMode(KstPlotView::Default);
    emit creationComplete();
    return;
  }
}

}

#include "lineitem.moc"

// vim: ts=2 sw=2 et
