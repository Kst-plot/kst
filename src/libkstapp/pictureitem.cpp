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

PictureItem::PictureItem(View *parent, const QImage& im)
  : ViewItem(parent), _image(QPixmap::fromImage(im)) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
}


PictureItem::~PictureItem() {
}


void PictureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  // We can do better here.  Cache the scaled pixmap also.
  if (!_image.isNull()) {
    const qreal w = pen().widthF();
    painter->drawPixmap(rect().adjusted(w, w, -w, -w), _image, _image.rect());
  }
  QBrush b = brush();
  setBrush(Qt::NoBrush);
  QGraphicsRectItem::paint(painter, option, widget);
  setBrush(b);
}


void PictureItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MousePress));
    setRect(poly.first().x(), poly.first().y(), poly.last().x() - poly.first().x(), poly.last().y() - poly.first().y());
    parentView()->scene()->addItem(this);
    setZValue(1);
    return;
  }

  if (event == View::MouseMove) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseMove));
    setRect(rect().x(), rect().y(), poly.last().x() - rect().x(), poly.last().y() - rect().y());
    return;
  }

  if (event == View::MouseRelease) {
    const QPolygonF poly = mapFromScene(parentView()->creationPolygon(View::MouseRelease));
    setRect(rect().x(), rect().y(), poly.last().x() - rect().x(), poly.last().y() - rect().y());

#ifdef DEBUG_GEOMETRY
    debugGeometry();
#endif

    parentView()->disconnect(this, SLOT(deleteLater())); //Don't delete ourself
    parentView()->disconnect(this, SLOT(creationPolygonChanged(View::CreationEvent)));
    parentView()->setMouseMode(View::Default);
    emit creationComplete();
    return;
  }
}

void CreatePictureCommand::createItem() {
  QString fn = QFileDialog::getOpenFileName(_view, tr("Kst: Open Image"));
  QImage im;
  if (!fn.isEmpty()) {
    im = QImage(fn);
  }
  _item = new PictureItem(_view, im);
  _view->setMouseMode(View::Create);
  _view->setCursor(Qt::CrossCursor);

  //If the mouseMode is changed again before we're done with creation
  //delete ourself.
  connect(_view, SIGNAL(mouseModeChanged()), _item, SLOT(deleteLater()));
  connect(_view, SIGNAL(creationPolygonChanged(View::CreationEvent)),
          _item, SLOT(creationPolygonChanged(View::CreationEvent)));
  connect(_item, SIGNAL(creationComplete()), this, SLOT(creationComplete()));
  //If the item is interrupted while creating itself it will destroy itself
  //need to delete this too in response...
  connect(_item, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
}

}

#include "pictureitem.moc"

// vim: ts=2 sw=2 et
