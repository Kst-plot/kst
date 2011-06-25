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

#ifndef ARROWITEM_H
#define ARROWITEM_H

#include "lineitem.h"
#include "graphicsfactory.h"

namespace Kst {

class ArrowItem : public LineItem
{
  Q_OBJECT
  public:
    ArrowItem(View *parent);
    virtual ~ArrowItem();

    const QString defaultsGroupName() const {return ArrowItem::staticDefaultsGroupName();}
    static QString staticDefaultsGroupName() { return QString("arrow");}

    // for view item dialogs
    virtual bool hasStroke() const {return true;}
    virtual bool hasBrush() const {return false;}

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    bool startArrowHead() { return _startArrowHead; }
    void setStartArrowHead(const bool enabled) { _startArrowHead = enabled; }

    bool endArrowHead() { return _endArrowHead; }
    void setEndArrowHead(const bool enabled) { _endArrowHead = enabled; }

    qreal startArrowScale() { return _startArrowScale; }
    void setStartArrowScale(const qreal scale) { _startArrowScale = scale; }

    qreal endArrowScale() { return _endArrowScale; }
    void setEndArrowScale(const qreal scale) { _endArrowScale = scale; }

    virtual QPainterPath shape() const;

  public Q_SLOTS:
    virtual void edit();

  private:
    bool _startArrowHead;
    bool _endArrowHead;
    qreal _startArrowScale;
    qreal _endArrowScale;
    QPolygonF start, end;
};

class CreateArrowCommand : public CreateCommand
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
