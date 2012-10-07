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

#include "layoutboxitem.h"

#include "viewgridlayout.h"

#include "debug.h"
#include <QDebug>
#include <QMenu>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

LayoutBoxItem::LayoutBoxItem(View *parent)
    : ViewItem(parent) {
  setAllowsLayout(false);

  setTypeName("Layout Box");
  setPen(Qt::NoPen);
  setBrush(Qt::NoBrush);
  setAllowedGripModes(Move);

  view()->scene()->addItem(this);
  setPos(view()->sceneRect().topLeft());
  setViewRect(view()->sceneRect());

  setEnabled(true);
}


void LayoutBoxItem::appendItem(ViewItem *item) {
  if (item->parentItem() == this)
    return;

  item->setParentViewItem(this);
  setPos(view()->sceneRect().topLeft());
  setViewRect(view()->sceneRect());
}



void LayoutBoxItem::save(QXmlStreamWriter &xml) {
  // layout box items are transient and are never saved..
}


LayoutBoxItem::~LayoutBoxItem() {
}


void LayoutBoxItem::setEnabled(bool enabled) {
  if (!view())
    return;

  if (enabled) {
    QList<QGraphicsItem*> list = view()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
      if (!viewItem || viewItem->parentItem() || !viewItem->isVisible() || viewItem == this)
        continue;

      viewItem->setParentViewItem(this);
    }

    view()->setLayoutBoxItem(this);

    show();
  } else {
    QList<QGraphicsItem*> list = QGraphicsItem::childItems();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
      if (!viewItem)
        continue;

      viewItem->setParentViewItem(0);
    }

    view()->setLayoutBoxItem(0);

    hide();
  }
}


void LayoutBoxItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  event->ignore();
}


void LayoutBoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  event->ignore();
}



LayoutBoxItemFactory::LayoutBoxItemFactory()
: GraphicsFactory() {
  registerFactory("layoutbox", this);
}


LayoutBoxItemFactory::~LayoutBoxItemFactory() {
}


ViewItem* LayoutBoxItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  return 0;
}

}

// vim: ts=2 sw=2 et
