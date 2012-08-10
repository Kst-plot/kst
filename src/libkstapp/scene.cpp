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

#include "scene.h"
#include "view.h"
#include "viewitem.h"
#include "plotitem.h"
#include "plotrenderitem.h"


#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>

namespace Kst {

Scene::Scene(View *view)
  : QGraphicsScene(view) {
}


Scene::~Scene() {
}

/*
 * This is reimplemented because of a bug in Qt 4.3 QGraphicScene in the
 * way it handles context menu events.  The docs say that if an item ignores
 * this event that it'll be passed down to the next item underneath.  However,
 * this does not happen in the default implementation.
 */
void Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QList<QGraphicsItem*> list = items(event->scenePos());
  if (list.isEmpty()) {
    if (View *view = qobject_cast<View*>(parent())) {
      view->contextMenuEvent();
    }
  } else {
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem)
        continue;

      if (viewItem->acceptsContextMenuEvents()) {
          event->setPos(viewItem->mapFromScene(event->scenePos()));
          if (viewItem->doSceneEvent(event))
            return;
      }
    }
  }
  QGraphicsScene::contextMenuEvent(event);
}


void Scene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  if (MimeDataViewItem::downcast(event->mimeData())) {
      event->acceptProposedAction();
  } else if (event->mimeData()->hasUrls()) {
    event->ignore();
  }
}

void Scene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  if (MimeDataViewItem::downcast(event->mimeData())) {
      event->acceptProposedAction();
  }
}

void Scene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  const MimeDataViewItem* m = MimeDataViewItem::downcast(event->mimeData());
  if (m && m->item) {
    View* view = qobject_cast<View*>(parent());
    if (view->viewMode() != View::Layout) {
      view->setViewMode(View::Layout);
    }
    if (view != m->item->view()) {
      m->item->setView(view);
      PlotItem* plotItem = qobject_cast<PlotItem*>(m->item);
      if (plotItem) {
        QList<PlotRenderItem*> renderItems = plotItem->renderItems();
        foreach (PlotRenderItem* renderItem, renderItems) {
          renderItem->setView(view);
        }
      }
      m->item->setParentViewItem(0);
      addItem(m->item);

    }
    m->item->show();
    QPointF viewpos = view->mapFromScene(event->scenePos());
    //qDebug() << "viewpos: " << viewpos << " scenepos: " << event->scenePos();
    //qDebug() << "m hotspot: " << m->hotSpot << " rect: " << m->item->rect() <<
    //            " drop hot spot" << m->item->dropHotSpot << " topleft:" << m->item->rect().topLeft();
    m->item->moveTo(viewpos - m->item->rect().center() - m->item->dropHotSpot.toPoint());
    event->acceptProposedAction();
  } else {
    event->ignore();
  }
}


}

// vim: ts=2 sw=2 et
