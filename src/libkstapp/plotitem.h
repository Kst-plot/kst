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

#ifndef PLOTITEM_H
#define PLOTITEM_H

#include "viewitem.h"
#include <QGraphicsRectItem>

namespace Kst {

class PlotItem : public ViewItem, public QGraphicsRectItem
{
  Q_OBJECT
public:
  PlotItem(View *parent);
  virtual ~PlotItem();

  virtual QGraphicsItem *graphicsItem() { return this; }

private Q_SLOTS:
  void creationPolygonChanged(View::CreationEvent event);
};

class KST_EXPORT CreatePlotCommand : public CreateCommand
{
public:
  CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
  CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
  virtual ~CreatePlotCommand() {}
  virtual void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
