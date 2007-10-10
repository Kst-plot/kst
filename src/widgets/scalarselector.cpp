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

#include "scalarselector.h"

#include "dialoglauncher.h"
#include "datacollection.h"

namespace Kst {

ScalarSelector::ScalarSelector(QWidget *parent)
  : QWidget(parent) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newScalar->setIcon(QPixmap(":kst_scalarnew.png"));
  _editScalar->setIcon(QPixmap(":kst_scalaredit.png"));

  _newScalar->setFixedSize(size + 8, size + 8);
  _editScalar->setFixedSize(size + 8, size + 8);
  _selectScalar->setFixedSize(size + 8, size + 8);

  fillScalars();

  connect(_newScalar, SIGNAL(pressed()), this, SLOT(newScalar()));
  connect(_editScalar, SIGNAL(pressed()), this, SLOT(editScalar()));

}


ScalarSelector::~ScalarSelector() {
}


ScalarPtr ScalarSelector::selectedScalar() const {
  return qVariantValue<Scalar*>(_scalar->itemData(_scalar->currentIndex()));
}


void ScalarSelector::setSelectedScalar(ScalarPtr selectedScalar) {
  Q_UNUSED(selectedScalar);
}


void ScalarSelector::newScalar() {
  DialogLauncher::self()->showScalarDialog();
  fillScalars();
}


void ScalarSelector::editScalar() {
  DialogLauncher::self()->showScalarDialog(ObjectPtr(selectedScalar()));
}


void ScalarSelector::fillScalars() {
  QHash<QString, ScalarPtr> scalars;

  scalarList.lock().readLock();

  ScalarList::ConstIterator it = scalarList.begin();
  for (; it != scalarList.end(); ++it) {
    ScalarPtr scalar = (*it);

    scalar->readLock();
    scalars.insert(scalar->tag().displayString(), scalar);
    scalar->unlock();
  }

  scalarList.lock().unlock();

  QStringList list = scalars.keys();

  qSort(list);

  ScalarPtr current = selectedScalar();

  _scalar->clear();
  foreach (QString string, list) {
    ScalarPtr v = scalars.value(string);
    _scalar->addItem(string, qVariantFromValue(v.data()));
  }

  if (current)
    setSelectedScalar(current);
}

}

// vim: ts=2 sw=2 et
