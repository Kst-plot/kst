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

  connect(_listWidget, SIGNAL(itemActivated(QListWidgetItem *)),
          this, SLOT(selectPageForItem(QListWidgetItem *)));

  connect(_buttonBox, SIGNAL(clicked(QAbstractButton *)),
          this, SLOT(buttonClicked(QAbstractButton *)));

  setMaximumSize(QSize(1024, 768));
}


Dialog::~Dialog() {
}


void Dialog::addDialogPage(DialogPage *page) {
  connect(page, SIGNAL(modified(bool)), this, SLOT(modified(bool)));
  connect(this, SIGNAL(apply()), page, SIGNAL(apply()));
  connect(this, SIGNAL(restoreDefaults()), page, SIGNAL(restoreDefaults()));
  QListWidgetItem *item = new QListWidgetItem(page->pageIcon(), page->pageTitle(), _listWidget);
  _listWidget->addItem(item);
  _stackedWidget->addWidget(page);
  _itemHash.insert(item, page);
}


void Dialog::accept() {
  modified(false);
  QDialog::accept();
}


void Dialog::reject() {
  modified(false);
  QDialog::reject();
}


void Dialog::setVisible(bool visible) {

  _listWidget->setVisible(_itemHash.count() > 2);

  QDialog::setVisible(visible);
}


void Dialog::selectPageForItem(QListWidgetItem *item) {
  if (_itemHash.contains(item))
    _stackedWidget->setCurrentWidget(_itemHash.value(item));
}


void Dialog::buttonClicked(QAbstractButton *button) {
  QDialogButtonBox::StandardButton std = _buttonBox->standardButton(button);
  switch(std) {
  case QDialogButtonBox::Apply:
    emit apply();
    break;
  case QDialogButtonBox::RestoreDefaults:
    emit restoreDefaults();
    break;
  default:
    break;
  }
}


void Dialog::modified(bool isModified) {
  Q_UNUSED(isModified)
/*FIXME
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(isModified);
  _buttonBox->button(QDialogButtonBox::RestoreDefaults)->setEnabled(isModified);*/
}

}

// vim: ts=2 sw=2 et
