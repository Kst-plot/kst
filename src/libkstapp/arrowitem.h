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

#ifndef ARROWITEM_H
#define ARROWITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class ArrowItem : public ViewItem
{
  Q_OBJECT
  public:
    ArrowItem(View *parent);
    virtual ~ArrowItem();

    QLineF line() const;
    void setLine(const QLineF &line);

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    virtual QPainterPath grips() const;

    virtual QPointF centerOfRotation() const;

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    virtual QPainterPath leftMidGrip() const;
    virtual QPainterPath rightMidGrip() const;

  private:
    QLineF _line;
};

class KST_EXPORT CreateArrowCommand : public CreateCommand
{
  public:
    CreateArrowCommand() : CreateCommand(QObject::tr("Create Arrow")) {}
    CreateArrowCommand(View *view) : CreateCommand(view, QObject::tr("Create Arrow")) {}
    virtual ~CreateArrowCommand() {}
    virtual void createItem();
};

class ArrowItemFactory : public GraphicsFactory {
  public:
    ArrowItemFactory();
    ~ArrowItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
