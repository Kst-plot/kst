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

#include "layoutboxitem.h"
#include "viewgridlayout.h"

#include <debug.h>

#include <QDebug>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

LayoutBoxItem::LayoutBoxItem(View *parent)
    : ViewItem(parent) {
  setName("Layout Box");
  setPen(Qt::NoPen);
  setBrush(Qt::NoBrush);
  setAllowedGripModes(Move);

  parentView()->scene()->addItem(this);
  setZValue(1);
  setPos(parentView()->sceneRect().topLeft());
  setViewRect(parentView()->sceneRect());

  setEnabled(true);
}


void LayoutBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(pen());
  painter->setBrush(brush());
  painter->drawRect(rect());
}


LayoutBoxItem::~LayoutBoxItem() {
}


void LayoutBoxItem::setEnabled(bool enabled) {
  if (!parentView())
    return;

  if (enabled) {
    QList<QGraphicsItem*> list = parentView()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem || viewItem->parentItem() || !viewItem->isVisible() || viewItem == this)
        continue;

      viewItem->setParentItem(this);
    }

    parentView()->setLayoutBoxItem(this);

    show();
  } else {
    QList<QGraphicsItem*> list = QGraphicsItem::children();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem)
        continue;

      viewItem->setParentItem(0);
    }

    parentView()->setLayoutBoxItem(0);

    hide();
  }
}


void LayoutBoxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  addTitle(&menu);

  QAction *editAction = menu.addAction(tr("Edit"));
  connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

  QAction *breakLayoutAction = menu.addAction(tr("Break layout"));
  connect(breakLayoutAction, SIGNAL(triggered()), this, SLOT(breakLayout()));

  menu.exec(event->screenPos());
}


void LayoutBoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  event->ignore();
}

}

// vim: ts=2 sw=2 et
