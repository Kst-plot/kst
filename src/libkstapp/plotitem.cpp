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

#include <QDebug>

static qreal MARGIN_WIDTH = 20.0;
static qreal MARGIN_HEIGHT = 20.0;

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
  _xAxisMajorTickMode(Normal),
  _yAxisMajorTickMode(Normal) {

  setName("Plot");
  setZValue(PLOT_ZVALUE);
  setBrush(Qt::white);

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
  QList<qreal> yMajorTicks;
  computeMajorTicks(&xMajorTicks, &yMajorTicks);
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

  paintMajorGridLines(painter, xMajorTicks, yMajorTicks);
  paintMajorTicks(painter, xMajorTicks, yMajorTicks);
  paintMajorTickLabels(painter, xMajorTicks, yMajorTicks);

  painter->restore();
}


void PlotItem::paintMajorGridLines(QPainter *painter,
                                       const QList<qreal> &xMajorTicks,
                                       const QList<qreal> &yMajorTicks) {

  QRectF rect = plotRect();

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


void PlotItem::paintMajorTicks(QPainter *painter,
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


void PlotItem::paintMajorTickLabels(QPainter *painter,
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
// //  qDebug() << "xLabelRect:" << xLabelRect << endl;
//   painter->fillRect(xLabelRect, Qt::blue);
// 
// //  qDebug() << "yLabelRect:" << yLabelRect << endl;
//   painter->fillRect(yLabelRect, Qt::green);
//   painter->restore();
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


QPointF PlotItem::mapFromAxisToProjection(const QPointF &point) const {
  return projectionAxisTransform().map(point);
}


QPointF PlotItem::mapToAxisFromProjection(const QPointF &point) const {
  return projectionAxisTransform().inverted().map(point);
}


QRectF PlotItem::mapFromAxisToProjection(const QRectF &rect) const {
  return projectionAxisTransform().mapRect(rect);
}


QRectF PlotItem::mapToAxisFromProjection(const QRectF &rect) const {
  return projectionAxisTransform().inverted().mapRect(rect);
}


QTransform PlotItem::projectionAxisTransform() const {
  QTransform t;

  QRectF rect = plotAxisRect();
  QRectF v = QRectF(rect.bottomLeft(), rect.topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(projectionRect());
  to_.pop_back(); //get rid of last closed point

  QTransform::quadToQuad(from_, to_, t);
  return t;
}


QPointF PlotItem::mapFromPlotToProjection(const QPointF &point) const {
  return projectionPlotTransform().map(point);
}


QPointF PlotItem::mapToPlotFromProjection(const QPointF &point) const {
  return projectionPlotTransform().inverted().map(point);
}


QRectF PlotItem::mapFromPlotToProjection(const QRectF &rect) const {
  return projectionPlotTransform().mapRect(rect);
}


QRectF PlotItem::mapToPlotFromProjection(const QRectF &rect) const {
  return projectionPlotTransform().inverted().mapRect(rect);
}


QTransform PlotItem::projectionPlotTransform() const {
  QTransform t;

  QRectF rect = plotRect();
  QRectF v = QRectF(rect.bottomLeft(), rect.topRight());

  QPolygonF from_ = QPolygonF(v);
  from_.pop_back(); //get rid of last closed point

  QPolygonF to_ = QPolygonF(projectionRect());
  to_.pop_back(); //get rid of last closed point

  QTransform::quadToQuad(from_, to_, t);
  return t;
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

  QRectF leftLabelRect = verticalLabelRect(false);
  leftLabelRect.moveTopLeft(QPointF(0.0, labelMarginHeight()));
  painter->drawText(t.mapRect(leftLabelRect), Qt::TextWordWrap | Qt::AlignCenter, leftLabel());

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

  QRectF leftLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
  painter->restore();

  QSizeF margins;
  margins.setWidth(leftLabelBound.height());
  return margins;
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!isBottomLabelVisible())
    return;

  painter->save();
  QRectF bottomLabelRect = horizontalLabelRect(false);
  bottomLabelRect.moveTopLeft(QPointF(labelMarginWidth(), height() - labelMarginHeight()));
  painter->drawText(bottomLabelRect, Qt::TextWordWrap | Qt::AlignCenter, bottomLabel());
  painter->restore();
}


QSizeF PlotItem::calculateBottomLabelBound(QPainter *painter) {
  if (!isBottomLabelVisible())
    return QSizeF();

  QRectF bottomLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                                  Qt::TextWordWrap | Qt::AlignCenter, bottomLabel());

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

  //same as left but painter is translated
  QRectF rightLabelRect = verticalLabelRect(false);
  rightLabelRect.moveTopLeft(QPointF(0.0, labelMarginHeight()));
  painter->drawText(t.mapRect(rightLabelRect), Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
  painter->restore();
}


QSizeF PlotItem::calculateRightLabelBound(QPainter *painter) {
  if (!isRightLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);
  QRectF rightLabelBound = painter->boundingRect(t.mapRect(verticalLabelRect(true)),
                                                 Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
  painter->restore();

  QSizeF margins;
  margins.setWidth(rightLabelBound.height());
  return margins;
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (!isTopLabelVisible())
    return;

  painter->save();
  QRectF topLabelRect = horizontalLabelRect(false);
  topLabelRect.moveTopLeft(QPointF(labelMarginWidth(), 0.0));
  painter->drawText(topLabelRect, Qt::TextWordWrap | Qt::AlignCenter, topLabel());
  painter->restore();
}


QSizeF PlotItem::calculateTopLabelBound(QPainter *painter) {
  if (!isTopLabelVisible())
    return QSizeF();

  QRectF topLabelBound = painter->boundingRect(horizontalLabelRect(true),
                                               Qt::TextWordWrap | Qt::AlignCenter, topLabel());

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


void PlotItem::computeMajorTicks(QList<qreal> *xMajorTicks, QList<qreal> *yMajorTicks) const {
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


QSizeF PlotItem::calculateYTickLabelBound(QPainter *painter,
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
        // TODO add any specialized PlotItem Properties here.
      } else if (xml.name().toString() == "plotaxis") {
        Q_ASSERT(rc);
/* FIXME       validTag = rc->plotAxisItem()->configureFromXml(xml, store);*/
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
