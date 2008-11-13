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
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

VectorSelector::VectorSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _allowEmptySelection(false), _store(store) {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newVector->setIcon(QPixmap(":kst_vectornew.png"));
  _editVector->setIcon(QPixmap(":kst_vectoredit.png"));

  _newVector->setFixedSize(size + 8, size + 8);
  _editVector->setFixedSize(size + 8, size + 8);

  fillVectors();

  connect(_newVector, SIGNAL(pressed()), this, SLOT(newVector()));
  connect(_editVector, SIGNAL(pressed()), this, SLOT(editVector()));
  connect(_vector, SIGNAL(activated(int)), this, SLOT(emitSelectionChanged()));
  connect(_vector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDescriptionTip()));
}


VectorSelector::~VectorSelector() {
}


void VectorSelector::setObjectStore(ObjectStore *store) {
  _store = store;
  fillVectors();
}

void VectorSelector::updateDescriptionTip() {
  if (selectedVector()) {
    setToolTip(selectedVector()->descriptionTip());
  } else {
    setToolTip(QString());
  }
}


void VectorSelector::emitSelectionChanged() {
  emit selectionChanged(_vector->currentText());
}


VectorPtr VectorSelector::selectedVector() const {
  return qVariantValue<Vector*>(_vector->itemData(_vector->currentIndex()));
}


bool VectorSelector::selectedVectorDirty() const {
  return _vector->currentIndex() != -1;
}


void VectorSelector::setSelectedVector(VectorPtr selectedVector) {
  // "findData can't work here" says the trolls... so we do it 'manually'.
  //int i = _vector->findData(qVariantFromValue(selectedVector.data()));
  int i=-1,j;
  for (j=0; j<_vector->count() ; j++) {
    if (selectedVector.data() == (qVariantValue<Vector*>(_vector->itemData(j)))) {
      i=j;
      break;
    }
  }
  Q_ASSERT(i != -1);
  _vector->setCurrentIndex(i);
}


void VectorSelector::clearSelection() {
  _vector->setCurrentIndex(-1);
}


bool VectorSelector::allowEmptySelection() const {
  return _allowEmptySelection;
}


void VectorSelector::setAllowEmptySelection(bool allowEmptySelection) {
  _allowEmptySelection = allowEmptySelection;

  int i = _vector->findText(tr("<None>"));
  if (i != -1)
    _vector->removeItem(i);

  if (_allowEmptySelection) {
    _vector->insertItem(0, tr("<None>"), qVariantFromValue(0));
    _vector->setCurrentIndex(0);
  }
}


void VectorSelector::newVector() {
  QString newName;
  DialogLauncher::self()->showVectorDialog(newName);
  fillVectors();
  VectorPtr vector = kst_cast<Vector>(_store->retrieveObject(newName));

  if (vector) {
    setSelectedVector(vector);
    emitSelectionChanged();
    emit contentChanged();
  }
}


void VectorSelector::editVector() {
  if (selectedVector()->provider()) {
    DialogLauncher::self()->showObjectDialog(selectedVector()->provider());
  } else {
    QString vectorname;
    DialogLauncher::self()->showVectorDialog(vectorname, ObjectPtr(selectedVector()));
  }
  fillVectors(); // we might have just changed the name, so refill the combo.

  emit contentChanged();
}


void VectorSelector::fillVectors() {
  if (!_store) {
    return;
  }

  QHash<QString, VectorPtr> vectors;

  VectorList vectorList = _store->getObjects<Vector>();

  VectorList::ConstIterator it = vectorList.begin();
  for (; it != vectorList.end(); ++it) {
    VectorPtr vector = (*it);
    if (vector->isScalarList())
      continue;

    vector->readLock();
    vectors.insert(vector->Name(), vector);
    vector->unlock();
  }

  QStringList list = vectors.keys();

  qSort(list);

  VectorPtr current = selectedVector();

  _vector->clear();
  foreach (QString string, list) {
    VectorPtr v = vectors.value(string);
    _vector->addItem(string, qVariantFromValue(v.data()));
  }

  if (_allowEmptySelection) //reset the <None>
    setAllowEmptySelection(true);

  if (current)
    setSelectedVector(current);

  _editVector->setEnabled(_vector->count() > 0);
}

}

// vim: ts=2 sw=2 et
