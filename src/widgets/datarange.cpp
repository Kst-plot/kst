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

namespace Kst {

DataRange::DataRange(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_countFromEnd, SIGNAL(toggled(bool)), this, SLOT(countFromEndChanged()));
  connect(_readToEnd, SIGNAL(toggled(bool)), this, SLOT(readToEndChanged()));
  connect(_doSkip, SIGNAL(toggled(bool)), this, SLOT(doSkipChanged()));
}


DataRange::~DataRange() {
}


qreal DataRange::start() const {
  return _start->text().toDouble();
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


void DataRange::setSkip(int skip) {
  _skip->setValue(skip);
}


bool DataRange::countFromEnd() const {
  return _countFromEnd->isChecked();
}


void DataRange::setCountFromEnd(bool countFromEnd) {
  _countFromEnd->setChecked(countFromEnd);
}


bool DataRange::readToEnd() const {
  return _readToEnd->isChecked();
}


void DataRange::setReadToEnd(bool readToEnd) {
  _readToEnd->setChecked(readToEnd);
}


bool DataRange::doSkip() const {
  return _doSkip->isChecked();
}


void DataRange::setDoSkip(bool doSkip) {
  _doSkip->setChecked(doSkip);
}


bool DataRange::doFilter() const {
  return _doFilter->isChecked();
}


void DataRange::setDoFilter(bool doFilter) {
  _doFilter->setChecked(doFilter);
}


void DataRange::countFromEndChanged() {
  if (countFromEnd()) {
    setReadToEnd(false);
  }

  _range->setEnabled(!countFromEnd());
  _rangeUnits->setEnabled(!countFromEnd());
}


void DataRange::readToEndChanged() {
  if (readToEnd()) {
    setCountFromEnd(false);
  }

  _start->setEnabled(!readToEnd());
  _startUnits->setEnabled(!readToEnd());
}


void DataRange::doSkipChanged() {
  _skip->setEnabled(doSkip());
  _doFilter->setEnabled(doSkip());
}

}

// vim: ts=2 sw=2 et
