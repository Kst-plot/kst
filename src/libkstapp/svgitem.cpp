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
#include <QGraphicsScene>
#include <QSvgRenderer>

namespace Kst {

SvgItem::SvgItem(View *parent, const QString &file)
  : ViewItem(parent), _svg(new QSvgRenderer(file)) {
  //FIXME need to set the element id??
}


SvgItem::~SvgItem() {
}


void SvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  // We can do better here.  Cache the svg also.
  if (_svg->isValid()) {
    _svg->render(painter, rect());
  }

  QPen p = pen();
  setPen(Qt::NoPen);
  ViewItem::paint(painter, option, widget);
  setPen(p);
}


void CreateSvgCommand::createItem() {
  QString file = QFileDialog::getOpenFileName(_view, tr("Kst: Open Svg Image"));
  if (file.isEmpty())
    return;

  _item = new SvgItem(_view, file);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

}

#include "svgitem.moc"

// vim: ts=2 sw=2 et
