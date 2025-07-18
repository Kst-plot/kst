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

#include "plotaxis.h"

#include "math_kst.h"
#include "dialogdefaults.h"

#include <QDate>
#include <limits>

#define MAJOR_TICK_DEBUG 0

using namespace std;

static int FULL_PRECISION = 15;
static qreal JD1900 = 2415020.5;
static qreal JD1899_12_30 = 2415018.5;
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
  _ns_zoom_level(0),
  _ticksUpdated(true),
  _axisLog(false),
  _axisReversed(false),
  _axisAutoBaseOffset(true),
  _axisBaseOffset(false),
  _axisBaseOffsetOverride(false),
  _axisForceOffsetMin(false),
  _axisInterpret(false),
  _axisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS),
  _axisDisplayFormatString("hh:mm:ss.zzz"),
  _axisInterpretation(AXIS_INTERP_CTIME),
  _axisMajorTickMode(TicksNormal),
  _axisOverrideMajorTicks(TicksNormal),
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

double PlotAxis::convertJDtoDisplayTime(double T) {
  switch (_axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      T -= JD1900 + 0.5;
      T *= 1.0/365.25; // FIXME: make sure this is right
      T += 1900.0;
      break;
    case AXIS_DISPLAY_JD:
      break;
    case AXIS_DISPLAY_MJD:
      T -= JD_MJD;
      break;
    case AXIS_DISPLAY_RJD:
      T -= JD_RJD;
      break;
    default:
      break;
  }

  return T;
}


double PlotAxis::convertJDtoCTime(double jdIn) {
  jdIn -= (JD1970);
  jdIn *= 24.0*3600.0;

  if (jdIn > double(std::numeric_limits<time_t>::max())-1.0) {
    jdIn = double(std::numeric_limits<time_t>::max())-1.0;
  }
  if (jdIn<0) {
    jdIn = 0.0;
  }
  return (jdIn);
}


double PlotAxis::convertTimeValueToJD(double valueIn) {
  double value = valueIn;

  switch (_axisInterpretation) {
    case AXIS_INTERP_YEAR:
      value -= 1900.0;
      value *= 365.25;  // FIXME: seems wrong
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
    case AXIS_INTERP_EXCEL:
      value += JD1899_12_30;
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


QString PlotAxis::convertJDToDateString(double jd, double range_jd) {
  QString label;
  QDate date;

  int accuracy;
  range_jd *= 24.0 * 60.0 * 60.0;


  if (range_jd == 0.0) {
    accuracy = FULL_PRECISION;
  } else {
    accuracy = 1 - int(log10(range_jd));
    if (accuracy < 0) {
      accuracy = 0;
    }
  }

  // gmtOffset() is returned in seconds... as it must be since
  //  some time zones are not an integer number of hours offset
  //  from UTC...
  jd += (_timeZone.gmtOffset(convertJDtoCTime(jd)))/(3600.0*24.0);

  // get the date from the Julian day number
  double jd_day = floor(jd);
  double jd_fraction = jd - jd_day;

  // gregorian calendar correction
  if (jd >= 2299160.5) {
    double tmp = int( ( (jd_day - 1867216.0) - 0.25 ) / 36524.25 );
    jd_day += 1.0 + tmp - floor(0.25*tmp);
  }

  // correction for half day offset
  double dDayFraction = jd_fraction + 0.5;
  if (dDayFraction >= 1.0) {
    dDayFraction -= 1.0;
    jd_day += 1.0;
  }

  // get time of day from day fraction
  int hour   = int(dDayFraction*24.0);
  int minute = int((dDayFraction*24.0 - double(hour))*60.0);
  double second = ((dDayFraction*24.0 - double(hour))*60.0 - double(minute))*60.0;
  double fullseconds;
  double millisec = modf(second + 0.0005, &fullseconds) * 1000;

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

  double j2 = jd_day + 1524.0;
  double j3 = floor(6680.0 + ( (j2 - 2439870.0) - 122.1 )/365.25);
  double j4 = floor(j3 * 365.25);
  double j5 = floor((j2 - j4)/30.6001);

  double day_d = floor(j2 - j4 - floor(j5*30.6001));
  int day = int(qBound(-double(std::numeric_limits<int>::max()-1), day_d, double(std::numeric_limits<int>::max())));

  double month_d = floor(j5-1.0);
  int month = int(qBound(-double(std::numeric_limits<int>::max()-1), month_d, double(std::numeric_limits<int>::max())));
  if (month > 12) {
    month -= 12;
  }

  double year_d = floor(j3 - 4715.0);
  int year = int(qBound(-double(std::numeric_limits<int>::max()-1), year_d, double(std::numeric_limits<int>::max())));
  if (month > 2) {
    --year;
  }
  if (year <= 0) {
    --year;
  }
  // check how many decimal places for the seconds we actually need to show
  if (accuracy > 0) {
    QString strSecond;

    strSecond.asprintf("%02.*f", accuracy, second);
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
  hourminute = QString::asprintf(" %02d:%02d:", hour, minute);
  seconds = QString::asprintf("%02.*f", accuracy, second);
  switch (_axisDisplay) {
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
      label.asprintf("%d/%02d/%02d", year, month, day);
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
      label.asprintf("%02d/%02d/%d", day, month, year);
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
      date.setDate(year, month, day);
      label = date.toString(Qt::TextDate).toLatin1();
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      date.setDate(year, month, day);
      label = QLocale::system().toString(date, QLocale::ShortFormat); //date.toString(Qt::LocalDate).toLatin1();
      label += hourminute + seconds;
      break;
    case AXIS_DISPLAY_QTDATETIME_FORMAT:
      label += QDateTime(QDate(year, month, day), QTime(hour, minute, second, millisec))
                        .toString(_axisDisplayFormatString);
      break;
    default:
      label = QString::number(convertJDtoDisplayTime(jd), 'G', FULL_PRECISION-2);
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
    _ns_zoom_level = 0;
    _dirty = true;
  } else if (mode == PlotAxis::SpikeInsensitive) {
    _ns_zoom_level = (_ns_zoom_level+1)%5;
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


MajorTickMode PlotAxis::axisMajorTickMode() const {
  return _axisMajorTickMode;
}


void PlotAxis::setAxisMajorTickMode(MajorTickMode mode) {
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


void PlotAxis::setAxisForceOffsetMin(bool enabled) {
  if (_axisForceOffsetMin != enabled) {
     _axisForceOffsetMin = enabled;
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


QString PlotAxis::timezoneName() const {
  return _timeZone.tzName();
}


void PlotAxis::setTimezoneName(QString timezone) {
  if (_timeZone.tzName() != timezone) {
    _timeZone.setTZ(timezone);
    _dirty = true;
  }
}


AxisDisplayType PlotAxis::axisDisplay() const {
  return _axisDisplay;
}

QString PlotAxis::axisDisplayFormatString() const {
  return _axisDisplayFormatString;
}


void PlotAxis::setAxisDisplay(const AxisDisplayType display) {
  if (_axisDisplay != display) {
    _axisDisplay = display;
    _dirty = true;
  }
}

void PlotAxis::setAxisDisplayFormatString(const QString& formatString) {
  if (_axisDisplayFormatString != formatString) {
    _axisDisplayFormatString = formatString;
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

// Function validates that the labels will not overlap.  Only functions for X-axis.
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
  QMapIterator<double, QString> iLabelCheck(_axisLabels);
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
      MajorTickMode old_override_major_ticks = _axisOverrideMajorTicks;

      if (_orientation == Qt::Horizontal) {
        labelSize = qMax(labels[i].boundingRect().width(), labels[i+1].boundingRect().width());
        plotSize = plotItem()->plotRect().width();
      } else {
        labelSize = qMax(labels[i].boundingRect().height(), labels[i+1].boundingRect().height());
        plotSize = plotItem()->plotRect().height();
      }

      _axisOverrideMajorTicks = convertToMajorTickMode((plotSize / labelSize) - 1, old_override_major_ticks);

      if (_axisOverrideMajorTicks == TicksNone) {
        qreal scale = plotSize / (labelSize * (TicksNormal - 1));
        if (scale < 1) {
          plotItem()->scaleAxisLabels(scale);
        }
        _axisOverrideMajorTicks = TicksCoarse;
      }

      updateTicks(true);
      break;
    }
  }
  setTicksUpdated();

}


MajorTickMode PlotAxis::convertToMajorTickMode(int tickCount, MajorTickMode old_mode) {
  MajorTickMode mode = TicksNone;
  if ((tickCount >= TicksVeryFine) && (old_mode > TicksVeryFine)) {
    mode = TicksVeryFine;
  } else if ((tickCount >= TicksFine) && (old_mode > TicksFine)) {
    mode = TicksFine;
  } else if ((tickCount >= TicksNormal) && (old_mode > TicksNormal)) {
    mode = TicksNormal;
  } else if ((tickCount >= TicksCoarse) && (old_mode > TicksCoarse)) {
    mode = TicksCoarse;
  }
  return mode;
}


void PlotAxis::updateLogTicks(MajorTickMode tickMode) {
  QMap<double, QString> labels;
  QList<double> ticks;
  QList<double> minTicks;
  const int format_precision = 5;

  double min = _orientation == Qt::Horizontal ? plotItem()->xMin() : plotItem()->yMin();
  double max = _orientation == Qt::Horizontal ? plotItem()->xMax() : plotItem()->yMax();

  double tick;
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
    double majorPoint = pow(10.0, i);
    if (majorPoint == 0) majorPoint = -350;
    if (i >= min && i <= max) {
      ticks << majorPoint;
      // 'x' is a sign to the plot item to write 10^i rather than 1.0x10^i
      labels.insert(majorPoint, QString("xe%1").arg(i));
    }

    if (tick == 1.0) {
      // draw minor lines
      bool first = true;
      double powMin = pow(10, min), powMax = pow(10, max);
      for (int j = 2; j < 10; j++) {
        double minorPoint = majorPoint * j;
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
  if (minorLabels && !minTicks.isEmpty()) {
    double lastMinorTick = minTicks.last();
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

// returns true if axis is linear ticks
// with no specially formatted time.
bool PlotAxis::isLinearTickMode() {
  if (_axisLog) {
    return false;
  }

  if (_axisInterpret) {
    switch (_axisDisplay) {
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTDATETIME_FORMAT:
      return false;
    default:
      return true;
    }
  }
  return true;
}

void PlotAxis::updateInterpretTicks(MajorTickMode tickMode) {
  double min;
  double max;

  if (_orientation == Qt::Horizontal) {
    min = plotItem()->projectionRect().left();
    max = plotItem()->projectionRect().right();
  } else {
    min = plotItem()->projectionRect().top();
    max = plotItem()->projectionRect().bottom();
  }

  double range = max - min;
  double min_jd = convertTimeValueToJD(min);
  double max_jd = convertTimeValueToJD(max);
  double range_jd = fabs(max_jd - min_jd);
  double base_jd;
  double range_u;
  double tickspacing_u;
  double tickspacing;
  QString units;

  double minimum_units = tickMode;

  // find base_jd, range_u, units
  //double valid = true;
  if (range_jd > minimum_units*365.0*1.0e150) {
    _ticksUpdated = true;
    _axisLabels.clear();
    _axisMinorTicks.clear();
    _axisMajorTicks.clear();
    _baseLabel.clear();
    return;

  } else if (range_jd > minimum_units*365.0) {
    // use years
    range_u = range_jd/365.25;
    units = tr(" [Years]");
    computeMajorTickSpacing(&tickspacing_u, &_automaticMinorTickCount, tickMode, range_u);
    // round base to year;
    base_jd = floor((min_jd - (JD1900 + 0.5))/365.25) * 365.25 + (JD1900 + 0.5) + 1.0;
  } else if (range_jd > minimum_units) {
    // use days
    range_u = range_jd;
    units = tr(" [Days]");
    computeMajorTickSpacing(&tickspacing_u, &_automaticMinorTickCount, tickMode, range_u);
    // round base to day
    base_jd = floor(min_jd)+1.0;
  } else if (range_jd > minimum_units/24.0) {
    // use hours
    range_u = range_jd*24.0;
    units = tr(" [Hours]");
    computeMajorTickSpacing(&tickspacing_u, &_automaticMinorTickCount, tickMode, range_u, Hour);
    // round base to hour
    double d_jd = min_jd - floor(min_jd);
    base_jd = floor(min_jd) + (floor(d_jd*24.0/tickspacing_u)+1.0)/(24.0/tickspacing_u);
  } else if (range_jd > minimum_units/(24.0*60.0)) {
    // use minutes
    range_u = range_jd*24.0*60.0;
    units = tr(" [Minutes]");
    computeMajorTickSpacing(&tickspacing_u, &_automaticMinorTickCount, tickMode, range_u, Minute);
    double d_jd = min_jd - floor(min_jd);
    base_jd = floor(min_jd) + (floor(d_jd*24.0*60.0/tickspacing_u)+1.0)/(24.0*60.0/tickspacing_u);
  } else {
    // use seconds
    range_u = range_jd*24.0*3600.0;
    units = tr(" [Seconds]");
    double d_jd = min_jd - floor(min_jd);
    computeMajorTickSpacing(&tickspacing_u, &_automaticMinorTickCount, tickMode, range_u, Second);
    base_jd = floor(min_jd) + (floor(d_jd*24.0*3600.0/tickspacing_u)+1.0)/(24.0*3600.0/tickspacing_u);
    if (base_jd < min_jd) base_jd = min_jd;
    if (base_jd > max_jd) base_jd = min_jd;
  }

  //TODO Why could range_u be 0? Then it hangs in while(1)
  if (range_u != 0)
    tickspacing = tickspacing_u * range/range_u;
  else
    tickspacing = range;



  if (_axisForceOffsetMin) {
    base_jd = min_jd;
  }
  double base = (base_jd - min_jd) * range/range_jd + min;

  int i0 = -floor((base-min)/tickspacing);

  double tick;
  double first_tick;
  QMap<double, QString> labels;
  QList<double> ticks;
  QList<double> minTicks;

  QString tick_label;
  first_tick = base+i0*tickspacing;
  for (int i_tick = i0; base + i_tick*tickspacing<=max; i_tick++) {
    tick = base+i_tick*tickspacing;
    ticks << tick;
    tick_label = '[' + QString::number(i_tick*tickspacing_u, 'g', FULL_PRECISION-2) + ']';
    labels.insert(tick, tick_label);
  }

  double minorTickSpacing = 0.0;
  int desiredTicks;
  if (_automaticMinorTicks) {
    desiredTicks = _automaticMinorTickCount;
  } else {
    desiredTicks = _axisMinorTickCount;
  }
  if (desiredTicks > 0) {
    minorTickSpacing = tickspacing / double(desiredTicks);
  }

  if (minorTickSpacing != 0) {
    double firstMinorTick = (first_tick - tickspacing) + minorTickSpacing;

    int i_minor = 0;
    double nextMinorTick = firstMinorTick;
    while (1) {
      nextMinorTick = firstMinorTick + (i_minor++ * minorTickSpacing);
      if (nextMinorTick > max || isnan(nextMinorTick))
        break;
      if (!ticks.contains(nextMinorTick) && (nextMinorTick > min)) {
        minTicks << nextMinorTick;
      }
    }
  }

  if (_axisMajorTicks == ticks && _axisMinorTicks == minTicks && !_dirty) {
    _ticksUpdated = false;
    return;
  }

  _axisLabels = labels;
  _axisMinorTicks = minTicks;
  _axisMajorTicks = ticks;
  _baseLabel = convertJDToDateString(base_jd, range_jd) + units;
  _dirty = false;
  _ticksUpdated = true;

}

QString PlotAxis::statusBarString(double X) {
  if (_axisInterpret) {
    double X_jd = convertTimeValueToJD(X);
    double min;
    double max;

    if (_orientation == Qt::Horizontal) {
      min = plotItem()->projectionRect().left();
      max = plotItem()->projectionRect().right();
    } else {
      min = plotItem()->projectionRect().top();
      max = plotItem()->projectionRect().bottom();
    }

    double min_jd = convertTimeValueToJD(min);
    double max_jd = convertTimeValueToJD(max);
    double range_jd = fabs(max_jd - min_jd);

    return convertJDToDateString(X_jd, range_jd/100.0);
  } else {
    return QString::number(X, 'G', FULL_PRECISION-2);
  }
}


void PlotAxis::updateLinearTicks(MajorTickMode tickMode) {
  QMap<double, QString> labels;
  QList<double> ticks;
  QList<double> minTicks;
  double min;
  double max;
  double R;
  double uR; // range in interpreted units
  double uMin; // min and max in interpreted units
  double uMax;
  double drdu = 1.0; // interpreted units per raw units;
  double rOffset = 0.0; // r = drdu*u + rOffset;
  double uMajorTickSpacing; // major Tick spacing in iterpreted units

  if (_orientation == Qt::Horizontal) {
    min = plotItem()->projectionRect().left();
    max = plotItem()->projectionRect().right();
    R = plotItem()->projectionRect().width();
  } else {
    min = plotItem()->projectionRect().top();
    max = plotItem()->projectionRect().bottom();
    R = plotItem()->projectionRect().height();
  }

  if (_axisInterpret) {
    uMin = convertJDtoDisplayTime(convertTimeValueToJD(min));
    uMax = convertJDtoDisplayTime(convertTimeValueToJD(max));
    uR = fabs(uMax - uMin);
    drdu = (max - min)/(uMax - uMin);
    rOffset = min - drdu * uMin;
  } else {
    uR = R;
    uMin = min;
    uMax = max;
  }

  computeMajorTickSpacing(&uMajorTickSpacing, &_automaticMinorTickCount, tickMode, uR);

  double uFirstTick;
  bool offset_is_min = (_axisInterpret || _axisBaseOffset || _axisBaseOffsetOverride ) && (_axisForceOffsetMin);
  if (offset_is_min) {
    uFirstTick = uMin;
  } else {
    uFirstTick = ceil(uMin / uMajorTickSpacing) * uMajorTickSpacing;
  }
  double firstTick = uFirstTick*drdu + rOffset;
  double majorTickSpacing = uMajorTickSpacing * drdu;

  int i = 0;
  double lastTick = 12345678;
  while (1) {
    double uNextTick = uFirstTick + i++ * uMajorTickSpacing;
    if (fabs(uNextTick)<uMajorTickSpacing*0.5) { // fix roundoff...
      uNextTick = 0.0;
    }
    if (uNextTick > uMax)
      break;    
    double nextTick = uNextTick * drdu + rOffset;
    if (lastTick == uNextTick) // prevent endless loop
      break;
    lastTick = nextTick;
    ticks << nextTick;
    // FULL_PRECISION - 2 because round off errors mean you never actually quite get
    // full precision...
    labels.insert(nextTick, QString::number(uNextTick, 'g', FULL_PRECISION-2));
  }

  double minorTickSpacing = 0;
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
    double firstMinorTick = (firstTick - majorTickSpacing) + minorTickSpacing;

    i = 0;
    double nextMinorTick = firstMinorTick;
    while (1) {
      nextMinorTick = firstMinorTick + (i++ * minorTickSpacing);
      if (nextMinorTick > max)
        break;
      if (!ticks.contains(nextMinorTick) && nextMinorTick > min) {
        minTicks << nextMinorTick;
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
  double base=10;
  QMapIterator<double, QString> iLabel(labels);
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
  if (offset_is_min) {
    base = ticks[0];
  }

  // (shortest > 3) so that you don't use automatic base/offset mode when
  // it wouldn't actually take up less space.
  if (_axisBaseOffset || ((longest > _axisSignificantDigits)&&(shortest>3)) || _axisBaseOffsetOverride ) {
    _baseLabel = QString::number(base, 'g', FULL_PRECISION-2);
    QMapIterator<double, QString> i(labels);
    while (i.hasNext()) {
      i.next();
      double offset;
      offset = i.key() - base;
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

      label = label + num + ']';
      _axisLabels.insert(i.key(), label);
    }
  } else {
    _axisLabels = labels;
  }
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
  } else if (isLinearTickMode()) {
    updateLinearTicks(majorTickCount);
    return;
  } else {
    updateInterpretTicks(majorTickCount);
    return;
  }
}


void PlotAxis::copyProperties(PlotAxis *source) {
  if (source) {
    setAxisVisible(source->isAxisVisible());
    setAxisLog(source->axisLog());
    setAxisReversed(source->axisReversed());
    setAxisBaseOffset(source->axisBaseOffset());
    setAxisForceOffsetMin(source->axisForceOffsetMin());
    setAxisInterpret(source->axisInterpret());
    setAxisInterpretation(source->axisInterpretation());
    setAxisDisplay(source->axisDisplay());
    setAxisDisplayFormatString(source->axisDisplayFormatString());
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
  dialogDefaults().setValue(group+"Visible", QVariant(isAxisVisible()).toString());
  dialogDefaults().setValue(group+"Log", QVariant(axisLog()).toString());
  dialogDefaults().setValue(group+"Reversed", QVariant(axisReversed()).toString());
  dialogDefaults().setValue(group+"AutoBaseOffset", QVariant(axisAutoBaseOffset()).toString());
  dialogDefaults().setValue(group+"BaseOffset", QVariant(axisBaseOffset()).toString());
  dialogDefaults().setValue(group+"ForceOffsetMin", QVariant(axisForceOffsetMin()).toString());
  dialogDefaults().setValue(group+"Interpret", QVariant(axisInterpret()).toString());
  dialogDefaults().setValue(group+"Interpretation", QVariant(axisInterpretation()).toString());
  dialogDefaults().setValue(group+"Display", QVariant(axisDisplay()).toString());
  dialogDefaults().setValue(group+"DisplayFormatString", QVariant(axisDisplayFormatString()).toString());
  dialogDefaults().setValue(group+"Timezone", QVariant(timezoneName()));
  dialogDefaults().setValue(group+"MajorTickMode", QVariant(axisMajorTickMode()).toString());
  dialogDefaults().setValue(group+"MinorTickCount", QVariant(axisMinorTickCount()).toString());
  dialogDefaults().setValue(group+"AutoMinorTickCount", QVariant(axisAutoMinorTicks()).toString());
  dialogDefaults().setValue(group+"DrawMajorTicks", QVariant(drawAxisMajorTicks()).toString());
  dialogDefaults().setValue(group+"DrawMajorTicks", QVariant(drawAxisMajorTicks()).toString());
  dialogDefaults().setValue(group+"DrawMinorTicks", QVariant(drawAxisMinorTicks()).toString());
  dialogDefaults().setValue(group+"DrawMajorGridLines", QVariant(drawAxisMajorGridLines()).toString());
  dialogDefaults().setValue(group+"DrawMinorGridLines", QVariant(drawAxisMinorGridLines()).toString());
  dialogDefaults().setValue(group+"DrawMajorGridLinecolor", QVariant(axisMajorGridLineColor()).toString());
  dialogDefaults().setValue(group+"DrawMinorGridLinecolor", QVariant(axisMinorGridLineColor()).toString());
  dialogDefaults().setValue(group+"DrawMajorGridLinestyle", QVariant((int)axisMajorGridLineStyle()).toString());
  dialogDefaults().setValue(group+"DrawMinorGridLinestyle", QVariant((int)axisMinorGridLineStyle()).toString());
  dialogDefaults().setValue(group+"DrawMajorGridLinewidth", QVariant(axisMajorGridLineWidth()).toString());
  dialogDefaults().setValue(group+"DrawMinorGridLinewidth", QVariant(axisMinorGridLineWidth()).toString());
  dialogDefaults().setValue(group+"SignificantDigits", QVariant(axisSignificantDigits()).toString());
  dialogDefaults().setValue(group+"Rotation", QVariant(axisLabelRotation()).toString());
}

void PlotAxis::saveInPlot(QXmlStreamWriter &xml, QString axisId) {
  xml.writeStartElement("plotaxis");
  xml.writeAttribute("id", axisId);
  xml.writeAttribute("visible", QVariant(isAxisVisible()).toString());
  xml.writeAttribute("log", QVariant(axisLog()).toString());
  xml.writeAttribute("reversed", QVariant(axisReversed()).toString());
  xml.writeAttribute("autobaseoffset", QVariant(axisAutoBaseOffset()).toString());
  xml.writeAttribute("baseoffset", QVariant(axisBaseOffset()).toString());
  xml.writeAttribute("forceoffsetmin", QVariant(axisForceOffsetMin()).toString());
  xml.writeAttribute("interpret", QVariant(axisInterpret()).toString());
  xml.writeAttribute("interpretation", QVariant(axisInterpretation()).toString());
  xml.writeAttribute("display", QVariant(axisDisplay()).toString());
  xml.writeAttribute("displayformatstring", QVariant(axisDisplayFormatString()).toString());
  xml.writeAttribute("majortickmode", QVariant(axisMajorTickMode()).toString());
  xml.writeAttribute("minortickcount", QVariant(axisMinorTickCount()).toString());
  xml.writeAttribute("autominortickcount", QVariant(axisAutoMinorTicks()).toString());
  xml.writeAttribute("drawmajorticks", QVariant(drawAxisMajorTicks()).toString());
  xml.writeAttribute("drawminorticks", QVariant(drawAxisMinorTicks()).toString());
  xml.writeAttribute("drawmajorgridlines", QVariant(drawAxisMajorGridLines()).toString());
  xml.writeAttribute("drawminorgridlines", QVariant(drawAxisMinorGridLines()).toString());
  xml.writeAttribute("drawmajorgridlinecolor", QVariant(axisMajorGridLineColor()).toString());
  xml.writeAttribute("drawminorgridlinecolor", QVariant(axisMinorGridLineColor()).toString());
  xml.writeAttribute("drawmajorgridlinestyle", QVariant((int)axisMajorGridLineStyle()).toString());
  xml.writeAttribute("drawminorgridlinestyle", QVariant((int)axisMinorGridLineStyle()).toString());
  xml.writeAttribute("drawmajorgridlinewidth", QVariant(axisMajorGridLineWidth()).toString());
  xml.writeAttribute("drawminorgridlinewidth", QVariant(axisMinorGridLineWidth()).toString());
  xml.writeAttribute("significantdigits", QVariant(axisSignificantDigits()).toString());
  xml.writeAttribute("rotation", QVariant(axisLabelRotation()).toString());
  xml.writeAttribute("zoommode", QVariant(axisZoomMode()).toString());
  xml.writeAttribute("timezonename", _timeZone.tzName());
  xml.writeAttribute("timezoneoffset", QVariant(_timeZone.gmtOffset(0)).toString());
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
  av = attrs.value("forceoffsetmin");
  if (!av.isNull()) {
    setAxisForceOffsetMin(QVariant(av.toString()).toBool());
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
  av = attrs.value("displayformatstring");
  if (!av.isNull()) {
    setAxisDisplayFormatString(av.toString());
  }
  av = attrs.value("majortickmode");
  if (!av.isNull()) {
    setAxisMajorTickMode((MajorTickMode)QVariant(av.toString()).toInt());
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
  av = attrs.value("timezonename");
  if (!av.isNull()) {
    setTimezoneName(av.toString());
    // fixme: handle unrecognised timezone name by using timezoneoffset
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
