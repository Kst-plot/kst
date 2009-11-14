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

#include "curveselector.h"

#include "dialoglauncher.h"
#include "datacollection.h"
#include "objectstore.h"

namespace Kst {

CurveSelector::CurveSelector(QWidget *parent, ObjectStore *store)
  : QWidget(parent), _allowEmptySelection(false), _store(store) {

  setupUi(this);

  fillCurves();

  connect(_curve, SIGNAL(currentIndexChanged(int)), this, SLOT(emitSelectionChanged()));
}


CurveSelector::~CurveSelector() {
}


void CurveSelector::setObjectStore(ObjectStore *store) {
  _store = store;
  fillCurves();
}


void CurveSelector::emitSelectionChanged() {
  emit selectionChanged(_curve->currentText());
}


CurvePtr CurveSelector::selectedCurve() const {
  return qVariantValue<Curve*>(_curve->itemData(_curve->currentIndex()));
}


bool CurveSelector::selectedCurveDirty() const {
  return _curve->currentIndex() != -1;
}


void CurveSelector::setSelectedCurve(CurvePtr selectedCurve) {
  //FIXME: findData doesn't work, but the loop here - which is supposed
  // to do exactly the same thing - does.  WHY???
  //int i = _curve->findData(qVariantFromValue(selectedVector.data()));
  int i=-1,j;
  for (j=0; j<_curve->count() ; j++) {
    if (selectedCurve.data() == (qVariantValue<Curve*>(_curve->itemData(j)))) {
      i=j;
      break;
    }
  }
  Q_ASSERT(i != -1);
  _curve->setCurrentIndex(i);
}


void CurveSelector::clearSelection() {
  _curve->setCurrentIndex(-1);
}


bool CurveSelector::allowEmptySelection() const {
  return _allowEmptySelection;
}


void CurveSelector::setAllowEmptySelection(bool allowEmptySelection) {
  _allowEmptySelection = allowEmptySelection;

  int i = _curve->findText(tr("<None>"));
  if (i != -1)
    _curve->removeItem(i);

  if (_allowEmptySelection) {
    _curve->insertItem(0, tr("<None>"), qVariantFromValue(0));
    _curve->setCurrentIndex(0);
  }
}


void CurveSelector::fillCurves() {
  if (!_store) {
    return;
  }

  QHash<QString, CurvePtr> curves;

  CurveList curveList = _store->getObjects<Curve>();

  CurveList::ConstIterator it = curveList.begin();
  for (; it != curveList.end(); ++it) {
    CurvePtr curve = (*it);

    curve->readLock();
    curves.insert(curve->Name(), curve);
    curve->unlock();
  }

  QStringList list = curves.keys();

  qSort(list);

  CurvePtr current = selectedCurve();

  _curve->clear();
  foreach (QString string, list) {
    CurvePtr r = curves.value(string);
    _curve->addItem(string, qVariantFromValue(r.data()));
  }

  if (_allowEmptySelection) //reset the <None>
    setAllowEmptySelection(true);

  if (current)
    setSelectedCurve(current);
}


bool CurveSelector::event(QEvent * event) {
  if (event->type() == QEvent::WindowActivate) {
    fillCurves();
  }
  return QWidget::event(event);
}

}

// vim: ts=2 sw=2 et
