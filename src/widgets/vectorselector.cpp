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

#include "vectorselector.h"

#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"
#include "dialogdefaults.h"
#include "curve.h"

#include <QLabel>

namespace Kst {

VectorSelector::VectorSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _allowEmptySelection(false), _isX(false), _store(store)  {

  setupUi(this);

  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);

  _newVector->setIcon(QPixmap(":kst_vectornew.png"));
  _editVector->setIcon(QPixmap(":kst_vectoredit.png"));

  _newVector->setFixedSize(size + 8, size + 8);
  _editVector->setFixedSize(size + 8, size + 8);

  fillVectors();

  _vector->setProperty("si","data vector");

  connect(_newVector, SIGNAL(pressed()), this, SLOT(newVector()));
  connect(_editVector, SIGNAL(pressed()), this, SLOT(editVector()));
  connect(_vector, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSelectionChanged()));
  connect(_vector, SIGNAL(editTextChanged(QString)), this, SLOT(emitSelectionChanged()));
  connect(_vector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDescriptionTip()));
  connect(_vector, SIGNAL(editTextChanged(QString)), this, SLOT(updateDescriptionTip()));
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
  if(_vector->currentIndex()!=-1&&_vector->currentIndex()<_vector->count()&&
         _vector->currentText()!=_vector->itemText(_vector->currentIndex())){
      for(int i=0;i<_vector->count();i++) {
          if(_vector->itemText(i).contains(_vector->currentText())) {
              _vector->setCurrentIndex(i);
              break;
          }
      }
  }
  if (_allowEmptySelection && (_vector->count()>0)) {
      _editVector->setDisabled(_vector->currentIndex()==0);
  }
  emit selectionChanged(_vector->currentText());
}


VectorPtr VectorSelector::selectedVector() const {
  QString shortName;
  QRegExp rx("(\\(|^)([A-Z]\\d+)(\\)$|$)");
  rx.indexIn(_vector->currentText());
  shortName = rx.cap(2);

  for(int i=0;i<_vector->count();i++) {
      if(_vector->itemText(i).contains(shortName)) {
          return qVariantValue<Vector*>(_vector->itemData(i));
      }
  }
  return qVariantValue<Vector*>(_vector->itemData(_vector->currentIndex()));
}


bool VectorSelector::vectorSelected() const {
  return (_vector->currentIndex()>=0);
}

bool VectorSelector::selectedVectorDirty() const {
  return _vector->currentIndex() != -1;
}


void VectorSelector::setSelectedVector(VectorPtr selectedVector) {
  if (!selectedVector) {
    return;
  }
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
    _editVector->setEnabled(false);
  }
}


void VectorSelector::newVector() {
  QString newName;
  if (_isX) {
    newName = _dialogDefaults->value("curve/xvectorfield","INDEX").toString();
  }
  DialogLauncher::self()->showVectorDialog(newName, 0, true);
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
    DialogLauncher::self()->showVectorDialog(vectorname, ObjectPtr(selectedVector()), true);
  }
  fillVectors(); // we might have just changed the name, so refill the combo.

  emit contentChanged();
}


void VectorSelector::setToLastX(QString field) {
  if (!_store) {
    return;
  }
  int match = -1;
  VectorList vectors = _store->getObjects<Vector>();
  int size = vectors.size();
  for (int i = 0; i < size; ++i) {
    if (vectors.at(i)->descriptiveName() == field) {
      match = i;
    }
  }

  if (match >-1) {
    setSelectedVector(vectors.at(match));
  } else {
    CurveList objects = _store->getObjects<Curve>();
    if (objects.count()>0) {
      setSelectedVector(objects.at(objects.count()-1)->xVector());
    }
  }
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
    vectors.insert(vector->sizeLimitedName(_vector), vector);
    vector->unlock();
  }

  QStringList list = vectors.keys();

  qSort(list);

  int current_index = _vector->currentIndex();
  VectorPtr current = selectedVector();

  _vector->clear();
  foreach (const QString &string, list) {
    VectorPtr v = vectors.value(string);
    _vector->addItem(string, qVariantFromValue(v.data()));
  }
  _editVector->setEnabled(_vector->count() > 0);

  if (_allowEmptySelection) //reset the <None>
    setAllowEmptySelection(true);

  if (current_index==-1) {
    _vector->setCurrentIndex(current_index);
  } else if (current) {
    setSelectedVector(current);
  }
}


bool VectorSelector::event(QEvent * event) {
  if ((event->type() == QEvent::WindowActivate) || (event->type() == QEvent::Resize)) {
    fillVectors();
  }
  return QWidget::event(event);
}

}

// vim: ts=2 sw=2 et
