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

  connect(_start, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_range, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_skip, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_doFilter, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SIGNAL(modified()));

  QLabel* siHack=new QLabel(this);
  siHack->hide();
  siHack->setProperty("si","read 1 sample per");
  siHack->setBuddy(_skip);
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


void DataRange::setStart(qreal start) {
  _start->setText(QString::number(start, 'g', 12));
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


void DataRange::setRange(qreal range) {
  _range->setText(QString::number(range));
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
  _dialogDefaults->setValue("vector/rangeUnits", rangeUnits());
  _dialogDefaults->setValue("vector/startUnits", rangeUnits());
}

void DataRange::loadWidgetDefaults() {
  setRange(_dialogDefaults->value("vector/range", 1).toInt());
  setStart(_dialogDefaults->value("vector/start", 0).toInt());
  setCountFromEnd(_dialogDefaults->value("vector/countFromEnd",false).toBool());
  setReadToEnd(_dialogDefaults->value("vector/readToEnd",true).toBool());
  setSkip(_dialogDefaults->value("vector/skip", 0).toInt());
  setDoSkip(_dialogDefaults->value("vector/doSkip", false).toBool());
  setDoFilter(_dialogDefaults->value("vector/doAve",false).toBool());
  setRangeUnits(_dialogDefaults->value("vector/rangeUnits",tr("frames")).toString());
  setStartUnits(_dialogDefaults->value("vector/startUnits",tr("frames")).toString());
}

}

// vim: ts=2 sw=2 et
