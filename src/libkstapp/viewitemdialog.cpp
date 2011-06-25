/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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

#include "dialogdefaults.h"

#include "editmultiplewidget.h"

#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QtGlobal>

namespace Kst {

ViewItemDialog::ViewItemDialog(ViewItem *item, QWidget *parent)
    : Dialog(parent), _item(item), _mode(Single) {

  setWindowTitle(tr("Edit View Item"));

  // semi-hack: set the width of the list widget to 15 characters, which is enough
  // to say "X-Axis Markers" in english.  This is better than setting it to a fixed
  // number of pixels, as it scales with font size or screen resolution, but
  // it won't necessairly survive translations, or someone adding a option like
  // "Do something super important", which has more than 15 characters.
  // We have to do it here, before the layout is set, and we don't yet know how
  // what is going into the listWidget.
  _listWidget->setMinimumWidth(_listWidget->fontMetrics().averageCharWidth()*15);

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

  if (_item->hasBrush()) {
    _fillTab = new FillTab(this);
    connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
  }
  if (_item->hasStroke()) {
    _strokeTab = new StrokeTab(this);
    connect(_strokeTab, SIGNAL(apply()), this, SLOT(strokeChanged()));
  }
  _layoutTab = new LayoutTab(this);
  connect(_layoutTab, SIGNAL(apply()), this, SLOT(layoutChanged()));

  DialogPageTab *page = new DialogPageTab(this);
  page->setPageTitle(tr("Appearance"));
  if (_item->hasBrush()) {
    page->addDialogTab(_fillTab);
  }

  if (_item->hasStroke()) {
    page->addDialogTab(_strokeTab);
  }
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
  _saveAsDefault->show();

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
  foreach(const QString &name, _editMultipleWidget->selectedObjects()) {
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
  if (_item->hasBrush()) {
    QBrush b = _item->brush();
    _fillTab->enableSingleEditOptions(true);
    _fillTab->initialize(&b);
  }
}

void ViewItemDialog::setupStroke() {
  Q_ASSERT(_item);
  if (_item->hasStroke()) {
    QPen p = _item->pen();

    _strokeTab->initialize(&p);
  }
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

  if (_item->hasBrush()) {
    if (_mode == Multiple) {
      foreach(ViewItem* item, selectedMultipleEditObjects()) {
        saveFill(item);
      }
    } else {
      saveFill(_item);
      if (_saveAsDefault->isChecked()) {
        saveDialogDefaultsBrush(_item->defaultsGroupName(), _item->brush());
      }
    }
    kstApp->mainWindow()->document()->setChanged(true);
  }
}

void ViewItemDialog::saveFill(ViewItem *item) {
  if (_item->hasBrush()) {
    QBrush b = _fillTab->brush(item->brush());
    item->setBrush(b);
  }
}


void ViewItemDialog::strokeChanged() {
  Q_ASSERT(_item);
  if (_mode == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      saveStroke(item);
    }
  } else {
    saveStroke(_item);
    if (_saveAsDefault->isChecked()) {
      saveDialogDefaultsPen(_item->defaultsGroupName(), _item->pen());
    }
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void ViewItemDialog::saveStroke(ViewItem *item) {
  if (_item->hasStroke()) {
    item->setItemPen(_strokeTab->pen(item->pen()));
  }
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
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      saveDimensions(item);
    }
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
  } else if (item->view()) {
    parentWidth = item->view()->width();
    parentHeight = item->view()->height();
    parentX = item->view()->rect().x();
    parentY = item->view()->rect().y();
  } else {
    Q_ASSERT_X(false,"parent test", "item has no parentview item");
    parentWidth = parentHeight = 1.0;
    parentX = parentY = 0.0;
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


  if (_mode == Multiple) {
    item->setPos(parentX + item->relativeCenter().x()*parentWidth,
                 parentY + item->relativeCenter().y()*parentHeight);
  } else {
    item->setPos(parentX + _dimensionsTab->x()*parentWidth, parentY + _dimensionsTab->y()*parentHeight);
  }
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
  if (_item->hasBrush()) {
    _fillTab->clearTabValues();
  }
  if (_item->hasStroke()) {
    _strokeTab->clearTabValues();
  }
  _layoutTab->clearTabValues();
  _editMultipleButton->setText(tr("<< Edit One"));
  setAlwaysAllowApply(true);
}

}

// vim: ts=2 sw=2 et
