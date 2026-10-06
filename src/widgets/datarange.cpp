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
#include "geticon.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QTimeZone>
#include <QtMath>

#include <cmath>
#include <limits>

namespace Kst {

DataRange::DataRange(QWidget *parent) : QWidget(parent) {
  setupUi(this);

  _range->setToolTip(tr("Time durations can be specified in the format: [Xw] "
                        "[Xd] [Xh] [Xm] [X[s]]\ne.g. 1h30m or 6w5d4h3m2.5s"));

  connect(_countFromEnd, SIGNAL(toggled(bool)), this,
          SLOT(countFromEndChanged()));
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
  connect(_startUnits, SIGNAL(currentIndexChanged(int)), this,
          SLOT(unitsChanged()));
  connect(_rangeUnits, SIGNAL(currentIndexChanged(int)), this,
          SLOT(unitsChanged()));

  _rangeToDuration->setIcon(KstGetIcon("clock"));
  _rangeToDuration->setToolTip(
      tr("Convert current range to a time duration string (e.g. 1h30m3s)"));
  _rangeToNumber->setIcon(KstGetIcon("draw-number"));
  _rangeToNumber->setToolTip(
      tr("Convert current range to a plain number of frames or seconds"));
  connect(_rangeToNumber, &QPushButton::clicked, this,
          &DataRange::convertRangeToNumber);
  connect(_rangeToDuration, &QPushButton::clicked, this,
          &DataRange::convertRangeToDuration);

  _startToDuration->setIcon(KstGetIcon("clock"));
  _startToDuration->setToolTip(
      tr("Convert current start to a time duration string (e.g. 1h30m3s)"));
  _startToNumber->setIcon(KstGetIcon("draw-number"));
  _startToNumber->setToolTip(
      tr("Convert current start to a plain number of frames or seconds"));
  _startToISOTime->setIcon(KstGetIcon("calendar"));
  _startToISOTime->setToolTip(tr("Convert current start to an ISO time string "
                                 "(e.g. 2024-01-01T12:34:56.789)"));
  connect(_startToNumber, &QPushButton::clicked, this,
          &DataRange::convertStartToNumber);
  connect(_startToDuration, &QPushButton::clicked, this,
          &DataRange::convertStartToDuration);
  connect(_startToISOTime, &QPushButton::clicked, this,
          &DataRange::convertStartToISOTime);

  QFontMetrics fm(_start->font());
  _start->setMinimumWidth(
      fm.horizontalAdvance("0000-00-00T88:88:88.888+00:00000"));
  _last->setMinimumWidth(
      fm.horizontalAdvance("0000-00-00T88:88:88.888+00:00000"));

  _controlField0 = Range;
  _controlField1 = Start;
  _canonicalFrameRangeValid = false;
}

DataRange::~DataRange() {}

// Forward declarations for static helpers defined later in this file.
static double parseTimeDuration(const QString &text);
static double parseISOTime(const QString &text);
static QString secondsToDurationString(double totalSeconds);
static QString ctimeToISOString(double ctime);

void DataRange::clearValues() {
  _canonicalFrameRangeValid = false;
  _canonicalCountFromEndValid = false;
  _start->clear();
  _range->clear();
  _skip->clear();
  _doFilter->setCheckState(Qt::PartiallyChecked);
  _readToEnd->setCheckState(Qt::PartiallyChecked);
  _doSkip->setCheckState(Qt::PartiallyChecked);
}

qreal DataRange::start() const {
  if (startIsISOTime()) {
    return parseISOTime(_start->text());
  }
  if (startIsSeconds()) {
    return parseTimeDuration(_start->text());
  }
  return _start->text().toDouble();
}

bool DataRange::startDirty() const { return !_start->text().isEmpty(); }

void DataRange::setStart(qreal start, bool callUpdateFields) {
  // Match the display format already used by _last so both fields stay in sync.
  if (lastIsISOTime() && startIsCTime()) {
    _start->setText(ctimeToISOString(start));
  } else if (lastIsDuration() && startIsSeconds()) {
    _start->setText(secondsToDurationString(start));
  } else {
    _start->setText(QString::number(start, 'g', 17));
  }
  if (callUpdateFields) {
    updateFields(None);
  }
}

qreal DataRange::last() const {
  if (lastIsISOTime()) {
    return parseISOTime(_last->text());
  }
  if (startIsSeconds()) {
    return parseTimeDuration(_last->text());
  }
  return _last->text().toDouble();
}

bool DataRange::lastDirty() const { return !_last->text().isEmpty(); }

void DataRange::setLast(qreal last, bool callUpdateFields) {
  // Match the display format already used by _start so both fields stay in sync.
  if (startIsISOTime() && startIsCTime()) {
    _last->setText(ctimeToISOString(last));
  } else if (startIsDuration() && startIsSeconds()) {
    _last->setText(secondsToDurationString(last));
  } else {
    _last->setText(QString::number(last, 'g', 17));
  }
  if (callUpdateFields) {
    updateFields(None);
  }
}

void DataRange::clearIndexList() {
  _canonicalFrameRangeValid = false;
  _canonicalCountFromEndValid = false;
  _startUnits->clear();
  _rangeUnits->clear();
  _indexFieldProps.clear();
  _dataSource = DataSourcePtr();
  _referenceField.clear();
}

void DataRange::updateIndexList(
    const QList<Kst::IndexFieldProperties> &indexFields) {
  _indexFieldProps = indexFields;
  QStringList startLabelsList;
  QStringList rangeUnitsList;
  for (const Kst::IndexFieldProperties &ifp : indexFields) {
    startLabelsList.append(ifp.name);
    rangeUnitsList.append(ifp.name);
  }

  QSignalBlocker startUnitsBlocker(_startUnits);
  QSignalBlocker rangeUnitsBlocker(_rangeUnits);

  _startUnits->clear();
  _startUnits->addItems(startLabelsList);
  setStartUnits(_requestedStartUnits);
  _rangeUnits->clear();
  _rangeUnits->addItems(rangeUnitsList);
  setRangeUnits(_requestedRangeUnits);

  // These selections were set programmatically while wiring up the dialog.
  // Treat them as the displayed baseline so unitsChanged() does not convert
  // values before the dialog is fully initialized.
  _displayedStartUnits = startUnits();
  _displayedRangeUnits = rangeUnits();

  updateFields(None);
  if (_canonicalFrameRangeValid) {
    displayCanonicalFrameRange();
  }
  if (_canonicalCountFromEndValid && !displayCanonicalCountFromEnd()) {
    for (const IndexFieldProperties &field : indexFields) {
      if (field.is_frame) {
        setRangeUnits(field.name);
        _displayedRangeUnits = rangeUnits();
        displayCanonicalCountFromEnd();
        break;
      }
    }
  }
}

QString DataRange::startUnits() const { return _startUnits->currentText(); }

// int DataRange::startUnitsIndex() const {
//   return _startUnits->currentIndex();
// }

void DataRange::setStartUnits(const QString &startUnits) {
  _requestedStartUnits = startUnits;
  int i = _startUnits->findText(startUnits);
  if (i < 0 && startUnits.compare("frames", Qt::CaseInsensitive) == 0) {
    for (const IndexFieldProperties &field : _indexFieldProps) {
      if (field.is_frame) {
        i = _startUnits->findText(field.name);
        if (i >= 0) {
          break;
        }
      }
    }
  }
  if (i >= 0) {
    _startUnits->setCurrentIndex(i);
  }
}

// Format a seconds value as a compact KST duration string, e.g. 3661.5 ->
// "1h1m1.5s". Returns "0s" for zero or negative values.
static QString secondsToDurationString(double totalSeconds) {
  if (totalSeconds <= 0.0) {
    return QStringLiteral("0s");
  }
  const qint64 totalSecs = static_cast<qint64>(totalSeconds);
  const double fracSecs = totalSeconds - double(totalSecs);
  const qint64 secsPerWeek = 7LL * 24 * 3600;
  const qint64 secsPerDay = 24LL * 3600;
  const qint64 weeks = totalSecs / secsPerWeek;
  const qint64 rem1 = totalSecs % secsPerWeek;
  const qint64 days = rem1 / secsPerDay;
  const qint64 rem2 = rem1 % secsPerDay;
  const qint64 hours = rem2 / 3600;
  const qint64 rem3 = rem2 % 3600;
  const qint64 minutes = rem3 / 60;
  const qint64 secs = rem3 % 60;
  const double fullSecs = double(secs) + fracSecs;

  QString result;
  if (weeks)
    result += QString::number(weeks) + QLatin1Char('w');
  if (days)
    result += QString::number(days) + QLatin1Char('d');
  if (hours)
    result += QString::number(hours) + QLatin1Char('h');
  if (minutes)
    result += QString::number(minutes) + QLatin1Char('m');
  if (fullSecs != 0.0 || result.isEmpty()) {
    result += QString::number(fullSecs, 'g', 10) + QLatin1Char('s');
  }
  return result;
}

// Returns true if the text is a valid KST duration string (not a plain number).
static bool isDurationString(const QString &text) {
  const QString trimmed = text.trimmed();
  if (trimmed.isEmpty()) {
    return false;
  }
  bool ok = false;
  trimmed.toDouble(&ok);
  if (ok) {
    return false;
  }
  static const QRegularExpression durationRe(
      QStringLiteral(R"(^\s*)"
                     R"((?:[0-9]+(?:\.[0-9]+)?\s*[wW]\s*)?)"
                     R"((?:[0-9]+(?:\.[0-9]+)?\s*[dD]\s*)?)"
                     R"((?:[0-9]+(?:\.[0-9]+)?\s*[hH]\s*)?)"
                     R"((?:[0-9]+(?:\.[0-9]+)?\s*[mM]\s*)?)"
                     R"((?:[0-9]+(?:\.[0-9]+)?\s*[sS]?\s*)?)"
                     R"(\s*$)"));
  static const QRegularExpression hasUnit(QStringLiteral("[wWdDhHmMsS]"));
  return durationRe.match(trimmed).hasMatch() && trimmed.contains(hasUnit);
}

// Returns true if the text is a valid ISO 8601 date-time string.
static bool isISOTimeString(const QString &text) {
  return QDateTime::fromString(text.trimmed(), Qt::ISODateWithMs).isValid();
}

// Parse an ISO 8601 date-time string to a ctime double (seconds since epoch,
// UTC).
static double parseISOTime(const QString &text) {
  QDateTime dt = QDateTime::fromString(text.trimmed(), Qt::ISODateWithMs);
  if (!dt.isValid()) {
    return text.toDouble();
  }
  dt.setTimeZone(QTimeZone::utc());
  return double(dt.toMSecsSinceEpoch()) / 1000.0;
}

// Format a ctime double (seconds since epoch) as an ISO 8601 UTC string.
static QString ctimeToISOString(double ctime) {
  QDateTime dt =
      QDateTime::fromMSecsSinceEpoch(qint64(ctime * 1000.0), QTimeZone::utc());
  return dt.toString(Qt::ISODateWithMs);
}

// Parse an optional convenience syntax for a duration in seconds:
//   [<weeks>w][ ][<days>d][ ][<hours>h][ ][<minutes>m][ ][<seconds>[s]]
// Falls back to plain toDouble() for strings that contain no unit letters.
static double parseTimeDuration(const QString &text) {
  // Fast path: plain number (no unit letters present)
  bool ok = false;
  double plain = text.toDouble(&ok);
  if (ok) {
    return plain;
  }

  // [<w>w][ ][<d>d][ ][<h>h][ ][<m>m][ ][<s>[s]]
  static const QRegularExpression re(QStringLiteral(
      R"(^\s*)"
      R"((?:([0-9]+(?:\.[0-9]+)?)\s*[wW]\s*)?)"  // weeks
      R"((?:([0-9]+(?:\.[0-9]+)?)\s*[dD]\s*)?)"  // days
      R"((?:([0-9]+(?:\.[0-9]+)?)\s*[hH]\s*)?)"  // hours
      R"((?:([0-9]+(?:\.[0-9]+)?)\s*[mM]\s*)?)"  // minutes
      R"((?:([0-9]+(?:\.[0-9]+)?)\s*[sS]?\s*)?)" // seconds (suffix optional)
      R"(\s*$)"));

  QRegularExpressionMatch m = re.match(text.trimmed());
  if (!m.hasMatch()) {
    return text.toDouble(); // fallback
  }

  const double weeks = m.captured(1).toDouble();
  const double days = m.captured(2).toDouble();
  const double hours = m.captured(3).toDouble();
  const double minutes = m.captured(4).toDouble();
  const double seconds = m.captured(5).toDouble();

  return weeks * (7.0 * 24.0 * 3600.0) + days * (24.0 * 3600.0) +
         hours * 3600.0 + minutes * 60.0 + seconds;
}

bool DataRange::rangeIsDuration() const {
  return isDurationString(_range->text());
}

bool DataRange::startIsDuration() const {
  return isDurationString(_start->text());
}

bool DataRange::lastIsDuration() const {
  return isDurationString(_last->text());
}

bool DataRange::startIsISOTime() const {
  return isISOTimeString(_start->text());
}

bool DataRange::lastIsISOTime() const { return isISOTimeString(_last->text()); }

qreal DataRange::range() const {
  if (rangeIsSeconds()) {
    return parseTimeDuration(_range->text());
  }
  return _range->text().toDouble();
}

// int DataRange::rangeUnitsIndex()  const {
//   return _rangeUnits->currentIndex();
// }

bool DataRange::rangeDirty() const { return !_range->text().isEmpty(); }

void DataRange::setRange(qreal range, bool callUpdateFields) {
  _range->setText(QString::number(range, 'g', 17));
  if (callUpdateFields) {
    updateFields(None);
  }
}

QString DataRange::rangeUnits() const { return _rangeUnits->currentText(); }

void DataRange::setRangeUnits(const QString &rangeUnits) {
  _requestedRangeUnits = rangeUnits;
  int i = _rangeUnits->findText(rangeUnits);
  if (i < 0 && rangeUnits.compare("frames", Qt::CaseInsensitive) == 0) {
    for (const IndexFieldProperties &field : _indexFieldProps) {
      if (field.is_frame) {
        i = _rangeUnits->findText(field.name);
        if (i >= 0) {
          break;
        }
      }
    }
  }
  if (i >= 0) {
    _rangeUnits->setCurrentIndex(i);
  } else {
    _rangeUnits->setCurrentIndex(0);
  }
}

// helper implementations ---------------------------------------------------
static const Kst::IndexFieldProperties *
findField(const QList<Kst::IndexFieldProperties> &list, const QString &name) {
  for (const Kst::IndexFieldProperties &ifp : list) {
    if (ifp.name == name)
      return &ifp;
  }
  return nullptr;
}

bool DataRange::startIsFrame() const {
  const QString name = startUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_frame : false;
}

bool DataRange::startIsSeconds() const {
  const QString name = startUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_seconds : false;
}

bool DataRange::startIsCTime() const {
  const QString name = startUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_ctime : false;
}

bool DataRange::rangeIsFrame() const {
  const QString name = rangeUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_frame : false;
}

bool DataRange::rangeIsSeconds() const {
  const QString name = rangeUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_seconds : false;
}

bool DataRange::rangeIsCTime() const {
  const QString name = rangeUnits();
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, name);
  return ifp ? ifp->is_ctime : false;
}

void DataRange::setDataSource(const DataSourcePtr &dataSource) {
  if (_dataSource != dataSource) {
    _canonicalFrameRangeValid = false;
    _canonicalCountFromEndValid = false;
  }
  _dataSource = dataSource;
  updateFields(None);
}

DataSourcePtr DataRange::dataSource() const { return _dataSource; }

void DataRange::setReferenceField(const QString &field) {
  if (_referenceField != field) {
    _referenceField = field;
    _canonicalFrameRangeValid = false;
    _canonicalCountFromEndValid = false;
  }
}

bool DataRange::setFrameRange(qint64 startFrame, qint64 endFrameExclusive) {
  if (startFrame < 0 || endFrameExclusive <= startFrame || countFromEnd() ||
      readToEnd()) {
    return false;
  }

  _canonicalStartFrame = startFrame;
  _canonicalEndFrameExclusive = endFrameExclusive;
  _canonicalFrameRangeValid = true;
  _canonicalCountFromEndValid = false;
  return displayCanonicalFrameRange();
}

qint64 DataRange::availableFramesForReferenceField() {
  if (!_dataSource) {
    return -1;
  }

  DataSource *ds = _dataSource.data();
  ds->readLock();
  QStringList candidates;
  if (!_referenceField.isEmpty()) {
    candidates.append(_referenceField);
  }
  if (!rangeUnits().isEmpty() && !isFrameUnits(rangeUnits()) &&
      !candidates.contains(rangeUnits())) {
    candidates.append(rangeUnits());
  }
  candidates.append(ds->vector().list());

  qint64 availableFrames = -1;
  for (const QString &field : candidates) {
    if (ds->vector().isValid(field)) {
      const DataVector::DataInfo info = ds->vector().dataInfo(field);
      if (info.frameCount > 0.0 &&
          static_cast<long double>(info.frameCount) <=
              static_cast<long double>(std::numeric_limits<qint64>::max())) {
        availableFrames = qint64(std::floor(info.frameCount));
        break;
      }
    }
  }
  ds->unlock();
  return availableFrames;
}

bool DataRange::setCountFromEndFrameCount(qint64 frameCount) {
  if (!countFromEnd() || readToEnd() || frameCount < 0) {
    return false;
  }

  const qint64 availableFrames = availableFramesForReferenceField();
  if (availableFrames >= 0) {
    frameCount = qMin(frameCount, availableFrames);
    if (availableFrames >= 2 && frameCount < 2) {
      frameCount = qMin(qint64(2), availableFrames);
    }
  }

  _canonicalFrameRangeValid = false;
  _canonicalCountFromEnd = frameCount;
  _canonicalCountFromEndValid = true;
  return displayCanonicalCountFromEnd();
}

bool DataRange::displayCanonicalCountFromEnd() {
  if (!_canonicalCountFromEndValid || !countFromEnd() || readToEnd()) {
    return false;
  }

  const qint64 availableFrames = availableFramesForReferenceField();
  if (availableFrames < 0) {
    if (!isFrameUnits(rangeUnits())) {
      return false;
    }
    _range->setText(QString::number(_canonicalCountFromEnd, 'g', 17));
    return true;
  }

  qint64 frameCount = qMin(_canonicalCountFromEnd, availableFrames);
  if (availableFrames >= 2 && frameCount < 2) {
    frameCount = 2;
  }
  _canonicalCountFromEnd = frameCount;
  if (isFrameUnits(rangeUnits()) || frameCount == 0) {
    _range->setText(QString::number(frameCount, 'g', 17));
    return true;
  }

  const qint64 startFrame = availableFrames - frameCount;
  double startValue = 0.0;
  double endValue = 0.0;
  if (!unitsFromFrame(startFrame, rangeUnits(), &startValue) ||
      !unitsFromFrame(availableFrames, rangeUnits(), &endValue) ||
      !std::isfinite(startValue) || !std::isfinite(endValue) ||
      endValue < startValue) {
    return false;
  }

  const double displayedRange = endValue - startValue;
  if (!std::isfinite(displayedRange) || displayedRange <= 0.0) {
    return false;
  }
  _range->setText(QString::number(displayedRange, 'g', 17));
  return true;
}

bool DataRange::frameCountFromEndRange(double value, const QString &units,
                                       qint64 *frameCountOut) {
  if (!frameCountOut || !std::isfinite(value) || value < 0.0) {
    return false;
  }

  qint64 frameCount = 0;
  if (isFrameUnits(units)) {
    frameCount = static_cast<long double>(value) >=
                         static_cast<long double>(std::numeric_limits<qint64>::max())
                     ? std::numeric_limits<qint64>::max()
                     : qint64(std::floor(value));
  } else {
    const qint64 availableFrames = availableFramesForReferenceField();
    if (availableFrames < 0) {
      return false;
    }
    if (availableFrames == 0) {
      *frameCountOut = 0;
      return true;
    }

    double endValue = 0.0;
    if (!unitsFromFrame(availableFrames, units, &endValue)) {
      return false;
    }
    const double requestedStartValue = endValue - value;
    double firstValue = 0.0;
    if (!unitsFromFrame(0, units, &firstValue)) {
      return false;
    }

    qint64 startFrame = 0;
    if (requestedStartValue > firstValue &&
        !frameFromUnits(requestedStartValue, units, false, &startFrame)) {
      return false;
    }
    startFrame = qBound(qint64(0), startFrame, availableFrames);
    frameCount = availableFrames - startFrame;
  }

  const qint64 availableFrames = availableFramesForReferenceField();
  if (availableFrames >= 0) {
    frameCount = qMin(frameCount, availableFrames);
    if (availableFrames >= 2 && frameCount < 2) {
      frameCount = qMin(qint64(2), availableFrames);
    }
  }

  *frameCountOut = frameCount;
  return true;
}

bool DataRange::displayCanonicalFrameRange() {
  if (!_canonicalFrameRangeValid || countFromEnd() || readToEnd()) {
    return false;
  }

  double startValue = 0.0;
  double lastValue = 0.0;
  double rangeStartValue = 0.0;
  double rangeEndValue = 0.0;
  if (!unitsFromFrame(_canonicalStartFrame, startUnits(), &startValue) ||
      !unitsFromFrame(_canonicalEndFrameExclusive - 1, startUnits(),
                      &lastValue) ||
      !unitsFromFrame(_canonicalStartFrame, rangeUnits(), &rangeStartValue) ||
      !unitsFromFrame(_canonicalEndFrameExclusive, rangeUnits(),
                      &rangeEndValue)) {
    return false;
  }

  const QString startText = QString::number(startValue, 'g', 17);
  const QString lastText = QString::number(lastValue, 'g', 17);
  const QString rangeText = QString::number(
      isFrameUnits(rangeUnits())
          ? double(_canonicalEndFrameExclusive - _canonicalStartFrame)
          : qMax(0.0, rangeEndValue - rangeStartValue),
      'g', 17);
  _start->setText(startText);
  _last->setText(lastText);
  _range->setText(rangeText);
  _controlField0 = Start;
  _controlField1 = Last;
  updateFields(None);
  // updateFields() also solves range from the displayed endpoints. Restore the
  // duration calculated from the canonical boundaries in the selected units.
  _start->setText(startText);
  _last->setText(lastText);
  _range->setText(rangeText);
  return true;
}

bool DataRange::isFrameUnits(const QString &units) {
  const Kst::IndexFieldProperties *ifp = findField(_indexFieldProps, units);
  if (ifp) {
    return ifp->is_frame;
  }
  return units.compare("frames", Qt::CaseInsensitive) == 0;
}

bool DataRange::canConvertUnits(const QString &units) {
  if (isFrameUnits(units)) {
    return true;
  }
  if (!_dataSource) {
    return false;
  }

  _dataSource->readLock();
  bool valid = _dataSource->vector().isValid(units);
  _dataSource->unlock();
  return valid;
}

// int DataRange::maxFrameForClamp(const QString &preferredUnits) {
//   if (!_dataSource) {
//     return -1;
//   }

//   DataSource *ds = _dataSource.data();
//   ds->readLock();

//   QStringList candidates;
//   if (!preferredUnits.isEmpty() && !isFrameUnits(preferredUnits) &&
//   ds->vector().isValid(preferredUnits)) {
//     candidates.append(preferredUnits);
//   }

//   const QString su = startUnits();
//   if (!isFrameUnits(su) && !candidates.contains(su) &&
//   ds->vector().isValid(su)) {
//     candidates.append(su);
//   }

//   const QString ru = rangeUnits();
//   if (!isFrameUnits(ru) && !candidates.contains(ru) &&
//   ds->vector().isValid(ru)) {
//     candidates.append(ru);
//   }

//   for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
//     if (!ifp.is_frame && !candidates.contains(ifp.name) &&
//     ds->vector().isValid(ifp.name)) {
//       candidates.append(ifp.name);
//       break;
//     }
//   }

//   if (candidates.isEmpty()) {
//     const QStringList fields = ds->vector().list();
//     if (!fields.isEmpty()) {
//       candidates.append(fields.first());
//     }
//   }

//   int maxFrame = -1;
//   foreach (const QString &field, candidates) {
//     if (ds->vector().isValid(field)) {
//       const DataVector::DataInfo info = ds->vector().dataInfo(field);
//       if (info.frameCount > 0) {
//         maxFrame = int(info.frameCount) - 1;
//         break;
//       }
//     }
//   }

//   ds->unlock();
//   return maxFrame;
// }

bool DataRange::frameFromUnits(double value, const QString &units, bool roundUp,
                               qint64 *frameOut) {
  if (!frameOut) {
    return false;
  }

  const auto toFrame = [](double frameValue, bool ceiling) -> qint64 {
    if (!std::isfinite(frameValue) || frameValue <= 0) return 0;
    if (static_cast<long double>(frameValue) >=
        static_cast<long double>(std::numeric_limits<qint64>::max()))
      return std::numeric_limits<qint64>::max();
    return qint64(ceiling ? std::ceil(frameValue) : std::floor(frameValue));
  };

  qint64 frame = 0;
  if (isFrameUnits(units)) {
    frame = toFrame(value, roundUp);
    *frameOut = frame;
    return true;
  }

  if (!_dataSource) {
    return false;
  }

  DataSource *ds = _dataSource.data();
  ds->readLock();
  if (!ds->vector().isValid(units)) {
    ds->unlock();
    return false;
  }

  frame = ds->indexToFrame(value, units);
  if (frame < 0) {
    ds->unlock();
    return false;
  }

  const DataVector::DataInfo info = ds->vector().dataInfo(units);
  const qint64 maxFrame = qMax(qint64(0), qint64(info.frameCount) - 1);

  const double minValue = ds->frameToIndex(0, units);
  const double maxValue = ds->frameToIndex(maxFrame, units);
  if (value <= minValue) {
    ds->unlock();
    *frameOut = 0;
    return true;
  }

  if (value > maxValue) {
    const double fps = ds->framePerIndex(units);
    if (fps <= 0.0) {
      ds->unlock();
      return false;
    }

    const double extrapolated = double(maxFrame) + (value - maxValue) * fps;
    ds->unlock();
    *frameOut = toFrame(extrapolated, roundUp);
    return true;
  }

  double atFrame = ds->frameToIndex(frame, units);

  if (roundUp) {
    if (atFrame < value && frame < maxFrame) {
      ++frame;
    }
  } else {
    if (atFrame > value && frame > 0) {
      --frame;
    }
  }

  frame = qBound(qint64(0), frame, maxFrame);
  ds->unlock();
  *frameOut = frame;
  return true;
}

bool DataRange::unitsFromFrame(qint64 frame, const QString &units,
                               double *valueOut) {
  if (!valueOut) {
    return false;
  }

  if (isFrameUnits(units)) {
    *valueOut = frame;
    return true;
  }

  if (!_dataSource) {
    return false;
  }

  DataSource *ds = _dataSource.data();
  ds->readLock();
  if (!ds->vector().isValid(units)) {
    ds->unlock();
    return false;
  }

  const DataVector::DataInfo info = ds->vector().dataInfo(units);
  const qint64 maxFrame = qMax(qint64(0), qint64(info.frameCount) - 1);
  if (frame <= maxFrame) {
    *valueOut = ds->frameToIndex(frame, units);
  } else {
    // Extrapolate past end of file using average frames-per-index
    const double fps = ds->framePerIndex(units);
    if (fps <= 0.0) {
      ds->unlock();
      return false;
    }
    const double valueAtMax = ds->frameToIndex(maxFrame, units);
    *valueOut = valueAtMax + double(frame - maxFrame) / fps;
  }
  ds->unlock();
  return true;
}

int DataRange::skip() const { return _skip->value(); }

bool DataRange::skipDirty() const { return !_skip->text().isEmpty(); }

void DataRange::setSkip(int skip) { _skip->setValue(skip); }

bool DataRange::countFromEnd() const { return _countFromEnd->isChecked(); }

void DataRange::setCountFromEnd(bool countFromEnd) {
  const bool wasSettingMode = _settingRangeModeProgrammatically;
  _settingRangeModeProgrammatically = true;
  _countFromEnd->setChecked(countFromEnd);
  _settingRangeModeProgrammatically = wasSettingMode;
  updateFields(None);
}

bool DataRange::countFromEndDirty() const {
  return _readToEnd->checkState() == Qt::PartiallyChecked;
}

bool DataRange::readToEnd() const { return _readToEnd->isChecked(); }

bool DataRange::readToEndDirty() const {
  return _readToEnd->checkState() == Qt::PartiallyChecked;
}

void DataRange::setReadToEnd(bool readToEnd) {
  const bool wasSettingMode = _settingRangeModeProgrammatically;
  _settingRangeModeProgrammatically = true;
  _readToEnd->setChecked(readToEnd);
  _settingRangeModeProgrammatically = wasSettingMode;
  updateFields(None);
}

bool DataRange::doSkip() const { return _doSkip->isChecked(); }

bool DataRange::doSkipDirty() const {
  return _doSkip->checkState() == Qt::PartiallyChecked;
}

void DataRange::setDoSkip(bool doSkip) { _doSkip->setChecked(doSkip); }

bool DataRange::doFilter() const { return _doFilter->isChecked(); }

bool DataRange::doFilterDirty() const {
  return _doFilter->checkState() == Qt::PartiallyChecked;
}

void DataRange::setDoFilter(bool doFilter) { _doFilter->setChecked(doFilter); }

void DataRange::countFromEndChanged() {
  if (_settingRangeModeProgrammatically) {
    _canonicalFrameRangeValid = false;
    _canonicalCountFromEndValid = false;
    if (countFromEnd() && readToEnd()) {
      setReadToEnd(false);
    }
    updateFields(None);
    return;
  }

  if (!countFromEnd()) {
    const QString oldRangeText = _range->text();
    const QString oldStartUnits = startUnits();
    const QString oldRangeUnits = rangeUnits();
    bool unitsChangedForDisplay = false;
    qint64 frameCount = _canonicalCountFromEnd;
    bool haveFrameCount = _canonicalCountFromEndValid;
    if (!haveFrameCount) {
      haveFrameCount =
          frameCountFromEndRange(range(), rangeUnits(), &frameCount);
    }

    const qint64 availableFrames = availableFramesForReferenceField();
    if (!haveFrameCount || availableFrames <= 0) {
      QSignalBlocker countFromEndBlocker(_countFromEnd);
      _countFromEnd->setChecked(true);
      updateFields(None);
      return;
    }

    frameCount = qMin(frameCount, availableFrames);
    if (availableFrames >= 2 && frameCount < 2) {
      frameCount = qMin(qint64(2), availableFrames);
    }
    const qint64 startFrame = availableFrames - frameCount;
    const qint64 endFrameExclusive = availableFrames;

    auto canDisplayInterval = [&](const QString &startUnit,
                                  const QString &rangeUnit) {
      double startValue = 0.0;
      double lastValue = 0.0;
      double rangeStartValue = 0.0;
      double rangeEndValue = 0.0;
            return unitsFromFrame(startFrame, startUnit, &startValue) &&
              unitsFromFrame(endFrameExclusive - 1, startUnit, &lastValue) &&
              unitsFromFrame(startFrame, rangeUnit, &rangeStartValue) &&
              unitsFromFrame(endFrameExclusive, rangeUnit, &rangeEndValue) &&
              std::isfinite(startValue) && std::isfinite(lastValue) &&
              std::isfinite(rangeStartValue) && std::isfinite(rangeEndValue) &&
              rangeEndValue >= rangeStartValue;
    };

    QString frameUnits;
    for (const IndexFieldProperties &field : _indexFieldProps) {
      if (field.is_frame) {
        frameUnits = field.name;
        break;
      }
    }

    bool canDisplay = canDisplayInterval(startUnits(), rangeUnits());
    if (!canDisplay && !frameUnits.isEmpty()) {
      QSignalBlocker startUnitsBlocker(_startUnits);
      QSignalBlocker rangeUnitsBlocker(_rangeUnits);
      setStartUnits(frameUnits);
      setRangeUnits(frameUnits);
      _displayedStartUnits = startUnits();
      _displayedRangeUnits = rangeUnits();
      unitsChangedForDisplay = oldStartUnits != startUnits() ||
               oldRangeUnits != rangeUnits();
      canDisplay = canDisplayInterval(startUnits(), rangeUnits());
    }

    if (!canDisplay || !setFrameRange(startFrame, endFrameExclusive)) {
      _canonicalFrameRangeValid = false;
      _canonicalCountFromEnd = frameCount;
      _canonicalCountFromEndValid = true;
      _range->setText(oldRangeText);
      {
        QSignalBlocker startUnitsBlocker(_startUnits);
        QSignalBlocker rangeUnitsBlocker(_rangeUnits);
        setStartUnits(oldStartUnits);
        setRangeUnits(oldRangeUnits);
        _displayedStartUnits = startUnits();
        _displayedRangeUnits = rangeUnits();
      }
      QSignalBlocker countFromEndBlocker(_countFromEnd);
      _countFromEnd->setChecked(true);
      updateFields(None);
      return;
    }

    updateFields(None);
    if (unitsChangedForDisplay) {
      emit indexUnitsChanged();
    }
    return;
  }

  _canonicalFrameRangeValid = false;
  _canonicalCountFromEndValid = false;
  if (countFromEnd()) {
    setReadToEnd(false);
  }

  updateFields(None);
}

void DataRange::readToEndChanged() {
  _canonicalFrameRangeValid = false;
  _canonicalCountFromEndValid = false;
  if (readToEnd()) {
    setCountFromEnd(false);
  }

  updateFields(None);
}

void DataRange::unitsChanged() {
  const bool startUnitsChanged = (sender() == _startUnits);
  const bool rangeUnitsChanged = (sender() == _rangeUnits);
  const QString newStartUnits = startUnits();
  const QString newRangeUnits = rangeUnits();

  if (_displayedStartUnits.isEmpty()) {
    _displayedStartUnits = newStartUnits;
  }
  if (_displayedRangeUnits.isEmpty()) {
    _displayedRangeUnits = newRangeUnits;
  }

  _requestedStartUnits = newStartUnits;
  _requestedRangeUnits = newRangeUnits;

  // Read-to-End keeps Start as a requested boundary, so translate that value
  // when its display units change. Do this independently of finite-range and
  // Count-from-End handling so those modes retain their existing semantics.
  if (startUnitsChanged && _displayedStartUnits != newStartUnits &&
      readToEnd() && !_start->text().isEmpty()) {
    const QString oldUnits = _displayedStartUnits;
    const QString oldText = _start->text();
    const bool startWasDuration = startIsDuration();
    const bool startWasISOTime = startIsISOTime();
    const Kst::IndexFieldProperties *oldField =
        findField(_indexFieldProps, oldUnits);
    const Kst::IndexFieldProperties *newField =
        findField(_indexFieldProps, newStartUnits);

    bool valueOk = false;
    double oldValue = 0.0;
    if (startWasDuration && oldField && oldField->is_seconds) {
      oldValue = parseTimeDuration(oldText);
      valueOk = std::isfinite(oldValue);
    } else if (startWasISOTime && oldField && oldField->is_ctime) {
      oldValue = parseISOTime(oldText);
      valueOk = std::isfinite(oldValue);
    } else {
      oldValue = oldText.toDouble(&valueOk);
      valueOk = valueOk && std::isfinite(oldValue);
    }

    qint64 startFrame = 0;
    double translatedStart = 0.0;
    const bool converted = valueOk &&
        frameFromUnits(oldValue, oldUnits, false, &startFrame) &&
        unitsFromFrame(startFrame, newStartUnits, &translatedStart) &&
        std::isfinite(translatedStart);
    if (!converted) {
      QSignalBlocker startUnitsBlocker(_startUnits);
      setStartUnits(oldUnits);
      _displayedStartUnits = oldUnits;
      _requestedStartUnits = oldUnits;
      updateFields(None);
      return;
    }

    if (startWasISOTime && newField && newField->is_ctime) {
      _start->setText(ctimeToISOString(translatedStart));
    } else if (startWasDuration && newField && newField->is_seconds) {
      _start->setText(secondsToDurationString(translatedStart));
    } else {
      _start->setText(QString::number(translatedStart, 'g', 17));
    }
    _displayedStartUnits = newStartUnits;
    updateFields(None);
    emit indexUnitsChanged();
    return;
  }

  // When start/last hold duration or ISO strings and the start units change,
  // resolve them to plain numbers in the OLD units before the translation block
  // converts those numbers to the new units.
  if (startUnitsChanged && (_displayedStartUnits != newStartUnits)) {
    const Kst::IndexFieldProperties *oldIfp =
        findField(_indexFieldProps, _displayedStartUnits);
    if (oldIfp) {
      if (startIsDuration() && oldIfp->is_seconds) {
        _start->setText(
            QString::number(parseTimeDuration(_start->text()), 'g', 12));
      } else if (startIsISOTime() && oldIfp->is_ctime) {
        _start->setText(QString::number(parseISOTime(_start->text()), 'g', 12));
      }
      if (lastIsDuration() && oldIfp->is_seconds) {
        _last->setText(
            QString::number(parseTimeDuration(_last->text()), 'g', 12));
      } else if (lastIsISOTime() && oldIfp->is_ctime) {
        _last->setText(QString::number(parseISOTime(_last->text()), 'g', 12));
      }
    }
  }

  if (_canonicalFrameRangeValid && displayCanonicalFrameRange()) {
    _displayedStartUnits = newStartUnits;
    _displayedRangeUnits = newRangeUnits;
    emit indexUnitsChanged();
    return;
  }

  if (countFromEnd() && rangeUnitsChanged &&
      _displayedRangeUnits != newRangeUnits) {
    if (!_canonicalCountFromEndValid) {
      bool ok = false;
      double enteredRange = _range->text().toDouble(&ok);
      QString enteredUnits = _displayedRangeUnits;
      if (rangeIsDuration()) {
        enteredRange = parseTimeDuration(_range->text());
        bool durationUnitsFound = false;
        const Kst::IndexFieldProperties *newField =
            findField(_indexFieldProps, newRangeUnits);
        if (newField && newField->is_seconds) {
          enteredUnits = newRangeUnits;
          durationUnitsFound = true;
        } else {
          for (const Kst::IndexFieldProperties &field : _indexFieldProps) {
            if (field.is_seconds) {
              enteredUnits = field.name;
              durationUnitsFound = true;
              break;
            }
          }
        }
        ok = durationUnitsFound;
      }
      qint64 frameCount = 0;
      if (ok && frameCountFromEndRange(enteredRange, enteredUnits,
                                       &frameCount)) {
        _canonicalCountFromEnd = frameCount;
        _canonicalCountFromEndValid = true;
      }
    }

    if (_canonicalCountFromEndValid && displayCanonicalCountFromEnd()) {
      _displayedStartUnits = newStartUnits;
      _displayedRangeUnits = newRangeUnits;
      updateFields(None);
      emit indexUnitsChanged();
      return;
    }

    QSignalBlocker rangeUnitsBlocker(_rangeUnits);
    setRangeUnits(_displayedRangeUnits);
    updateFields(None);
    return;
  }

  if (startUnitsChanged && (_displayedStartUnits != newStartUnits) &&
      !countFromEnd() && !readToEnd() && !_start->text().isEmpty() &&
      !_last->text().isEmpty() && canConvertUnits(_displayedStartUnits) &&
      canConvertUnits(newStartUnits)) {

    qint64 startFrame = 0;
    qint64 lastFrame = 0;
    if (frameFromUnits(start(), _displayedStartUnits, false, &startFrame) &&
        frameFromUnits(last(), _displayedStartUnits, true, &lastFrame)) {
      if (lastFrame < startFrame) {
        lastFrame = startFrame;
      }

      double translatedStart = 0;
      double translatedLast = 0;
      if (unitsFromFrame(startFrame, newStartUnits, &translatedStart) &&
          unitsFromFrame(lastFrame, newStartUnits, &translatedLast)) {
        _start->setText(QString::number(translatedStart, 'g', 12));
        _last->setText(QString::number(translatedLast, 'g', 12));

        // Keep translated start/last fixed and recompute range in the selected
        // range units.
        _controlField0 = Start;
        _controlField1 = Last;
      }
    }
  }

  if (rangeUnitsChanged && (_displayedRangeUnits != newRangeUnits) &&
      !rangeIsDuration() && !countFromEnd() && !readToEnd() &&
      !_start->text().isEmpty() && !_last->text().isEmpty() &&
      canConvertUnits(startUnits()) && canConvertUnits(_displayedRangeUnits) &&
      canConvertUnits(newRangeUnits)) {

    qint64 startFrame = 0;
    qint64 lastFrame = 0;
    if (frameFromUnits(start(), startUnits(), false, &startFrame) &&
        frameFromUnits(last(), startUnits(), true, &lastFrame)) {
      if (lastFrame < startFrame) {
        lastFrame = startFrame;
      }

      double startInRangeUnits = 0;
      double endExclusiveInRangeUnits = 0;
      if (unitsFromFrame(startFrame, newRangeUnits, &startInRangeUnits) &&
          unitsFromFrame(lastFrame + 1, newRangeUnits,
                         &endExclusiveInRangeUnits)) {
        const double translatedRange =
            qMax(0.0, endExclusiveInRangeUnits - startInRangeUnits);
        _range->setText(QString::number(translatedRange, 'g', 12));

        // Keep translated start/last fixed and recompute any dependent fields.
        _controlField0 = Start;
        _controlField1 = Last;
      }
    }
  }

  // When range holds a duration string and the units are switched away from a
  // seconds-compatible field, convert the duration to an equivalent numeric
  // span in the new units (e.g. "1h30m" -> 5400 frames at 1 fps).
  if (rangeUnitsChanged && (_displayedRangeUnits != newRangeUnits) &&
      rangeIsDuration()) {
    const Kst::IndexFieldProperties *oldIfp =
        findField(_indexFieldProps, _displayedRangeUnits);
    if (oldIfp && oldIfp->is_seconds) {
      const double secs = parseTimeDuration(_range->text());
      // Determine span in frames by placing it immediately after frame 0
      // in the old units, then convert that frame count to the new units.
      double originInOldUnits = 0.0;
      unitsFromFrame(0, _displayedRangeUnits, &originInOldUnits);
      qint64 endFrame = 0;
      if (frameFromUnits(originInOldUnits + secs, _displayedRangeUnits, false,
                         &endFrame)) {
        double originInNewUnits = 0.0;
        double endInNewUnits = 0.0;
        if (unitsFromFrame(0, newRangeUnits, &originInNewUnits) &&
            unitsFromFrame(endFrame, newRangeUnits, &endInNewUnits)) {
          _range->setText(QString::number(
              qMax(0.0, endInNewUnits - originInNewUnits), 'g', 12));
        }
      }
    }
  }

  _displayedStartUnits = newStartUnits;
  _displayedRangeUnits = newRangeUnits;
  updateFields(None);
  emit indexUnitsChanged();
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
  _canonicalFrameRangeValid = false;
  // If the user typed a duration string and the current start units are not
  // seconds-compatible, auto-switch to the first seconds-compatible unit.
  if (startIsDuration() && !startIsSeconds()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        setStartUnits(ifp.name);
        break;
      }
    }
  }
  // If the user typed a valid ISO time (not a plain number), auto-switch to
  // the first ctime-compatible unit.
  {
    bool ok = false;
    _start->text().toDouble(&ok);
    if (!ok && startIsISOTime() && !startIsCTime()) {
      for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
        if (ifp.is_ctime) {
          QSignalBlocker blocker(_startUnits);
          _startUnits->setCurrentIndex(_startUnits->findText(ifp.name));
          _displayedStartUnits = ifp.name;
          break;
        }
      }
    }
  }
  updateFields(Start);
  // Keep _last's format in sync with _start.
  if (!_last->text().isEmpty()) {
    if (startIsDuration() && !lastIsDuration() && startIsSeconds()) {
      _last->setText(secondsToDurationString(last()));
    } else if (startIsISOTime() && !lastIsISOTime() && startIsCTime()) {
      _last->setText(ctimeToISOString(last()));
    } else if (!startIsDuration() && !startIsISOTime() &&
               (lastIsDuration() || lastIsISOTime())) {
      _last->setText(QString::number(last(), 'g', 12));
    }
  }
  emit modified();
}

void DataRange::lastChanged() {
  _canonicalFrameRangeValid = false;
  // If the user typed a duration string and the current start units are not
  // seconds-compatible, auto-switch to the first seconds-compatible unit.
  if (lastIsDuration() && !startIsSeconds()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        setStartUnits(ifp.name);
        break;
      }
    }
  }
  // If the user typed a valid ISO time (not a plain number), auto-switch to
  // the first ctime-compatible unit.
  {
    bool ok = false;
    _last->text().toDouble(&ok);
    if (!ok && lastIsISOTime() && !startIsCTime()) {
      for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
        if (ifp.is_ctime) {
          QSignalBlocker blocker(_startUnits);
          _startUnits->setCurrentIndex(_startUnits->findText(ifp.name));
          _displayedStartUnits = ifp.name;
          break;
        }
      }
    }
  }
  updateFields(Last);
  // Keep _start's format in sync with _last.
  if (!_start->text().isEmpty()) {
    if (lastIsDuration() && !startIsDuration() && startIsSeconds()) {
      _start->setText(secondsToDurationString(start()));
    } else if (lastIsISOTime() && !startIsISOTime() && startIsCTime()) {
      _start->setText(ctimeToISOString(start()));
    } else if (!lastIsDuration() && !lastIsISOTime() &&
               (startIsDuration() || startIsISOTime())) {
      _start->setText(QString::number(start(), 'g', 12));
    }
  }
  emit modified();
}

void DataRange::rangeChanged() {
  _canonicalFrameRangeValid = false;
  _canonicalCountFromEndValid = false;
  // If the user typed a duration string and the current range units are not
  // seconds-compatible, auto-switch to the first seconds-compatible unit.
  if (rangeIsDuration() && !rangeIsSeconds()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        setRangeUnits(ifp.name);
        break;
      }
    }
  }
  updateFields(Range);
  emit modified();
}

void DataRange::convertRangeToNumber() {
  if (!rangeIsDuration()) {
    return;
  }
  const double seconds = parseTimeDuration(_range->text());
  _range->setText(QString::number(seconds, 'g', 12));
  updateFields(Range);
  emit modified();
}

void DataRange::convertRangeToDuration() {
  if (rangeIsDuration() || _range->text().isEmpty()) {
    return;
  }
  // Mirror the auto-switch that happens when typing a duration string:
  // if not already on a seconds-compatible unit, switch to the first one.
  // setRangeUnits() triggers unitsChanged() which translates the range value
  // from the old units (e.g. frames) to seconds when start/last are available.
  if (!rangeIsSeconds()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        setRangeUnits(ifp.name);
        break;
      }
    }
  }
  const double seconds = _range->text().toDouble();
  _range->setText(secondsToDurationString(seconds));
  updateFields(Range);
  emit modified();
}

void DataRange::convertStartToNumber() {
  bool changed = false;
  if (startIsDuration()) {
    _start->setText(
        QString::number(parseTimeDuration(_start->text()), 'g', 12));
    changed = true;
  } else if (startIsISOTime()) {
    _start->setText(QString::number(parseISOTime(_start->text()), 'g', 12));
    changed = true;
  }
  if (lastIsDuration()) {
    _last->setText(QString::number(parseTimeDuration(_last->text()), 'g', 12));
    changed = true;
  } else if (lastIsISOTime()) {
    _last->setText(QString::number(parseISOTime(_last->text()), 'g', 12));
    changed = true;
  }
  if (changed) {
    updateFields(Start);
    emit modified();
  }
}

void DataRange::convertStartToDuration() {
  if (_start->text().isEmpty()) {
    return;
  }
  // If not already on a seconds-compatible unit, switch to the first one.
  if (!startIsSeconds()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        setStartUnits(ifp.name);
        break;
      }
    }
  }
  if (!startIsDuration()) {
    _start->setText(secondsToDurationString(_start->text().toDouble()));
  }
  if (!lastIsDuration() && !_last->text().isEmpty()) {
    _last->setText(secondsToDurationString(_last->text().toDouble()));
  }
  updateFields(Start);
  emit modified();
}

void DataRange::convertStartToISOTime() {
  if (_start->text().isEmpty()) {
    return;
  }
  // Save range text — this is a format-only conversion and must not alter
  // range.
  const QString savedRange = _range->text();
  // If not already on a ctime-compatible unit, switch to the first one.
  if (!startIsCTime()) {
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_ctime) {
        setStartUnits(ifp.name);
        break;
      }
    }
  }
  if (!startIsISOTime()) {
    _start->setText(ctimeToISOString(start()));
  }
  if (!lastIsISOTime() && !_last->text().isEmpty()) {
    _last->setText(ctimeToISOString(last()));
  }
  _range->setText(savedRange);
  updateFields(None);
  _range->setText(savedRange);
  emit modified();
}

void DataRange::updateFields(ControlField cf) {
  // Capture format state before any text changes so solved fields can
  // preserve the display format of start/last and range.
  const bool startWasDuration = startIsDuration();
  const bool startWasISO = startIsISOTime();
  const bool lastWasDuration = lastIsDuration();
  const bool lastWasISO = lastIsISOTime();
  const bool rangeWasDuration = rangeIsDuration();

  const bool sameUnits =
      (_rangeUnits->currentIndex() == _startUnits->currentIndex());
  const bool mixedUnitsConvertible =
      canConvertUnits(startUnits()) && canConvertUnits(rangeUnits());
  bool enable_last =
      !readToEnd() && !countFromEnd() && (sameUnits || mixedUnitsConvertible);

  _last->setEnabled(enable_last);
  _lastLabel->setEnabled(enable_last);

  _start->setEnabled(!countFromEnd());
  _startLabel->setEnabled(!countFromEnd());
  _startUnits->setEnabled(!countFromEnd());
  _range->setEnabled(!readToEnd());
  _rangeLabel->setEnabled(!readToEnd());
  _rangeUnits->setEnabled(!readToEnd());

  {
    bool hasSeconds = false;
    bool hasCTime = false;
    for (const Kst::IndexFieldProperties &ifp : _indexFieldProps) {
      if (ifp.is_seconds) {
        hasSeconds = true;
      }
      if (ifp.is_ctime) {
        hasCTime = true;
      }
    }
    _rangeToDuration->setEnabled(!readToEnd() && !rangeIsDuration() &&
                                 !_range->text().isEmpty() && hasSeconds);
    _rangeToNumber->setEnabled(!readToEnd() && rangeIsDuration());

    const bool startHasText = !_start->text().isEmpty();
    const bool startIsSpecial = startIsDuration() || startIsISOTime();
    _startToDuration->setEnabled(!countFromEnd() && startHasText &&
                                 !startIsDuration() && hasSeconds);
    _startToNumber->setEnabled(!countFromEnd() && startIsSpecial);
    _startToISOTime->setEnabled(!countFromEnd() && startHasText &&
                                !startIsISOTime() && hasCTime);
  }

  if ((cf != None) && (cf != _controlField1)) {
    _controlField0 = _controlField1;
    _controlField1 = cf;
  }

  // Determine format for solved start/last values.
  // The edited field (cf) dictates the format; when cf is Range or None the
  // existing start/last format is preserved.
  const bool isSec = startIsSeconds();
  const bool isCT = startIsCTime();
  const bool rSec = rangeIsSeconds();
  enum SLFmt { SLPlain, SLDuration, SLISO };
  SLFmt slFmt = SLPlain;
  if (cf == Start) {
    if (startWasDuration)
      slFmt = SLDuration;
    else if (startWasISO)
      slFmt = SLISO;
  } else if (cf == Last) {
    if (lastWasDuration)
      slFmt = SLDuration;
    else if (lastWasISO)
      slFmt = SLISO;
  } else {
    if (startWasDuration || lastWasDuration)
      slFmt = SLDuration;
    else if (startWasISO || lastWasISO)
      slFmt = SLISO;
  }
  auto formatSL = [&](double value) -> QString {
    if (slFmt == SLDuration && isSec)
      return secondsToDurationString(value);
    if (slFmt == SLISO && isCT)
      return ctimeToISOString(value);
    return QString::number(value, 'g', 12);
  };
  auto formatR = [&](double value) -> QString {
    if (rangeWasDuration && rSec)
      return secondsToDurationString(value);
    return QString::number(value, 'g', 12);
  };

  // don't do anything if it wouldn't make sense to.
  if (readToEnd() || countFromEnd()) {
    return;
  }

  if (sameUnits) {
    if ((_controlField0 != Start) && (_controlField1 != Start)) {
      // Determine the minimum valid start for these units (frame 0 in the file)
      double minStart = 0.0;
      if (!isFrameUnits(startUnits())) {
        double minVal = 0.0;
        if (unitsFromFrame(0, startUnits(), &minVal)) {
          minStart = minVal;
        }
      }
      double startVal = last() - range() + 1;
      if (startVal < minStart) {
        // Range pushes start before the beginning of the file.
        // Keep start at the minimum and push last forward so that
        // start, last, and range remain fully consistent.
        startVal = minStart;
        _last->setText(formatSL(minStart + range() - 1));
      }
      _start->setText(formatSL(startVal));
    } else if ((_controlField0 != Last) && (_controlField1 != Last)) {
      _last->setText(formatSL(start() + range() - 1));
    } else if ((_controlField0 != Range) && (_controlField1 != Range)) {
      _range->setText(formatR(last() - start() + 1));
    }
    return;
  }

  if (!mixedUnitsConvertible) {
    return;
  }

  const bool solveStart =
      (_controlField0 != Start) && (_controlField1 != Start);
  const bool solveLast = (_controlField0 != Last) && (_controlField1 != Last);
  const bool solveRange =
      (_controlField0 != Range) && (_controlField1 != Range);

  if (solveStart) {
    qint64 lastFrame = 0;
    if (!frameFromUnits(last(), startUnits(), true, &lastFrame)) {
      return;
    }

    qint64 startFrame = 0;
    qint64 adjustedEndFrame =
        -1; // >=0 when start was clamped and last must be updated
    if (isFrameUnits(rangeUnits())) {
      const qint64 spanFrames = qMax(qint64(1), qint64(qRound64(range())));
      const qint64 rawStart = lastFrame - spanFrames + 1;
      startFrame = qMax(0, rawStart);
      if (rawStart < 0) {
        // Start would be before the beginning; push last forward instead.
        adjustedEndFrame = startFrame + spanFrames - 1; // = spanFrames - 1
      }
    } else {
      double lastExclusiveInRangeUnits = 0;
      if (!unitsFromFrame(lastFrame + 1, rangeUnits(),
                          &lastExclusiveInRangeUnits)) {
        return;
      }

      const double startInRangeUnits = lastExclusiveInRangeUnits - range();
      double minRangeUnit = 0.0;
      const bool haveMin = unitsFromFrame(0, rangeUnits(), &minRangeUnit);
      if (haveMin && startInRangeUnits < minRangeUnit) {
        // Start would be before the beginning of the file.
        startFrame = 0;
        qint64 exclusiveEndFrame = 0;
        if (frameFromUnits(minRangeUnit + range(), rangeUnits(), true,
                           &exclusiveEndFrame)) {
          adjustedEndFrame = qMax(startFrame, exclusiveEndFrame - 1);
        }
      } else {
        if (!frameFromUnits(startInRangeUnits, rangeUnits(), false,
                            &startFrame)) {
          return;
        }
      }
    }

    double startValue = 0;
    if (!unitsFromFrame(startFrame, startUnits(), &startValue)) {
      return;
    }
    _start->setText(formatSL(startValue));

    if (adjustedEndFrame >= 0) {
      // Update last to stay consistent: start is now at the beginning,
      // range is unchanged, so last moves forward (may be past end of file).
      double lastValue = 0;
      if (unitsFromFrame(adjustedEndFrame, startUnits(), &lastValue)) {
        _last->setText(formatSL(lastValue));
      }
    }
    return;
  }

  if (solveLast) {
    qint64 startFrame = 0;
    if (!frameFromUnits(start(), startUnits(), false, &startFrame)) {
      return;
    }

    qint64 endFrame = startFrame;
    if (isFrameUnits(rangeUnits())) {
      const qint64 spanFrames = qMax(qint64(1), qint64(qRound64(range())));
      // Do NOT clamp endFrame to maxFrame: it is valid and desirable for last
      // to be past the current end of file (file may still be growing).
      endFrame = qMax(0, startFrame + spanFrames - 1);
    } else {
      double startInRangeUnits = 0;
      if (!unitsFromFrame(startFrame, rangeUnits(), &startInRangeUnits)) {
        return;
      }

      qint64 exclusiveEndFrame = 0;
      if (!frameFromUnits(startInRangeUnits + range(), rangeUnits(), true,
                          &exclusiveEndFrame)) {
        return;
      }
      endFrame = qMax(startFrame, exclusiveEndFrame - 1);
    }

    double lastValue = 0;
    if (!unitsFromFrame(endFrame, startUnits(), &lastValue)) {
      return;
    }
    _last->setText(formatSL(lastValue));
    return;
  }

  if (solveRange) {
    qint64 startFrame = 0;
    qint64 endFrame = 0;
    if (!frameFromUnits(start(), startUnits(), false, &startFrame)) {
      return;
    }
    if (!frameFromUnits(last(), startUnits(), true, &endFrame)) {
      return;
    }

    if (endFrame < startFrame) {
      endFrame = startFrame;
    }

    double rangeValue = 1.0;
    if (isFrameUnits(rangeUnits())) {
      rangeValue = endFrame - startFrame + 1.0;
    } else {
      double startInRangeUnits = 0;
      double endExclusiveInRangeUnits = 0;
      if (!unitsFromFrame(startFrame, rangeUnits(), &startInRangeUnits)) {
        return;
      }
      if (!unitsFromFrame(endFrame + 1, rangeUnits(),
                          &endExclusiveInRangeUnits)) {
        return;
      }
      rangeValue = endExclusiveInRangeUnits - startInRangeUnits;
      if (rangeValue < 0.0) {
        rangeValue = 0.0;
      }
    }
    _range->setText(formatR(rangeValue));
  }
}

void DataRange::setWidgetDefaults() {
  dialogDefaults().setValue("vector/range", range());
  dialogDefaults().setValue("vector/start", start());
  dialogDefaults().setValue("vector/countFromEnd", countFromEnd());
  dialogDefaults().setValue("vector/readToEnd", readToEnd());
  dialogDefaults().setValue("vector/skip", skip());
  dialogDefaults().setValue("vector/doSkip", doSkip());
  dialogDefaults().setValue("vector/doAve", doFilter());
  dialogDefaults().setValue("vector/rangeUnits", rangeUnits());
  dialogDefaults().setValue("vector/startUnits", startUnits());
}

void DataRange::loadWidgetDefaults() {
  const qreal defaultRange =
      dialogDefaults().value("vector/range", 1).toDouble();
  const qreal defaultStart =
      dialogDefaults().value("vector/start", 0).toDouble();
  bool defaultCountFromEnd =
      dialogDefaults().value("vector/countFromEnd", false).toBool();
  const bool defaultReadToEnd =
      dialogDefaults().value("vector/readToEnd", true).toBool();
  const int defaultSkip = dialogDefaults().value("vector/skip", 0).toInt();
  const bool defaultDoSkip =
      dialogDefaults().value("vector/doSkip", false).toBool();
  const bool defaultDoFilter =
      dialogDefaults().value("vector/doAve", false).toBool();
  const QString defaultRangeUnits =
      dialogDefaults().value("vector/rangeUnits", tr("frames")).toString();
  const QString defaultStartUnits =
      dialogDefaults().value("vector/startUnits", tr("frames")).toString();

  // Preserve previous behavior where readToEnd wins if both flags were saved
  // true.
  if (defaultReadToEnd && defaultCountFromEnd) {
    defaultCountFromEnd = false;
  }

  {
    QSignalBlocker countFromEndBlocker(_countFromEnd);
    QSignalBlocker readToEndBlocker(_readToEnd);
    QSignalBlocker doSkipBlocker(_doSkip);
    QSignalBlocker doFilterBlocker(_doFilter);
    QSignalBlocker skipBlocker(_skip);
    QSignalBlocker startUnitsBlocker(_startUnits);
    QSignalBlocker rangeUnitsBlocker(_rangeUnits);

    setRange(defaultRange, false);
    setStart(defaultStart, false);
    _countFromEnd->setChecked(defaultCountFromEnd);
    _readToEnd->setChecked(defaultReadToEnd);
    setSkip(defaultSkip);
    _doSkip->setChecked(defaultDoSkip);
    _doFilter->setChecked(defaultDoFilter);
    setRangeUnits(defaultRangeUnits);
    setStartUnits(defaultStartUnits);

    // Mark restored units as already displayed so startup index changes do not
    // trigger conversions before the dialog is shown.
    _displayedStartUnits = startUnits();
    _displayedRangeUnits = rangeUnits();
  }

  doSkipChanged();
  updateFields(None);
}

bool DataRange::rangeIsValid() {
  if (readToEnd()) {
    return true;
  } else if (countFromEnd()) {
    if (_canonicalCountFromEndValid) {
      return _canonicalCountFromEnd > 0;
    }
    const qreal requestedRange = range();
    return std::isfinite(requestedRange) && requestedRange > 0.0;
  } else {
    return (range() > 1);
  }
}

} // namespace Kst

// vim: ts=2 sw=2 et
