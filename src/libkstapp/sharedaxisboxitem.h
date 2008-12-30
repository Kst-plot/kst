/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHAREDAXISBOXITEM_H
#define SHAREDAXISBOXITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class SharedAxisBoxItem : public ViewItem
{
  Q_OBJECT
  public:
    SharedAxisBoxItem(View *parent);
    virtual ~SharedAxisBoxItem();

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    virtual void addToMenuForContextEvent(QMenu &menu);
    void triggerContextEvent(QGraphicsSceneContextMenuEvent *event);

  Q_SIGNALS:
    void breakShareSignal();

  public slots:
    void breakShare();
    void acceptItems();
    void lockItems();

  private:
    QAction *_breakAction;
    QPointer<ViewGridLayout> _layout;
};

class KST_EXPORT CreateSharedAxisBoxCommand : public CreateCommand
{
  public:
    CreateSharedAxisBoxCommand() : CreateCommand(QObject::tr("Create Shared Axis Box")) {}
    CreateSharedAxisBoxCommand(View *view) : CreateCommand(view, QObject::tr("Create Shared Axis Box")) {}
    virtual ~CreateSharedAxisBoxCommand() {}
    virtual void undo();
    virtual void redo();
    virtual void createItem();
};

class SharedAxisBoxItemFactory : public GraphicsFactory {
  public:
    SharedAxisBoxItemFactory();
    ~SharedAxisBoxItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
