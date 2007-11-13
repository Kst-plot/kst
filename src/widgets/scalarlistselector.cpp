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

#include "scalarlistselector.h"

namespace Kst {

ScalarListSelector::ScalarListSelector(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  connect(_search, SIGNAL(textChanged(QString)), this, SLOT(filter(const QString&)));
}


ScalarListSelector::~ScalarListSelector() {
}


QString ScalarListSelector::selectedScalar() const {
  return _scalars->currentItem()->text();
}


void ScalarListSelector::fillScalars(QStringList &scalars) {
  _scalars->addItems(scalars);
}


void ScalarListSelector::clear() {
  _scalars->clear();
}


void ScalarListSelector::filter(const QString& filter) {
  for (int i = 0; i < _scalars->count(); i++) {
    QListWidgetItem *item = _scalars->item(i);
    item->setHidden(!item->text().contains(filter, Qt::CaseInsensitive));
  }
}


}

// vim: ts=2 sw=2 et
