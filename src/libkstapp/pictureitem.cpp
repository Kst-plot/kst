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

#include "pictureitem.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

namespace Kst {

PictureItem::PictureItem(View *parent, const QImage &image)
  : ViewItem(parent), _image(QPixmap::fromImage(image)) {
}


PictureItem::~PictureItem() {
}


void PictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  // We can do better here.  Cache the scaled pixmap also.
  if (!_image.isNull()) {
    const qreal w = pen().widthF();
    painter->drawPixmap(rect().adjusted(w, w, -w, -w), _image, _image.rect());
  }

  QPen p = pen();
  setPen(Qt::NoPen);
  ViewItem::paint(painter, option, widget);
  setPen(p);
}


void CreatePictureCommand::createItem() {
  QString file = QFileDialog::getOpenFileName(_view, tr("Kst: Open Image"));
  if (file.isEmpty())
    return;

  _item = new PictureItem(_view, QImage(file));
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

}

#include "pictureitem.moc"

// vim: ts=2 sw=2 et
