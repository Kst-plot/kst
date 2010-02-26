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

#include "plotaxis.h"

#include "math_kst.h"
#include "settings.h"
#include "dialogdefaults.h"

#include <QDate>

#define MAJOR_TICK_DEBUG 0

static int FULL_PRECISION = 15;
static qreal JD1900 = 2415020.5;
static qreal JD1970 = 2440587.5;
static qreal JD_RJD = 2400000.0;
static qreal JD_MJD = 2400000.5;

namespace Kst {

PlotAxis::PlotAxis(PlotItem *plotItem, Qt::Orientation orientation) :
  _plotItem(plotItem),
  _orientation(orientation),
  _dirty(true),
  _axisZoomMode(Auto),
  _isAxisVisible(true),
  _ticksUpdated(true),
  _axisLog(false),
  _axisReversed(false),
  _axisAutoBaseOffset(true),
  _axisBaseOffset(false),
  _axisBaseOffsetOverride(false),
  _axisInterpret(false),
  _axisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS),
  _axisInterpretation(AXIS_INTERP_CTIME),
  _axisMajorTickMode(Normal),
  _axisOverrideMajorTicks(Normal),
  _axisMinorTickCount(4),
  _automaticMinorTicks(true),
  _automaticMinorTickCount(5),
  _axisSignificantDigits(9),
  _drawAxisMajorTicks(true),
  _drawAxisMinorTicks(true),
  _drawAxisMajorGridLines(true),
  _drawAxisMinorGridLines(false),
  _axisMajorGridLineColor(Qt::gray),
  _axisMinorGridLineColor(Qt::gray),
  _axisMajorGridLineStyle(Qt::DashLine),
  _axisMinorGridLineStyle(Qt::DashLine),
  _axisMajorGridLineWidth(1.0),
  _axisMinorGridLineWidth(1.0),
  _axisPlotMarkers(orientation == Qt::Horizontal),
  _labelRotation(0)
 {
  connect(_plotItem, SIGNAL(updateAxes()), this, SLOT(updateTicks()));
}


PlotAxis::~PlotAxis() {
}

void PlotAxis::getTimeUnits(QString &units, double &dayToUnit,
                            AxisInterpretationType axisInterpretation,
                            AxisDisplayType axisDisplay, double base, double lastValue) {
  double value = convertTimeValueToJD(axisInterpretation, base);
  double days = convertTimeValueToJD(axisInterpretation, lastValue) - value;

  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      dayToUnit = 1.0/365.25;
      units = i18n(" [years]");
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      if( days > 10.0*365.0) {
        units = i18n(" [years]");
        dayToUnit = 1.0/365.0;
      } else if( days > 10.0) {
        units = i18n(" [days]");
        dayToUnit = 1.0;
      } else if( days > 10.0/24.0) {
        dayToUnit = 24.0;
        units = i18n(" [hours]");
      } else if( days > 10.0/24.0/60.0 ) {
        dayToUnit = 24.0*60.0;
        units = i18n(" [minutes]");
      } else {
        dayToUnit = 24.0*60.0*60.0;
        units = i18n(" [seconds]");
      }
      break;
    case AXIS_DISPLAY_JD:
    case AXIS_DISPLAY_MJD:
    case AXIS_DISPLAY_RJD:
      dayToUnit = 1.0;
      units = i18n(" [days]");
      break;
  }
}

QString PlotAxis::interpretLabel(AxisInterpretationType axisInterpretation,
                                 AxisDisplayType axisDisplay, double base,
                                 QString units, double units_per_day) {
  double value = convertTimeValueToJD(axisInterpretation, base);
  QString label;

  // print value in appropriate format
  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      value -= JD1900 + 0.5;
      value *= units_per_day;
      value += 1900.0;
      label = i18n("J");
      label += QString::number(value, 'g', FULL_PRECISION-2);
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      label = convertJDToDateString(axisInterpretation, axisDisplay, value);
      break;
    case AXIS_DISPLAY_JD:
      label = i18n("JD");
      label += QString::number(value, 'g', FULL_PRECISION-2);
      break;
    case AXIS_DISPLAY_MJD:
      value -= JD_MJD;
      label = i18n("MJD");
      label += QString::number(value, 'g', FULL_PRECISION-2);
      break;
    case AXIS_DISPLAY_RJD:
      value -= JD_RJD;
      label = i18n("RJD");
      label += QString::number(value, 'g', FULL_PRECISION-2);
      break;
  }
  label += units;
  return label;
}


double PlotAxis::interpretOffset(AxisInterpretationType axisInterpretation,
                                 double base, double value, double units_per_day) {
  double offset;
  offset = value - base;

  offset = convertTimeDiffValueToDays(axisInterpretation, offset) * units_per_day;

  return offset;
}


double PlotAxis::convertTimeValueToJD(AxisInterpretationType axisInterpretation, double valueIn) {
  double value = valueIn;

  switch (axisInterpretation) {
    case AXIS_INTERP_YEAR:
      value -= 1900.0;
      value *= 365.25;
      value += JD1900 + 0.5;
      break;
    case AXIS_INTERP_CTIME:
      value /= 24.0 * 60.0 * 60.0;
      value += JD1970;
      break;
    case AXIS_INTERP_JD:
      break;
    case AXIS_INTERP_MJD:
      value += JD_MJD;
      break;
    case AXIS_INTERP_RJD:
      value += JD_RJD;
      break;
    case AXIS_INTERP_AIT:
      value -= 86400.0 * (365.0 * 12.0 + 3.0);
      // current difference (seconds) between UTC and AIT
      // refer to the following for more information:
      // http://hpiers.obspm.fr/eop-pc/earthor/utc/TAI-UTC_tab.html
      value -= 32.0;
      value /= 24.0 * 60.0 * 60.0;
      value += JD1970;
    default:
      break;
  }

  return value;
}


double PlotAxis::convertTimeDiffValueToDays(AxisInterpretationType axisInterpretation, double offsetIn) {
  double offset = offsetIn;

  switch (axisInterpretation) {
    case AXIS_INTERP_YEAR:
      offset *= 365.25;
      break;
    case AXIS_INTERP_CTIME:
      offset /= 24.0 * 60.0 * 60.0;
      break;
    case AXIS_INTERP_JD:
    case AXIS_INTERP_MJD:
    case AXIS_INTERP_RJD:
      break;
    case AXIS_INTERP_AIT:
      offset /= 24.0 * 60.0 * 60.0;
      break;
    default:
      break;
  }

  return offset;
}


QString PlotAxis::convertJDToDateString(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double dJD) {
  QString label;
  QDate date;

  int accuracy;
  double xdelta = (plotItem()->projectionRect().right()-plotItem()->projectionRect().left())/double(plotItem()->projectionRect().width());
  xdelta = convertTimeDiffValueToDays(axisInterpretation, xdelta);
  xdelta *= 24.0 * 60.0 * 60.0;

  if (xdelta == 0.0) {
    accuracy = FULL_PRECISION;
  } else {
    accuracy = 1 - int(log10(xdelta));
    if (accuracy < 0) {
      accuracy = 0;
    }
  }

  // utcOffset() is returned in seconds... as it must be since
  //  some time zones are not an integer number of hours offset
  //  from UTC...
  dJD += double(Settings::globalSettings()->utcOffset()) / 86400.0;

  // get the date from the Julian day number
  double dJDDay = floor(dJD);
  double dJDFraction = dJD - dJDDay;

  // gregorian calendar correction
  if (dJD >= 2299160.5) {
    double tmp = int( ( (dJDDay - 1867216.0) - 0.25 ) / 36524.25 );
    dJDDay += 1.0 + tmp - floor(0.25*tmp);
  }

  // correction for half day offset
  double dDayFraction = dJDFraction + 0.5;
  if (dDayFraction >= 1.0) {
    dDayFraction -= 1.0;
    dJDDay += 1.0;
  }

  // get time of day from day fraction
  int hour   = int(dDayFraction*24.0);
  int minute = int((dDayFraction*24.0 - double(hour))*60.0);
  double second = ((dDayFraction*24.0 - double(hour))*60.0 - double(minute))*60.0;

  if (accuracy >= 0) {
    second *= pow(10.0, accuracy);
    second  = floor(second+0.5);
    second /= pow(10.0,accuracy);
    if (second >= 60.0) {
      second -= 60.0;
      minute++;
      if (minute == 60) {
        minute = 0;
        hour++;
        if (hour == 24) {
          hour = 0;
        }
      }
    }
  }

  double j2 = dJDDay + 1524.0;
  double j3 = floor(6680.0 + ( (j2 - 2439870.0) - 122.1 )/365.25);
  double j4 = floor(j3 * 365.25);
  double j5 = floor((j2 - j4)/30.6001);

  int day = int(j2 - j4 - floor(j5*30.6001));
  int month = int(j5 - 1.0);
  if (month > 12) {
    month -= 12;
  }
  int year = int(j3 - 4715.0);
  if (month > 2) {
    --year;
  }
  if (year <= 0) {
    --year;
  }
  // check how many decimal places for the seconds we actually need to show
  if (accuracy > 0) {
    QString strSecond;

    strSecond.sprintf("%02.*f", accuracy, second);
    for (int i=strSecond.length()-1; i>0; i--) {
      if (strSecond.at(i) == '0') {
        accuracy--;
      } else if (!strSecond.at(i).isDigit()) {
        break;
      }
    }
  }

  if (accuracy < 0) {
    accuracy = 0;
  }

  QString seconds;
  QString hourminute;
  hourminute.sprintf(" %02d:%02d:", hour, minute);
  seconds.sprintf(" %02.*f", accuracy, second);
  switch (axisDisplay) {
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
      label.sprintf("%d/%02d/%02d", year, month, day);
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
      label.sprintf("%02d/%02d/%d", day, month, year);
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
      date.setYMD(year, month, day);
      label = date.toString(Qt::TextDate).toAscii();
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      date.setYMD(year, month, day);
      label = date.toString(Qt::LocalDate).toAscii();
      label += hourminute + seconds;
      break;
    default:
      break;
  }
  return label;
}


PlotAxis::ZoomMode PlotAxis::axisZoomMode() const {
  return _axisZoomMode;
}


void PlotAxis::setAxisZoomMode(ZoomMode mode) {
  if (_axisZoomMode != mode) {
    _axisZoomMode = mode;
    _dirty = true;
  }
}


bool PlotAxis::axisLog() const {
  return _axisLog;
}


void PlotAxis::setAxisLog(bool log) {
  if (_axisLog != log) {
    _axisLog = log;
    _dirty = true;
  }
}


int PlotAxis::axisSignificantDigits() const {
  return _axisSignificantDigits;
}


void PlotAxis::setAxisSignificantDigits(const int digits) {
  if (_axisSignificantDigits != digits) {
    _axisSignificantDigits = digits;
    _dirty = true;
  }
}


PlotAxis::MajorTickMode PlotAxis::axisMajorTickMode() const {
  return _axisMajorTickMode;
}


void PlotAxis::setAxisMajorTickMode(PlotAxis::MajorTickMode mode) {
  if (_axisMajorTickMode != mode) {
    _axisMajorTickMode = mode;
    _dirty = true;
  }
}


int PlotAxis::axisMinorTickCount() const {
  return _axisMinorTickCount;
}


void PlotAxis::setAxisMinorTickCount(const int count) {
  if (_axisMinorTickCount != count) {
    _axisMinorTickCount = count;
    _dirty = true;
  }
}



bool PlotAxis::axisAutoMinorTicks() const {
  return _automaticMinorTicks;
}


void PlotAxis::setAxisAutoMinorTicks(const bool enabled) {
  if (_automaticMinorTicks != enabled) {
    _automaticMinorTicks = enabled;
    _dirty = true;
  }
}


bool PlotAxis::drawAxisMajorTicks() const {
  return _drawAxisMajorTicks;
}


void PlotAxis::setDrawAxisMajorTicks(bool draw) {
  if (_drawAxisMajorTicks != draw) {
    _drawAxisMajorTicks = draw;
    _dirty = true;
  }
}


bool PlotAxis::drawAxisMinorTicks() const {
  return _drawAxisMinorTicks;
}


void PlotAxis::setDrawAxisMinorTicks(bool draw) {
  if (_drawAxisMinorTicks != draw) {
    _drawAxisMinorTicks = draw;
    _dirty = true;
  }
}


bool PlotAxis::drawAxisMajorGridLines() const {
  return _drawAxisMajorGridLines;
}


void PlotAxis::setDrawAxisMajorGridLines(bool draw) {
  if (_drawAxisMajorGridLines != draw) {
    _drawAxisMajorGridLines = draw;
    _dirty = true;
  }
}


bool PlotAxis::drawAxisMinorGridLines() const {
  return _drawAxisMinorGridLines;
}


void PlotAxis::setDrawAxisMinorGridLines(bool draw) {
  if (_drawAxisMinorGridLines != draw) {
    _drawAxisMinorGridLines = draw;
    _dirty = true;
  }
}


QColor PlotAxis::axisMajorGridLineColor() const {
  return _axisMajorGridLineColor;
}


void PlotAxis::setAxisMajorGridLineColor(const QColor &color) {
  if (_axisMajorGridLineColor != color) {
    _axisMajorGridLineColor = color;
    _dirty = true;
  }
}


QColor PlotAxis::axisMinorGridLineColor() const {
  return _axisMinorGridLineColor;
}


void PlotAxis::setAxisMinorGridLineColor(const QColor &color) {
  if (_axisMinorGridLineColor != color) {
    _axisMinorGridLineColor = color;
    _dirty = true;
  }
}


Qt::PenStyle PlotAxis::axisMajorGridLineStyle() const {
  return _axisMajorGridLineStyle;
}


void PlotAxis::setAxisMajorGridLineStyle(const Qt::PenStyle style) {
  if (_axisMajorGridLineStyle != style) {
    _axisMajorGridLineStyle = style;
    _dirty = true;
  }
}


Qt::PenStyle PlotAxis::axisMinorGridLineStyle() const {
  return _axisMinorGridLineStyle;
}


void PlotAxis::setAxisMinorGridLineStyle(const Qt::PenStyle style) {
  if (_axisMinorGridLineStyle != style) {
    _axisMinorGridLineStyle = style;
    _dirty = true;
  }
}

qreal PlotAxis::axisMajorGridLineWidth() const {
  return _axisMajorGridLineWidth;
}

void PlotAxis::setAxisMajorGridLineWidth(qreal width) {
  if (_axisMajorGridLineWidth != width) {
    _axisMajorGridLineWidth = width;
    _dirty = true;
  }
}

qreal PlotAxis::axisMinorGridLineWidth() const {
  return _axisMinorGridLineWidth;
}

void PlotAxis::setAxisMinorGridLineWidth(qreal width) {
  if (_axisMinorGridLineWidth != width) {
    _axisMinorGridLineWidth = width;
    _dirty = true;
  }
}

bool PlotAxis::isAxisVisible() const {
  return _isAxisVisible;
}


void PlotAxis::setAxisVisible(bool visible) {
  if (_isAxisVisible == visible) {
    return;
  }

  _isAxisVisible = visible;
  _dirty = true;
}


bool PlotAxis::axisReversed() const {
  return _axisReversed;
}


void PlotAxis::setAxisReversed(const bool enabled) {
  if (_axisReversed != enabled) {
    _axisReversed = enabled;
    _dirty = true;
  }
}


bool PlotAxis::axisAutoBaseOffset() const {
  return _axisAutoBaseOffset;
}


void PlotAxis::setAxisAutoBaseOffset(const bool enabled) {
  if (_axisAutoBaseOffset != enabled) {
    _axisAutoBaseOffset = enabled;
    _dirty = true;
  }
}


bool PlotAxis::axisBaseOffset() const {
  return _axisBaseOffset;
}


void PlotAxis::setAxisBaseOffset(const bool enabled) {
  if (_axisBaseOffset != enabled) {
    _axisBaseOffset = enabled;
    _dirty = true;
  }
}


bool PlotAxis::axisInterpret() const {
  return _axisInterpret;
}


void PlotAxis::setAxisInterpret(const bool enabled) {
  if (_axisInterpret != enabled) {
    _axisInterpret = enabled;
    _dirty = true;
  }
}


AxisDisplayType PlotAxis::axisDisplay() const {
  return _axisDisplay;
}


void PlotAxis::setAxisDisplay(const AxisDisplayType display) {
  if (_axisDisplay != display) {
    _axisDisplay = display;
    _dirty = true;
  }
}


AxisInterpretationType PlotAxis::axisInterpretation() const {
  return _axisInterpretation;
}


void PlotAxis::setAxisInterpretation(const AxisInterpretationType display) {
  if (_axisInterpretation != display) {
    _axisInterpretation = display;
    _dirty = true;
  }
}


int PlotAxis::axisLabelRotation() const {
  return _labelRotation;
}


void PlotAxis::setAxisLabelRotation(const int rotation) {
  if (_labelRotation != rotation) {
    _labelRotation = rotation;
    _dirty = true;
  }
}

void PlotAxis::updateLogTicks(MajorTickMode tickMode) {
  QMap<qreal, QString> labels;
  QList<qreal> ticks;
  QList<qreal> minTicks;
  const int format_precision = 5;

  qreal min = _orientation == Qt::Horizontal ? plotItem()->xMin() : plotItem()->yMin();
  qreal max = _orientation == Qt::Horizontal ? plotItem()->xMax() : plotItem()->yMax();

  qreal tick;
  if (max - min <= (double)tickMode*1.5) {
    // show in logarithmic mode with major ticks nicely labelled and the
    // specified number of minor ticks between each major label
    tick = 1.0;
  } else {
    // show in logarithmic mode with major ticks nicely labelled and no minor ticks
    tick = floor((max - min) / (double)tickMode);
  }

  int Low = ceil(min);
  int High = floor(max)+1;
  bool minorLabels = ((High - Low) <= 1);
  for (int i = Low - 1; i <= High; i+=tick) {
    qreal majorPoint = pow(10.0, i);
    if (majorPoint == 0) majorPoint = -350;
    if (i >= min && i <= max) {
      ticks << majorPoint;
      labels.insert(majorPoint, QString::number(majorPoint, 'g', format_precision));
    }

    if (tick == 1.0) {
      // draw minor lines
      bool first = true;
      qreal powMin = pow(10, min), powMax = pow(10, max);
      for (int j = 2; j < 10; j++) {
        qreal minorPoint = majorPoint * j;
        if (minorPoint >= powMin && minorPoint <= powMax) {
          minTicks << minorPoint;
          if (minorLabels && first) {
            labels.insert(minorPoint, QString::number(minorPoint, 'g', format_precision));
            first = false;
          }
        }
      }
    }
  }
  if (minorLabels && minTicks.isEmpty()) {
    qreal lastMinorTick = minTicks.last();
    if (ticks.isEmpty() || ticks.last() < lastMinorTick) {
      if (labels.contains(lastMinorTick)) {
        labels.insert(lastMinorTick, QString::number(lastMinorTick, 'g', format_precision));
      }
    }
  }

  if (_axisMajorTicks == ticks && _axisMinorTicks == minTicks && !_dirty) {
    return;
  }

  _dirty = false;

  _axisMajorTicks = ticks;
  _axisMinorTicks = minTicks;
  _ticksUpdated = true;

  _axisLabels = labels;
  _baseLabel.clear();
}


// Function validates that the labels will not overlap.  Only functions for x Axis.
void PlotAxis::validateDrawingRegion(QPainter *painter) {
  // Always try to use the settings requested.
  if (_axisOverrideMajorTicks != _axisMajorTickMode) {
    _axisBaseOffsetOverride = false;
    updateTicks();
  }

  int flags = Qt::TextSingleLine | Qt::AlignCenter;
  int rotation = axisLabelRotation();
  QTransform t;
  t.rotate(rotation);

  QVector<QPolygonF> labels;
  QMapIterator<qreal, QString> iLabelCheck(_axisLabels);
  while (iLabelCheck.hasNext()) {
    iLabelCheck.next();
    QRectF bound = painter->boundingRect(QRectF(), flags, iLabelCheck.value());
    QPointF p;
    QPolygonF mappedPoly;

    if (rotation == 0) {
      if (_orientation == Qt::Horizontal) {
        p = QPointF(plotItem()->mapXToPlot(iLabelCheck.key()), 0);
      } else {
        p = QPointF(0, plotItem()->mapYToPlot(iLabelCheck.key()));
      }

      bound.moveCenter(p);
      mappedPoly = QPolygonF(bound);
    } else {
      if (_orientation == Qt::Horizontal) {
        p = QPointF(plotItem()->mapXToPlot(iLabelCheck.key()) - bound.height() * 0.5, 0);
      } else {
        p = QPointF(0, plotItem()->mapYToPlot(iLabelCheck.key()) - bound.height() * 0.5);
      }

      mappedPoly = t.map(QPolygonF(bound));
      mappedPoly.translate(p - bound.topLeft());
    }

    labels << mappedPoly;
  }

  for (int i = 0; i < (labels.count() - 1); i++) {
    if (!labels[i].intersected(labels[i+1]).isEmpty()) {
      qreal labelSize;
      qreal plotSize;
      PlotAxis::MajorTickMode old_override_major_ticks = _axisOverrideMajorTicks;

      if (_orientation == Qt::Horizontal) {
        labelSize = qMax(labels[i].boundingRect().width(), labels[i+1].boundingRect().width());
        plotSize = plotItem()->plotRect().width();
      } else {
        labelSize = qMax(labels[i].boundingRect().height(), labels[i+1].boundingRect().height());
        plotSize = plotItem()->plotRect().height();
      }

      _axisOverrideMajorTicks = convertToMajorTickMode((plotSize / labelSize) - 1, old_override_major_ticks);

      if (_axisOverrideMajorTicks == None) {
        qreal scale = plotSize / (labelSize * (Normal - 1));
        if (scale < 1) {
          plotItem()->scaleAxisLabels(scale);
        }
        _axisOverrideMajorTicks = Coarse;
      }

      updateTicks(true);
      break;
    }
  }
  setTicksUpdated();

}


PlotAxis::MajorTickMode PlotAxis::convertToMajorTickMode(int tickCount, PlotAxis::MajorTickMode old_mode) {
  MajorTickMode mode = None;
  if ((tickCount >= VeryFine) && (old_mode > VeryFine)) {
    mode = VeryFine;
  } else if ((tickCount >= Fine) && (old_mode > Fine)) {
    mode = Fine;
  } else if ((tickCount >= Normal) && (old_mode > Normal)) {
    mode = Normal;
  } else if ((tickCount >= Coarse) && (old_mode > Coarse)) {
    mode = Coarse;
  }
  return mode;
}


void PlotAxis::updateTicks(bool useOverrideTicks) {
  MajorTickMode majorTickCount;
  if (useOverrideTicks) {
    majorTickCount = _axisOverrideMajorTicks;
  } else {
    _axisOverrideMajorTicks = _axisMajorTickMode;
    majorTickCount = _axisMajorTickMode;
    _axisBaseOffsetOverride = false;
  }

  plotItem()->updateScale();

  if (_axisLog) {
    updateLogTicks(majorTickCount);
    return;
  }

  QMap<qreal, QString> labels;
  QList<qreal> ticks;
  QList<qreal> minTicks;
  QString time_units;
  double time_units_per_day = 1.0;

  if (_axisLog) {
  } else {
    qreal min = _orientation == Qt::Horizontal ? plotItem()->projectionRect().left() : plotItem()->projectionRect().top();
    qreal max = _orientation == Qt::Horizontal ? plotItem()->projectionRect().right() : plotItem()->projectionRect().bottom();
    qreal majorTickSpacing = computedMajorTickSpacing(majorTickCount, _orientation);
//FIXME: decide on tick spacing based on interpretation!
//FIXME: decide on scale factor, and units... (min, hours, days, weeks, years)
    qreal firstTick = ceil(min / majorTickSpacing) * majorTickSpacing;

    int i = 0;
    qreal nextTick = firstTick;
    while (1) {
      nextTick = firstTick + i++ * majorTickSpacing;
      if (fabs(nextTick)<majorTickSpacing*0.5) { // fix roundoff...
        nextTick = 0.0;
      }
      if (nextTick > max)
        break;
      ticks << nextTick;
      // FULL_PRECISION - 2 because round off errors mean you never actually quite get
      // full precision...
      labels.insert(nextTick, QString::number(nextTick, 'g', FULL_PRECISION-2));
    }

    qreal minorTickSpacing = 0;
    int desiredTicks;
    if (_automaticMinorTicks) {
      desiredTicks = _automaticMinorTickCount;
    } else {
      desiredTicks = _axisMinorTickCount;
    }
    if (desiredTicks > 0) {
      minorTickSpacing = majorTickSpacing / desiredTicks;
    }

    if (minorTickSpacing != 0) {
      qreal firstMinorTick = (firstTick - majorTickSpacing) + minorTickSpacing;

      i = 0;
      qreal nextMinorTick = firstMinorTick;
      while (1) {
        nextMinorTick = firstMinorTick + (i++ * minorTickSpacing);
        if (nextMinorTick > max)
          break;
        if (!ticks.contains(nextMinorTick) && nextMinorTick > min) {
          minTicks << nextMinorTick;
        }
      }
    }
  }

  if (_axisMajorTicks == ticks && _axisMinorTicks == minTicks && !_dirty) {
    return;
  }

  _dirty = false;

  _axisMajorTicks = ticks;
  _axisMinorTicks = minTicks;
  _ticksUpdated = true;

  _axisLabels.clear();
  _baseLabel.clear();

  int longest = 0, shortest = 1000;
  qreal base=10;
  QMapIterator<qreal, QString> iLabel(labels);
  while (iLabel.hasNext()) {
    iLabel.next();
    if (iLabel.value().length() < shortest) {
      shortest = iLabel.value().length();
      base = iLabel.key();
    }
    if (iLabel.value().length() > longest) {
      longest = iLabel.value().length();
    }
  }


  // (shortest > 3) so that you don't use automatic base/offset mode when
  // it wouldn't actually take up less space.
  if (_axisBaseOffset || _axisInterpret || ((longest > _axisSignificantDigits)&&(shortest>3)) || _axisBaseOffsetOverride ) {
    if (_axisInterpret) {
      getTimeUnits(time_units, time_units_per_day, _axisInterpretation, _axisDisplay, (_axisMajorTicks).first(), (_axisMajorTicks).last());
      _baseLabel = interpretLabel(_axisInterpretation, _axisDisplay, base, time_units, time_units_per_day);
    } else {
      _baseLabel = QString::number(base);
    }
    QMapIterator<qreal, QString> i(labels);
    while (i.hasNext()) {
      i.next();
      qreal offset;
      if (_axisInterpret) {
        offset = interpretOffset(_axisInterpretation, base, i.key(), time_units_per_day);
      } else {
        offset = i.key() - base;
      }
      QString label, num;
      if (offset < 0) {
        label += "-[";
        offset = offset * -1;
      } else if (offset > 0) {
        label += "+[";
      }

      if (offset==0.0) {
        num = "[0";
      } else if ((fabs(offset)>9.9E3)||(fabs(offset)<0.99E-3)) {
        num = QString::number(offset, 'e', 1);
      } else {
        num = QString::number(offset, 'g', 5);
      }

      label = label + num + "]";
      _axisLabels.insert(i.key(), label);
    }
  } else {
    _axisLabels = labels;
  }
}


/*
 * Major ticks are always spaced by D = A*10^B where B is an integer,
 * and A is 1, 2 or 5. So: 1, 0.02, 50, 2000 are all possible major tick
 * spacings, but 30 is not.
 *
 * A and B are chosen so that there are as close as possible to M major ticks
 * on the axis (but at least 2). The value of M is set by the requested
 * MajorTickMode.
 */
qreal PlotAxis::computedMajorTickSpacing(MajorTickMode majorTickCount, Qt::Orientation orientation) {
  qreal R = orientation == Qt::Horizontal ? plotItem()->projectionRect().width() : plotItem()->projectionRect().height();
  qreal M = majorTickCount;
  qreal B = floor(log10(R/M));

  qreal d1 = 1 * pow(10, B); // tick spacing
  qreal d2 = 2 * pow(10, B);
  qreal d5 = 5 * pow(10, B);

  qreal r1 = d1 * M; // tick range
  qreal r2 = d2 * M;
  qreal r5 = d5 * M;

  qreal s1 = qAbs(r1 - R);
  qreal s2 = qAbs(r2 - R);
  qreal s5 = qAbs(r5 - R);

  _automaticMinorTickCount = 5;
  if (s1 <= s2 && s1 <= s5) {
    return d1;
  } else if (s2 <= s5) {
    if ((M == 2) && (r2 > R)) {
      return d1; // Minimum ticks not met using d2 using d1 instead
    } else {
      _automaticMinorTickCount = 4;
      return d2;
    }
  } else {
    if ((M == 2) && (r5 > R)) {
      _automaticMinorTickCount = 4;
      return d2; // Minimum ticks not met using d5 using d2 instead
    } else {
      return d5;
    }
  }
}

void PlotAxis::copyProperties(PlotAxis *source) {
  if (source) {
    setAxisVisible(source->isAxisVisible());
    setAxisLog(source->axisLog());
    setAxisReversed(source->axisReversed());
    setAxisBaseOffset(source->axisBaseOffset());
    setAxisInterpret(source->axisInterpret());
    setAxisInterpretation(source->axisInterpretation());
    setAxisDisplay(source->axisDisplay());
    setAxisMajorTickMode(source->axisMajorTickMode());
    setAxisMinorTickCount(source->axisMinorTickCount());
    setAxisAutoMinorTicks(source->axisAutoMinorTicks());
    setDrawAxisMajorTicks(source->drawAxisMajorTicks());
    setDrawAxisMinorTicks(source->drawAxisMinorTicks());
    setDrawAxisMajorGridLines(source->drawAxisMajorGridLines());
    setDrawAxisMinorGridLines(source->drawAxisMinorGridLines());
    setAxisMajorGridLineColor(source->axisMajorGridLineColor());
    setAxisMinorGridLineColor(source->axisMinorGridLineColor());
    setAxisMajorGridLineStyle(source->axisMajorGridLineStyle());
    setAxisMinorGridLineStyle(source->axisMinorGridLineStyle());
    setAxisMajorGridLineWidth(source->axisMinorGridLineWidth());
    setAxisMinorGridLineWidth(source->axisMinorGridLineWidth());
    setAxisSignificantDigits(source->axisSignificantDigits());
    setAxisLabelRotation(source->axisLabelRotation());
    setAxisZoomMode(source->axisZoomMode());
  }
}

void PlotAxis::saveAsDialogDefaults(const QString &group) const {
  _dialogDefaults->setValue(group+"Visible", QVariant(isAxisVisible()).toString());
  _dialogDefaults->setValue(group+"Log", QVariant(axisLog()).toString());
  _dialogDefaults->setValue(group+"Reversed", QVariant(axisReversed()).toString());
  _dialogDefaults->setValue(group+"AutoBaseOffset", QVariant(axisAutoBaseOffset()).toString());
  _dialogDefaults->setValue(group+"BaseOffset", QVariant(axisBaseOffset()).toString());
  _dialogDefaults->setValue(group+"Interpret", QVariant(axisInterpret()).toString());
  _dialogDefaults->setValue(group+"Interpretation", QVariant(axisInterpretation()).toString());
  _dialogDefaults->setValue(group+"Display", QVariant(axisDisplay()).toString());
  _dialogDefaults->setValue(group+"MajorTickMode", QVariant(axisMajorTickMode()).toString());
  _dialogDefaults->setValue(group+"MinorTickCount", QVariant(axisMinorTickCount()).toString());
  _dialogDefaults->setValue(group+"AutoMinorTickCount", QVariant(axisAutoMinorTicks()).toString());
  _dialogDefaults->setValue(group+"DrawMajorTicks", QVariant(drawAxisMajorTicks()).toString());
  _dialogDefaults->setValue(group+"DrawMajorTicks", QVariant(drawAxisMajorTicks()).toString());
  _dialogDefaults->setValue(group+"DrawMinorTicks", QVariant(drawAxisMinorTicks()).toString());
  _dialogDefaults->setValue(group+"DrawMajorGridLines", QVariant(drawAxisMajorGridLines()).toString());
  _dialogDefaults->setValue(group+"DrawMinorGridLines", QVariant(drawAxisMinorGridLines()).toString());
  _dialogDefaults->setValue(group+"DrawMajorGridLinecolor", QVariant(axisMajorGridLineColor()).toString());
  _dialogDefaults->setValue(group+"DrawMinorGridLinecolor", QVariant(axisMinorGridLineColor()).toString());
  _dialogDefaults->setValue(group+"DrawMajorGridLinestyle", QVariant(axisMajorGridLineStyle()).toString());
  _dialogDefaults->setValue(group+"DrawMinorGridLinestyle", QVariant(axisMinorGridLineStyle()).toString());
  _dialogDefaults->setValue(group+"DrawMajorGridLinewidth", QVariant(axisMajorGridLineWidth()).toString());
  _dialogDefaults->setValue(group+"DrawMinorGridLinewidth", QVariant(axisMinorGridLineWidth()).toString());
  _dialogDefaults->setValue(group+"SignificantDigits", QVariant(axisSignificantDigits()).toString());
  _dialogDefaults->setValue(group+"Rotation", QVariant(axisLabelRotation()).toString());
}

void PlotAxis::saveInPlot(QXmlStreamWriter &xml, QString axisId) {
  xml.writeStartElement("plotaxis");
  xml.writeAttribute("id", axisId);
  xml.writeAttribute("visible", QVariant(isAxisVisible()).toString());
  xml.writeAttribute("log", QVariant(axisLog()).toString());
  xml.writeAttribute("reversed", QVariant(axisReversed()).toString());
  xml.writeAttribute("autobaseoffset", QVariant(axisAutoBaseOffset()).toString());
  xml.writeAttribute("baseoffset", QVariant(axisBaseOffset()).toString());
  xml.writeAttribute("interpret", QVariant(axisInterpret()).toString());
  xml.writeAttribute("interpretation", QVariant(axisInterpretation()).toString());
  xml.writeAttribute("display", QVariant(axisDisplay()).toString());
  xml.writeAttribute("majortickmode", QVariant(axisMajorTickMode()).toString());
  xml.writeAttribute("minortickcount", QVariant(axisMinorTickCount()).toString());
  xml.writeAttribute("autominortickcount", QVariant(axisAutoMinorTicks()).toString());
  xml.writeAttribute("drawmajorticks", QVariant(drawAxisMajorTicks()).toString());
  xml.writeAttribute("drawminorticks", QVariant(drawAxisMinorTicks()).toString());
  xml.writeAttribute("drawmajorgridlines", QVariant(drawAxisMajorGridLines()).toString());
  xml.writeAttribute("drawminorgridlines", QVariant(drawAxisMinorGridLines()).toString());
  xml.writeAttribute("drawmajorgridlinecolor", QVariant(axisMajorGridLineColor()).toString());
  xml.writeAttribute("drawminorgridlinecolor", QVariant(axisMinorGridLineColor()).toString());
  xml.writeAttribute("drawmajorgridlinestyle", QVariant(axisMajorGridLineStyle()).toString());
  xml.writeAttribute("drawminorgridlinestyle", QVariant(axisMinorGridLineStyle()).toString());
  xml.writeAttribute("drawmajorgridlinewidth", QVariant(axisMajorGridLineWidth()).toString());
  xml.writeAttribute("drawminorgridlinewidth", QVariant(axisMinorGridLineWidth()).toString());
  xml.writeAttribute("significantdigits", QVariant(axisSignificantDigits()).toString());
  xml.writeAttribute("rotation", QVariant(axisLabelRotation()).toString());
  xml.writeAttribute("zoommode", QVariant(axisZoomMode()).toString());
  _axisPlotMarkers.saveInPlot(xml);
  xml.writeEndElement();
}


bool PlotAxis::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av = attrs.value("visible");
  if (!av.isNull()) {
    setAxisVisible(QVariant(av.toString()).toBool());
  }
  av = attrs.value("log");
  if (!av.isNull()) {
    setAxisLog(QVariant(av.toString()).toBool());
  }
  av = attrs.value("reversed");
  if (!av.isNull()) {
    setAxisReversed(QVariant(av.toString()).toBool());
  }
  av = attrs.value("autobaseoffset");
  if (!av.isNull()) {
    setAxisAutoBaseOffset(QVariant(av.toString()).toBool());
  }
  av = attrs.value("baseoffset");
  if (!av.isNull()) {
    setAxisBaseOffset(QVariant(av.toString()).toBool());
  }
  av = attrs.value("interpret");
  if (!av.isNull()) {
    setAxisInterpret(QVariant(av.toString()).toBool());
  }
  av = attrs.value("interpretation");
  if (!av.isNull()) {
    setAxisInterpretation((AxisInterpretationType)QVariant(av.toString()).toInt());
  }
  av = attrs.value("display");
  if (!av.isNull()) {
    setAxisDisplay((AxisDisplayType)QVariant(av.toString()).toInt());
  }
  av = attrs.value("majortickmode");
  if (!av.isNull()) {
    setAxisMajorTickMode((PlotAxis::MajorTickMode)QVariant(av.toString()).toInt());
  }
  av = attrs.value("minortickcount");
  if (!av.isNull()) {
    setAxisMinorTickCount(QVariant(av.toString()).toInt());
  }
  av = attrs.value("autominortickcount");
  if (!av.isNull()) {
    setAxisAutoMinorTicks(QVariant(av.toString()).toBool());
  }
  av = attrs.value("drawmajorticks");
  if (!av.isNull()) {
    setDrawAxisMajorTicks(QVariant(av.toString()).toBool());
  }
  av = attrs.value("drawminorticks");
  if (!av.isNull()) {
    setDrawAxisMinorTicks(QVariant(av.toString()).toBool());
  }
  av = attrs.value("drawmajorgridlines");
  if (!av.isNull()) {
    setDrawAxisMajorGridLines(QVariant(av.toString()).toBool());
  }
  av = attrs.value("drawminorgridlines");
  if (!av.isNull()) {
    setDrawAxisMinorGridLines(QVariant(av.toString()).toBool());
  }
  av = attrs.value("drawmajorgridlinecolor");
  if (!av.isNull()) {
    setAxisMajorGridLineColor(QColor(av.toString()));
  }
  av = attrs.value("drawminorgridlinecolor");
  if (!av.isNull()) {
    setAxisMinorGridLineColor(QColor(av.toString()));
  }
  av = attrs.value("drawmajorgridlinestyle");
  if (!av.isNull()) {
    setAxisMajorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
  }
  av = attrs.value("drawminorgridlinestyle");
  if (!av.isNull()) {
    setAxisMinorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
  }
  av = attrs.value("drawmajorgridlinewidth");
  if (!av.isNull()) {
    setAxisMajorGridLineWidth((qreal)QVariant(av.toString()).toDouble());
  }
  av = attrs.value("drawminorgridlinewidth");
  if (!av.isNull()) {
    setAxisMinorGridLineWidth((qreal)QVariant(av.toString()).toDouble());
  }
  av = attrs.value("significantdigits");
  if (!av.isNull()) {
    setAxisSignificantDigits(QVariant(av.toString()).toInt());
  }
  av = attrs.value("rotation");
  if (!av.isNull()) {
    setAxisLabelRotation(QVariant(av.toString()).toInt());
  }
  av = attrs.value("zoommode");
  if (!av.isNull()) {
    setAxisZoomMode((PlotAxis::ZoomMode)av.toString().toInt());
  }

  QString expectedEnd;
  while (!(xml.isEndElement() && (xml.name().toString() == primaryTag))) {
   if (xml.isStartElement() && xml.name().toString() == "plotmarkers") {
      validTag = _axisPlotMarkers.configureFromXml(xml, store);
    } else if (xml.isEndElement()) {
      if (xml.name().toString() != expectedEnd) {
        validTag = false;
        break;
      }
    }
    xml.readNext();
  }

  return validTag;
}


}

// vim: ts=2 sw=2 et
