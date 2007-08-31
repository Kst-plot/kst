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

#include "viewitemdialog.h"

#include "fillandstroke.h"

#include <QPen>
#include <QBrush>
#include <QAbstractGraphicsShapeItem>
#include <QDebug>

namespace Kst {

static ViewItemDialog *_self = 0;
void ViewItemDialog::cleanup() {
  delete _self;
  _self = 0;
}


ViewItemDialog *ViewItemDialog::self() {
  if (!_self) {
    _self = new ViewItemDialog;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


ViewItemDialog::ViewItemDialog(QWidget *parent)
    : QDialog(parent) {

  setModal(false);
  setWindowTitle(tr("Edit View Item"));

  _fillAndStroke = new FillAndStroke(this);
  connect(_fillAndStroke, SIGNAL(fillChanged()), this, SLOT(fillChanged()));
  connect(_fillAndStroke, SIGNAL(strokeChanged()), this, SLOT(strokeChanged()));

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addWidget(_fillAndStroke);
  setLayout(layout);
}


ViewItemDialog::~ViewItemDialog() {
}

void ViewItemDialog::show(QList<QGraphicsItem*> items) {
  Q_ASSERT(!items.isEmpty());

  _items = items;

  setupFill();
  setupStroke();

  QDialog::show();
  QDialog::raise();
  QDialog::activateWindow();
}

void ViewItemDialog::setupFill() {
  QAbstractGraphicsShapeItem *first = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_items.first());
  Q_ASSERT(first);

  QBrush b = first->brush();

  _fillAndStroke->setFillColor(b.color());
  _fillAndStroke->setFillStyle(b.style());

  //FIXME gradient editor is disabled for now as it is not ready
#if 0
  if (const QGradient *gradient = b.gradient()) {
    _fillAndStroke->setFillGradient(*gradient);
  }
#endif
}


void ViewItemDialog::setupStroke() {
  QAbstractGraphicsShapeItem *first = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_items.first());
  Q_ASSERT(first);

  QPen p = first->pen();
  QBrush b = p.brush();

  _fillAndStroke->setStrokeStyle(p.style());
  _fillAndStroke->setStrokeWidth(p.widthF());

  _fillAndStroke->setBrushColor(b.color());
  _fillAndStroke->setBrushStyle(b.style());

  _fillAndStroke->setJoinStyle(p.joinStyle());
  _fillAndStroke->setCapStyle(p.capStyle());
}


void ViewItemDialog::fillChanged() {
  QAbstractGraphicsShapeItem *first = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_items.first());
  Q_ASSERT(first);

  QBrush b = first->brush();

  b.setColor(_fillAndStroke->fillColor());
  b.setStyle(_fillAndStroke->fillStyle());

  //FIXME gradient editor is disabled for now as it is not ready
#if 0
  QGradient gradient = _fillAndStroke->fillGradient();
  if (gradient.type() != QGradient::NoGradient)
    b = QBrush(gradient);
#endif

  foreach(QGraphicsItem *item, _items) {
    QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
    shape->setBrush(b);
  }
}


void ViewItemDialog::strokeChanged() {
  QAbstractGraphicsShapeItem *first = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(_items.first());
  Q_ASSERT(first);

  QPen p = first->pen();
  QBrush b = p.brush();

  p.setStyle(_fillAndStroke->strokeStyle());
  p.setWidthF(_fillAndStroke->strokeWidth());

  b.setColor(_fillAndStroke->brushColor());
  b.setStyle(_fillAndStroke->brushStyle());

  p.setJoinStyle(_fillAndStroke->joinStyle());
  p.setCapStyle(_fillAndStroke->capStyle());
  p.setBrush(b);

  foreach(QGraphicsItem *item, _items) {
    QAbstractGraphicsShapeItem *shape = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(item);
    shape->setPen(p);
  }
}


void ViewItemDialog::setVisible(bool visible) {
  if (visible && _items.isEmpty())
    return; //nothing to show...

  QDialog::setVisible(visible);
}


}

// vim: ts=2 sw=2 et
