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

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "kst_export.h"

namespace Kst {

class View;

class Scene : public QGraphicsScene
{
  Q_OBJECT
  public:
    Scene(View *view);
    virtual ~Scene();

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void dragEnterEvent(QGraphicsSceneDragDropEvent*);
    void dragMoveEvent(QGraphicsSceneDragDropEvent*);
    void dropEvent(QGraphicsSceneDragDropEvent*);
};

}

#endif

// vim: ts=2 sw=2 et
