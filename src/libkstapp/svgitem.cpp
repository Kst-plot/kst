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

#include "svgitem.h"

#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSvgRenderer>

namespace Kst {

SvgItem::SvgItem(View *parent, const QString &file)
  : ViewItem(parent), _svg(new QSvgRenderer(file)) {
  //FIXME need to set the element id??
  setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
}


SvgItem::~SvgItem() {
}


void SvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  // We can do better here.  Cache the svg also.
  if (_svg->isValid()) {
    _svg->render(painter, boundingRect());
  }

  QBrush b = brush();
  setBrush(Qt::NoBrush);
  QGraphicsRectItem::paint(painter, option, widget);
  setBrush(b);
}


void SvgItem::creationPolygonChanged(View::CreationEvent event) {
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

void CreateSvgCommand::createItem() {
  QString file = QFileDialog::getOpenFileName(_view, tr("Kst: Open Svg Image"));
  if (file.isEmpty())
    return;

  _item = new SvgItem(_view, file);
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

#include "svgitem.moc"

// vim: ts=2 sw=2 et
