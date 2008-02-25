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

#include "plotitem.h"

#include "viewitemzorder.h"
#include "plotitemmanager.h"
#include "plotrenderitem.h"

#include "layoutboxitem.h"
#include "viewgridlayout.h"
#include "debug.h"

#include "application.h"
#include "mainwindow.h"
#include "tabwidget.h"

#include "datacollection.h"
#include "dataobjectcollection.h"
#include "cartesianrenderitem.h"

#include "math_kst.h"

#include "settings.h"

#include <QDebug>

static qreal MARGIN_WIDTH = 20.0;
static qreal MARGIN_HEIGHT = 20.0;
static int FULL_PRECISION = 15;
static qreal JD1900 = 2415020.5;
static qreal JD1970 = 2440587.5;
static qreal JD_RJD = 2400000.0;
static qreal JD_MJD = 2400000.5;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent),
  _isTiedZoom(false),
  _isLeftLabelVisible(true),
  _isBottomLabelVisible(true),
  _isRightLabelVisible(true),
  _isTopLabelVisible(true),
  _calculatedLabelMarginWidth(0.0),
  _calculatedLabelMarginHeight(0.0),
  _calculatedAxisMarginWidth(0.0),
  _calculatedAxisMarginHeight(0.0),
  _xAxisLog(false),
  _yAxisLog(false),
  _xAxisReversed(false),
  _yAxisReversed(false),
  _xAxisBaseOffset(false),
  _yAxisBaseOffset(false),
  _xAxisInterpret(false),
  _yAxisInterpret(false),
  _xAxisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS),
  _yAxisDisplay(AXIS_DISPLAY_QTLOCALDATEHHMMSS_SS),
  _xAxisInterpretation(AXIS_INTERP_CTIME),
  _yAxisInterpretation(AXIS_INTERP_CTIME),
  _xAxisMajorTickMode(Normal),
  _yAxisMajorTickMode(Normal),
  _xAxisMinorTickCount(4),
  _yAxisMinorTickCount(4),
  _drawXAxisMajorTicks(true),
  _drawXAxisMinorTicks(true),
  _drawYAxisMajorTicks(true),
  _drawYAxisMinorTicks(true),
  _drawXAxisMajorGridLines(true),
  _drawXAxisMinorGridLines(false),
  _drawYAxisMajorGridLines(true),
  _drawYAxisMinorGridLines(false),
  _xAxisMajorGridLineColor(Qt::gray),
  _xAxisMinorGridLineColor(Qt::gray),
  _yAxisMajorGridLineColor(Qt::gray),
  _yAxisMinorGridLineColor(Qt::gray),
  _xAxisMajorGridLineStyle(Qt::DashLine),
  _xAxisMinorGridLineStyle(Qt::DashLine),
  _yAxisMajorGridLineStyle(Qt::DashLine),
  _yAxisMinorGridLineStyle(Qt::DashLine)
 {

  setName("Plot");
  setZValue(PLOT_ZVALUE);
  setBrush(Qt::white);

  QFont defaultFont;
  _leftLabelFont = defaultFont;
  _bottomLabelFont = defaultFont;
  _topLabelFont = defaultFont;
  _rightLabelFont = defaultFont;

  PlotItemManager::self()->addPlot(this);
}


PlotItem::~PlotItem() {
  PlotItemManager::self()->removePlot(this);
}


QString PlotItem::plotName() const {
  return name();
}


void PlotItem::save(QXmlStreamWriter &xml) {
  xml.writeStartElement("plot");
  xml.writeAttribute("tiedzoom", QVariant(_isTiedZoom).toString());
  xml.writeAttribute("leftlabelvisible", QVariant(_isLeftLabelVisible).toString());
  xml.writeAttribute("bottomlabelvisible", QVariant(_isBottomLabelVisible).toString());
  xml.writeAttribute("rightlabelvisible", QVariant(_isRightLabelVisible).toString());
  xml.writeAttribute("toplabelvisible", QVariant(_isTopLabelVisible).toString());
  xml.writeAttribute("leftlabeloverride", _leftLabelOverride);
  xml.writeAttribute("leftlabelfont", QVariant(_leftLabelFont).toString());
  xml.writeAttribute("bottomlabeloverride", _bottomLabelOverride);
  xml.writeAttribute("bottomlabelfont", QVariant(_bottomLabelFont).toString());
  xml.writeAttribute("toplabeloverride", _topLabelOverride);
  xml.writeAttribute("toplabelfont", QVariant(_topLabelFont).toString());
  xml.writeAttribute("rightlabeloverride", _rightLabelOverride);
  xml.writeAttribute("rightlabelfont", QVariant(_rightLabelFont).toString());
  xml.writeAttribute("xaxislog", QVariant(_xAxisLog).toString());
  xml.writeAttribute("yaxislog", QVariant(_yAxisLog).toString());
  xml.writeAttribute("xaxisreversed", QVariant(_xAxisReversed).toString());
  xml.writeAttribute("yaxisreversed", QVariant(_yAxisReversed).toString());
  xml.writeAttribute("xaxisbaseoffset", QVariant(_xAxisBaseOffset).toString());
  xml.writeAttribute("yaxisbaseoffset", QVariant(_yAxisBaseOffset).toString());
  xml.writeAttribute("xaxisinterpret", QVariant(_xAxisInterpret).toString());
  xml.writeAttribute("yaxisinterpret", QVariant(_yAxisInterpret).toString());
  xml.writeAttribute("xaxisinterpretation", QVariant(_xAxisInterpretation).toString());
  xml.writeAttribute("yaxisinterpretation", QVariant(_yAxisInterpretation).toString());
  xml.writeAttribute("xaxisdisplay", QVariant(_xAxisDisplay).toString());
  xml.writeAttribute("yaxisdisplay", QVariant(_yAxisDisplay).toString());
  xml.writeAttribute("xaxismajortickmode", QVariant(_xAxisMajorTickMode).toString());
  xml.writeAttribute("yaxismajortickmode", QVariant(_yAxisMajorTickMode).toString());
  xml.writeAttribute("xaxisminortickcount", QVariant(_xAxisMinorTickCount).toString());
  xml.writeAttribute("yaxisminortickcount", QVariant(_yAxisMinorTickCount).toString());
  xml.writeAttribute("xaxisdrawmajorticks", QVariant(_drawXAxisMajorTicks).toString());
  xml.writeAttribute("xaxisdrawminorticks", QVariant(_drawXAxisMinorTicks).toString());
  xml.writeAttribute("yaxisdrawmajorticks", QVariant(_drawYAxisMajorTicks).toString());
  xml.writeAttribute("yaxisdrawminorticks", QVariant(_drawYAxisMinorTicks).toString());
  xml.writeAttribute("xaxisdrawmajorgridlines", QVariant(_drawXAxisMajorGridLines).toString());
  xml.writeAttribute("xaxisdrawminorgridlines", QVariant(_drawXAxisMinorGridLines).toString());
  xml.writeAttribute("yaxisdrawmajorgridlines", QVariant(_drawYAxisMajorGridLines).toString());
  xml.writeAttribute("yaxisdrawminorgridlines", QVariant(_drawYAxisMinorGridLines).toString());
  xml.writeAttribute("xaxisdrawmajorgridlinecolor", QVariant(_xAxisMajorGridLineColor).toString());
  xml.writeAttribute("xaxisdrawminorgridlinecolor", QVariant(_xAxisMinorGridLineColor).toString());
  xml.writeAttribute("yaxisdrawmajorgridlinecolor", QVariant(_yAxisMajorGridLineColor).toString());
  xml.writeAttribute("yaxisdrawminorgridlinecolor", QVariant(_yAxisMinorGridLineColor).toString());
  xml.writeAttribute("xaxisdrawmajorgridlinestyle", QVariant(_xAxisMajorGridLineStyle).toString());
  xml.writeAttribute("xaxisdrawminorgridlinestyle", QVariant(_xAxisMinorGridLineStyle).toString());
  xml.writeAttribute("yaxisdrawmajorgridlinestyle", QVariant(_yAxisMajorGridLineStyle).toString());
  xml.writeAttribute("yaxisdrawminorgridlinestyle", QVariant(_yAxisMinorGridLineStyle).toString());

  ViewItem::save(xml);
  foreach (PlotRenderItem *renderer, renderItems()) {
    renderer->saveInPlot(xml);
  }
  xml.writeEndElement();
}


QList<PlotRenderItem*> PlotItem::renderItems() const {
  return _renderers.values();
}


PlotRenderItem *PlotItem::renderItem(PlotRenderItem::RenderType type) {
  if (_renderers.contains(type))
    return _renderers.value(type);

  switch (type) {
  case PlotRenderItem::Cartesian:
    {
      CartesianRenderItem *renderItem = new CartesianRenderItem(this);
      _renderers.insert(type, renderItem);
      return renderItem;
    }
  case PlotRenderItem::Polar:
  case PlotRenderItem::Sinusoidal:
  default:
    return 0;
  }
}


void PlotItem::paint(QPainter *painter) {
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->drawRect(rect());
  painter->restore();

  painter->save();
  painter->translate(QPointF(rect().x(), rect().y()));

  QList<qreal> xMajorTicks;
  QList<qreal> xMinorTicks;
  QList<qreal> yMajorTicks;
  QList<qreal> yMinorTicks;
  QMap<qreal, QString> xLabels, yLabels;
  computeTicks(&xMajorTicks, &xMinorTicks, &yMajorTicks, &yMinorTicks, &xLabels, &yLabels);
  setCalculatedAxisMarginWidth(calculateYTickLabelBound(painter, yMajorTicks).width());
  setCalculatedAxisMarginHeight(calculateXTickLabelBound(painter, xMajorTicks).height());

  //Calculate and adjust the margins based on the bounds...
  QSizeF margins;
  margins = margins.expandedTo(calculateLeftLabelBound(painter));
  margins = margins.expandedTo(calculateBottomLabelBound(painter));
  margins = margins.expandedTo(calculateRightLabelBound(painter));
  margins = margins.expandedTo(calculateTopLabelBound(painter));

  margins.setHeight(margins.height() + _calculatedAxisMarginHeight);
  margins.setHeight(margins.width() + _calculatedAxisMarginWidth);

//  qDebug() << "setting margin width" << margins.width() << endl;
  setCalculatedLabelMarginWidth(margins.width());

//  qDebug() << "setting margin height" << margins.height() << endl;
  setCalculatedLabelMarginHeight(margins.height());

//  qDebug() << "=============> leftLabel:" << leftLabel() << endl;
  paintLeftLabel(painter);
//  qDebug() << "=============> bottomLabel:" << bottomLabel() << endl;
  paintBottomLabel(painter);
//  qDebug() << "=============> rightLabel:" << rightLabel() << endl;
  paintRightLabel(painter);
//  qDebug() << "=============> topLabel:" << topLabel() << endl;
  paintTopLabel(painter);

  paintPlotMarkers(painter, xMajorTicks, xMinorTicks, yMajorTicks, yMinorTicks);
  paintMajorTickLabels(painter, xMajorTicks, yMajorTicks, xLabels, yLabels);

  painter->restore();
}

void PlotItem::paintPlotMarkers(QPainter *painter,
                                       const QList<qreal> &xMajorTicks,
                                       const QList<qreal> &xMinorTicks,
                                       const QList<qreal> &yMajorTicks,
                                       const QList<qreal> &yMinorTicks) {
  paintMajorGridLines(painter, xMajorTicks, yMajorTicks);
  paintMinorGridLines(painter, xMinorTicks, yMinorTicks);
  paintMajorTicks(painter, xMajorTicks, yMajorTicks);
  paintMinorTicks(painter, xMinorTicks, yMinorTicks);
}


void PlotItem::paintMajorGridLines(QPainter *painter,
                                       const QList<qreal> &xMajorTicks,
                                       const QList<qreal> &yMajorTicks) {

  QRectF rect = plotRect();

  if (_drawXAxisMajorGridLines) {
    QVector<QLineF> xMajorTickLines;
    foreach (qreal x, xMajorTicks) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      xMajorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_xAxisMajorGridLineColor), 1.0, _xAxisMajorGridLineStyle));
    painter->drawLines(xMajorTickLines);
    painter->restore();
  }

  if (_drawYAxisMajorGridLines) {
    QVector<QLineF> yMajorTickLines;
    foreach (qreal y, yMajorTicks) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      yMajorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_yAxisMajorGridLineColor), 1.0, _yAxisMajorGridLineStyle));
    painter->drawLines(yMajorTickLines);
    painter->restore();
  }
}


void PlotItem::paintMinorGridLines(QPainter *painter,
                                       const QList<qreal> &xMinorTicks,
                                       const QList<qreal> &yMinorTicks) {

  QRectF rect = plotRect();

  if (_drawXAxisMinorGridLines) {
    QVector<QLineF> xMinorTickLines;
    foreach (qreal x, xMinorTicks) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      xMinorTickLines << QLineF(p1, p2);
    }
    painter->save();
    painter->setPen(QPen(QBrush(_xAxisMinorGridLineColor), 1.0, _xAxisMinorGridLineStyle));
    painter->drawLines(xMinorTickLines);
    painter->restore();
  }

  if (_drawYAxisMinorGridLines) {
    QVector<QLineF> yMinorTickLines;
    foreach (qreal y, yMinorTicks) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      yMinorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_yAxisMinorGridLineColor), 1.0, _yAxisMinorGridLineStyle));
    painter->drawLines(yMinorTickLines);
    painter->restore();
  }
}


void PlotItem::paintMajorTicks(QPainter *painter,
                                   const QList<qreal> &xMajorTicks,
                                   const QList<qreal> &yMajorTicks) {

  qreal majorTickLength = qMin(rect().width(), rect().height()) * .02; //two percent

  if (_drawXAxisMajorTicks) {
    QVector<QLineF> xMajorTickLines;
    foreach (qreal x, xMajorTicks) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, majorTickLength);
      xMajorTickLines << QLineF(p1, p2);

      p1.setY(plotRect().top());
      p2 = p1 + QPointF(0, majorTickLength);
      xMajorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(xMajorTickLines);
  }

  if (_drawYAxisMajorTicks) {
    QVector<QLineF> yMajorTickLines;
    foreach (qreal y, yMajorTicks) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(majorTickLength, 0);
      yMajorTickLines << QLineF(p1, p2);

      p1.setX(plotRect().right());
      p2 = p1 - QPointF(majorTickLength, 0);
      yMajorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(yMajorTickLines);
  }
}


void PlotItem::paintMinorTicks(QPainter *painter,
                                   const QList<qreal> &xMinorTicks,
                                   const QList<qreal> &yMinorTicks) {

  qreal minorTickLength = qMin(rect().width(), rect().height()) * 0.01; //two percent

  if (_drawXAxisMinorTicks) {
    QVector<QLineF> xMinorTickLines;
    foreach (qreal x, xMinorTicks) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, minorTickLength);
      xMinorTickLines << QLineF(p1, p2);

      p1.setY(plotRect().top());
      p2 = p1 + QPointF(0, minorTickLength);
      xMinorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(xMinorTickLines);
  }

  if (_drawYAxisMinorTicks) {
    QVector<QLineF> yMinorTickLines;
    foreach (qreal y, yMinorTicks) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(minorTickLength, 0);
      yMinorTickLines << QLineF(p1, p2);

      p1.setX(plotRect().right());
      p2 = p1 - QPointF(minorTickLength, 0);
      yMinorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(yMinorTickLines);
  }
}


void PlotItem::paintMajorTickLabels(QPainter *painter,
                                    const QList<qreal> &xMajorTicks,
                                    const QList<qreal> &yMajorTicks,
                                    const QMap<qreal, QString> &xLabelsIn,
                                    const QMap<qreal, QString> &yLabelsIn) {

  QRectF yLabelRect, xLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignVCenter;

  QMap<qreal, QString> xLabels;
  QString xBaseLabel, yBaseLabel;
  if (_xAxisBaseOffset || _xAxisInterpret) {
    qreal base;
    int shortest = 1000;
    QMapIterator<qreal, QString> iShort(xLabelsIn);
    while (iShort.hasNext()) {
      iShort.next();
      if (iShort.value().length() < shortest) {
        shortest = iShort.value().length();
        base = iShort.key();
      }
    }

    if (_xAxisInterpret) {
      xBaseLabel = interpretLabel(_xAxisInterpretation, _xAxisDisplay, base, xMajorTicks.last());
    } else {
      xBaseLabel = QString::number(base);
    }
    QMapIterator<qreal, QString> i(xLabelsIn);
    while (i.hasNext()) {
      i.next();
      qreal offset;
      if (_xAxisInterpret) {
        offset = interpretOffset(_xAxisInterpretation, _xAxisDisplay, base, i.key());
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
      label += QString::number(offset, 'g', FULL_PRECISION);
      label += "]";
      xLabels.insert(i.key(), label);
    }
  } else {
    xLabels = xLabelsIn;
  }

  QMapIterator<qreal, QString> xLabelIt(xLabels);
  while (xLabelIt.hasNext()) {
    xLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
    QPointF p = QPointF(mapXToPlot(xLabelIt.key()), plotRect().bottom() + bound.height() / 2.0);
    bound.moveCenter(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    painter->drawText(bound, flags, xLabelIt.value());
  }

  QMap<qreal, QString> yLabels;
  if (_yAxisBaseOffset || _yAxisInterpret) {
    qreal base;
    int shortest = 1000;
    QMapIterator<qreal, QString> iShort(yLabelsIn);
    while (iShort.hasNext()) {
      iShort.next();
      if (iShort.value().length() < shortest) {
        shortest = iShort.value().length();
        base = iShort.key();
      }
    }

    if (_yAxisInterpret) {
      yBaseLabel = interpretLabel(_yAxisInterpretation, _yAxisDisplay, base, yMajorTicks.last());
    } else {
      yBaseLabel = QString::number(base);
    }
    QMapIterator<qreal, QString> i(yLabelsIn);
    while (i.hasNext()) {
      i.next();
      qreal offset;
      if (_yAxisInterpret) {
        offset = interpretOffset(_yAxisInterpretation, _yAxisDisplay, base, i.key());
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
      label += QString::number(offset, 'g', FULL_PRECISION);
      label += "]";
      yLabels.insert(i.key(), label);
    }
  } else {
    yLabels = yLabelsIn;
  }

  QMapIterator<qreal, QString> yLabelIt(yLabels);
  while (yLabelIt.hasNext()) {
    yLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
    QPointF p = QPointF(plotRect().left() - bound.width() / 2.0, mapYToPlot(yLabelIt.key()));
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    painter->drawText(bound, flags, yLabelIt.value());
  }

  if (!yBaseLabel.isEmpty()) {
    painter->save();
    QTransform t;
    t.rotate(90.0);
    painter->rotate(-90.0);

    QRectF bound = painter->boundingRect(QRectF(), flags, yBaseLabel);
    bound = QRectF(bound.x(), bound.bottomRight().y() - bound.width(), bound.height(), bound.width());
    QPointF p = QPointF(plotRect().left() - bound.width() * 2.0, plotRect().top());
    bound.moveBottomLeft(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    painter->drawText(t.mapRect(bound), flags, yBaseLabel);
    painter->restore();
  }

  if (!xBaseLabel.isEmpty()) {
    QRectF bound = painter->boundingRect(QRectF(), flags, xBaseLabel);
    QPointF p = QPointF(plotRect().left(), plotRect().bottom() + bound.height() * 2.0);
    bound.moveBottomLeft(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    painter->drawText(bound, flags, xBaseLabel);
  }

  _xLabelRect = xLabelRect;
  _yLabelRect = yLabelRect;

//   painter->save();
//   painter->setOpacity(0.3);
// //  qDebug() << "xLabelRect:" << xLabelRect << endl;
//   painter->fillRect(xLabelRect, Qt::blue);
// 
// //  qDebug() << "yLabelRect:" << yLabelRect << endl;
//   painter->fillRect(yLabelRect, Qt::green);
//   painter->restore();
}


QString PlotItem::interpretLabel(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double base, double lastValue) {
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


double PlotItem::interpretOffset(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double base, double value) {
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


double PlotItem::convertTimeValueToJD(KstAxisInterpretation axisInterpretation, double valueIn) {
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


double PlotItem::convertTimeDiffValueToDays(KstAxisInterpretation axisInterpretation, double offsetIn) {
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


QString PlotItem::convertJDToDateString(KstAxisInterpretation axisInterpretation, KstAxisDisplay axisDisplay, double dJD) {
  QString label;
  QDate date;

  int accuracy;
  double xdelta = (projectionRect().right()-projectionRect().left())/double(projectionRect().width());
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


QRectF PlotItem::plotAxisRect() const {
  qreal left = isLeftLabelVisible() ? labelMarginWidth() : 0.0;
  qreal bottom = isBottomLabelVisible() ? labelMarginHeight() : 0.0;
  qreal right = isRightLabelVisible() ? labelMarginWidth() : 0.0;
  qreal top = isTopLabelVisible() ? labelMarginHeight() : 0.0;

  QPointF topLeft(rect().topLeft() + QPointF(left, top));
  QPointF bottomRight(rect().bottomRight() - QPointF(right, bottom));

  return QRectF(topLeft, bottomRight);
}


QRectF PlotItem::plotRect() const {
  //the PlotRenderItems use this to set their rects
  QRectF plot = plotAxisRect();
  plot.setLeft(plot.left() + axisMarginWidth());
  plot.setBottom(plot.bottom() - axisMarginHeight());
  return plot;
}


QRectF PlotItem::projectionRect() const {
  return _projectionRect;
}


bool PlotItem::isTiedZoom() const {
  return _isTiedZoom;
}


void PlotItem::setTiedZoom(bool tiedZoom) {
  if (_isTiedZoom == tiedZoom)
    return;

  _isTiedZoom = tiedZoom;

  if (_isTiedZoom)
    PlotItemManager::self()->addTiedZoomPlot(this);
  else
    PlotItemManager::self()->removeTiedZoomPlot(this);

  //FIXME ugh, this is expensive, but need to redraw the renderitems checkboxes...
  update();
}


qreal PlotItem::marginWidth() const {
    return labelMarginWidth() + axisMarginWidth();
}


qreal PlotItem::marginHeight() const {
    return labelMarginHeight() + axisMarginHeight();
}


qreal PlotItem::labelMarginWidth() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotLabelMarginWidth(this);
  } else {
    return calculatedLabelMarginWidth();
  }
}


qreal PlotItem::labelMarginHeight() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotLabelMarginHeight(this);
  } else {
    return calculatedLabelMarginHeight();
  }
}


qreal PlotItem::axisMarginWidth() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotAxisMarginWidth(this);
  } else {
    return calculatedAxisMarginWidth();
  }
}


qreal PlotItem::axisMarginHeight() const {
  ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(parentItem());
  if (viewItem && viewItem->layout()) {
    return viewItem->layout()->plotAxisMarginHeight(this);
  } else {
    return calculatedAxisMarginHeight();
  }
}


bool PlotItem::xAxisLog() const {
  return _xAxisLog;
}


void PlotItem::setXAxisLog(bool log) {
  _xAxisLog = log;
}


bool PlotItem::yAxisLog() const {
  return _yAxisLog;
}


void PlotItem::setYAxisLog(bool log) {
  _yAxisLog = log;
}


PlotItem::MajorTickMode PlotItem::xAxisMajorTickMode() const {
  return _xAxisMajorTickMode;
}


void PlotItem::setXAxisMajorTickMode(PlotItem::MajorTickMode mode) {
  _xAxisMajorTickMode = mode;
}


PlotItem::MajorTickMode PlotItem::yAxisMajorTickMode() const {
  return _yAxisMajorTickMode;
}


void PlotItem::setYAxisMajorTickMode(PlotItem::MajorTickMode mode) {
  _yAxisMajorTickMode = mode;
}


int PlotItem::xAxisMinorTickCount() const {
  return _xAxisMinorTickCount;
}


void PlotItem::setXAxisMinorTickCount(const int count) {
  _xAxisMinorTickCount = count;
}


int PlotItem::yAxisMinorTickCount() const {
  return _yAxisMinorTickCount;
}


void PlotItem::setYAxisMinorTickCount(const int count) {
  _yAxisMinorTickCount = count;
}


bool PlotItem::drawXAxisMajorTicks() const {
  return _drawXAxisMajorTicks;
}


void PlotItem::setDrawXAxisMajorTicks(bool draw) {
  _drawXAxisMajorTicks = draw;
}


bool PlotItem::drawYAxisMajorTicks() const {
  return _drawYAxisMajorTicks;
}


void PlotItem::setDrawYAxisMajorTicks(bool draw) {
  _drawYAxisMajorTicks = draw;
}


bool PlotItem::drawXAxisMinorTicks() const {
  return _drawXAxisMinorTicks;
}


void PlotItem::setDrawXAxisMinorTicks(bool draw) {
  _drawXAxisMinorTicks = draw;
}


bool PlotItem::drawYAxisMinorTicks() const {
  return _drawYAxisMinorTicks;
}


void PlotItem::setDrawYAxisMinorTicks(bool draw) {
  _drawYAxisMinorTicks = draw;
}


bool PlotItem::drawXAxisMajorGridLines() const {
  return _drawXAxisMajorGridLines;
}


void PlotItem::setDrawXAxisMajorGridLines(bool draw) {
  _drawXAxisMajorGridLines = draw;
}


bool PlotItem::drawYAxisMajorGridLines() const {
  return _drawYAxisMajorGridLines;
}


void PlotItem::setDrawYAxisMajorGridLines(bool draw) {
  _drawYAxisMajorGridLines = draw;
}


bool PlotItem::drawXAxisMinorGridLines() const {
  return _drawXAxisMinorGridLines;
}


void PlotItem::setDrawXAxisMinorGridLines(bool draw) {
  _drawXAxisMinorGridLines = draw;
}


bool PlotItem::drawYAxisMinorGridLines() const {
  return _drawYAxisMinorGridLines;
}


void PlotItem::setDrawYAxisMinorGridLines(bool draw) {
  _drawYAxisMinorGridLines = draw;
}


QColor PlotItem::xAxisMajorGridLineColor() const {
  return _xAxisMajorGridLineColor;
}


void PlotItem::setXAxisMajorGridLineColor(const QColor &color) {
  _xAxisMajorGridLineColor = color;
}


QColor PlotItem::xAxisMinorGridLineColor() const {
  return _xAxisMinorGridLineColor;
}


void PlotItem::setXAxisMinorGridLineColor(const QColor &color) {
  _xAxisMinorGridLineColor = color;
}


QColor PlotItem::yAxisMajorGridLineColor() const {
  return _yAxisMajorGridLineColor;
}


void PlotItem::setYAxisMajorGridLineColor(const QColor &color) {
  _yAxisMajorGridLineColor = color;
}


QColor PlotItem::yAxisMinorGridLineColor() const {
  return _yAxisMinorGridLineColor;
}


void PlotItem::setYAxisMinorGridLineColor(const QColor &color) {
  _yAxisMinorGridLineColor = color;
}


Qt::PenStyle PlotItem::xAxisMajorGridLineStyle() const {
  return _xAxisMajorGridLineStyle;
}


void PlotItem::setXAxisMajorGridLineStyle(const Qt::PenStyle style) {
  _xAxisMajorGridLineStyle = style;
}


Qt::PenStyle PlotItem::xAxisMinorGridLineStyle() const {
  return _xAxisMinorGridLineStyle;
}


void PlotItem::setXAxisMinorGridLineStyle(const Qt::PenStyle style) {
  _xAxisMinorGridLineStyle = style;
}


Qt::PenStyle PlotItem::yAxisMajorGridLineStyle() const {
  return _yAxisMajorGridLineStyle;
}


void PlotItem::setYAxisMajorGridLineStyle(const Qt::PenStyle style) {
  _yAxisMajorGridLineStyle = style;
}


Qt::PenStyle PlotItem::yAxisMinorGridLineStyle() const {
  return _yAxisMinorGridLineStyle;
}


void PlotItem::setYAxisMinorGridLineStyle(const Qt::PenStyle style) {
  _yAxisMinorGridLineStyle = style;
}

void PlotItem::updateScale() {
  if (_xAxisLog) {
    _xMax = logXHi(projectionRect().right());
    _xMin = logXLo(projectionRect().left());
  } else {
    _xMax = projectionRect().right();
    _xMin = projectionRect().left();
  }

  if (_yAxisLog) {
    _yMax = logYHi(projectionRect().bottom());
    _yMin = logYLo(projectionRect().top());
  } else {
    _yMax = projectionRect().bottom();
    _yMin = projectionRect().top();
  }
}


QPointF PlotItem::mapPointToProjection(const QPointF &point) {
  QRectF pr = plotRect();
  double xpos, ypos;

  updateScale();

  if (_xAxisReversed) {
    xpos = (double)(pr.right() - point.x())/(double)pr.width();
  } else {
    xpos = (double)(point.x() - pr.left())/(double)pr.width();
  }
  xpos = xpos * (_xMax - _xMin) + _xMin;

  if (_xAxisLog) {
    xpos = pow(10, xpos);
  }

  if (_yAxisReversed) {
    ypos = (double)(point.y() - pr.top())/(double)pr.height();
  } else {
    ypos = (double)(pr.bottom() - point.y())/(double)pr.height();
  }
  ypos = ypos * (_yMax - _yMin) + _yMin;

  if (_yAxisLog) {
    ypos = pow(10, ypos);
  }

  return QPointF(xpos, ypos);
}


QPointF PlotItem::mapPointToPlot(const QPointF &point) const {
  return QPointF(mapXToPlot(point.x()), mapYToPlot(point.y()));
}


qreal PlotItem::mapXToPlot(const qreal &x) const {
  QRectF pr = plotRect();
  double newX = x;

  if (_xAxisLog) {
    newX = logXLo(x);
  }

  newX -= _xMin;
  newX = newX / (_xMax - _xMin);

  newX = newX * pr.width();

  if (_xAxisLog && x == -350) {
    newX = 0;
  }

  if (_xAxisReversed) {
    newX = pr.right() - newX;
  } else {
    newX = newX + pr.left();
  }
  return newX;
}


qreal PlotItem::mapYToPlot(const qreal &y) const {
  QRectF pr = plotRect();
  double newY = y;

  if (_yAxisLog) {
    newY = logYLo(y);
  }

  newY -= _yMin;
  newY = newY / (_yMax - _yMin);

  newY = newY * pr.height();

  if (_yAxisLog && y == -350) {
    newY = 0;
  }

  if (_yAxisReversed) {
    newY = newY + pr.top();
  } else {
    newY = pr.bottom() - newY;
  }
  return newY;
}


QFont PlotItem::rightLabelFont() const {
  return _rightLabelFont;
}


void PlotItem::setRightLabelFont(const QFont &font) {
  _rightLabelFont = font;
}


QFont PlotItem::topLabelFont() const {
  return _topLabelFont;
}


void PlotItem::setTopLabelFont(const QFont &font) {
  _topLabelFont = font;
}


QFont PlotItem::leftLabelFont() const {
  return _leftLabelFont;
}


void PlotItem::setLeftLabelFont(const QFont &font) {
  _leftLabelFont = font;
}


QFont PlotItem::bottomLabelFont() const {
  return _bottomLabelFont;
}


void PlotItem::setBottomLabelFont(const QFont &font) {
  _bottomLabelFont = font;
}


QString PlotItem::leftLabelOverride() const {
  if (_leftLabelOverride.isEmpty()) {
    return leftLabel();
  } else {
    return _leftLabelOverride;
  }
}


void PlotItem::setLeftLabelOverride(const QString &label) {
  if (label == leftLabel()) {
    _leftLabelOverride.clear();
  } else {
    _leftLabelOverride = label;
  }
}


QString PlotItem::bottomLabelOverride() const {
  if (_bottomLabelOverride.isEmpty()) {
    return bottomLabel();
  } else {
    return _bottomLabelOverride;
  }
}


void PlotItem::setBottomLabelOverride(const QString &label) {
  if (label == bottomLabel()) {
    _bottomLabelOverride.clear();
  } else {
    _bottomLabelOverride = label;
  }
}


QString PlotItem::topLabelOverride() const {
  if (_topLabelOverride.isEmpty()) {
    return topLabel();
  } else {
    return _topLabelOverride;
  }
}


void PlotItem::setTopLabelOverride(const QString &label) {
  if (label == topLabel()) {
    _topLabelOverride.clear();
  } else {
    _topLabelOverride = label;
  }
}


QString PlotItem::rightLabelOverride() const {
  if (_rightLabelOverride.isEmpty()) {
    return rightLabel();
  } else {
    return _rightLabelOverride;
  }
}


void PlotItem::setRightLabelOverride(const QString &label) {
  if (label == rightLabel()) {
    _rightLabelOverride.clear();
  } else {
    _rightLabelOverride = label;
  }
}


QString PlotItem::leftLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->leftLabel().isEmpty())
      return renderer->leftLabel();
  }
  return QString();
}


QString PlotItem::bottomLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->bottomLabel().isEmpty())
      return renderer->bottomLabel();
  }
  return QString();
}


QString PlotItem::rightLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->rightLabel().isEmpty())
      return renderer->rightLabel();
  }
  return QString();
}


QString PlotItem::topLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->topLabel().isEmpty())
      return renderer->topLabel();
  }
  return QString();
}


bool PlotItem::isLeftLabelVisible() const {
  return _isLeftLabelVisible;
}


void PlotItem::setLeftLabelVisible(bool visible) {
  if (_isLeftLabelVisible == visible)
    return;

  _isLeftLabelVisible = visible;
  emit marginsChanged();
}


bool PlotItem::isBottomLabelVisible() const {
  return _isBottomLabelVisible;
}


void PlotItem::setBottomLabelVisible(bool visible) {
  if (_isBottomLabelVisible == visible)
    return;

  _isBottomLabelVisible = visible;
  emit marginsChanged();
}


bool PlotItem::isRightLabelVisible() const {
  return _isRightLabelVisible;
}


void PlotItem::setRightLabelVisible(bool visible) {
  if (_isRightLabelVisible == visible)
    return;

  _isRightLabelVisible = visible;
  emit marginsChanged();
}


bool PlotItem::isTopLabelVisible() const {
  return _isTopLabelVisible;
}


void PlotItem::setTopLabelVisible(bool visible) {
  if (_isTopLabelVisible == visible)
    return;

  _isTopLabelVisible = visible;
  emit marginsChanged();
}


void PlotItem::setLabelsVisible(bool visible) {
  setLeftLabelVisible(visible);
  setRightLabelVisible(visible);
  setBottomLabelVisible(visible);
  setTopLabelVisible(visible);
}


bool PlotItem::xAxisReversed() const {
  return _xAxisReversed;
}


void PlotItem::setXAxisReversed(const bool enabled) {
  _xAxisReversed = enabled;
}


bool PlotItem::yAxisReversed() const {
  return _yAxisReversed;
}


void PlotItem::setYAxisReversed(const bool enabled) {
  _yAxisReversed = enabled;
}


bool PlotItem::xAxisBaseOffset() const {
  return _xAxisBaseOffset;
}


void PlotItem::setXAxisBaseOffset(const bool enabled) {
  _xAxisBaseOffset = enabled;
}


bool PlotItem::yAxisBaseOffset() const {
  return _yAxisBaseOffset;
}


void PlotItem::setYAxisBaseOffset(const bool enabled) {
  _yAxisBaseOffset = enabled;
}


bool PlotItem::xAxisInterpret() const {
  return _xAxisInterpret;
}


void PlotItem::setXAxisInterpret(const bool enabled) {
  _xAxisInterpret = enabled;
}


bool PlotItem::yAxisInterpret() const {
  return _yAxisInterpret;
}


void PlotItem::setYAxisInterpret(const bool enabled) {
  _yAxisInterpret = enabled;
}


KstAxisDisplay PlotItem::xAxisDisplay() const {
  return _xAxisDisplay;
}


void PlotItem::setXAxisDisplay(const KstAxisDisplay display) {
  _xAxisDisplay = display;
}


KstAxisDisplay PlotItem::yAxisDisplay() const {
  return _yAxisDisplay;
}


void PlotItem::setYAxisDisplay(const KstAxisDisplay display) {
  _yAxisDisplay = display;
}


KstAxisInterpretation PlotItem::xAxisInterpretation() const {
  return _xAxisInterpretation;
}


void PlotItem::setXAxisInterpretation(const KstAxisInterpretation display) {
  _xAxisInterpretation = display;
}


KstAxisInterpretation PlotItem::yAxisInterpretation() const {
  return _yAxisInterpretation;
}


void PlotItem::setYAxisInterpretation(const KstAxisInterpretation display) {
  _yAxisInterpretation = display;
}


qreal PlotItem::calculatedLabelMarginWidth() const {
  qreal m = qMax(MARGIN_WIDTH, _calculatedLabelMarginWidth);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setCalculatedLabelMarginWidth(qreal marginWidth) {
  qreal before = this->calculatedLabelMarginWidth();
  _calculatedLabelMarginWidth = marginWidth;
  if (before != this->calculatedLabelMarginWidth())
    emit marginsChanged();
}


qreal PlotItem::calculatedLabelMarginHeight() const {
  qreal m = qMax(MARGIN_HEIGHT, _calculatedLabelMarginHeight);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setCalculatedLabelMarginHeight(qreal marginHeight) {
  qreal before = this->calculatedLabelMarginHeight();
  _calculatedLabelMarginHeight = marginHeight;
  if (before != this->calculatedLabelMarginHeight())
    emit marginsChanged();
}


QRectF PlotItem::horizontalLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, width() - 2.0 * calculatedLabelMarginWidth(), calculatedLabelMarginHeight());
  else
    return QRectF(0.0, 0.0, width() - 2.0 * labelMarginWidth(), labelMarginHeight());
}


QRectF PlotItem::verticalLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, calculatedLabelMarginWidth(), height() - 2.0 * calculatedLabelMarginHeight());
  else
    return QRectF(0.0, 0.0, labelMarginWidth(), height() - 2.0 * labelMarginHeight());
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!isLeftLabelVisible())
    return;

  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  painter->setFont(_leftLabelFont);

  QRectF leftLabelRect = verticalLabelRect(false);
  leftLabelRect.moveTopLeft(QPointF(0.0, labelMarginHeight()));
  painter->drawText(t.mapRect(leftLabelRect), Qt::TextWordWrap | Qt::AlignCenter, leftLabelOverride());

//   painter->save();
//   painter->setOpacity(0.3);
// //  qDebug() << "leftLabelRect:" << t.mapRect(leftLabelRect) << endl;
//   painter->fillRect(t.mapRect(leftLabelRect), Qt::red);
//   painter->restore();

  painter->restore();
}


QSizeF PlotItem::calculateLeftLabelBound(QPainter *painter) {
  if (!isLeftLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  painter->setFont(_leftLabelFont);

  QRectF leftLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                Qt::TextWordWrap | Qt::AlignCenter, leftLabelOverride());
  painter->restore();

  QSizeF margins;
  margins.setWidth(leftLabelBound.height());
  return margins;
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!isBottomLabelVisible())
    return;

  painter->save();
  painter->setFont(_bottomLabelFont);

  QRectF bottomLabelRect = horizontalLabelRect(false);
  bottomLabelRect.moveTopLeft(QPointF(labelMarginWidth(), height() - labelMarginHeight()));
  painter->drawText(bottomLabelRect, Qt::TextWordWrap | Qt::AlignCenter, bottomLabelOverride());
  painter->restore();
}


QSizeF PlotItem::calculateBottomLabelBound(QPainter *painter) {
  if (!isBottomLabelVisible())
    return QSizeF();

  painter->save();
  painter->setFont(_bottomLabelFont);

  QRectF bottomLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                                  Qt::TextWordWrap | Qt::AlignCenter, bottomLabelOverride());
  painter->restore();
  QSizeF margins;
  margins.setHeight(bottomLabelBound.height());
  return margins;
}


void PlotItem::paintRightLabel(QPainter *painter) {
  if (!isRightLabelVisible())
    return;

  painter->save();
  painter->translate(width() - labelMarginWidth(), 0.0);
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  painter->setFont(_rightLabelFont);

  //same as left but painter is translated
  QRectF rightLabelRect = verticalLabelRect(false);
  rightLabelRect.moveTopLeft(QPointF(0.0, labelMarginHeight()));
  painter->drawText(t.mapRect(rightLabelRect), Qt::TextWordWrap | Qt::AlignCenter, rightLabelOverride());
  painter->restore();
}


QSizeF PlotItem::calculateRightLabelBound(QPainter *painter) {
  if (!isRightLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);
  painter->setFont(_rightLabelFont);

  QRectF rightLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                 Qt::TextWordWrap | Qt::AlignCenter, rightLabelOverride());
  painter->restore();

  QSizeF margins;
  margins.setWidth(rightLabelBound.height());
  return margins;
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (!isTopLabelVisible())
    return;

  painter->save();
  painter->setFont(_topLabelFont);
  QRectF topLabelRect = horizontalLabelRect(false);
  topLabelRect.moveTopLeft(QPointF(labelMarginWidth(), 0.0));
  painter->drawText(topLabelRect, Qt::TextWordWrap | Qt::AlignCenter, topLabelOverride());
  painter->restore();
}


QSizeF PlotItem::calculateTopLabelBound(QPainter *painter) {
  if (!isTopLabelVisible())
    return QSizeF();

  painter->save();

  painter->setFont(_topLabelFont);
  QRectF topLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                               Qt::TextWordWrap | Qt::AlignCenter, topLabelOverride());

  painter->restore();

  QSizeF margins;
  margins.setHeight(topLabelBound.height());
  return margins;
}


qreal PlotItem::calculatedAxisMarginWidth() const {
  return _calculatedAxisMarginWidth;
}


void PlotItem::setCalculatedAxisMarginWidth(qreal marginWidth) {
  qreal before = this->calculatedAxisMarginWidth();
  _calculatedAxisMarginWidth = marginWidth;
  if (before != this->calculatedAxisMarginWidth())
    emit marginsChanged();
}


qreal PlotItem::calculatedAxisMarginHeight() const {
  return _calculatedAxisMarginHeight;
}


void PlotItem::setCalculatedAxisMarginHeight(qreal marginHeight) {
  qreal before = this->calculatedAxisMarginHeight();
  _calculatedAxisMarginHeight = marginHeight;
  if (before != this->calculatedAxisMarginHeight())
    emit marginsChanged();
}


void PlotItem::computeLogTicks(QList<qreal> *MajorTicks, QList<qreal> *MinorTicks, QMap<qreal, QString> *Labels, qreal min, qreal max, MajorTickMode tickMode) {

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
    qreal majorPoint = pow(10, i);
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


void PlotItem::computeTicks(QList<qreal> *xMajorTicks, QList<qreal> *xMinorTicks, QList<qreal> *yMajorTicks, QList<qreal> *yMinorTicks, QMap<qreal, QString> *xLabelsIn, QMap<qreal, QString> *yLabelsIn) {

  QList<qreal> xTicks;
  QList<qreal> xMinTicks;
  updateScale();
  if (_xAxisLog) {
    computeLogTicks(&xTicks, &xMinTicks, xLabelsIn, _xMin, _xMax, _xAxisMajorTickMode);
  } else {
    qreal xMajorTickSpacing = computedMajorTickSpacing(Qt::Horizontal);
    qreal firstXTick = ceil(projectionRect().left() / xMajorTickSpacing) * xMajorTickSpacing;

    int ix = 0;
    qreal nextXTick = firstXTick;
    while (1) {
      nextXTick = firstXTick + (ix++ * xMajorTickSpacing);
      if (!projectionRect().contains(nextXTick, projectionRect().y()))
        break;
      xTicks << nextXTick;
      xLabelsIn->insert(nextXTick, QString::number(nextXTick, 'g', FULL_PRECISION));
    }

    qreal xMinorTickSpacing = 0;
    if (_xAxisMinorTickCount > 0) {
      xMinorTickSpacing = xMajorTickSpacing / _xAxisMinorTickCount;
    }

    if (xMinorTickSpacing != 0) {
      qreal firstXMinorTick = (firstXTick - xMajorTickSpacing) + xMinorTickSpacing;

      ix = 0;
      qreal nextXMinorTick = firstXMinorTick;
      while (1) {
        nextXMinorTick = firstXMinorTick + (ix++ * xMinorTickSpacing);
        if (projectionRect().right() < nextXMinorTick)
          break;
        if (!xTicks.contains(nextXMinorTick) && projectionRect().contains(nextXMinorTick, projectionRect().y())) {
          xMinTicks << nextXMinorTick;
        }
      }
    }
  }

  QList<qreal> yTicks;
  QList<qreal> yMinTicks;
  if (_yAxisLog) {
    computeLogTicks(&yTicks, &yMinTicks, yLabelsIn, _yMin, _yMax, _yAxisMajorTickMode);
  } else {
    qreal yMajorTickSpacing = computedMajorTickSpacing(Qt::Vertical);
    qreal firstYTick = ceil(projectionRect().top() / yMajorTickSpacing) * yMajorTickSpacing;

    int iy = 0;
    qreal nextYTick = firstYTick;
    while (1) {
      nextYTick = firstYTick + (iy++ * yMajorTickSpacing);
      if (!projectionRect().contains(projectionRect().x(), nextYTick))
        break;
      yTicks << nextYTick;
      yLabelsIn->insert(nextYTick, QString::number(nextYTick, 'g', FULL_PRECISION));
    }

    qreal yMinorTickSpacing = 0;
    if (_yAxisMinorTickCount > 0) {
      yMinorTickSpacing = yMajorTickSpacing / _yAxisMinorTickCount;
    }
    qreal firstYMinorTick = (firstYTick - yMajorTickSpacing) + yMinorTickSpacing;

    if (yMinorTickSpacing != 0) {
      iy = 0;
      qreal nextYMinorTick = firstYMinorTick;
      while (1) {
        nextYMinorTick = firstYMinorTick + (iy++ * yMinorTickSpacing);
        if (projectionRect().bottom() < nextYMinorTick)
          break;
        if (!yTicks.contains(nextYMinorTick) && projectionRect().contains(projectionRect().x(), nextYMinorTick)) {
          yMinTicks << nextYMinorTick;
        }
      }
    }
  }

  *xMajorTicks = xTicks;
  *xMinorTicks = xMinTicks;
  *yMajorTicks = yTicks;
  *yMinorTicks = yMinTicks;
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
qreal PlotItem::computedMajorTickSpacing(Qt::Orientation orientation) const {
  qreal R = orientation == Qt::Horizontal ? projectionRect().width() : projectionRect().height();
  qreal M = orientation == Qt::Horizontal ? xAxisMajorTickMode() : yAxisMajorTickMode();
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


QSizeF PlotItem::calculateXTickLabelBound(QPainter *painter,
                                              const QList<qreal> &xMajorTicks) {
  QRectF xLabelRect;
  foreach (qreal x, xMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(x);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p(mapXToPlot(x), plotRect().bottom() + bound.height() / 2.0);
    bound.moveCenter(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }
  }

  return xLabelRect.size();
}


QSizeF PlotItem::calculateYTickLabelBound(QPainter *painter,
                                              const QList<qreal> &yMajorTicks) {
  QRectF yLabelRect;
  foreach (qreal y, yMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(y);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p(plotRect().left() - bound.width() / 2.0, mapYToPlot(y));
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }
  }

  return yLabelRect.size();
}


void PlotItem::calculateProjectionRect() {
  QRectF rect;
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->projectionRect().isEmpty()) {
      if (rect.isValid()) {
        rect = rect.united(renderer->projectionRect());
      } else {
        rect = renderer->projectionRect();
      }
    }
  }

  if (!rect.isValid())
    rect = QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)); //default

  if (rect != _projectionRect) {
    _projectionRect = rect;
    emit projectionRectChanged();
    update(); //slow, but need to update everything...
  }
}


void CreatePlotCommand::createItem() {
  _item = new PlotItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}

void CreatePlotForCurve::createItem() {
  QPointF center = _view->sceneRect().center();
  center -= QPointF(100.0, 100.0);

  _item = new PlotItem(_view);
  _item->setPos(center);
  _item->setViewRect(0.0, 0.0, 200.0, 200.0);
  //_item->setZValue(1);
  _view->scene()->addItem(_item);

  if (_createLayout) {
    _view->createLayout();
  }

  if (_appendToLayout && _view->layoutBoxItem()) {
    _view->layoutBoxItem()->appendItem(_item);
  }

  creationComplete(); //add to undo stack
}


PlotItemFactory::PlotItemFactory()
: GraphicsFactory() {
  registerFactory("plot", this);
}


PlotItemFactory::~PlotItemFactory() {
}


ViewItem* PlotItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  PlotItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "plot") {
        Q_ASSERT(!rc);
        rc = new PlotItem(view);
        if (parent) {
          rc->setParentItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("tiedzoom");
        if (!av.isNull()) {
          rc->setTiedZoom(QVariant(av.toString()).toBool());
        }
        av = attrs.value("leftlabelvisible");
        if (!av.isNull()) {
          rc->setLeftLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("bottomlabelvisible");
        if (!av.isNull()) {
          rc->setBottomLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("rightlabelvisible");
        if (!av.isNull()) {
          rc->setRightLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("toplabelvisible");
        if (!av.isNull()) {
          rc->setTopLabelVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxislog");
        if (!av.isNull()) {
          rc->setXAxisLog(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxislog");
        if (!av.isNull()) {
          rc->setYAxisLog(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisreversed");
        if (!av.isNull()) {
          rc->setXAxisReversed(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisreversed");
        if (!av.isNull()) {
          rc->setYAxisReversed(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisbaseoffset");
        if (!av.isNull()) {
          rc->setXAxisBaseOffset(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisbaseoffset");
        if (!av.isNull()) {
          rc->setYAxisBaseOffset(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisinterpret");
        if (!av.isNull()) {
          rc->setXAxisInterpret(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisinterpret");
        if (!av.isNull()) {
          rc->setYAxisInterpret(QVariant(av.toString()).toBool());
        }
        av = attrs.value("leftlabeloverride");
        if (!av.isNull()) {
          rc->setLeftLabelOverride(av.toString());
        }
        av = attrs.value("leftlabelfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setLeftLabelFont(font);
        }
        av = attrs.value("bottomlabeloverride");
        if (!av.isNull()) {
          rc->setBottomLabelOverride(av.toString());
        }
        av = attrs.value("bottomlabelfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setBottomLabelFont(font);
        }
        av = attrs.value("toplabeloverride");
        if (!av.isNull()) {
          rc->setTopLabelOverride(av.toString());
        }
        av = attrs.value("toplabelfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setTopLabelFont(font);
        }
        av = attrs.value("rightlabeloverride");
        if (!av.isNull()) {
          rc->setRightLabelOverride(av.toString());
        }
        av = attrs.value("rightlabelfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setRightLabelFont(font);
        }
        av = attrs.value("xaxisinterpretation");
        if (!av.isNull()) {
          rc->setXAxisInterpretation((KstAxisInterpretation)QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxisinterpretation");
        if (!av.isNull()) {
          rc->setYAxisInterpretation((KstAxisInterpretation)QVariant(av.toString()).toInt());
        }
        av = attrs.value("xaxisdisplay");
        if (!av.isNull()) {
          rc->setXAxisDisplay((KstAxisDisplay)QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxisdisplay");
        if (!av.isNull()) {
          rc->setYAxisDisplay((KstAxisDisplay)QVariant(av.toString()).toInt());
        }
        av = attrs.value("xaxismajortickmode");
        if (!av.isNull()) {
          rc->setXAxisMajorTickMode((PlotItem::MajorTickMode)QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxismajortickmode");
        if (!av.isNull()) {
          rc->setYAxisMajorTickMode((PlotItem::MajorTickMode)QVariant(av.toString()).toInt());
        }
        av = attrs.value("xaxisminortickcount");
        if (!av.isNull()) {
          rc->setXAxisMinorTickCount(QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxisminortickcount");
        if (!av.isNull()) {
          rc->setYAxisMinorTickCount(QVariant(av.toString()).toInt());
        }
        av = attrs.value("xaxisdrawmajorticks");
        if (!av.isNull()) {
          rc->setDrawXAxisMajorTicks(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisdrawminorticks");
        if (!av.isNull()) {
          rc->setDrawXAxisMinorTicks(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisdrawmajorticks");
        if (!av.isNull()) {
          rc->setDrawYAxisMajorTicks(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisdrawminorticks");
        if (!av.isNull()) {
          rc->setDrawYAxisMinorTicks(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisdrawmajorgridlines");
        if (!av.isNull()) {
          rc->setDrawXAxisMajorGridLines(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisdrawminorgridlines");
        if (!av.isNull()) {
          rc->setDrawXAxisMinorGridLines(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisdrawmajorgridlines");
        if (!av.isNull()) {
          rc->setDrawYAxisMajorGridLines(QVariant(av.toString()).toBool());
        }
        av = attrs.value("yaxisdrawminorgridlines");
        if (!av.isNull()) {
          rc->setDrawYAxisMinorGridLines(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xaxisdrawmajorgridlinecolor");
        if (!av.isNull()) {
          rc->setXAxisMajorGridLineColor(QColor(av.toString()));
        }
        av = attrs.value("xaxisdrawminorgridlinecolor");
        if (!av.isNull()) {
          rc->setXAxisMinorGridLineColor(QColor(av.toString()));
        }
        av = attrs.value("yaxisdrawmajorgridlinecolor");
        if (!av.isNull()) {
          rc->setYAxisMajorGridLineColor(QColor(av.toString()));
        }
        av = attrs.value("yaxisdrawminorgridlinecolor");
        if (!av.isNull()) {
          rc->setYAxisMinorGridLineColor(QColor(av.toString()));
        }
        av = attrs.value("xaxisdrawmajorgridlinestyle");
        if (!av.isNull()) {
          rc->setXAxisMajorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
        }
        av = attrs.value("xaxisdrawminorgridlinestyle");
        if (!av.isNull()) {
          rc->setXAxisMinorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxisdrawmajorgridlinestyle");
        if (!av.isNull()) {
          rc->setYAxisMajorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
        }
        av = attrs.value("yaxisdrawminorgridlinestyle");
        if (!av.isNull()) {
          rc->setYAxisMinorGridLineStyle((Qt::PenStyle)QVariant(av.toString()).toInt());
        }

      // TODO add any specialized PlotItem Properties here.
      } else if (xml.name().toString() == "cartesianrender") {
        Q_ASSERT(rc);
        PlotRenderItem * renderItem = rc->renderItem(PlotRenderItem::Cartesian);
        if (renderItem) {
          validTag = renderItem->configureFromXml(xml, store);
        }
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "plot") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating plot object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  return rc;
}

}

// vim: ts=2 sw=2 et
