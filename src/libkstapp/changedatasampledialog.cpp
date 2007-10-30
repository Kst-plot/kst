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

#include "changedatasampledialog.h"

#include "datacollection.h"
#include "datavector.h"

namespace Kst {

ChangeDataSampleDialog::ChangeDataSampleDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  connect(_clear, SIGNAL(clicked()), _curveList, SLOT(clearSelection()));
  connect(_selectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
}


ChangeDataSampleDialog::~ChangeDataSampleDialog() {
}


void ChangeDataSampleDialog::exec() {
  updateCurveListDialog();
  QDialog::exec();
}


void ChangeDataSampleDialog::updateCurveListDialog() {
  QStringList selectedCurves;
  for (int i_vector = 0; i_vector < _curveList->count(); i_vector++) {
    if (_curveList->item(i_vector)->isSelected()) {
      selectedCurves.append(_curveList->item(i_vector)->text());
    }
  }
  _curveList->clear();

//  DataVectorList rvl = ObjectSubList<Vector,DataVector>(vectorList);
  DataVectorList rvl; // FIXME

  _curveList->blockSignals(true);
  int inserted = 0;
  for (DataVectorList::ConstIterator i = rvl.begin(); i != rvl.end(); ++i) {
    DataVectorPtr vector = *i;
    vector->readLock();
    QString tag = vector->tag().displayString();
    _curveList->addItem(tag);
    if (selectedCurves.contains(tag)) {
      _curveList->item(inserted)->setSelected(true);
    }
    ++inserted;
    vector->unlock();
  }
  _curveList->blockSignals(false);
}


void ChangeDataSampleDialog::selectAll() {
  _curveList->selectAll();
}


}

// vim: ts=2 sw=2 et
