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

#ifndef LAYOUTBOXITEM_H
#define LAYOUTBOXITEM_H

#include "viewitem.h"

namespace Kst {

class LayoutBoxItem : public ViewItem
{
  Q_OBJECT
  public:
    LayoutBoxItem(View *parent);
    virtual ~LayoutBoxItem();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

  public Q_SLOTS:
    void setEnabled(bool enabled);

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

}

#endif

// vim: ts=2 sw=2 et
