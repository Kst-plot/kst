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
#include <QGraphicsLineItem>

namespace Kst { 

class LineItem : public ViewItem, public QGraphicsLineItem
{
  Q_OBJECT
public:
  LineItem(View *parent);
  virtual ~LineItem();

  virtual QGraphicsItem *graphicsItem() { return this; }

private Q_SLOTS:
  void creationPolygonChanged(View::CreationEvent event);
};

class KST_EXPORT CreateLineCommand : public CreateCommand
{
public:
  CreateLineCommand() : CreateCommand(QObject::tr("Create Line")) {}
  CreateLineCommand(View *view) : CreateCommand(view, QObject::tr("Create Line")) {}
  virtual ~CreateLineCommand() {}
  virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
