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

#ifndef ELLIPSEITEM_H
#define ELLIPSEITEM_H

#include "viewitem.h"
#include <QGraphicsRectItem>

namespace Kst {

class EllipseItem : public ViewItem, public QGraphicsRectItem
{
  Q_OBJECT
public:
  EllipseItem(View *parent);
  virtual ~EllipseItem();

  virtual QGraphicsItem *graphicsItem() { return this; }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private Q_SLOTS:
  void creationPolygonChanged(View::CreationEvent event);
};

class KST_EXPORT CreateEllipseCommand : public CreateCommand
{
public:
  CreateEllipseCommand() : CreateCommand(QObject::tr("Create Ellipse")) {}
  CreateEllipseCommand(View *view) : CreateCommand(view, QObject::tr("Create Ellipse")) {}
  virtual ~CreateEllipseCommand() {}
  virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
