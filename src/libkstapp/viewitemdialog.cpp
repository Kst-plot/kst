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

#include "viewitem.h"

#include "dialogpage.h"

#include "filltab.h"
#include "stroketab.h"

#include <QPen>
#include <QBrush>
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
    : Dialog(parent) {

  setWindowTitle(tr("Edit View Item"));

  setModal(true);

  _fillTab = new FillTab(this);
  _strokeTab = new StrokeTab(this);
  connect(_fillTab, SIGNAL(changed()), this, SLOT(fillChanged()));
  connect(_strokeTab, SIGNAL(changed()), this, SLOT(strokeChanged()));

  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tr("Appearance"));
  page->addDialogTab(_fillTab);
  page->addDialogTab(_strokeTab);
  addDialogPage(page);
}


ViewItemDialog::~ViewItemDialog() {
}

void ViewItemDialog::show(ViewItem *item) {
  _item = item;

  setupFill();
  setupStroke();

  Dialog::show();
  Dialog::raise();
  Dialog::activateWindow();
}

void ViewItemDialog::setupFill() {
  Q_ASSERT(_item);
  QBrush b = _item->brush();

  _fillTab->setColor(b.color());
  _fillTab->setStyle(b.style());

  //FIXME gradient editor is disabled for now as it is not ready
#if 0
  if (const QGradient *gradient = b.gradient()) {
    _fillTab->setGradient(*gradient);
  }
#endif
}


void ViewItemDialog::setupStroke() {
  Q_ASSERT(_item);
  QPen p = _item->pen();
  QBrush b = p.brush();

  _strokeTab->setStyle(p.style());
  _strokeTab->setWidth(p.widthF());

  _strokeTab->setBrushColor(b.color());
  _strokeTab->setBrushStyle(b.style());

  _strokeTab->setJoinStyle(p.joinStyle());
  _strokeTab->setCapStyle(p.capStyle());
}


void ViewItemDialog::fillChanged() {
  QBrush b = _item->brush();

  b.setColor(_fillTab->color());
  b.setStyle(_fillTab->style());

  //FIXME gradient editor is disabled for now as it is not ready
#if 0
  QGradient gradient = _fillTab->gradient();
  if (gradient.type() != QGradient::NoGradient)
    b = QBrush(gradient);
#endif

  Q_ASSERT(_item);
  _item->setBrush(b);
}


void ViewItemDialog::strokeChanged() {
  Q_ASSERT(_item);
  QPen p = _item->pen();
  QBrush b = p.brush();

  p.setStyle(_strokeTab->style());
  p.setWidthF(_strokeTab->width());

  b.setColor(_strokeTab->brushColor());
  b.setStyle(_strokeTab->brushStyle());

  p.setJoinStyle(_strokeTab->joinStyle());
  p.setCapStyle(_strokeTab->capStyle());
  p.setBrush(b);

  _item->setPen(p);
}


void ViewItemDialog::setVisible(bool visible) {
  if (visible && !_item)
    return; //nothing to show...

  Dialog::setVisible(visible);
}


}

// vim: ts=2 sw=2 et
