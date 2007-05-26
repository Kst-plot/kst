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
#include <QInputDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

KstPlotItem::KstPlotItem(KstPlotView *parent)
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


KstPlotItem::~KstPlotItem() {
}


KstPlotView *KstPlotItem::parentView() const {
  return qobject_cast<KstPlotView*>(parent());
}


#ifdef DEBUG_GEOMETRY
void KstPlotItem::debugGeometry() {
  _rectItem->setRect(graphicsItem()->boundingRect());
}
#endif


LabelItem::LabelItem(KstPlotView *parent)
    : KstPlotItem(parent) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
  parent->setMouseMode(KstPlotView::Create);
  parent->setCursor(Qt::IBeamCursor);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(parent, SIGNAL(mouseModeChanged()), this, SLOT(deleteLater()));

  connect(parent, SIGNAL(creationPolygonChanged(KstPlotView::CreationEvent)),
          this, SLOT(creationPolygonChanged(KstPlotView::CreationEvent)));
}


LabelItem::~LabelItem() {
}


void LabelItem::creationPolygonChanged(KstPlotView::CreationEvent event) {
  if (event == KstPlotView::MousePress) {

    bool ok;
    QString text = QInputDialog::getText(parentView(), QObject::tr("label"),
                                         QObject::tr("label:"), QLineEdit::Normal,
                                         QString::null, &ok);
    if (!ok || text.isEmpty()) {
      //This will delete...
      parentView()->setMouseMode(KstPlotView::Default);
      return;
    }

    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(KstPlotView::MousePress));
    setText(text);
    setPos(poly[0]);
    parentView()->scene()->addItem(this);
    setZValue(1);

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


LineItem::LineItem(KstPlotView *parent)
    : KstPlotItem(parent) {
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

#include "kstplotitems.moc"

// vim: ts=2 sw=2 et
