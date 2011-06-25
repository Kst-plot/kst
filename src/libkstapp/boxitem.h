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

#ifndef BOXITEM_H
#define BOXITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class BoxItem : public ViewItem
{
  Q_OBJECT
  public:
    BoxItem(View *parent);
    virtual ~BoxItem();

    const QString defaultsGroupName() const {return BoxItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("box");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return true;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);
};

class CreateBoxCommand : public CreateCommand
{
  public:
    CreateBoxCommand() : CreateCommand(QObject::tr("Create Box")) {}
    CreateBoxCommand(View *view) : CreateCommand(view, QObject::tr("Create Box")) {}
    virtual ~CreateBoxCommand() {}
    virtual void createItem();
};

class BoxItemFactory : public GraphicsFactory {
  public:
    BoxItemFactory();
    ~BoxItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
