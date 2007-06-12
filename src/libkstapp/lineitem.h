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

#ifndef LINEITEM_H
#define LINEITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

namespace Kst {

class LineItem : public ViewItem
{
  Q_OBJECT
public:
  LineItem(View *parent);
  virtual ~LineItem();

  virtual QPainterPath itemShape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

  QLineF line() const;
  void setLine(const QLineF &line);

protected Q_SLOTS:
  virtual void creationPolygonChanged(View::CreationEvent event);

private:
  QLineF _line;
};

class KST_EXPORT CreateLineCommand : public CreateCommand
{
public:
  CreateLineCommand() : CreateCommand(QObject::tr("Create Line")) {}
  CreateLineCommand(View *view) : CreateCommand(view, QObject::tr("Create Line")) {}
  virtual ~CreateLineCommand() {}
  virtual void createItem();
};

class LineItemFactory : public GraphicsFactory {
  public:
    LineItemFactory();
    ~LineItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
