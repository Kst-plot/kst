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
#include "filltab.h"
#include "stroketab.h"
#include "layouttab.h"
#include "dialogpage.h"
#include "viewgridlayout.h"

#include <QPen>
#include <QBrush>
#include <QDebug>

namespace Kst {

ViewItemDialog::ViewItemDialog(ViewItem *item, QWidget *parent)
    : Dialog(parent), _item(item) {

  setWindowTitle(tr("Edit View Item"));

  _fillTab = new FillTab(this);
  _strokeTab = new StrokeTab(this);
  _layoutTab = new LayoutTab(this);
  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
  connect(_strokeTab, SIGNAL(apply()), this, SLOT(strokeChanged()));
  connect(_layoutTab, SIGNAL(apply()), this, SLOT(layoutChanged()));

  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tr("Appearance"));
  page->addDialogTab(_fillTab);
  page->addDialogTab(_strokeTab);
  page->addDialogTab(_layoutTab);
  addDialogPage(page);

  QList<DialogPage*> dialogPages = _item->dialogPages();
  foreach (DialogPage *dialogPage, dialogPages)
    addDialogPage(dialogPage);

  setupFill();
  setupStroke();
  setupLayout();
}


ViewItemDialog::~ViewItemDialog() {
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


void ViewItemDialog::setupLayout() {
  Q_ASSERT(_item);
  ViewGridLayout *layout = _item->layout();

  if (!layout) {
    _layoutTab->setEnabled(false);
    return;
  }

  _layoutTab->setHorizontalMargin(layout->margin().width());
  _layoutTab->setVerticalMargin(layout->margin().height());
  _layoutTab->setHorizontalSpacing(layout->spacing().width());
  _layoutTab->setVerticalSpacing(layout->spacing().height());
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


void ViewItemDialog::layoutChanged() {
  Q_ASSERT(_item);
  ViewGridLayout *layout = _item->layout();

  if (!layout) {
    _layoutTab->setEnabled(false);
    return;
  }

  layout->setMargin(QSizeF(_layoutTab->horizontalMargin(),
                              _layoutTab->verticalMargin()));
  layout->setSpacing(QSizeF(_layoutTab->horizontalSpacing(),
                               _layoutTab->verticalSpacing()));
  layout->update();
}


}

// vim: ts=2 sw=2 et
