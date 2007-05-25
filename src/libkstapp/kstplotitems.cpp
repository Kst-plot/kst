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

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>

KstPlotItem::KstPlotItem(KstPlotView *parent)
    : QObject(parent) {
  connect(parent, SIGNAL(resized()), this, SLOT(updateGeometry()));
}


KstPlotItem::~KstPlotItem() {
}


KstPlotView *KstPlotItem::parentView() const {
  return qobject_cast<KstPlotView*>(parent());
}


void KstPlotItem::updateAspectFromGeometry() {
  KstPlotView *v = parentView();
  Q_ASSERT(v);
  QGraphicsItem *i = graphicsItem();
  Q_ASSERT(i);

  _aspectPos = QPointF(100.0 * i->scenePos().x() / v->width(),
                       100.0 * i->scenePos().y() / v->height());
  _aspectSize = QSizeF(100.0 * i->boundingRect().width() / v->width(),
                       100.0 * i->boundingRect().height() / v->height());
}


void KstPlotItem::updateGeometry() {
  KstPlotView *v = parentView();
  Q_ASSERT(v);
  QGraphicsItem *i = graphicsItem();
  Q_ASSERT(i);

  i->setPos(_aspectPos.x() * v->width() / 100.0,
            _aspectPos.y() * v->height() / 100.0);
  qreal sx = _aspectSize.width() * v->width() / (i->boundingRect().width() * 100.0);
  qreal sy = _aspectSize.height() * v->height() / (i->boundingRect().height() * 100.0);
  i->scale(sx, sy);

  updateAspectFromGeometry();
}


LabelItem::LabelItem(const QString &text, KstPlotView *parent)
    : KstPlotItem(parent), QGraphicsSimpleTextItem(text) {
  updateAspectFromGeometry();
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
}


LabelItem::~LabelItem() {
}


LineItem::LineItem(KstPlotView *parent)
    : KstPlotItem(parent) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  parent->setMouseMode(KstPlotView::Create);
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
    updateAspectFromGeometry();
    parentView()->setMouseMode(KstPlotView::Default);
    parentView()->disconnect(this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
    return;
  }
}

#include "kstplotitems.moc"

// vim: ts=2 sw=2 et
