/***************************************************************************
                   matrixselector.cpp
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
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

#include "matrixselector.h"

#include <QTimer>
#include <QListWidget>
#include <QAbstractItemView>

#include <kiconloader.h>

#include <kstdatacollection.h>
#include <kstcombobox.h>
#include <kstmatrix.h>
#include <kstdataobject.h>
#include <dialoglauncher.h>

MatrixSelector::MatrixSelector(QWidget *parent)
    : QWidget(parent), _provideNoneMatrix(false) {
  setupUi(this);

  connect(this, SIGNAL(selectionChanged(const QString&)), this, SLOT(selectionWatcher(const QString&)));

  connect(_editMatrix, SIGNAL(clicked()), this, SLOT(editMatrix()));

  connect(_newMatrix, SIGNAL(clicked()), this, SLOT(createNewMatrix()));
}


MatrixSelector::~MatrixSelector() {}


void MatrixSelector::allowNewMatrices(bool allowed) {
  _newMatrix->setEnabled(allowed);
}


QString MatrixSelector::selectedMatrix() {
  if (_provideNoneMatrix && _matrix->currentIndex() == 0) {
    return QString::null;
  }
  return _matrix->currentText();
}


void MatrixSelector::update() {
  if (_matrix->view()->isVisible()) {
    QTimer::singleShot(250, this, SLOT(update()));
    return;
  }

  blockSignals(true);
  QString prev = _matrix->currentText();
  bool found = false;
  _matrix->clear();
  if (_provideNoneMatrix) {
    _matrix->addItem("<None>");
  }

  KstMatrixList matrices = KST::matrixList.list();
  KST::matrixList.lock().readLock();
  for (KstMatrixList::ConstIterator i = matrices.begin(); i != matrices.end(); ++i) {
    (*i)->readLock();
    QString tag = (*i)->tag().displayString();
    (*i)->unlock();
    _matrix->addItem(tag);
    if (!found && tag == prev) {
      found = true;
    }
  }
  KST::matrixList.lock().unlock();
  if (found) {
    _matrix->setItemText(_matrix->currentIndex(), prev);
  }
  blockSignals(false);
  setEdit(_matrix->currentText());
}


void MatrixSelector::init() {
  _newMatrix->setIcon(BarIcon("kst_matrixnew"));
  _editMatrix->setIcon(BarIcon("kst_matrixedit"));
  _provideNoneMatrix = false;
  update();
  connect(_matrix, SIGNAL(activated(const QString&)), this, SIGNAL(selectionChanged(const QString&)));
}


void MatrixSelector::createNewMatrix() {
  KstDialogs::self()->newMatrixDialog(this, SLOT(newMatrixCreated(KstMatrixPtr)), SLOT(setSelection(KstMatrixPtr)), SLOT(update()));
}


void MatrixSelector::selectionWatcher(const QString &tag) {
  QString label = "[" + tag + "]";
  emit selectionChangedLabel(label);
  setEdit(tag);
}


void MatrixSelector::setSelection(const QString &tag) {
  if (tag.isEmpty()) {
    if (_provideNoneMatrix) {
      blockSignals(true);
      _matrix->setCurrentIndex(0);
      blockSignals(false);
      _editMatrix->setEnabled(false);
    }
    return;
  }
  blockSignals(true);
  _matrix->setItemText(_matrix->currentIndex(), tag);  // What if it isn't in the combo?
  blockSignals(false);

  setEdit(tag);
}


void MatrixSelector::newMatrixCreated(KstMatrixPtr v) {
  v->readLock();
  QString name = v->tagName();
  v->unlock();
  v = 0L; // deref
  emit newMatrixCreated(name);
}


void MatrixSelector::setSelection(KstMatrixPtr v) {
  v->readLock();
  setSelection(v->tagName());
  v->unlock();
}


void MatrixSelector::provideNoneMatrix(bool provide) {
  if (provide != _provideNoneMatrix) {
    _provideNoneMatrix = provide;
    update();
  }
}


void MatrixSelector::editMatrix() {
  KST::matrixList.lock().readLock();
  KstMatrixPtr mat = *KST::matrixList.findTag(_matrix->currentText());
  KST::matrixList.lock().unlock();
  KstDataObjectPtr pro = 0L;
  if (mat) {
    pro = kst_cast<KstDataObject>(mat->provider());
  }
  if (pro) {
    pro->readLock();
    pro->showDialog(false);
    pro->unlock();
  } else {
    KstDialogs::self()->showMatrixDialog(_matrix->currentText(), true);
  }

}


void MatrixSelector::setEdit(const QString& tag) {
  KST::matrixList.lock().readLock();
  _editMatrix->setEnabled(KST::matrixList.findTag(tag) != KST::matrixList.end());
  KST::matrixList.lock().unlock();
}

#include "matrixselector.moc"

// vim: ts=2 sw=2 et
