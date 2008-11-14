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
#include <QDate>

static int FULL_PRECISION = 15;
static qreal JD1900 = 2415020.5;
static qreal JD1970 = 2440587.5;
static qreal JD_RJD = 2400000.0;
static qreal JD_MJD = 2400000.5;

namespace Kst {

PlotAxis::PlotAxis(PlotItem *plotItem, Qt::Orientation orientation) :
  _plotItem(plotItem),
  _orientation(orientation),
  _axisZoomMode(Auto),
  _isAxisVisible(true),
  _axisLog(false),
  _axisReversed(false),
  _axisBaseOffset(false),
  _axisBaseOffsetOverride(false),
  _axisInterpret(false),
  _axisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS),
  _axisInterpretation(AXIS_INTERP_CTIME),
  _axisMajorTickMode(Normal),
  _axisOverrideMajorTicks(Normal),
  _axisMinorTickCount(4),
  _axisSignificantDigits(9),
  _drawAxisMajorTicks(true),
  _drawAxisMinorTicks(true),
  _drawAxisMajorGridLines(true),
  _drawAxisMinorGridLines(false),
  _axisMajorGridLineColor(Qt::gray),
  _axisMinorGridLineColor(Qt::gray),
  _axisMajorGridLineStyle(Qt::DashLine),
  _axisMinorGridLineStyle(Qt::DashLine),
  _axisPlotMarkers(orientation == Qt::Horizontal)
 {
  connect(_plotItem, SIGNAL(updateAxes()), this, SLOT(update()));
}


PlotAxis::~PlotAxis() {
}


QString PlotAxis::interpretLabel(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double base, double lastValue) {
  double value = convertTimeValueToJD(axisInterpretation, base);
  double scaleValue = convertTimeValueToJD(axisInterpretation, lastValue) - value;

  switch (axisInterpretation) {
    case AXIS_INTERP_YEAR:
      scaleValue *= 365.25 * 24.0 * 60.0 * 60.0;
      break;
    case AXIS_INTERP_CTIME:
      break;
    case AXIS_INTERP_JD:
    case AXIS_INTERP_MJD:
    case AXIS_INTERP_RJD:
      scaleValue *= 24.0 * 60.0 * 60.0;
      break;
    case AXIS_INTERP_AIT:
      break;
  }

  QString label;

  // print value in appropriate format
  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      value -= JD1900 + 0.5;
      value /= 365.25;
      value += 1900.0;
      label = i18n("J");
      label += QString::number(value, 'g', FULL_PRECISION);
      label += " [years]";
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      label = convertJDToDateString(axisInterpretation, axisDisplay, value);
      if( scaleValue > 10.0 * 24.0 * 60.0 * 60.0 ) {
        label += i18n(" [days]");
      } else if( scaleValue > 10.0 * 24.0 * 60.0 ) {
        label += i18n(" [hours]");
      } else if( scaleValue > 10.0 * 60.0 ) {
        label += i18n(" [minutes]");
      } else {
        label += i18n(" [seconds]");
      }
      break;
    case AXIS_DISPLAY_JD:
      label = i18n("JD");
      label += QString::number(value, 'g', FULL_PRECISION);
      label += " [days]";
      break;
    case AXIS_DISPLAY_MJD:
      value -= JD_MJD;
      label = i18n("MJD");
      label += QString::number(value, 'g', FULL_PRECISION);
      label += " [days]";
      break;
    case AXIS_DISPLAY_RJD:
      value -= JD_RJD;
      label = i18n("RJD");
      label += QString::number(value, 'g', FULL_PRECISION);
      label += " [days]";
      break;
  }

  return label;
}


double PlotAxis::interpretOffset(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double base, double value) {
  double offset;
  offset = value - base;

  offset = convertTimeDiffValueToDays(axisInterpretation, offset);

  // convert difference to desired format
  switch (axisDisplay) {
    case AXIS_DISPLAY_YEAR:
      offset /= 365.25;
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
    case AXIS_DISPLAY_QTTEXTDATEHHMMSS_SS:
    case AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS:
      offset *= 24.0 * 60.0 * 60.0;
      break;
    case AXIS_DISPLAY_JD:
    case AXIS_DISPLAY_MJD:
    case AXIS_DISPLAY_RJD:
      break;
  }
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
  _axisZoomMode = mode;
}


bool PlotAxis::axisLog() const {
  return _axisLog;
}


void PlotAxis::setAxisLog(bool log) {
  _axisLog = log;
}


int PlotAxis::axisSignificantDigits() const {
  return _axisSignificantDigits;
}


void PlotAxis::setAxisSignificantDigits(const int digits) {
  _axisSignificantDigits = digits;
}


PlotAxis::MajorTickMode PlotAxis::axisMajorTickMode() const {
  return _axisMajorTickMode;
}


void PlotAxis::setAxisMajorTickMode(PlotAxis::MajorTickMode mode) {
  _axisMajorTickMode = mode;
}


int PlotAxis::axisMinorTickCount() const {
  return _axisMinorTickCount;
}


void PlotAxis::setAxisMinorTickCount(const int count) {
  _axisMinorTickCount = count;
}


bool PlotAxis::drawAxisMajorTicks() const {
  return _drawAxisMajorTicks;
}


void PlotAxis::setDrawAxisMajorTicks(bool draw) {
  _drawAxisMajorTicks = draw;
}


bool PlotAxis::drawAxisMinorTicks() const {
  return _drawAxisMinorTicks;
}


void PlotAxis::setDrawAxisMinorTicks(bool draw) {
  _drawAxisMinorTicks = draw;
}


bool PlotAxis::drawAxisMajorGridLines() const {
  return _drawAxisMajorGridLines;
}


void PlotAxis::setDrawAxisMajorGridLines(bool draw) {
  _drawAxisMajorGridLines = draw;
}


bool PlotAxis::drawAxisMinorGridLines() const {
  return _drawAxisMinorGridLines;
}


void PlotAxis::setDrawAxisMinorGridLines(bool draw) {
  _drawAxisMinorGridLines = draw;
}


QColor PlotAxis::axisMajorGridLineColor() const {
  return _axisMajorGridLineColor;
}


void PlotAxis::setAxisMajorGridLineColor(const QColor &color) {
  _axisMajorGridLineColor = color;
}


QColor PlotAxis::axisMinorGridLineColor() const {
  return _axisMinorGridLineColor;
}


void PlotAxis::setAxisMinorGridLineColor(const QColor &color) {
  _axisMinorGridLineColor = color;
}


Qt::PenStyle PlotAxis::axisMajorGridLineStyle() const {
  return _axisMajorGridLineStyle;
}


void PlotAxis::setAxisMajorGridLineStyle(const Qt::PenStyle style) {
  _axisMajorGridLineStyle = style;
}


Qt::PenStyle PlotAxis::axisMinorGridLineStyle() const {
  return _axisMinorGridLineStyle;
}


void PlotAxis::setAxisMinorGridLineStyle(const Qt::PenStyle style) {
  _axisMinorGridLineStyle = style;
}


bool PlotAxis::isAxisVisible() const {
  return _isAxisVisible;
}


void PlotAxis::setAxisVisible(bool visible) {
  if (_isAxisVisible == visible)
    return;

  _isAxisVisible = visible;
  emit marginsChanged();
}


bool PlotAxis::axisReversed() const {
  return _axisReversed;
}


void PlotAxis::setAxisReversed(const bool enabled) {
  _axisReversed = enabled;
}


bool PlotAxis::axisBaseOffset() const {
  return _axisBaseOffset;
}


void PlotAxis::setAxisBaseOffset(const bool enabled) {
  _axisBaseOffset = enabled;
}


bool PlotAxis::axisInterpret() const {
  return _axisInterpret;
}


void PlotAxis::setAxisInterpret(const bool enabled) {
  _axisInterpret = enabled;
}


AxisDisplayType PlotAxis::axisDisplay() const {
  return _axisDisplay;
}


void PlotAxis::setAxisDisplay(const AxisDisplayType display) {
  _axisDisplay = display;
}


AxisInterpretationType PlotAxis::axisInterpretation() const {
  return _axisInterpretation;
}


void PlotAxis::setAxisInterpretation(const AxisInterpretationType display) {
  _axisInterpretation = display;
}


void PlotAxis::computeLogTicks(QList<qreal> *MajorTicks, QList<qreal> *MinorTicks, QMap<qreal, QString> *Labels, qreal min, qreal max, MajorTickMode tickMode) {

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
      *MajorTicks << majorPoint;
      *Labels->insert(majorPoint, QString::number(majorPoint, 'g', FULL_PRECISION));
    }

    if (tick == 1.0) {
      // draw minor lines
      bool first = true;
      qreal powMin = pow(10, min), powMax = pow(10, max);
      for (int j = 2; j < 10; j++) {
        qreal minorPoint = majorPoint * j;
        if (minorPoint >= powMin && minorPoint <= powMax) {
          *MinorTicks << minorPoint;
          if (minorLabels && first) {
            *Labels->insert(minorPoint, QString::number(minorPoint, 'g', FULL_PRECISION));
            first = false;
          }
        }
      }
    }
  }
  if (minorLabels && !MinorTicks->isEmpty()) {
    qreal lastMinorTick = MinorTicks->last();
    if (MajorTicks->isEmpty() || MajorTicks->last() < lastMinorTick) {
      if (!Labels->contains(lastMinorTick)) {
        *Labels->insert(lastMinorTick, QString::number(lastMinorTick, 'g', FULL_PRECISION));
      }
    }
  }
}


// Function validates that the labels will not overlap.  Only functions for x Axis.
void PlotAxis::validateDrawingRegion(int flags, QPainter *painter) {
  if (_orientation != Qt::Horizontal) {
    return;
  }

  // Always try to use the settings requested.
  if (_axisOverrideMajorTicks != _axisMajorTickMode) {
    _axisBaseOffsetOverride = false;
    update();
  }

  int longest = 0;
  QMapIterator<qreal, QString> iLongestLabelCheck(_axisLabels);
  while (iLongestLabelCheck.hasNext()) {
    iLongestLabelCheck.next();
    QRectF bound = painter->boundingRect(QRectF(), flags, iLongestLabelCheck.value());
    if (bound.width() > longest) {
      longest = bound.width();
    }
  }

// Make local... Use begin.
  qreal firstTick = 0, secondTick = 0;
  QMapIterator<qreal, QString> iLabelCheck(_axisLabels);
  if (iLabelCheck.hasNext()) {
    iLabelCheck.next();
    firstTick = iLabelCheck.key();
    if (iLabelCheck.hasNext()) {
      iLabelCheck.next();
      secondTick = iLabelCheck.key();
    }
  }

  qreal labelSpace = plotItem()->mapXToPlot(secondTick) - plotItem()->mapXToPlot(firstTick);
  if (labelSpace < (longest + 2)) {
    _axisOverrideMajorTicks = convertToMajorTickMode((plotItem()->plotRect().width() / (longest + 2)) - 1);
    if (_axisOverrideMajorTicks == None) {
      _axisBaseOffsetOverride = true;
      _axisOverrideMajorTicks = Coarse;
    }
    update(true);
  }
}


PlotAxis::MajorTickMode PlotAxis::convertToMajorTickMode(int tickCount) {
  MajorTickMode mode = None;
  if (tickCount >= VeryFine) {
    mode = VeryFine;
  } else if (tickCount >= Fine) {
    mode = Fine;
  } else if (tickCount >= Normal) {
    mode = Normal;
  } else if (tickCount >= Coarse) {
    mode = Coarse;
  }
  return mode;
}


void PlotAxis::update(bool useOverrideTicks) {
  MajorTickMode majorTickCount;
  if (useOverrideTicks) {
    majorTickCount = _axisOverrideMajorTicks;
  } else {
    _axisOverrideMajorTicks = _axisMajorTickMode;
    majorTickCount = _axisMajorTickMode;
    _axisBaseOffsetOverride = false;
  }

  QMap<qreal, QString> labels;
  QList<qreal> ticks;
  QList<qreal> minTicks;
  plotItem()->updateScale();
  if (_axisLog) {
    qreal min = _orientation == Qt::Horizontal ? plotItem()->xMin() : plotItem()->yMin();
    qreal max = _orientation == Qt::Horizontal ? plotItem()->xMax() : plotItem()->yMax();
    computeLogTicks(&ticks, &minTicks, &labels, min, max, majorTickCount);
  } else {
    qreal min = _orientation == Qt::Horizontal ? plotItem()->projectionRect().left() : plotItem()->projectionRect().top();
    qreal max = _orientation == Qt::Horizontal ? plotItem()->projectionRect().right() : plotItem()->projectionRect().bottom();
    qreal majorTickSpacing = computedMajorTickSpacing(majorTickCount, _orientation);
    qreal firstTick = ceil(min / majorTickSpacing) * majorTickSpacing;

    int i = 0;
    qreal nextTick = firstTick;
    while (1) {
      nextTick = firstTick + (i++ * majorTickSpacing);
      if (nextTick > max)
        break;
      ticks << nextTick;
      labels.insert(nextTick, QString::number(nextTick, 'g', FULL_PRECISION));
    }

    qreal minorTickSpacing = 0;
    if (_axisMinorTickCount > 0) {
      minorTickSpacing = majorTickSpacing / _axisMinorTickCount;
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

  _axisMajorTicks = ticks;
  _axisMinorTicks = minTicks;

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

  if (_axisBaseOffset || _axisInterpret || (longest > _axisSignificantDigits) || _axisBaseOffsetOverride ) {
    if (_axisInterpret) {
      _baseLabel = interpretLabel(_axisInterpretation, _axisDisplay, base, (_axisMajorTicks).last());
    } else {
      _baseLabel = QString::number(base);
    }
    QMapIterator<qreal, QString> i(labels);
    while (i.hasNext()) {
      i.next();
      qreal offset;
      if (_axisInterpret) {
        offset = interpretOffset(_axisInterpretation, _axisDisplay, base, i.key());
      } else {
        offset = i.key() - base;
      }
      QString label;
      if (offset < 0) {
        label += "-";
        offset = offset * -1;
      } else if (offset > 0) {
        label += "+";
      }
      label += "[";
      label += QString::number(offset, 'g', _axisSignificantDigits);
      label += "]";
      _axisLabels.insert(i.key(), label);
    }
  } else {
    _axisLabels = labels;
  }
}


/*
 * Major ticks are always spaced by D = A*10B where B is an integer,
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

  qreal d1 = 1 * pow(10, B);
  qreal d2 = 2 * pow(10, B);
  qreal d5 = 5 * pow(10, B);

  qreal r1 = d1 * M - 1;
  qreal r2 = d2 * M - 1;
  qreal r5 = d5 * M - 1;

#ifdef MAJOR_TICK_DEBUG
  qDebug() << "MajorTickMode:" << M << "Range:" << R
           << "\n\tranges:" << r1 << r2 << r5
           << "\n\tspaces:" << d1 << d2 << d5
           << endl;
#endif

  qreal s1 = qAbs(r1 - R);
  qreal s2 = qAbs(r2 - R);
  qreal s5 = qAbs(r5 - R);

  if (s1 < s2 && s1 < s5)
    return d1;
  else if (s2 < s5)
    return d2;
  else
    return d5;
}


void PlotAxis::saveInPlot(QXmlStreamWriter &xml, QString axisId) {
  xml.writeStartElement("plotaxis");
  xml.writeAttribute("id", axisId);
  xml.writeAttribute("visible", QVariant(isAxisVisible()).toString());
  xml.writeAttribute("log", QVariant(axisLog()).toString());
  xml.writeAttribute("reversed", QVariant(axisReversed()).toString());
  xml.writeAttribute("baseoffset", QVariant(axisBaseOffset()).toString());
  xml.writeAttribute("interpret", QVariant(axisInterpret()).toString());
  xml.writeAttribute("interpretation", QVariant(axisInterpretation()).toString());
  xml.writeAttribute("display", QVariant(axisDisplay()).toString());
  xml.writeAttribute("majortickmode", QVariant(axisMajorTickMode()).toString());
  xml.writeAttribute("minortickcount", QVariant(axisMinorTickCount()).toString());
  xml.writeAttribute("drawmajorticks", QVariant(drawAxisMajorTicks()).toString());
  xml.writeAttribute("drawminorticks", QVariant(drawAxisMinorTicks()).toString());
  xml.writeAttribute("drawmajorgridlines", QVariant(drawAxisMajorGridLines()).toString());
  xml.writeAttribute("drawminorgridlines", QVariant(drawAxisMinorGridLines()).toString());
  xml.writeAttribute("drawmajorgridlinecolor", QVariant(axisMajorGridLineColor()).toString());
  xml.writeAttribute("drawminorgridlinecolor", QVariant(axisMinorGridLineColor()).toString());
  xml.writeAttribute("drawmajorgridlinestyle", QVariant(axisMajorGridLineStyle()).toString());
  xml.writeAttribute("drawminorgridlinestyle", QVariant(axisMinorGridLineStyle()).toString());
  xml.writeAttribute("significantdigits", QVariant(axisSignificantDigits()).toString());
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
  av = attrs.value("significantdigits");
  if (!av.isNull()) {
    setAxisSignificantDigits(QVariant(av.toString()).toInt());
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
