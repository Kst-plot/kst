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

#include "datarange.h"
#include "dialogdefaults.h"

namespace Kst {

DataRange::DataRange(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SLOT(countFromEndChanged()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SLOT(readToEndChanged()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SLOT(doSkipChanged()));

  connect(_start, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_range, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_skip, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_doFilter, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SIGNAL(modified()));

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
  return (!_start->text().isEmpty());
}


void DataRange::setStart(qreal start) {
  _start->setText(QString::number(start));
}


QString DataRange::startUnits() const {
  return _startUnits->currentText();
}


void DataRange::setStartUnits(const QString &startUnits) const {
  _startUnits->setItemText(_startUnits->currentIndex(), startUnits);
}


qreal DataRange::range() const {
  return _range->text().toDouble();
}


bool DataRange::rangeDirty() const {
  return (!_range->text().isEmpty());
}


void DataRange::setRange(qreal range) {
  _range->setText(QString::number(range));
}


QString DataRange::rangeUnits() const {
  return _rangeUnits->currentText();
}


void DataRange::setRangeUnits(const QString &rangeUnits) const {
  _rangeUnits->setItemText(_startUnits->currentIndex(), rangeUnits);
}


int DataRange::skip() const {
  return _skip->value();
}


bool DataRange::skipDirty() const {
  return (!_skip->text().isEmpty());
}


void DataRange::setSkip(int skip) {
  _skip->setValue(skip);
}


bool DataRange::countFromEnd() const {
  return _countFromEnd->isChecked();
}


void DataRange::setCountFromEnd(bool countFromEnd) {
  _countFromEnd->setChecked(countFromEnd);
}


bool DataRange::countFromEndDirty() const {
  return (_readToEnd->state() == Qt::PartiallyChecked);
}


bool DataRange::readToEnd() const {
  return _readToEnd->isChecked();
}


bool DataRange::readToEndDirty() const {
  return (_readToEnd->state() == Qt::PartiallyChecked);
}


void DataRange::setReadToEnd(bool readToEnd) {
  _readToEnd->setChecked(readToEnd);
}


bool DataRange::doSkip() const {
  return _doSkip->isChecked();
}


bool DataRange::doSkipDirty() const {
  return (_doSkip->state() == Qt::PartiallyChecked);
}


void DataRange::setDoSkip(bool doSkip) {
  _doSkip->setChecked(doSkip);
}


bool DataRange::doFilter() const {
  return _doFilter->isChecked();
}


bool DataRange::doFilterDirty() const {
  return (_doFilter->state() == Qt::PartiallyChecked);
}


void DataRange::setDoFilter(bool doFilter) {
  _doFilter->setChecked(doFilter);
}


void DataRange::countFromEndChanged() {
  if (countFromEnd()) {
    setReadToEnd(false);
  }

  _start->setEnabled(!countFromEnd());
  _startUnits->setEnabled(!countFromEnd());
}


void DataRange::readToEndChanged() {
  if (readToEnd()) {
    setCountFromEnd(false);
  }

  _range->setEnabled(!readToEnd());
  _rangeUnits->setEnabled(!readToEnd());
}


void DataRange::doSkipChanged() {
  _skip->setEnabled(doSkip());
  _doFilter->setEnabled(doSkip());
}

void DataRange::setWidgetDefaults() {
  //FIXME Do we need a V->readLock() here?
  _dialogDefaults->setValue("vector/range", range());
  _dialogDefaults->setValue("vector/start", start());
  _dialogDefaults->setValue("vector/countFromEnd", countFromEnd());
  _dialogDefaults->setValue("vector/readToEnd", readToEnd());
  _dialogDefaults->setValue("vector/skip", skip());
  _dialogDefaults->setValue("vector/doSkip", doSkip());
  _dialogDefaults->setValue("vector/doAve", doFilter());
}

void DataRange::loadWidgetDefaults() {
  setRange(_dialogDefaults->value("vector/range", 1).toInt());
  setStart(_dialogDefaults->value("vector/start", 0).toInt());
  setCountFromEnd(_dialogDefaults->value("vector/countFromEnd",false).toBool());
  setReadToEnd(_dialogDefaults->value("vector/readToEnd",true).toBool());
  setSkip(_dialogDefaults->value("vector/skip", 0).toInt());
  setDoSkip(_dialogDefaults->value("vector/doSkip", false).toBool());
  setDoFilter(_dialogDefaults->value("vector/doAve",false).toBool());
}

}

// vim: ts=2 sw=2 et
