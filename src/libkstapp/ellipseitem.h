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

#ifndef ELLIPSEITEM_H
#define ELLIPSEITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class EllipseItem : public ViewItem
{
  Q_OBJECT
  public:
    EllipseItem(View *parent);
    virtual ~EllipseItem();

    const QString defaultsGroupName() const {return QString("ellipse");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}

    virtual void save(QXmlStreamWriter &xml);
    virtual QPainterPath itemShape() const;
    virtual void paint(QPainter *painter);
};

class CreateEllipseCommand : public CreateCommand
{
  public:
    CreateEllipseCommand() : CreateCommand(QObject::tr("Create Ellipse")) {}
    CreateEllipseCommand(View *view) : CreateCommand(view, QObject::tr("Create Ellipse")) {}
    virtual ~CreateEllipseCommand() {}
    virtual void createItem();
};

class EllipseItemFactory : public GraphicsFactory {
  public:
    EllipseItemFactory();
    ~EllipseItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};
}

#endif

// vim: ts=2 sw=2 et
