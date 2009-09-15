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
#include "dimensionstab.h"
#include "dialogpage.h"
#include "viewgridlayout.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"

#include "editmultiplewidget.h"

#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QtGlobal>

namespace Kst {

ViewItemDialog::ViewItemDialog(ViewItem *item, QWidget *parent)
    : Dialog(parent), _item(item), _mode(Single) {

  setWindowTitle(tr("Edit View Item"));

  QWidget *extension = extensionWidget();

  QVBoxLayout *extensionLayout = new QVBoxLayout(extension);
  extensionLayout->setContentsMargins(0, -1, 0, -1);

  _editMultipleWidget = new EditMultipleWidget();
  extensionLayout->addWidget(_editMultipleWidget);

  extension->setLayout(extensionLayout);

  _editMultipleBox = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(_editMultipleBox);

  _tagStringLabel = new QLabel(tr("&Name:"), _editMultipleBox);
  _tagString = new QLineEdit(_editMultipleBox);
  connect(_tagString, SIGNAL(textChanged(QString)), this, SLOT(modified()));
  _tagStringLabel->setBuddy(_tagString);

  _editMultipleButton = new QPushButton(tr("Edit Multiple >>"));
  connect(_editMultipleButton, SIGNAL(clicked()), this, SLOT(slotEditMultiple()));

  layout->addWidget(_tagStringLabel);
  layout->addWidget(_tagString);
  layout->addWidget(_editMultipleButton);

  _editMultipleBox->setLayout(layout);

  setSupportsMultipleEdit(false);

  _fillTab = new FillTab(this);
  _strokeTab = new StrokeTab(this);
  _layoutTab = new LayoutTab(this);
  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
  connect(_strokeTab, SIGNAL(apply()), this, SLOT(strokeChanged()));
  connect(_layoutTab, SIGNAL(apply()), this, SLOT(layoutChanged()));

  DialogPageTab *page = new DialogPageTab(this);
  page->setPageTitle(tr("Appearance"));
  page->addDialogTab(_fillTab);
  page->addDialogTab(_strokeTab);
  page->addDialogTab(_layoutTab);
  addDialogPage(page);

  _dimensionsTab = new DimensionsTab(_item, this);
  DialogPage *dimensionsPage = new DialogPage(this);
  dimensionsPage->setPageTitle(tr("Dimensions"));
  dimensionsPage->addDialogTab(_dimensionsTab);
  addDialogPage(dimensionsPage);
  connect(_dimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));

  QList<DialogPage*> dialogPages = _item->dialogPages();
  foreach (DialogPage *dialogPage, dialogPages)
    addDialogPage(dialogPage);

  setupFill();
  setupStroke();
  setupLayout();
  setupDimensions();

  selectDialogPage(page);

  connect(_dimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));

  connect(this, SIGNAL(editMultipleMode()), this, SLOT(setMultipleEdit()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(setSingleEdit()));
}


ViewItemDialog::~ViewItemDialog() {
}


void ViewItemDialog::setSupportsMultipleEdit(bool enabled) {
  _editMultipleBox->setVisible(enabled);
}


void ViewItemDialog::slotEditMultiple() {
  int currentWidth = width();
  int extensionWidth = extensionWidget()->width();
  if (extensionWidth<204) extensionWidth = 204; // FIXME: magic number hack...
  extensionWidget()->setVisible(!extensionWidget()->isVisible());
 _tagString->setEnabled(!extensionWidget()->isVisible());
  if (!extensionWidget()->isVisible()) {
    setMinimumWidth(currentWidth - extensionWidth);
    resize(currentWidth - extensionWidth, height());
    _mode = Single;
    emit editSingleMode();
  } else {
    setMinimumWidth(currentWidth + extensionWidth);
    resize(currentWidth + extensionWidth, height());
    _mode = Multiple;
    emit editMultipleMode();
  }
}


void ViewItemDialog::addMultipleEditOption(QString name, QString descriptionTip, QString shortName) {
  _editMultipleWidget->addObject(name, descriptionTip);
  _multiNameShortName.insert(name, shortName);
}


QList<ViewItem*> ViewItemDialog::selectedMultipleEditObjects() {
  QList<ViewItem*> selectedItems;
  QList<ViewItem*> allItiems = ViewItem::getItems<ViewItem>();
  foreach(QString name, _editMultipleWidget->selectedObjects()) {
    if (_multiNameShortName.contains(name)) {
      QString shortName = _multiNameShortName[name];
      foreach (ViewItem *item, allItiems) {
        if (item->shortName() == shortName) {
          selectedItems.append(item);
        }
      }
    //  selectedItems.append(multiItems[name]);
    }
  }
  return selectedItems;
}


void ViewItemDialog::clearMultipleEditOptions() {
  _editMultipleWidget->clearObjects();
  _multiNameShortName.clear();
}


void ViewItemDialog::setupFill() {
  Q_ASSERT(_item);
  QBrush b = _item->brush();

  _fillTab->enableSingleEditOptions(true);
  _fillTab->setColor(b.color());
  _fillTab->setStyle(b.style());

  if (const QGradient *gradient = b.gradient()) {
    _fillTab->setGradient(*gradient);
  } else {
    _fillTab->setUseGradient(false);
  }
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
  _layoutTab->setHorizontalMargin(_item->layoutMargins().width());
  _layoutTab->setVerticalMargin(_item->layoutMargins().height());
  _layoutTab->setHorizontalSpacing(_item->layoutSpacing().width());
  _layoutTab->setVerticalSpacing(_item->layoutSpacing().height());
}


void ViewItemDialog::setupDimensions() {
  _dimensionsTab->enableSingleEditOptions(true);
  _dimensionsTab->setupDimensions();
}


void ViewItemDialog::fillChanged() {
  Q_ASSERT(_item);

  if (_mode == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      saveFill(item);
    }
  } else {
    saveFill(_item);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void ViewItemDialog::saveFill(ViewItem *item) {
  QBrush b = item->brush();

  QColor color = _fillTab->colorDirty() ? _fillTab->color() : b.color();
  Qt::BrushStyle style = _fillTab->styleDirty() ? _fillTab->style() : b.style();

  if (_fillTab->useGradientDirty()) {
    // Apply / unapply gradient
    if (_fillTab->useGradient()) {
      b = QBrush(_fillTab->gradient());
    } else {
      b.setColor(color);
      b.setStyle(style);
    }
  } else {
    // Leave gradient but make other changes.
    QGradient gradient;
    if (const QGradient *grad = b.gradient()) {
      if (_fillTab->gradientDirty()) {
        gradient = _fillTab->gradient();
      } else {
        gradient = *grad;
      }
      b = QBrush(gradient);
    } else {
      b.setColor(color);
      b.setStyle(style);
    }
  }
  item->setBrush(b);
}


void ViewItemDialog::strokeChanged() {
  Q_ASSERT(_item);
  if (_mode == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      saveStroke(item);
    }
  } else {
    saveStroke(_item);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void ViewItemDialog::saveStroke(ViewItem *item) {
  QPen p = item->pen();
  QBrush b = p.brush();

  Qt::PenStyle style = _strokeTab->styleDirty() ? _strokeTab->style() : p.style();
  qreal width = _strokeTab->widthDirty() ? _strokeTab->width() : p.widthF();
  QColor brushColor = _strokeTab->brushColorDirty() ? _strokeTab->brushColor() : b.color();
  Qt::BrushStyle brushStyle = _strokeTab->brushStyleDirty() ? _strokeTab->brushStyle() : b.style();

  Qt::PenJoinStyle joinStyle = _strokeTab->joinStyleDirty() ? _strokeTab->joinStyle() : p.joinStyle();
  Qt::PenCapStyle capStyle = _strokeTab->capStyleDirty() ? _strokeTab->capStyle() : p.capStyle();


  p.setStyle(style);
  p.setWidthF(width);

  b.setColor(brushColor);
  b.setStyle(brushStyle);

  p.setJoinStyle(joinStyle);
  p.setCapStyle(capStyle);
  p.setBrush(b);
#ifdef Q_WS_WIN32
  if (p.isCosmetic()) {
    p.setWidth(1);
  }
#endif
  item->setItemPen(p);
}


void ViewItemDialog::layoutChanged() {
  Q_ASSERT(_item);
  if (_mode == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      saveLayout(item);
    }
  } else {
    saveLayout(_item);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void ViewItemDialog::saveLayout(ViewItem *item) {
  Q_ASSERT(_item);
  qreal horizontalMargin = _layoutTab->horizontalMarginDirty() ? _layoutTab->horizontalMargin() :item->layoutMargins().width();
  qreal verticalMargin = _layoutTab->verticalMarginDirty() ? _layoutTab->verticalMargin() :item->layoutMargins().height();
  qreal horizontalSpacing = _layoutTab->horizontalSpacingDirty() ? _layoutTab->horizontalSpacing() :item->layoutSpacing().width();
  qreal verticalSpacing = _layoutTab->verticalSpacingDirty() ? _layoutTab->verticalSpacing() :item->layoutSpacing().height();

  item->setLayoutMargins(QSizeF(horizontalMargin, verticalMargin));
  item->setLayoutSpacing(QSizeF(horizontalSpacing, verticalSpacing));
}


void ViewItemDialog::dimensionsChanged() {
  Q_ASSERT(_item);
  if (_mode == Multiple) {
    // FIXME: what makes sense for edit multiple in here?
    // decide, then make it work.  Probably rotation.  Maybe size.
    //foreach(ViewItem* item, selectedMultipleEditObjects()) {
    //  saveDimensions(item);
    //}
  } else {
    saveDimensions(_item);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void ViewItemDialog::saveDimensions(ViewItem *item) {
  Q_ASSERT(item);
  qreal parentWidth;
  qreal parentHeight;
  qreal parentX;
  qreal parentY;

  if (item->parentViewItem()) {
    parentWidth = item->parentViewItem()->width();
    parentHeight = item->parentViewItem()->height();
    parentX = item->parentViewItem()->rect().x();
    parentY = item->parentViewItem()->rect().y();
  } else if (item->parentView()) {
    parentWidth = item->parentView()->width();
    parentHeight = item->parentView()->height();
    parentX = item->parentView()->rect().x();
    parentY = item->parentView()->rect().y();
  } else {
    Q_ASSERT_X(false,"parent test", "item has no parentview item");
    parentWidth = parentHeight = 1.0;
  }

  qreal aspectRatio;
  if (rect().width() > 0) {
    aspectRatio = qreal(item->rect().height()) / qreal(item->rect().width());
  } else {
    aspectRatio = 10000.0;
  }

  qreal relativeWidth = _dimensionsTab->widthDirty() ? _dimensionsTab->width() :item->relativeWidth();
  qreal relativeHeight = _dimensionsTab->heightDirty() ? _dimensionsTab->height() :item->relativeHeight();
  bool fixedAspect = _dimensionsTab->fixedAspectDirty() ? _dimensionsTab->fixedAspect() :item->lockAspectRatio();

  qreal width = relativeWidth * parentWidth;
  qreal height;
  if (fixedAspect) {
    height = width * aspectRatio;
    item->setLockAspectRatio(true);
  } else {
    height = relativeHeight * parentHeight;
    item->setLockAspectRatio(false);
  }

  item->setPos(parentX + _dimensionsTab->x()*parentWidth, parentY + _dimensionsTab->y()*parentHeight);
  item->setViewRect(-width/2, -height/2, width, height);

  qreal rotation = _dimensionsTab->rotationDirty() ? _dimensionsTab->rotation() :item->rotationAngle();

  QTransform transform;
  transform.rotate(rotation);

  item->setTransform(transform);
  item->updateRelativeSize();
}


void ViewItemDialog::setSingleEdit() {
  setupFill();
  setupStroke();
  setupLayout();
  setupDimensions();
  _mode = Single;
  _editMultipleButton->setText(tr("Edit Multiple >>"));
}


void ViewItemDialog::setMultipleEdit() {
  _mode = Multiple;
  _dimensionsTab->clearTabValues();
  _dimensionsTab->enableSingleEditOptions(false);
  _fillTab->clearTabValues();
  _strokeTab->clearTabValues();
  _layoutTab->clearTabValues();
  _editMultipleButton->setText(tr("<< Edit One"));
  setAlwaysAllowApply(true);
}

}

// vim: ts=2 sw=2 et
