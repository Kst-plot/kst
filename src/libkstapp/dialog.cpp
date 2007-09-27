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

#include "dialog.h"

#include "dialogpage.h"

#include <QHash>
#include <QPushButton>

namespace Kst {

Dialog::Dialog(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  extensionWidget()->hide();

  connect(_listWidget, SIGNAL(itemClicked(QListWidgetItem *)),
          this, SLOT(selectPageForItem(QListWidgetItem *)));

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton *)),
          this, SLOT(buttonClicked(QAbstractButton *)));

  setMaximumSize(QSize(1024, 768));
  resize(minimumSizeHint());
}


Dialog::~Dialog() {
}


void Dialog::addDialogPage(DialogPage *page) {
  connect(page, SIGNAL(modified()), this, SLOT(modified()));
  connect(this, SIGNAL(apply()), page, SIGNAL(apply()));
  QListWidgetItem *item = new QListWidgetItem(page->pageIcon(), page->pageTitle(), _listWidget);
  _listWidget->addItem(item);
  _stackedWidget->addWidget(page);
  _itemHash.insert(item, page);
}


void Dialog::setVisible(bool visible) {

  _listWidget->setVisible(_itemHash.count() > 1);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

  QDialog::setVisible(visible);
}


QWidget *Dialog::leftCustomWidget() const {
  return _leftCustom;
}


QWidget *Dialog::rightCustomWidget() const {
  return _rightCustom;
}


QWidget *Dialog::topCustomWidget() const {
  return _topCustom;
}


QWidget *Dialog::bottomCustomWidget() const {
  return _bottomCustom;
}


QWidget *Dialog::extensionWidget() const {
  return _extensionWidget;
}


QDialogButtonBox *Dialog::buttonBox() const {
 return _buttonBox;
}


void Dialog::selectPageForItem(QListWidgetItem *item) {
  if (_itemHash.contains(item))
    _stackedWidget->setCurrentWidget(_itemHash.value(item));
}


void Dialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Ok:
    emit ok();
    break;
  case QDialogButtonBox::Apply:
    _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    emit apply();
    break;
  case QDialogButtonBox::Cancel:
    emit cancel();
    break;
  default:
    break;
  }
}


void Dialog::modified() {
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

}

// vim: ts=2 sw=2 et
