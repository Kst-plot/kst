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

#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "viewitem.h"
#include <QGraphicsRectItem>

namespace Kst { 

class PictureItem : public ViewItem, public QGraphicsRectItem
{
  Q_OBJECT
  public:
    PictureItem(View *parent, const QImage& im);
    ~PictureItem();

    QGraphicsItem *graphicsItem() { return this; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

  private Q_SLOTS:
    void creationPolygonChanged(View::CreationEvent event);

  private:
    QPixmap _image;
};


class KST_EXPORT CreatePictureCommand : public CreateCommand
{
  public:
    CreatePictureCommand() : CreateCommand(QObject::tr("Create Picture")) {}
    CreatePictureCommand(View *view) : CreateCommand(view, QObject::tr("Create Picture")) {}
    ~CreatePictureCommand() {}
    void createItem();
};

}

#endif

// vim: ts=2 sw=2 et
