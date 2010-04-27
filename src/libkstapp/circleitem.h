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

#ifndef CIRCLEITEM_H
#define CIRCLEITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class CircleItem : public ViewItem
{
  Q_OBJECT
  public:
    CircleItem(View *parent);
    virtual ~CircleItem();

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);
    virtual void creationPolygonChanged(View::CreationEvent event);
};

class CreateCircleCommand : public CreateCommand
{
  public:
    CreateCircleCommand() : CreateCommand(QObject::tr("Create Circle")) {}
    CreateCircleCommand(View *view) : CreateCommand(view, QObject::tr("Create Circle")) {}
    virtual ~CreateCircleCommand() {}
    virtual void createItem();
};

class CircleItemFactory : public GraphicsFactory {
  public:
    CircleItemFactory();
    ~CircleItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
