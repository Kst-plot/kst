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

#ifndef CIRCLEITEM_H
#define CIRCLEITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class CircleItemDialog;

class CircleItem : public ViewItem
{
  Q_OBJECT
  public:
    CircleItem(View *parent);
    virtual ~CircleItem();

    const QString defaultsGroupName() const {return CircleItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("circle");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);
    virtual void creationPolygonChanged(View::CreationEvent event);
    virtual bool customDimensionsTab() {return true;}
    virtual void clearEditDialogPtr() {_circleEditDialog = 0;}
  public Q_SLOTS:
    virtual void edit();

  private:
    CircleItemDialog *_circleEditDialog;

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
