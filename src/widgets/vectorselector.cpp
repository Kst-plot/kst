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

#include "vectorselector.h"

#include "dialoglauncher.h"
#include "kstdatacollection.h"

namespace Kst {

VectorSelector::VectorSelector(QWidget *parent)
  : QWidget(parent), _allowEmptySelection(true) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newVector->setIcon(QPixmap(":kst_vectornew.png"));
  _editVector->setIcon(QPixmap(":kst_vectoredit.png"));

  _newVector->setFixedSize(size + 8, size + 8);
  _editVector->setFixedSize(size + 8, size + 8);

  fillVectors();

  connect(_newVector, SIGNAL(pressed()), this, SLOT(newVector()));
  connect(_editVector, SIGNAL(pressed()), this, SLOT(editVector()));

  //FIXME need to find a way to call fillVectors when the vectorList changes
}


VectorSelector::~VectorSelector() {
}


KstVectorPtr VectorSelector::selectedVector() const {
  return qVariantValue<KstVector*>(_vector->itemData(_vector->currentIndex()));
}


void VectorSelector::setSelectedVector(KstVectorPtr selectedVector) {
  int i = _vector->findData(qVariantFromValue(selectedVector.data()));
  Q_ASSERT(i != -1);
  _vector->setCurrentIndex(i);
}


void VectorSelector::newVector() {
  DialogLauncher::self()->showVectorDialog();
  fillVectors();
}


void VectorSelector::editVector() {
  DialogLauncher::self()->showVectorDialog(KstObjectPtr(selectedVector()));
}


void VectorSelector::fillVectors() {
  QHash<QString, KstVectorPtr> vectors;

  KST::vectorList.lock().readLock();

  KstVectorList::ConstIterator it = KST::vectorList.begin();
  for (; it != KST::vectorList.end(); ++it) {
    KstVectorPtr vector = (*it);
    if (vector->isScalarList())
      continue;

    vector->readLock();
    vectors.insert(vector->tag().displayString(), vector);
    vector->unlock();
  }

  KST::vectorList.lock().unlock();

  QStringList list = vectors.keys();

  qSort(list);

  if (allowEmptySelection()) {
    list.prepend(tr("<None>"));
    vectors.insert(tr("<None>"), KstVectorPtr());
  }

  KstVectorPtr current = selectedVector();

  _vector->clear();
  foreach (QString string, list) {
    KstVectorPtr v = vectors.value(string);
    _vector->addItem(string, qVariantFromValue(v.data()));
  }

  if (current)
    setSelectedVector(current);
}

}

// vim: ts=2 sw=2 et
