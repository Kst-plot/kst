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

#include "datarange.h"
#include "dialogdefaults.h"

namespace Kst {

DataRange::DataRange(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SLOT(countFromEndChanged()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SLOT(readToEndChanged()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SLOT(doSkipChanged()));

  connect(_start, SIGNAL(textEdited(QString)), this, SLOT(startChanged()));
  connect(_range, SIGNAL(textEdited(QString)), this, SLOT(rangeChanged()));
  connect(_last, SIGNAL(textEdited(QString)), this, SLOT(lastChanged()));
  connect(_skip, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_doFilter, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_startUnits, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));
  connect(_rangeUnits, SIGNAL(currentIndexChanged(int)), this, SLOT(unitsChanged()));

  _controlField0 = Range;
  _controlField1 = Start;
}


DataRange::~DataRange() {
}


void DataRange::clearValues() {
  _start->clear();
  _range->clear();
  _skip->clear();
  _doFilter->setCheckState(Qt::PartiallyChecked);
  _readToEnd->setCheckState(Qt::PartiallyChecked);
  _doSkip->setCheckState(Qt::PartiallyChecked);
}


qreal DataRange::start() const {
  return _start->text().toDouble();
}


bool DataRange::startDirty() const {
  return !_start->text().isEmpty();
}


void DataRange::setStart(qreal start, bool callUpdateFields) {
  _start->setText(QString::number(start, 'g', 12));
  if (callUpdateFields) {
    updateFields(None);
  }
}

qreal DataRange::last() const {
  return _last->text().toDouble();
}


bool DataRange::lastDirty() const {
  return !_last->text().isEmpty();
}


void DataRange::setLast(qreal last, bool callUpdateFields) {
  _last->setText(QString::number(last, 'g', 12));
  if (callUpdateFields) {
    updateFields(None);
  }
}

void DataRange::clearIndexList() {
  _startUnits->clear();
  _rangeUnits->clear();
}


void DataRange::updateIndexList(const QStringList &indexFields) {
  _startUnits->clear();
  _startUnits->addItems(indexFields);
  setStartUnits(_requestedStartUnits);
  _rangeUnits->clear();
  _rangeUnits->addItems(indexFields);
  setRangeUnits(_requestedRangeUnits);
}


QString DataRange::startUnits() const {
  return _startUnits->currentText();
}


int DataRange::startUnitsIndex() const {
  return _startUnits->currentIndex();
}


void DataRange::setStartUnits(const QString &startUnits) {
  _requestedStartUnits = startUnits;
  int i = _startUnits->findText(startUnits);
  if (i>=0) {
    _startUnits->setCurrentIndex(i);
  }
}

qreal DataRange::range() const {
  return _range->text().toDouble();
}


int DataRange::rangeUnitsIndex()  const {
  return _rangeUnits->currentIndex();
}


bool DataRange::rangeDirty() const {
  return !_range->text().isEmpty();
}


void DataRange::setRange(qreal range, bool callUpdateFields) {
  _range->setText(QString::number(range));
  if (callUpdateFields) {
    updateFields(None);
  }
}


QString DataRange::rangeUnits() const {
  return _rangeUnits->currentText();
}


void DataRange::setRangeUnits(const QString &rangeUnits) {
  _requestedRangeUnits = rangeUnits;
  int i = _rangeUnits->findText(rangeUnits);
  if (i>=0) {
    _rangeUnits->setCurrentIndex(i);
  } else {
    _rangeUnits->setCurrentIndex(0);
  }
}


int DataRange::skip() const {
  return _skip->value();
}


bool DataRange::skipDirty() const {
  return !_skip->text().isEmpty();
}


void DataRange::setSkip(int skip) {
  _skip->setValue(skip);
}


bool DataRange::countFromEnd() const {
  return _countFromEnd->isChecked();
}


void DataRange::setCountFromEnd(bool countFromEnd) {
  _countFromEnd->setChecked(countFromEnd);
  updateFields(None);
}


bool DataRange::countFromEndDirty() const {
  return _readToEnd->checkState() == Qt::PartiallyChecked;
}


bool DataRange::readToEnd() const {
  return _readToEnd->isChecked();
}


bool DataRange::readToEndDirty() const {
  return _readToEnd->checkState() == Qt::PartiallyChecked;
}


void DataRange::setReadToEnd(bool readToEnd) {
  _readToEnd->setChecked(readToEnd);
  updateFields(None);
}


bool DataRange::doSkip() const {
  return _doSkip->isChecked();
}


bool DataRange::doSkipDirty() const {
  return _doSkip->checkState() == Qt::PartiallyChecked;
}


void DataRange::setDoSkip(bool doSkip) {
  _doSkip->setChecked(doSkip);
}


bool DataRange::doFilter() const {
  return _doFilter->isChecked();
}


bool DataRange::doFilterDirty() const {
  return _doFilter->checkState() == Qt::PartiallyChecked;
}


void DataRange::setDoFilter(bool doFilter) {
  _doFilter->setChecked(doFilter);
}


void DataRange::countFromEndChanged() {
  if (countFromEnd()) {
    setReadToEnd(false);
  }

  updateFields(None);

}


void DataRange::readToEndChanged() {
  if (readToEnd()) {
    setCountFromEnd(false);
  }

  updateFields(None);

}


void DataRange::unitsChanged() {
  updateFields(None);
}


void DataRange::doSkipChanged() {
  _skip->setEnabled(doSkip());
  _doFilter->setEnabled(doSkip());
}

// control field logic:
// the last one changed, other than this one, should be the control field
// do we need a history?
// F0 R -> L
// F0 L -> R
// R L -> F0
// R F0 -> L

void DataRange::startChanged() {
  updateFields(Start);
  emit modified();
}


void DataRange::lastChanged() {
  updateFields(Last);
  emit modified();
}


void DataRange::rangeChanged() {
  updateFields(Range);
  emit modified();
}


void DataRange::updateFields(ControlField cf) {

  bool enable_last = (_rangeUnits->currentIndex() == _startUnits->currentIndex());
  enable_last &= !readToEnd();
  enable_last &= !countFromEnd();


  _last->setEnabled(enable_last);
  _lastLabel->setEnabled(enable_last);

  _start->setEnabled(!countFromEnd());
  _startLabel->setEnabled(!countFromEnd());
  _startUnits->setEnabled(!countFromEnd());
  _range->setEnabled(!readToEnd());
  _rangeLabel->setEnabled(!readToEnd());
  _rangeUnits->setEnabled(!readToEnd());

  if ((cf!=None) && (cf != _controlField1)) {
    _controlField0 = _controlField1;
    _controlField1 = cf;
  }

  // don't do anything if it wouldn't make sense to.
  if (readToEnd() || countFromEnd()) {
    return;
  } 

  if (startUnits() != (rangeUnits())) {
    return;
  }
  
  if ((_controlField0 != Start) && (_controlField1 != Start)) {
    _start->setText(QString::number(last() - range() + 1, 'g', 12));
  } else if ((_controlField0 != Last) && (_controlField1 != Last)) {
    _last->setText(QString::number(start() + range() - 1, 'g', 12));
  } else if ((_controlField0 != Range) && (_controlField1 != Range)) {
    _range->setText(QString::number(last() - start() + 1, 'g', 12));
  }
}


void DataRange::setWidgetDefaults() {
  //FIXME Do we need a V->readLock() here?
  dialogDefaults().setValue("vector/range", range());
  dialogDefaults().setValue("vector/start", start());
  dialogDefaults().setValue("vector/countFromEnd", countFromEnd());
  dialogDefaults().setValue("vector/readToEnd", readToEnd());
  dialogDefaults().setValue("vector/skip", skip());
  dialogDefaults().setValue("vector/doSkip", doSkip());
  dialogDefaults().setValue("vector/doAve", doFilter());
  dialogDefaults().setValue("vector/rangeUnits", rangeUnits());
  dialogDefaults().setValue("vector/startUnits", rangeUnits());
}

void DataRange::loadWidgetDefaults() {
  setRange(dialogDefaults().value("vector/range", 1).toInt());
  setStart(dialogDefaults().value("vector/start", 0).toInt());
  setCountFromEnd(dialogDefaults().value("vector/countFromEnd",false).toBool());
  setReadToEnd(dialogDefaults().value("vector/readToEnd",true).toBool());
  setSkip(dialogDefaults().value("vector/skip", 0).toInt());
  setDoSkip(dialogDefaults().value("vector/doSkip", false).toBool());
  setDoFilter(dialogDefaults().value("vector/doAve",false).toBool());
  setRangeUnits(dialogDefaults().value("vector/rangeUnits",tr("frames")).toString());
  setStartUnits(dialogDefaults().value("vector/startUnits",tr("frames")).toString());
}

bool DataRange::rangeIsValid() {
  if (readToEnd()) {
    return true;
  } else {
    return (range()>1);
  }
}

}

// vim: ts=2 sw=2 et
