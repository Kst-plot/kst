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

#include "plotaxisitem.h"

#include "plotitem.h"
#include "viewitemzorder.h"
#include "plotitemmanager.h"
#include "application.h"

#include <QDebug>
#include <QStatusBar>
#include <QMainWindow>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneContextMenuEvent>

// #define MAJOR_TICK_DEBUG

namespace Kst {

PlotAxisItem::PlotAxisItem(PlotItem *parentItem)
    : ViewItem(parentItem->parentView()),
    _marginWidth(0.0),
    _marginHeight(0.0),
    _xAxisMajorTickMode(Normal),
    _yAxisMajorTickMode(Normal) {

  setName("Plot Axis");
  setZValue(PLOTAXIS_ZVALUE);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, false);
  setParentItem(parentItem);
  setHasStaticGeometry(true);
  setAllowedGripModes(0);
  setAcceptedMouseButtons(0);
  setAcceptsHoverEvents(false);
  setAcceptsChildItems(false),
  setAcceptsContextMenuEvents(false);

  connect(parentItem, SIGNAL(geometryChanged()),
          this, SLOT(updateGeometry()));
  connect(parentItem, SIGNAL(labelVisibilityChanged()),
          this, SLOT(updateGeometry()));

  updateGeometry(); //the initial rect
}


PlotAxisItem::~PlotAxisItem() {
}


PlotItem *PlotAxisItem::plotItem() const {
  return qobject_cast<PlotItem*>(qgraphicsitem_cast<ViewItem*>(parentItem()));
}


PlotAxisItem::MajorTickMode PlotAxisItem::xAxisMajorTickMode() const {
  return _xAxisMajorTickMode;
}


void PlotAxisItem::setXAxisMajorTickMode(PlotAxisItem::MajorTickMode mode) {
  _xAxisMajorTickMode = mode;
}


PlotAxisItem::MajorTickMode PlotAxisItem::yAxisMajorTickMode() const {
  return _yAxisMajorTickMode;
}


void PlotAxisItem::setYAxisMajorTickMode(PlotAxisItem::MajorTickMode mode) {
  _yAxisMajorTickMode = mode;
}


QRectF PlotAxisItem::plotRect() const {
  QRectF plot = rect();
  plot.setLeft(plot.left() + marginWidth());
  plot.setBottom(plot.bottom() - marginHeight());
  return plot;
}


QRectF PlotAxisItem::projectionRect() const {
  return plotItem()->projectionRect();
}


qreal PlotAxisItem::marginWidth() const {
  return _marginWidth;
}


qreal PlotAxisItem::marginHeight() const {
  return _marginHeight;
}


void PlotAxisItem::paint(QPainter *painter) {
  painter->setRenderHint(QPainter::Antialiasing, false);

  painter->save();
  painter->setClipRect(rect());

  QList<qreal> xMajorTicks;
  QList<qreal> yMajorTicks;
  computeMajorTicks(&xMajorTicks, &yMajorTicks);

  qreal marginWidth = _marginWidth;
  qreal marginHeight = _marginHeight;
  _marginHeight = calculateXTickLabelBound(painter, xMajorTicks).height();
  _marginWidth = calculateYTickLabelBound(painter, yMajorTicks).width();
  if (_marginWidth != marginWidth || _marginHeight != marginHeight)
    emit marginChanged();

//   qDebug() << "marginHeight=" << _marginHeight << endl;
//   qDebug() << "marginWidth=" << _marginWidth << endl;

  paintMajorGridLines(painter, xMajorTicks, yMajorTicks);
  paintMajorTicks(painter, xMajorTicks, yMajorTicks);
  paintMajorTickLabels(painter, xMajorTicks, yMajorTicks);

  painter->restore();
}


void PlotAxisItem::save(QXmlStreamWriter &xml) {
    Q_UNUSED(xml);
}


void PlotAxisItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeStartElement("plotaxis");
  xml.writeAttribute("xtickmode", QVariant(_xAxisMajorTickMode).toString());
  xml.writeAttribute("ytickmode", QVariant(_yAxisMajorTickMode).toString());
  xml.writeEndElement();
}


bool PlotAxisItem::configureFromXml(QXmlStreamReader &xml) {
  bool validTag = false;
  if (xml.isStartElement() && xml.name().toString() == "plotaxis") {
    QXmlStreamAttributes attrs = xml.attributes();
    QStringRef av;
    av = attrs.value("xtickmode");
    if (!av.isNull()) {
      setXAxisMajorTickMode((MajorTickMode)av.toString().toInt());
    }
    av = attrs.value("ytickmode");
    if (!av.isNull()) {
      setYAxisMajorTickMode((MajorTickMode)av.toString().toInt());
    }
    xml.readNext();
    if (xml.isEndElement() && xml.name().toString() == "plotaxis") {
      validTag = true;
    }
  }
  return validTag;
}


void PlotAxisItem::paintMajorGridLines(QPainter *painter,
                                       const QList<qreal> &xMajorTicks,
                                       const QList<qreal> &yMajorTicks) {

  QRectF rect = plotItem()->plotAxisRect();

  QVector<QLineF> xMajorTickLines;
  foreach (qreal x, xMajorTicks) {
    QPointF p1 = mapToPlotFromProjection(QPointF(x, projectionRect().top()));
    QPointF p2 = p1 - QPointF(0, rect.height());
    xMajorTickLines << QLineF(p1, p2);
  }

  QVector<QLineF> yMajorTickLines;
  foreach (qreal y, yMajorTicks) {
    QPointF p1 = mapToPlotFromProjection(QPointF(projectionRect().left(), y));
    QPointF p2 = p1 + QPointF(rect.width(), 0);
    yMajorTickLines << QLineF(p1, p2);
  }

  painter->save();
  painter->setPen(QPen(QBrush(Qt::gray), 1.0, Qt::DashLine));
  painter->drawLines(xMajorTickLines);
  painter->drawLines(yMajorTickLines);
  painter->restore();
}


void PlotAxisItem::paintMajorTicks(QPainter *painter,
                                   const QList<qreal> &xMajorTicks,
                                   const QList<qreal> &yMajorTicks) {

  qreal majorTickLength = qMin(rect().width(), rect().height()) * 0.02; //two percent

  QVector<QLineF> xMajorTickLines;
  foreach (qreal x, xMajorTicks) {
    QPointF p1 = mapToPlotFromProjection(QPointF(x, projectionRect().top()));
    QPointF p2 = p1 - QPointF(0, majorTickLength);
    xMajorTickLines << QLineF(p1, p2);

    p1 = mapToPlotFromProjection(QPointF(x, projectionRect().bottom()));
    p2 = p1 + QPointF(0, majorTickLength);
    xMajorTickLines << QLineF(p1, p2);
  }

  QVector<QLineF> yMajorTickLines;
  foreach (qreal y, yMajorTicks) {
    QPointF p1 = mapToPlotFromProjection(QPointF(projectionRect().left(), y));
    QPointF p2 = p1 + QPointF(majorTickLength, 0);
    yMajorTickLines << QLineF(p1, p2);

    p1 = mapToPlotFromProjection(QPointF(projectionRect().right(), y));
    p2 = p1 - QPointF(majorTickLength, 0);
    yMajorTickLines << QLineF(p1, p2);
  }

  painter->drawLines(xMajorTickLines);
  painter->drawLines(yMajorTickLines);
}


void PlotAxisItem::paintMajorTickLabels(QPainter *painter,
                                        const QList<qreal> &xMajorTicks,
                                        const QList<qreal> &yMajorTicks) {

  QRectF yLabelRect;
  foreach (qreal y, yMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(y);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p = mapToPlotFromProjection(QPointF(projectionRect().left(), y));
    p.setX(p.x() - bound.width() / 2.0);
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    painter->drawText(bound, flags, label);
  }

  QRectF xLabelRect;
  foreach (qreal x, xMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(x);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p = mapToPlotFromProjection(QPointF(x, projectionRect().top()));
    p.setY(p.y() + bound.height() / 2.0);
    bound.moveCenter(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    painter->drawText(bound, flags, label);
  }

  _xLabelRect = xLabelRect;
  _yLabelRect = yLabelRect;

//   painter->save();
//   painter->setOpacity(0.3);
//   qDebug() << "xLabelRect:" << xLabelRect << endl;
//   painter->fillRect(xLabelRect, Qt::blue);
// 
//   qDebug() << "yLabelRect:" << yLabelRect << endl;
//   painter->fillRect(yLabelRect, Qt::green);
//   painter->restore();
}


QPointF PlotAxisItem::mapFromAxisToProjection(const QPointF &point) const {
  return projectionAxisTransform().map(point);
}


QPointF PlotAxisItem::mapToAxisFromProjection(const QPointF &point) const {
  return projectionAxisTransform().inverted().map(point);
}


QRectF PlotAxisItem::mapFromAxisToProjection(const QRectF &rect) const {
  return projectionAxisTransform().mapRect(rect);
}


QRectF PlotAxisItem::mapToAxisFromProjection(const QRectF &rect) const {
  return projectionAxisTransform().inverted().mapRect(rect);
}


QTransform PlotAxisItem::projectionAxisTransform() const {
  QTransform t;

  QRectF rect = plotItem()->plotAxisRect();
  QRectF v = QRectF(rect.bottomLeft(), rect.topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(projectionRect());
  to_.pop_back(); //get rid of last closed point

  QTransform::quadToQuad(from_, to_, t);
  return t;
}


QPointF PlotAxisItem::mapFromPlotToProjection(const QPointF &point) const {
  return projectionPlotTransform().map(point);
}


QPointF PlotAxisItem::mapToPlotFromProjection(const QPointF &point) const {
  return projectionPlotTransform().inverted().map(point);
}


QRectF PlotAxisItem::mapFromPlotToProjection(const QRectF &rect) const {
  return projectionPlotTransform().mapRect(rect);
}


QRectF PlotAxisItem::mapToPlotFromProjection(const QRectF &rect) const {
  return projectionPlotTransform().inverted().mapRect(rect);
}


QTransform PlotAxisItem::projectionPlotTransform() const {
  QTransform t;

  QRectF rect = plotItem()->plotRect();
  QRectF v = QRectF(rect.bottomLeft(), rect.topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(projectionRect());
  to_.pop_back(); //get rid of last closed point

  QTransform::quadToQuad(from_, to_, t);
  return t;
}


bool PlotAxisItem::maybeReparent() {
  return false; //never reparent a plot renderer
}


void PlotAxisItem::updateGeometry() {
  setViewRect(plotItem()->plotAxisRect()/*.adjusted(-20.0, -20.0, 20.0, 20.0)*/);
}


void PlotAxisItem::computeMajorTicks(QList<qreal> *xMajorTicks, QList<qreal> *yMajorTicks) const {
  qreal xMajorTickSpacing = computedMajorTickSpacing(Qt::Horizontal);
  qreal yMajorTickSpacing = computedMajorTickSpacing(Qt::Vertical);

  QList<qreal> xTicks;
  qreal firstXTick = ceil(projectionRect().left() / xMajorTickSpacing) * xMajorTickSpacing;

  int ix = 0;
  qreal nextXTick = firstXTick;
  while (1) {
    nextXTick = firstXTick + (ix++ * xMajorTickSpacing);
    if (!projectionRect().contains(nextXTick, projectionRect().y()))
      break;
    xTicks << nextXTick;
  }

  QList<qreal> yTicks;
  qreal firstYTick = ceil(projectionRect().top() / yMajorTickSpacing) * yMajorTickSpacing;

  int iy = 0;
  qreal nextYTick = firstYTick;
  while (1) {
    nextYTick = firstYTick + (iy++ * yMajorTickSpacing);
    if (!projectionRect().contains(projectionRect().x(), nextYTick))
      break;
    yTicks << nextYTick;
  }

  *xMajorTicks = xTicks;
  *yMajorTicks = yTicks;
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
qreal PlotAxisItem::computedMajorTickSpacing(Qt::Orientation orientation) const {
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


QSizeF PlotAxisItem::calculateXTickLabelBound(QPainter *painter,
                                              const QList<qreal> &xMajorTicks) {
  QRectF xLabelRect;
  foreach (qreal x, xMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(x);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p = mapToPlotFromProjection(QPointF(x, projectionRect().top()));
    p.setY(p.y() + bound.height() / 2.0);
    bound.moveCenter(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }
  }

  return xLabelRect.size();
}


QSizeF PlotAxisItem::calculateYTickLabelBound(QPainter *painter,
                                              const QList<qreal> &yMajorTicks) {
  QRectF yLabelRect;
  foreach (qreal y, yMajorTicks) {
    int flags = Qt::TextSingleLine | Qt::AlignVCenter;
    QString label = QString::number(y);

    QRectF bound = painter->boundingRect(QRectF(), flags, label);
    QPointF p = mapToPlotFromProjection(QPointF(projectionRect().left(), y));
    p.setX(p.x() - bound.width() / 2.0);
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }
  }

  return yLabelRect.size();
}

}

// vim: ts=2 sw=2 et
