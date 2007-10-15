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

#include "scene.h"
#include "view.h"
#include "viewitem.h"

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
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (viewItem->acceptsContextMenuEvents()) {
        event->setPos(viewItem->mapFromScene(event->scenePos()));
        if (viewItem->sceneEvent(event))
          return;
    }
  }

  QGraphicsScene::contextMenuEvent(event);
}

}

// vim: ts=2 sw=2 et
