/***************************************************************************
                   comboboxselection.cpp
                             -------------------
    begin                :  2005
    copyright            : (C) 2005 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "comboboxselection.h"

#include <q3listbox.h>
#include <QRegExp>
#include <QLineEdit>
#include <QPushButton>
#include <QStringList>

ComboBoxSelection::ComboBoxSelection(QWidget *parent)
    : QDialog(parent) {

  connect(OK, SIGNAL(clicked()), this, SLOT(ok()));

  connect(Cancel, SIGNAL(clicked()), this, SLOT(close()));

  connect(_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(sort()));

  connect(_listBox, SIGNAL(selectionChanged()), this, SLOT(changed()));

  OK->setEnabled(false);
}


ComboBoxSelection::~ComboBoxSelection() {}


void ComboBoxSelection::addString(const QString &str) {
  _strs.append(str);
}


void ComboBoxSelection::reset() {
  _listBox->clear();
}


void ComboBoxSelection::changed() {
  if (_listBox->selectedItem()) {
    OK->setEnabled(true);
  } else {
    OK->setEnabled(false);
  }
}


void ComboBoxSelection::sort() {
  QString search;
  int i;

  search = _lineEdit->text();
  if (search.isEmpty()) {
    search = "*";
  } else if (search.contains("*") == 0) {
    search.prepend("*");
    search.append("*");
  }
  QRegExp regexp(search, Qt::CaseInsensitive, QRegExp::Wildcard);

  OK->setEnabled(false);
  _listBox->clear();
  for (i=0; i<(int)_strs.count(); i++) {
    if (regexp.exactMatch(_strs[i])) {
      _listBox->insertItem(_strs[i]);
    }
  }
  _listBox->sort();
}


void ComboBoxSelection::ok() {
  if (_listBox->selectedItem() != 0L) {
    _selected = _listBox->selectedItem()->text();
  }
  accept();
}

#include "comboboxselection.moc"

// vim: ts=2 sw=2 et
