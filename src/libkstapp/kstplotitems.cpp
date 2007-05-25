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

#include "kstplotitems.h"
#include "kstplotview.h"

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

KstPlotItem::KstPlotItem(KstPlotView *parent)
    : QObject(parent) {
}


KstPlotItem::~KstPlotItem() {
}


KstPlotView *KstPlotItem::parentView() const {
  return qobject_cast<KstPlotView*>(parent());
}


LabelItem::LabelItem(const QString &text, KstPlotView *parent)
    : KstPlotItem(parent), QGraphicsSimpleTextItem(text) {
}


LabelItem::~LabelItem() {
}


LineItem::LineItem(KstPlotView *parent)
    : KstPlotItem(parent) {

  parent->setMouseMode(KstPlotView::Create);
  connect(parent, SIGNAL(creationPolygonChanged()),
          this, SLOT(creationPolygonChanged()));
}


LineItem::~LineItem() {
}


void LineItem::creationPolygonChanged() {
  const QPolygonF poly = mapFromScene(parentView()->creationPolygon());
  if (poly.count() > 1) {
    setLine(QLineF(poly[0], poly[1]));
    parentView()->scene()->addItem(this);
    parentView()->setMouseMode(KstPlotView::Default);
    parentView()->disconnect(this);
    setZValue(1);
  }
}

#include "kstplotitems.moc"

// vim: ts=2 sw=2 et
