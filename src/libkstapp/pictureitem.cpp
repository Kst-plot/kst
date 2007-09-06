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
  setName("PictureItem");
  setLockAspectRatio(true);
}


PictureItem::~PictureItem() {
}


void PictureItem::paint(QPainter *painter) {
  // We can do better here.  Cache the scaled pixmap also.
  if (!_image.isNull() && rect().isValid()) {
    const qreal w = pen().widthF();
    painter->drawPixmap(rect().adjusted(w, w, -w, -w), _image, _image.rect());
  }
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

// vim: ts=2 sw=2 et
