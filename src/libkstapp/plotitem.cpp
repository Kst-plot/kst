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
#include "labelrenderer.h"

#include "datacollection.h"
#include "cartesianrenderitem.h"

#include "plotitemdialog.h"

#include "math_kst.h"

#include "settings.h"

#include <QMenu>
#include <QDebug>

// Zoom Debugging.  0 Off, 1 On.
#define DEBUG_ZOOM 0

static qreal TOP_MARGIN = 20.0;
static qreal BOTTOM_MARGIN = 0.0;
static qreal LEFT_MARGIN = 0.0;
static qreal RIGHT_MARGIN = 20.0;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent),
  _isTiedZoom(false),
  _isLeftLabelVisible(true),
  _isBottomLabelVisible(true),
  _isRightLabelVisible(true),
  _isTopLabelVisible(true),
  _calculatedLeftLabelMargin(0.0),
  _calculatedRightLabelMargin(0.0),
  _calculatedTopLabelMargin(0.0),
  _calculatedBottomLabelMargin(0.0),
  _calculatedLabelMarginWidth(0.0),
  _calculatedLabelMarginHeight(0.0),
  _calculatedAxisMarginWidth(0.0),
  _calculatedAxisMarginHeight(0.0),
  _leftLabelFontScale(0.0),
  _bottomLabelFontScale(0.0),
  _topLabelFontScale(0.0),
  _rightLabelFontScale(0.0),
  _showLegend(false),
  _legend(0),
  _zoomMenu(0)
 {

  setName("Plot");
  setZValue(PLOT_ZVALUE);
  setBrush(Qt::white);

  _xAxis = new PlotAxis(this, Qt::Horizontal);
  _yAxis = new PlotAxis(this, Qt::Vertical);

  _leftLabelFont = parentView()->defaultFont();
  _bottomLabelFont = parentView()->defaultFont();
  _topLabelFont = parentView()->defaultFont();
  _rightLabelFont = parentView()->defaultFont();

  _undoStack = new QUndoStack(this);

  createActions();

  PlotItemManager::self()->addPlot(this);

  // Set the initial projection.
  setProjectionRect(QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)));
  renderItem(PlotRenderItem::Cartesian);
}


PlotItem::~PlotItem() {
  delete _xAxis;
  delete _yAxis;
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
  xml.writeAttribute("leftlabelfontscale", QVariant(_leftLabelFontScale).toString());
  xml.writeAttribute("bottomlabeloverride", _bottomLabelOverride);
  xml.writeAttribute("bottomlabelfont", QVariant(_bottomLabelFont).toString());
  xml.writeAttribute("bottomlabelfontscale", QVariant(_bottomLabelFontScale).toString());
  xml.writeAttribute("toplabeloverride", _topLabelOverride);
  xml.writeAttribute("toplabelfont", QVariant(_topLabelFont).toString());
  xml.writeAttribute("toplabelfontscale", QVariant(_topLabelFontScale).toString());
  xml.writeAttribute("rightlabeloverride", _rightLabelOverride);
  xml.writeAttribute("rightlabelfont", QVariant(_rightLabelFont).toString());
  xml.writeAttribute("rightlabelfontscale", QVariant(_rightLabelFontScale).toString());
  xml.writeAttribute("showlegend", QVariant(_showLegend).toString());

  ViewItem::save(xml);
  legend()->saveInPlot(xml);

  foreach (PlotRenderItem *renderer, renderItems()) {
    renderer->saveInPlot(xml);
  }
  _xAxis->saveInPlot(xml, QString("xaxis"));
  _yAxis->saveInPlot(xml, QString("yaxis"));

  xml.writeStartElement("projectionrect");
  xml.writeAttribute("x", QVariant(projectionRect().x()).toString());
  xml.writeAttribute("y", QVariant(projectionRect().y()).toString());
  xml.writeAttribute("width", QVariant(projectionRect().width()).toString());
  xml.writeAttribute("height", QVariant(projectionRect().height()).toString());
  xml.writeEndElement();
  xml.writeEndElement();
}


void PlotItem::edit() {
  PlotItemDialog editDialog(this);
  editDialog.exec();
}


void PlotItem::createActions() {
  _zoomMaximum = new QAction(tr("Zoom Maximum"), this);
  _zoomMaximum->setShortcut(Qt::Key_M);
  registerShortcut(_zoomMaximum);
  connect(_zoomMaximum, SIGNAL(triggered()), this, SLOT(zoomMaximum()));

  _zoomMaxSpikeInsensitive = new QAction(tr("Zoom Max Spike Insensitive"), this);
  _zoomMaxSpikeInsensitive->setShortcut(Qt::Key_S);
  registerShortcut(_zoomMaxSpikeInsensitive);
  connect(_zoomMaxSpikeInsensitive, SIGNAL(triggered()), this, SLOT(zoomMaxSpikeInsensitive()));

  _zoomPrevious = _undoStack->createUndoAction(this, tr("Zoom Previous"));
  _zoomPrevious->setShortcut(Qt::Key_R);
  registerShortcut(_zoomPrevious);

  _zoomYMeanCentered = new QAction(tr("Y-Zoom Mean-centered"), this);
  _zoomYMeanCentered->setShortcut(Qt::Key_A);
  registerShortcut(_zoomYMeanCentered);
  connect(_zoomYMeanCentered, SIGNAL(triggered()), this, SLOT(zoomYMeanCentered()));

  _zoomXMaximum = new QAction(tr("X-Zoom Maximum"), this);
  _zoomXMaximum->setShortcut(Qt::CTRL+Qt::Key_M);
  registerShortcut(_zoomXMaximum);
  connect(_zoomXMaximum, SIGNAL(triggered()), this, SLOT(zoomXMaximum()));

  _zoomXRight = new QAction(tr("X-Zoom Right"), this);
  _zoomXRight->setShortcut(Qt::Key_Right);
  registerShortcut(_zoomXRight);
  connect(_zoomXRight, SIGNAL(triggered()), this, SLOT(zoomXRight()));

  _zoomXLeft= new QAction(tr("X-Zoom Left"), this);
  _zoomXLeft->setShortcut(Qt::Key_Left);
  registerShortcut(_zoomXLeft);
  connect(_zoomXLeft, SIGNAL(triggered()), this, SLOT(zoomXLeft()));

  _zoomXOut = new QAction(tr("X-Zoom Out"), this);
  _zoomXOut->setShortcut(Qt::SHIFT+Qt::Key_Right);
  registerShortcut(_zoomXOut);
  connect(_zoomXOut, SIGNAL(triggered()), this, SLOT(zoomXOut()));

  _zoomXIn = new QAction(tr("X-Zoom In"), this);
  _zoomXIn->setShortcut(Qt::SHIFT+Qt::Key_Left);
  registerShortcut(_zoomXIn);
  connect(_zoomXIn, SIGNAL(triggered()), this, SLOT(zoomXIn()));

  _zoomNormalizeXtoY = new QAction(tr("Normalize X Axis to Y Axis"), this);
  _zoomNormalizeXtoY->setShortcut(Qt::Key_N);
  registerShortcut(_zoomNormalizeXtoY);
  connect(_zoomNormalizeXtoY, SIGNAL(triggered()), this, SLOT(zoomNormalizeXtoY()));

  _zoomLogX = new QAction(tr("Log X Axis"), this);
  _zoomLogX->setShortcut(Qt::Key_G);
  _zoomLogX->setCheckable(true);
  registerShortcut(_zoomLogX);
  connect(_zoomLogX, SIGNAL(triggered()), this, SLOT(zoomLogX()));

  _zoomYLocalMaximum = new QAction(tr("Y-Zoom Local Maximum"), this);
  _zoomYLocalMaximum->setShortcut(Qt::SHIFT+Qt::Key_L);
  registerShortcut(_zoomYLocalMaximum);
  connect(_zoomYLocalMaximum, SIGNAL(triggered()), this, SLOT(zoomYLocalMaximum()));

  _zoomYMaximum = new QAction(tr("Y-Zoom Maximum"), this);
  _zoomYMaximum->setShortcut(Qt::SHIFT+Qt::Key_M);
  registerShortcut(_zoomYMaximum);
  connect(_zoomYMaximum, SIGNAL(triggered()), this, SLOT(zoomYMaximum()));

  _zoomYUp= new QAction(tr("Y-Zoom Up"), this);
  _zoomYUp->setShortcut(Qt::Key_Up);
  registerShortcut(_zoomYUp);
  connect(_zoomYUp, SIGNAL(triggered()), this, SLOT(zoomYUp()));

  _zoomYDown= new QAction(tr("Y-Zoom Down"), this);
  _zoomYDown->setShortcut(Qt::Key_Down);
  registerShortcut(_zoomYDown);
  connect(_zoomYDown, SIGNAL(triggered()), this, SLOT(zoomYDown()));

  _zoomYOut = new QAction(tr("Y-Zoom Out"), this);
  _zoomYOut->setShortcut(Qt::SHIFT+Qt::Key_Up);
  registerShortcut(_zoomYOut);
  connect(_zoomYOut, SIGNAL(triggered()), this, SLOT(zoomYOut()));

  _zoomYIn = new QAction(tr("Y-Zoom In"), this);
  _zoomYIn->setShortcut(Qt::SHIFT+Qt::Key_Down);
  registerShortcut(_zoomYIn);
  connect(_zoomYIn, SIGNAL(triggered()), this, SLOT(zoomYIn()));

  _zoomNormalizeYtoX = new QAction(tr("Normalize Y Axis to X Axis"), this);
  _zoomNormalizeYtoX->setShortcut(Qt::SHIFT+Qt::Key_N);
  registerShortcut(_zoomNormalizeYtoX);
  connect(_zoomNormalizeYtoX, SIGNAL(triggered()), this, SLOT(zoomNormalizeYtoX()));

  _zoomLogY = new QAction(tr("Log Y Axis"), this);
  _zoomLogY->setShortcut(Qt::Key_L);
  _zoomLogY->setCheckable(true);
  registerShortcut(_zoomLogY);
  connect(_zoomLogY, SIGNAL(triggered()), this, SLOT(zoomLogY()));

  createZoomMenu();
}


void PlotItem::createZoomMenu() {
  if (_zoomMenu) {
    delete _zoomMenu;
  }

  _zoomMenu = new QMenu;
  _zoomMenu->setTitle(tr("Zoom"));

  _zoomMenu->addAction(_zoomMaximum);
  _zoomMenu->addAction(_zoomMaxSpikeInsensitive);
  _zoomMenu->addAction(_zoomPrevious);
  _zoomMenu->addAction(_zoomYMeanCentered);

  _zoomMenu->addSeparator();

  _zoomMenu->addAction(_zoomXMaximum);
  _zoomMenu->addAction(_zoomXRight);
  _zoomMenu->addAction(_zoomXLeft);
  _zoomMenu->addAction(_zoomXOut);
  _zoomMenu->addAction(_zoomXIn);
  _zoomMenu->addAction(_zoomNormalizeXtoY);
  _zoomMenu->addAction(_zoomLogX);

  _zoomMenu->addSeparator();

  _zoomMenu->addAction(_zoomYLocalMaximum);
  _zoomMenu->addAction(_zoomYMaximum);
  _zoomMenu->addAction(_zoomYUp);
  _zoomMenu->addAction(_zoomYDown);
  _zoomMenu->addAction(_zoomYOut);
  _zoomMenu->addAction(_zoomYIn);
  _zoomMenu->addAction(_zoomNormalizeYtoX);
  _zoomMenu->addAction(_zoomLogY);
}


void PlotItem::addToMenuForContextEvent(QMenu &menu) {
  _zoomLogX->setChecked(xAxis()->axisLog());
  _zoomLogY->setChecked(yAxis()->axisLog());
  menu.addMenu(_zoomMenu);
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
  painter->setFont(parentView()->defaultFont());

  setCalculatedAxisMarginWidth(calculateLeftTickLabelBound(painter).width());
  setCalculatedAxisMarginHeight(calculateBottomTickLabelBound(painter).height());

  setCalculatedLeftLabelMargin(calculateLeftLabelBound(painter).width());
  setCalculatedRightLabelMargin(calculateRightLabelBound(painter).width());
  setCalculatedTopLabelMargin(calculateTopLabelBound(painter).height());
  setCalculatedBottomLabelMargin(calculateBottomLabelBound(painter).height());

//  qDebug() << "=============> leftLabel:" << leftLabel() << endl;
  paintLeftLabel(painter);
//  qDebug() << "=============> bottomLabel:" << bottomLabel() << endl;
  paintBottomLabel(painter);
//  qDebug() << "=============> rightLabel:" << rightLabel() << endl;
  paintRightLabel(painter);
//  qDebug() << "=============> topLabel:" << topLabel() << endl;
  paintTopLabel(painter);

  paintPlot(painter);
  paintTickLabels(painter);
  paintPlotMarkers(painter);

  painter->restore();
}


void PlotItem::paintPlot(QPainter *painter) {
  paintMajorGridLines(painter);
  paintMinorGridLines(painter);

  painter->save();
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(plotRect());
  painter->restore();

  paintMajorTicks(painter);
  paintMinorTicks(painter);
}


void PlotItem::paintMajorGridLines(QPainter *painter) {

  QRectF rect = plotRect();

  if (xAxis()->drawAxisMajorGridLines()) {
    QVector<QLineF> xMajorTickLines;
    foreach (qreal x, _xAxis->axisMajorTicks()) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      xMajorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisMajorGridLineColor()), 1.0, _xAxis->axisMajorGridLineStyle()));
    painter->drawLines(xMajorTickLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMajorGridLines()) {
    QVector<QLineF> yMajorTickLines;
    foreach (qreal y, _yAxis->axisMajorTicks()) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      yMajorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMajorGridLineColor()), 1.0, _yAxis->axisMajorGridLineStyle()));
    painter->drawLines(yMajorTickLines);
    painter->restore();
  }
}


void PlotItem::paintMinorGridLines(QPainter *painter) {

  QRectF rect = plotRect();

  if (xAxis()->drawAxisMinorGridLines()) {
    QVector<QLineF> xMinorTickLines;
    foreach (qreal x, _xAxis->axisMinorTicks()) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      xMinorTickLines << QLineF(p1, p2);
    }
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisMinorGridLineColor()), 1.0, _xAxis->axisMinorGridLineStyle()));
    painter->drawLines(xMinorTickLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMinorGridLines()) {
    QVector<QLineF> yMinorTickLines;
    foreach (qreal y, _yAxis->axisMinorTicks()) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      yMinorTickLines << QLineF(p1, p2);
    }

    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMinorGridLineColor()), 1.0, _yAxis->axisMinorGridLineStyle()));
    painter->drawLines(yMinorTickLines);
    painter->restore();
  }
}


void PlotItem::paintMajorTicks(QPainter *painter) {

  qreal majorTickLength = qMin(rect().width(), rect().height()) * .02; //two percent

  if (xAxis()->drawAxisMajorTicks()) {
    QVector<QLineF> xMajorTickLines;
    foreach (qreal x, _xAxis->axisMajorTicks()) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, majorTickLength);
      xMajorTickLines << QLineF(p1, p2);

      p1.setY(plotRect().top());
      p2 = p1 + QPointF(0, majorTickLength);
      xMajorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(xMajorTickLines);
  }

  if (yAxis()->drawAxisMajorTicks()) {
    QVector<QLineF> yMajorTickLines;
    foreach (qreal y, _yAxis->axisMajorTicks()) {
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


void PlotItem::paintMinorTicks(QPainter *painter) {

  qreal minorTickLength = qMin(rect().width(), rect().height()) * 0.01; //one percent

  if (xAxis()->drawAxisMinorTicks()) {
    QVector<QLineF> xMinorTickLines;
    foreach (qreal x, _xAxis->axisMinorTicks()) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, minorTickLength);
      xMinorTickLines << QLineF(p1, p2);

      p1.setY(plotRect().top());
      p2 = p1 + QPointF(0, minorTickLength);
      xMinorTickLines << QLineF(p1, p2);
    }

    painter->drawLines(xMinorTickLines);
  }

  if (yAxis()->drawAxisMinorTicks()) {
    QVector<QLineF> yMinorTickLines;
    foreach (qreal y, _yAxis->axisMinorTicks()) {
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


void PlotItem::paintBottomTickLabels(QPainter *painter) {

  QRectF xLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignCenter;

  _xAxis->validateDrawingRegion(flags, painter);

  QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
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

    if ((rect().left() < bound.left()) && (rect().right() > bound.right())) {
      painter->drawText(bound, flags, xLabelIt.value());
    }
  }

  if (!_xAxis->baseLabel().isEmpty()) {
    QRectF bound = painter->boundingRect(QRectF(), flags, _xAxis->baseLabel());
    QPointF p = QPointF(plotRect().left(), plotRect().bottom() + bound.height() * 2.0);
    bound.moveBottomLeft(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    painter->drawText(bound, flags, _xAxis->baseLabel());
  }
  _xLabelRect = xLabelRect;

//   painter->save();
//   painter->setOpacity(0.3);
// //   qDebug() << "xLabelRect:" << xLabelRect;
//   painter->fillRect(xLabelRect, Qt::green);
//   painter->restore();
}


void PlotItem::paintLeftTickLabels(QPainter *painter) {

  QRectF yLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignVCenter;

  QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
  while (yLabelIt.hasNext()) {
    yLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
    bound.setWidth(bound.width() + 6);
    QPointF p = QPointF(plotRect().left() - (bound.width() / 2.0), mapYToPlot(yLabelIt.key()));
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    if ((rect().top() < bound.top()) && (rect().bottom() > bound.bottom())) {
      painter->drawText(bound, flags, yLabelIt.value());
    }
  }

  if (!_yAxis->baseLabel().isEmpty()) {
    painter->save();
    QTransform t;
    t.rotate(90.0);
    painter->rotate(-90.0);

    QRectF bound = painter->boundingRect(QRectF(), flags, _yAxis->baseLabel());
    bound = QRectF(bound.x(), bound.bottomRight().y() - bound.width(), bound.height(), bound.width());
    QPointF p = QPointF(rect().left(), plotRect().bottom());
    bound.moveBottomLeft(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    painter->drawText(t.mapRect(bound), flags, _yAxis->baseLabel());
    painter->restore();
  }
  _yLabelRect = yLabelRect;

//   painter->save();
//   painter->setOpacity(0.3);
// //   qDebug() << "yLabelRect:" << yLabelRect;
//   painter->fillRect(yLabelRect, Qt::green);
//   painter->restore();
}


void PlotItem::paintTickLabels(QPainter *painter) {

  if (_xAxis->isAxisVisible()) {
    paintBottomTickLabels(painter);
  }

  if (_yAxis->isAxisVisible()) {
    paintLeftTickLabels(painter);
  }
}


void PlotItem::paintPlotMarkers(QPainter *painter) {

  QRectF rect = plotRect();

  QVector<QLineF> xPlotMarkers;
  foreach (double x, _xAxis->axisPlotMarkers().markers()) {
    if (x > _xMin && x < _xMax) {
      QPointF p1 = QPointF(mapXToPlot(x), plotRect().bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      xPlotMarkers << QLineF(p1, p2);
    }
  }

  if (!xPlotMarkers.isEmpty()) {
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisPlotMarkers().lineColor()), _xAxis->axisPlotMarkers().lineWidth(), _xAxis->axisPlotMarkers().lineStyle()));
    painter->drawLines(xPlotMarkers);
    painter->restore();
  }

  QVector<QLineF> yPlotMarkers;
  foreach (double y, _yAxis->axisPlotMarkers().markers()) {
    if (y > _yMin && y < _yMax) {
      QPointF p1 = QPointF(plotRect().left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      yPlotMarkers << QLineF(p1, p2);
    }
  }

  if (!yPlotMarkers.isEmpty()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisPlotMarkers().lineColor()), _yAxis->axisPlotMarkers().lineWidth(), _yAxis->axisPlotMarkers().lineStyle()));
    painter->drawLines(yPlotMarkers);
    painter->restore();
  }

}


QRectF PlotItem::plotAxisRect() const {
  qreal left = isLeftLabelVisible() ? leftLabelMargin() : 0.0;
  qreal bottom = isBottomLabelVisible() ? bottomLabelMargin() : 0.0;
  qreal right = isRightLabelVisible() ? rightLabelMargin() : 0.0;
  qreal top = isTopLabelVisible() ? topLabelMargin() : 0.0;

  QPointF topLeft(rect().topLeft() + QPointF(left, top));
  QPointF bottomRight(rect().bottomRight() - QPointF(right, bottom));

  return QRectF(topLeft, bottomRight);
}


QRectF PlotItem::plotRect() const {
  //the PlotRenderItems use this to set their rects
  QRectF plot = plotAxisRect();
  qreal xOffset = _xAxis->isAxisVisible() ? axisMarginHeight() : 0.0;
  qreal yOffset = _yAxis->isAxisVisible() ? axisMarginWidth() : 0.0;

  plot.setLeft(plot.left() + yOffset);
  plot.setBottom(plot.bottom() - xOffset);
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


qreal PlotItem::leftLabelMargin() const {
  return calculatedLeftLabelMargin();
}


qreal PlotItem::rightLabelMargin() const {
  return calculatedRightLabelMargin();
}


qreal PlotItem::topLabelMargin() const {
  return calculatedTopLabelMargin();
}


qreal PlotItem::bottomLabelMargin() const {
  return calculatedBottomLabelMargin();
}


qreal PlotItem::axisMarginWidth() const {
  return calculatedAxisMarginWidth();
}


qreal PlotItem::axisMarginHeight() const {
  return calculatedAxisMarginHeight();
}


void PlotItem::updateScale() {
  if (_xAxis->axisLog()) {
    _xMax = logXHi(projectionRect().right());
    _xMin = logXLo(projectionRect().left());
  } else {
    _xMax = projectionRect().right();
    _xMin = projectionRect().left();
  }

  if (_yAxis->axisLog()) {
    _yMax = logYHi(projectionRect().bottom());
    _yMin = logYLo(projectionRect().top());
  } else {
    _yMax = projectionRect().bottom();
    _yMin = projectionRect().top();
  }
}


QRectF PlotItem::mapToProjection(const QRectF &rect) {
  QRectF projRect;

  // Invert and convert points.
  QPointF topLeft;
  QPointF bottomRight;
  if (!(_xAxis->axisReversed() || _yAxis->axisReversed())) {
    topLeft = mapToProjection(rect.bottomLeft());
    bottomRight = mapToProjection(rect.topRight());
  } else if (_xAxis->axisReversed() && _yAxis->axisReversed()) {
    topLeft = mapToProjection(rect.topRight());
    bottomRight = mapToProjection(rect.bottomLeft());
  } else if (_yAxis->axisReversed()) {
    topLeft = mapToProjection(rect.topLeft());
    bottomRight = mapToProjection(rect.bottomRight());
  } else {
    topLeft = mapToProjection(rect.bottomRight());
    bottomRight = mapToProjection(rect.topLeft());
  }

  projRect.setTopLeft(topLeft);

  projRect.setWidth(bottomRight.x() - topLeft.x());
  projRect.setHeight(bottomRight.y() - topLeft.y());

  return projRect;
}


QPointF PlotItem::mapToProjection(const QPointF &point) {
  QRectF pr = plotRect();
  double xpos, ypos;

  updateScale();

  if (_xAxis->axisReversed()) {
    xpos = (double)(pr.right() - point.x())/(double)pr.width();
  } else {
    xpos = (double)(point.x() - pr.left())/(double)pr.width();
  }
  xpos = xpos * (_xMax - _xMin) + _xMin;

  if (_xAxis->axisLog()) {
    xpos = pow(10, xpos);
  }

  if (_yAxis->axisReversed()) {
    ypos = (double)(point.y() - pr.top())/(double)pr.height();
  } else {
    ypos = (double)(pr.bottom() - point.y())/(double)pr.height();
  }
  ypos = ypos * (_yMax - _yMin) + _yMin;

  if (_yAxis->axisLog()) {
    ypos = pow(10, ypos);
  }

  return QPointF(xpos, ypos);
}


QPointF PlotItem::mapToPlot(const QPointF &point) const {
  return QPointF(mapXToPlot(point.x()), mapYToPlot(point.y()));
}


qreal PlotItem::mapXToPlot(const qreal &x) const {
  QRectF pr = plotRect();
  double newX = x;

  if (_xAxis->axisLog()) {
    newX = logXLo(x);
  }

  newX -= _xMin;
  newX = newX / (_xMax - _xMin);

  newX = newX * pr.width();

  if (_xAxis->axisLog() && x == -350) {
    newX = 0;
  }

  if (_xAxis->axisReversed()) {
    newX = pr.right() - newX;
  } else {
    newX = newX + pr.left();
  }
  return newX;
}


qreal PlotItem::mapYToPlot(const qreal &y) const {
  QRectF pr = plotRect();
  double newY = y;

  if (_yAxis->axisLog()) {
    newY = logYLo(y);
  }

  newY -= _yMin;
  newY = newY / (_yMax - _yMin);

  newY = newY * pr.height();

  if (_yAxis->axisLog() && y == -350) {
    newY = 0;
  }

  if (_yAxis->axisReversed()) {
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


qreal PlotItem::rightLabelFontScale() const {
  return _rightLabelFontScale;
}


void PlotItem::setRightLabelFontScale(const qreal scale) {
  _rightLabelFontScale = scale;
}


qreal PlotItem::leftLabelFontScale() const {
  return _leftLabelFontScale;
}


void PlotItem::setLeftLabelFontScale(const qreal scale) {
  _leftLabelFontScale = scale;
}


qreal PlotItem::topLabelFontScale() const {
  return _topLabelFontScale;
}


void PlotItem::setTopLabelFontScale(const qreal scale) {
  _topLabelFontScale = scale;
}


qreal PlotItem::bottomLabelFontScale() const {
  return _bottomLabelFontScale;
}


void PlotItem::setBottomLabelFontScale(const qreal scale) {
  _bottomLabelFontScale = scale;
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


void PlotItem::setTopSuppressed(bool suppressed) {
  setTopLabelVisible(!suppressed);
}


void PlotItem::setRightSuppressed(bool suppressed) {
  setRightLabelVisible(!suppressed);
}


void PlotItem::setLeftSuppressed(bool suppressed) {
  setLeftLabelVisible(!suppressed);
  _yAxis->setAxisVisible(!suppressed);
}


void PlotItem::setBottomSuppressed(bool suppressed) {
  setBottomLabelVisible(!suppressed);
  _xAxis->setAxisVisible(!suppressed);
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
  _xAxis->setAxisVisible(visible);
  _yAxis->setAxisVisible(visible);
}


qreal PlotItem::calculatedLabelMarginWidth() const {
  qreal m = qMax(_calculatedLeftLabelMargin, _calculatedRightLabelMargin);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


qreal PlotItem::calculatedLeftLabelMargin() const {
  qreal m = qMax(LEFT_MARGIN, _calculatedLeftLabelMargin);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setCalculatedLeftLabelMargin(qreal margin) {
  qreal before = this->calculatedLeftLabelMargin();
  _calculatedLeftLabelMargin = margin;
  if (before != this->calculatedLeftLabelMargin())
    emit marginsChanged();
}


qreal PlotItem::calculatedRightLabelMargin() const {
  qreal m = qMax(RIGHT_MARGIN, _calculatedRightLabelMargin);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setCalculatedRightLabelMargin(qreal margin) {
  qreal before = this->calculatedRightLabelMargin();
  _calculatedRightLabelMargin = margin;
  if (before != this->calculatedRightLabelMargin())
    emit marginsChanged();
}


qreal PlotItem::calculatedLabelMarginHeight() const {
  qreal m = qMax(_calculatedTopLabelMargin, _calculatedBottomLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


// void PlotItem::setCalculatedLabelMarginHeight(qreal marginHeight) {
//   qreal before = this->calculatedLabelMarginHeight();
//   _calculatedLabelMarginHeight = marginHeight;
//   if (before != this->calculatedLabelMarginHeight())
//     emit marginsChanged();
// }


qreal PlotItem::calculatedTopLabelMargin() const {
  qreal m = qMax(TOP_MARGIN, _calculatedTopLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setCalculatedTopLabelMargin(qreal margin) {
  qreal before = this->calculatedTopLabelMargin();
  _calculatedTopLabelMargin = margin;
  if (before != this->calculatedTopLabelMargin())
    emit marginsChanged();
}


qreal PlotItem::calculatedBottomLabelMargin() const {
  qreal m = qMax(BOTTOM_MARGIN, _calculatedBottomLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setCalculatedBottomLabelMargin(qreal margin) {
  qreal before = this->calculatedBottomLabelMargin();
  _calculatedBottomLabelMargin = margin;
  if (before != this->calculatedBottomLabelMargin())
    emit marginsChanged();
}


QRectF PlotItem::topLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, width() - calculatedLeftLabelMargin() - calculatedRightLabelMargin(), calculatedTopLabelMargin());
  else
    return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), topLabelMargin());
}


QRectF PlotItem::bottomLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, width() - calculatedLeftLabelMargin() - calculatedRightLabelMargin(), calculatedBottomLabelMargin());
  else
    return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), bottomLabelMargin());
}


QRectF PlotItem::leftLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, calculatedLeftLabelMargin(), height() - calculatedTopLabelMargin() - calculatedBottomLabelMargin());
  else
    return QRectF(0.0, 0.0, leftLabelMargin(), height() - topLabelMargin() - bottomLabelMargin());
}


QRectF PlotItem::rightLabelRect(bool calc) const {
  if (calc)
    return QRectF(0.0, 0.0, calculatedRightLabelMargin(), height() - calculatedTopLabelMargin() - calculatedBottomLabelMargin());
  else
    return QRectF(0.0, 0.0, rightLabelMargin(), height() - topLabelMargin() - bottomLabelMargin());
}


QFont PlotItem::calculatedLeftLabelFont() {
  QFont font(_leftLabelFont);
  font.setPointSizeF(parentView()->defaultFont(_leftLabelFontScale).pointSizeF());

  return font;
}


QFont PlotItem::calculatedRightLabelFont() {
  QFont font(_rightLabelFont);
  font.setPointSizeF(parentView()->defaultFont(_rightLabelFontScale).pointSizeF());

  return font;
}


QFont PlotItem::calculatedTopLabelFont() {
  QFont font(_topLabelFont);
  font.setPointSizeF(parentView()->defaultFont(_topLabelFontScale).pointSizeF());

  return font;
}


QFont PlotItem::calculatedBottomLabelFont() {
  QFont font(_bottomLabelFont);
  font.setPointSizeF(parentView()->defaultFont(_bottomLabelFontScale).pointSizeF());

  return font;
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!isLeftLabelVisible() || leftLabelOverride().isEmpty())
    return;

  Label::Parsed *parsed = Label::parse(leftLabelOverride());
  if (parsed) {
    QRectF leftLabel = leftLabelRect(false);
    QPixmap pixmap(leftLabel.height(), leftLabel.width());
    pixmap.fill(Qt::white);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(calculatedLeftLabelFont(), &pixmapPainter);
    QFontMetrics fm(calculatedLeftLabelFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    leftLabel.moveTopRight(plotAxisRect().topLeft());
    leftLabel.moveBottomLeft(QPointF(leftLabel.bottomLeft().x(), leftLabel.bottomLeft().y() - ((leftLabel.height() / 2) - (rc.x / 2))));

    painter->save();
    QTransform t;
    t.rotate(90.0);
    painter->rotate(-90.0);

    if (rc.x > 0)
      painter->drawPixmap(t.mapRect(leftLabel).topLeft(), pixmap, QRectF(0, 0, rc.x, leftLabel.height()));

    painter->restore();
    delete parsed;
    parsed = 0;
  }

//   painter->save();
//   QTransform t;
//   t.rotate(90.0);
//   painter->rotate(-90.0);
// 
//   QRectF leftLabel = leftLabelRect(false);
//   leftLabel.moveTopRight(plotAxisRect().topLeft());
// 
//   painter->save();
//   painter->setOpacity(0.3);
//   qDebug() << "leftLabel:" << t.mapRect(leftLabel) << endl;
//   painter->fillRect(t.mapRect(leftLabel), Qt::red);
//   painter->restore();
// 
//   painter->restore();
}


QSizeF PlotItem::calculateLeftLabelBound(QPainter *painter) {
  if (!isLeftLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  painter->setFont(calculatedLeftLabelFont());

  QRectF leftLabelBound = painter->boundingRect(t.mapRect(leftLabelRect(true)),
                                                Qt::TextWordWrap | Qt::AlignCenter, leftLabelOverride());
  painter->restore();

  QSizeF margins;
  margins.setWidth(leftLabelBound.height());
  return margins;
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!isBottomLabelVisible() || bottomLabelOverride().isEmpty())
    return;

  Label::Parsed *parsed = Label::parse(bottomLabelOverride());

  if (parsed) {
    painter->save();

    QRectF bottomLabel = bottomLabelRect(false);
    QPixmap pixmap(bottomLabel.width(), bottomLabel.height());
    pixmap.fill(Qt::white);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(calculatedBottomLabelFont(), &pixmapPainter);
    QFontMetrics fm(calculatedBottomLabelFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    bottomLabel.moveTopLeft(plotAxisRect().bottomLeft());
    bottomLabel.moveTopLeft(QPointF(bottomLabel.topLeft().x() + ((bottomLabel.width() / 2) - (rc.x / 2)), bottomLabel.topLeft().y()));

    if (rc.x > 0)
      painter->drawPixmap(bottomLabel.topLeft(), pixmap, QRectF(0, 0, rc.x, bottomLabel.height()));

    painter->restore();
    delete parsed;
    parsed = 0;
  }

//   painter->save();
// 
//   QRectF bottomLabel = bottomLabelRect(false);
//   bottomLabel.moveTopLeft(plotAxisRect().bottomLeft());
// 
//   painter->save();
//   painter->setOpacity(0.3);
// //   qDebug() << "bottomLabel:" << bottomLabel;
//   painter->fillRect(bottomLabel, Qt::red);
//   painter->restore();
// 
//   painter->restore();
}


QSizeF PlotItem::calculateBottomLabelBound(QPainter *painter) {
  if (!isBottomLabelVisible())
    return QSizeF();

  painter->save();

  painter->setFont(calculatedBottomLabelFont());

  QRectF bottomLabelBound = painter->boundingRect(bottomLabelRect(true),
                                                  Qt::TextWordWrap | Qt::AlignCenter, bottomLabelOverride());
  painter->restore();

  QSizeF margins;
  margins.setHeight(bottomLabelBound.height());
  return margins;
}


void PlotItem::paintRightLabel(QPainter *painter) {
  if (!isRightLabelVisible() || rightLabelOverride().isEmpty())
    return;

  Label::Parsed *parsed = Label::parse(rightLabelOverride());

  if (parsed) {
    QRectF rightLabel = rightLabelRect(false);
    if (rightLabel.isValid()) {
      QPixmap pixmap(rightLabel.height(), rightLabel.width());
      pixmap.fill(Qt::white);
      QPainter pixmapPainter(&pixmap);

      Label::RenderContext rc(calculatedRightLabelFont(), &pixmapPainter);
      QFontMetrics fm(calculatedRightLabelFont());
      rc.y = fm.ascent();
      Label::renderLabel(rc, parsed->chunk);

      rightLabel.moveTopLeft(plotAxisRect().topRight());
      rightLabel.moveTopLeft(QPointF(rightLabel.topLeft().x(), rightLabel.topLeft().y() + ((rightLabel.height() / 2) - (rc.x) / 2)));

      painter->save();
      QTransform t;
      t.rotate(-90.0);
      painter->rotate(90.0);

      if (rc.x > 0)
        painter->drawPixmap(t.mapRect(rightLabel).topLeft(), pixmap, QRectF(0, 0, rc.x, rightLabel.height()));
    }
    painter->restore();

    delete parsed;
    parsed = 0;
  }

//   painter->save();
//   QTransform t;
//   t.rotate(-90.0);
//   painter->rotate(90.0);
// 
//   QRectF rightLabel = rightLabelRect(false);
//   rightLabel.moveTopLeft(plotAxisRect().topRight());
// 
//   painter->save();
//   painter->setOpacity(0.3);
// //   qDebug() << "rightLabel:" << t.mapRect(rightLabel) << endl;
//   painter->fillRect(t.mapRect(rightLabel), Qt::red);
//   painter->restore();
// 
//   painter->restore();
}


QSizeF PlotItem::calculateRightLabelBound(QPainter *painter) {
  if (!isRightLabelVisible())
    return QSizeF();

  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  painter->setFont(calculatedRightLabelFont());

  QRectF rightLabelBound = painter->boundingRect(t.mapRect(rightLabelRect(true)),
                                                 Qt::TextWordWrap | Qt::AlignCenter, rightLabelOverride());
  painter->restore();

  QSizeF margins;
  margins.setWidth(rightLabelBound.height());
  return margins;
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (!isTopLabelVisible() || topLabelOverride().isEmpty())
    return;

  Label::Parsed *parsed = Label::parse(topLabelOverride());

  if (parsed) {
    painter->save();

    QRectF topLabel = topLabelRect(false);
    if (topLabel.isValid()) {
      QPixmap pixmap(topLabel.width(), topLabel.height());
      pixmap.fill(Qt::white);
      QPainter pixmapPainter(&pixmap);

      Label::RenderContext rc(calculatedTopLabelFont(), &pixmapPainter);
      QFontMetrics fm(calculatedTopLabelFont());
      rc.y = fm.ascent();
      Label::renderLabel(rc, parsed->chunk);

      topLabel.moveBottomLeft(plotAxisRect().topLeft());
      topLabel.moveTopLeft(QPointF(topLabel.topLeft().x() + ((topLabel.width() / 2) - (rc.x / 2)), topLabel.topLeft().y()));

      if (rc.x > 0) 
        painter->drawPixmap(topLabel.topLeft(), pixmap, QRectF(0, 0, rc.x, topLabel.height()));
    }
    painter->restore();

    delete parsed;
    parsed = 0;
  }

//   painter->save();
// 
//   QRectF topLabel = topLabelRect(false);
//   topLabel.moveBottomLeft(plotAxisRect().topLeft());
// 
//   painter->save();
//   painter->setOpacity(0.3);
// //   qDebug() << "topLabel:" << topLabel;
//   painter->fillRect(topLabel, Qt::red);
//   painter->restore();
// 
//   painter->restore();
}


QSizeF PlotItem::calculateTopLabelBound(QPainter *painter) {
  if (!isTopLabelVisible())
    return QSizeF();

  painter->save();

  painter->setFont(calculatedTopLabelFont());

  QRectF topLabelBound = painter->boundingRect(topLabelRect(true),
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


QSizeF PlotItem::calculateBottomTickLabelBound(QPainter *painter) {
  QRectF xLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignCenter;

  if (_xAxis->isAxisVisible()) {
    QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
    while (xLabelIt.hasNext()) {
      xLabelIt.next();

      QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
      QPointF p(mapXToPlot(xLabelIt.key()), plotRect().bottom() + bound.height() / 2.0);
      bound.moveCenter(p);

      if (xLabelRect.isValid()) {
        xLabelRect = xLabelRect.united(bound);
      } else {
        xLabelRect = bound;
      }
    }
  }

  if (!_xAxis->baseLabel().isEmpty()) {
    qreal height = painter->boundingRect(QRectF(), flags, _xAxis->baseLabel()).height();
    if (calculatedBottomLabelMargin() < height) {
      xLabelRect.setHeight(xLabelRect.height() + (height - calculatedBottomLabelMargin()));
    }
  }
  return xLabelRect.size();
}


QSizeF PlotItem::calculateLeftTickLabelBound(QPainter *painter) {
  QRectF yLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignCenter;
  if (_yAxis->isAxisVisible()) {

    QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
    while (yLabelIt.hasNext()) {
      yLabelIt.next();

      QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
      bound.setWidth(bound.width() + 6);
      QPointF p(plotRect().left() - bound.width() / 2.0, mapYToPlot(yLabelIt.key()));
      bound.moveCenter(p);

      if (yLabelRect.isValid()) {
        yLabelRect = yLabelRect.united(bound);
      } else {
        yLabelRect = bound;
      }
    }
  }
  if (!_yAxis->baseLabel().isEmpty()) {
    qreal height = painter->boundingRect(QRectF(), flags, _yAxis->baseLabel()).height();
    if (calculatedLeftLabelMargin() < height) {
      yLabelRect.setWidth(yLabelRect.width() + (height - calculatedLeftLabelMargin()));
    }
  }
  return yLabelRect.size();
}


bool PlotItem::showLegend() const {
  return _showLegend;
}


LegendItem* PlotItem::legend() {
  if (!_legend) {
    _legend = new LegendItem(this);
  }
  return _legend;
}


void PlotItem::setShowLegend(const bool show) {
  _showLegend = show;
  if (show) {
    legend()->setVisible(true);
  } else {
    legend()->setVisible(false);
  }
}


bool PlotItem::tryShortcut(const QString &keySequence) {
  return ViewItem::tryShortcut(keySequence);
}


void PlotItem::setProjectionRect(const QRectF &rect) {
  if (!(_projectionRect == rect || rect.isEmpty() || !rect.isValid())) {
#if DEBUG_ZOOM
    qDebug() << "=== setProjectionRect() ======================>\n"
              << "before:" << _projectionRect << "\n"
              << "after:" << rect << endl;
#endif

    _projectionRect = rect;
    emit marginsChanged();
  }
  emit updateAxes();
  update(); //slow, but need to update everything...
}


QRectF PlotItem::computedProjectionRect() {
  QRectF rect;
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->computedProjectionRect().isEmpty()) {
      if (rect.isValid()) {
        rect = rect.united(renderer->computedProjectionRect());
      } else {
        rect = renderer->computedProjectionRect();
      }
    }
  }

  if (!rect.isValid())
    rect = QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)); //default

  return rect;
}


void PlotItem::computedRelationalMax(qreal &minimum, qreal &maximum) {
  QRectF rect;
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
        if (relation->ignoreAutoScale())
          continue;

        qreal min, max;
        relation->yRange(projectionRect().left(),
                        projectionRect().right(),
                        &min, &max);

        //If the axis is in log mode, the lower extent will be the
        //minimum value larger than zero.
        if (yAxis()->axisLog())
          minimum = minimum <= 0.0 ? min : qMin(min, minimum);
        else
          minimum = qMin(min, minimum);

        maximum = qMax(max, maximum);
    }
  }
}


void PlotItem::computeBorder(Qt::Orientation orientation, qreal &minimum, qreal &maximum) const {
  QRectF rect;
  foreach (PlotRenderItem *renderer, renderItems()) {
    qreal min, max;
    renderer->computeBorder(orientation, &min, &max);
    minimum = qMin(min, minimum);
    maximum = qMax(max, maximum);
  }
}


void PlotItem::resetSelectionRect() {
  foreach (PlotRenderItem *renderer, renderItems()) {
    renderer->resetSelectionRect();
  }
}


void PlotItem::zoomFixedExpression(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomFixedExpression" << endl;
#endif
  ZoomCommand *cmd = new ZoomFixedExpressionCommand(this, projection);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXRange(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRange" << endl;
#endif
  ZoomCommand *cmd = new ZoomXRangeCommand(this, projection);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYRange(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomYRange" << endl;
#endif
  ZoomCommand *cmd = new ZoomYRangeCommand(this, projection);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomMaximum() {
#if DEBUG_ZOOM
  qDebug() << "zoomMaximum" << endl;
#endif
  ZoomCommand *cmd = new ZoomMaximumCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomMaxSpikeInsensitive() {
#if DEBUG_ZOOM
  qDebug() << "zoomMaxSpikeInsensitive" << endl;
#endif
  ZoomCommand *cmd = new ZoomMaxSpikeInsensitiveCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomPrevious() {
#if DEBUG_ZOOM
  qDebug() << "zoomPrevious" << endl;
#endif
  if (_undoStack->canUndo()) {
    QAction *undoAction = _undoStack->createUndoAction(this);
    if (undoAction) {
      undoAction->activate(QAction::Trigger);
    }
  }
}


void PlotItem::zoomYMeanCentered() {
#if DEBUG_ZOOM
  qDebug() << "zoomYMeanCentered" << endl;
#endif
  ZoomCommand *cmd = new ZoomYMeanCenteredCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXMaximum() {
#if DEBUG_ZOOM
  qDebug() << "zoomXMaximum" << endl;
#endif
  ZoomCommand *cmd = new ZoomXMaximumCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXRight() {
#if DEBUG_ZOOM
  qDebug() << "zoomXRight" << endl;
#endif
  ZoomCommand *cmd = new ZoomXRightCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXLeft() {
#if DEBUG_ZOOM
  qDebug() << "zoomXLeft" << endl;
#endif
  ZoomCommand *cmd = new ZoomXLeftCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXOut() {
#if DEBUG_ZOOM
  qDebug() << "zoomXOut" << endl;
#endif
  resetSelectionRect();
  ZoomCommand *cmd = new ZoomXOutCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomXIn() {
#if DEBUG_ZOOM
  qDebug() << "zoomXIn" << endl;
#endif
  resetSelectionRect();
  ZoomCommand *cmd = new ZoomXInCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomNormalizeXtoY() {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeXtoY" << endl;
#endif

  if (xAxis()->axisLog() || yAxis()->axisLog())
    return; //apparently we don't want to do anything here according to kst2dplot...

  ZoomCommand *cmd = new ZoomNormalizeXToYCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomLogX() {
#if DEBUG_ZOOM
  qDebug() << "zoomLogX" << endl;
#endif
  ZoomCommand *cmd = new ZoomXLogCommand(this, !xAxis()->axisLog());
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYLocalMaximum() {
#if DEBUG_ZOOM
  qDebug() << "zoomYLocalMaximum" << endl;
#endif
  ZoomCommand *cmd = new ZoomYLocalMaximumCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYMaximum() {
#if DEBUG_ZOOM
  qDebug() << "zoomYMaximum" << endl;
#endif
  ZoomCommand *cmd = new ZoomYMaximumCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYUp() {
#if DEBUG_ZOOM
  qDebug() << "zoomYUp" << endl;
#endif
  ZoomCommand *cmd = new ZoomYUpCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYDown() {
#if DEBUG_ZOOM
  qDebug() << "zoomYDown" << endl;
#endif
  ZoomCommand *cmd = new ZoomYDownCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYOut() {
#if DEBUG_ZOOM
  qDebug() << "zoomYOut" << endl;
#endif
  resetSelectionRect();
  ZoomCommand *cmd = new ZoomYOutCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomYIn() {
#if DEBUG_ZOOM
  qDebug() << "zoomYIn" << endl;
#endif
  resetSelectionRect();
  ZoomCommand *cmd = new ZoomYInCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomNormalizeYtoX() {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeYtoX" << endl;
#endif

  if (xAxis()->axisLog() || yAxis()->axisLog())
    return; //apparently we don't want to do anything here according to kst2dplot...

  ZoomCommand *cmd = new ZoomNormalizeYToXCommand(this);
  _undoStack->push(cmd);
  cmd->redo();
}


void PlotItem::zoomLogY() {
#if DEBUG_ZOOM
  qDebug() << "zoomLogY" << endl;
#endif
  ZoomCommand *cmd = new ZoomYLogCommand(this, !yAxis()->axisLog());
  _undoStack->push(cmd);
  cmd->redo();
}


ZoomState PlotItem::currentZoomState() {
  ZoomState zoomState;
  zoomState.item = this; //the origin of this ZoomState
  zoomState.projectionRect = projectionRect();
  zoomState.xAxisZoomMode = xAxis()->axisZoomMode();
  zoomState.yAxisZoomMode = yAxis()->axisZoomMode();
  zoomState.isXAxisLog = xAxis()->axisLog();
  zoomState.isYAxisLog = yAxis()->axisLog();
  zoomState.xLogBase = 10.0;
  zoomState.yLogBase = 10.0;
  return zoomState;
}


void PlotItem::setCurrentZoomState(ZoomState zoomState) {
  _xAxis->setAxisZoomMode(PlotAxis::ZoomMode(zoomState.xAxisZoomMode));
  _yAxis->setAxisZoomMode(PlotAxis::ZoomMode(zoomState.yAxisZoomMode));
  _xAxis->setAxisLog(zoomState.isXAxisLog);
  _yAxis->setAxisLog(zoomState.isYAxisLog);
  setProjectionRect(zoomState.projectionRect);
}


void CreatePlotCommand::createItem() {
  _item = new PlotItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


void CreatePlotForCurve::createItem() {
  _item = new PlotItem(_view);

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
  double x = 0, y = 0, w = 10, h = 10;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (xml.name().toString() == "plot") {
        Q_ASSERT(!rc);
        rc = new PlotItem(view);
        if (parent) {
          rc->setParent(parent);
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
        av = attrs.value("leftlabelfontscale");
        if (!av.isNull()) {
          rc->setLeftLabelFontScale(QVariant(av.toString()).toDouble());
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
        av = attrs.value("bottomlabelfontscale");
        if (!av.isNull()) {
          rc->setBottomLabelFontScale(QVariant(av.toString()).toDouble());
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
        av = attrs.value("toplabelfontscale");
        if (!av.isNull()) {
          rc->setTopLabelFontScale(QVariant(av.toString()).toDouble());
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
        av = attrs.value("rightlabelfontscale");
        if (!av.isNull()) {
          rc->setRightLabelFontScale(QVariant(av.toString()).toDouble());
        }
        av = attrs.value("showlegend");
        if (!av.isNull()) {
          rc->setShowLegend(QVariant(av.toString()).toBool());
        }

      // Add any new specialized PlotItem Properties here.
      } else if (xml.name().toString() == "projectionrect") {
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        av = attrs.value("width");
        if (!av.isNull()) {
          w = av.toString().toDouble();
        }
        av = attrs.value("height");
        if (!av.isNull()) {
          h = av.toString().toDouble();
        }
        av = attrs.value("x");
        if (!av.isNull()) {
          x = av.toString().toDouble();
        }
        av = attrs.value("y");
        if (!av.isNull()) {
          y = av.toString().toDouble();
        }
        xml.readNext();
      } else if (xml.name().toString() == "plotaxis") {
        Q_ASSERT(rc);
        QXmlStreamAttributes subattrs = xml.attributes();
        QStringRef av = subattrs.value("id");
        if (!av.isNull()) {
          if (av == "xaxis") {
            rc->xAxis()->configureFromXml(xml, store);
          } else if (av == "yaxis") {
            rc->yAxis()->configureFromXml(xml, store);
          }
        }
        xml.readNext();
      } else if (xml.name().toString() == "cartesianrender") {
        Q_ASSERT(rc);
        PlotRenderItem * renderItem = rc->renderItem(PlotRenderItem::Cartesian);
        if (renderItem) {
          validTag = renderItem->configureFromXml(xml, store);
        }
      } else if (xml.name().toString() == "legend") {
        Q_ASSERT(rc);
        validTag = rc->legend()->configureFromXml(xml, store);
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
  rc->setProjectionRect(QRectF(QPointF(x, y), QSizeF(w, h)));
  return rc;
}


ZoomCommand::ZoomCommand(PlotItem *item, const QString &text)
    : ViewItemCommand(item, text, false) {

  if (!item->isTiedZoom()) {
    _originalStates << item->currentZoomState();
  } else {
    QList<PlotItem*> plots = PlotItemManager::tiedZoomPlotsForView(item->parentView());
    foreach (PlotItem *plotItem, plots) {
      _originalStates << plotItem->currentZoomState();
    }
  }
}


ZoomCommand::~ZoomCommand() {
}


void ZoomCommand::undo() {
  foreach (ZoomState state, _originalStates) {
    state.item->setCurrentZoomState(state);
  }
}


void ZoomCommand::redo() {
  foreach (ZoomState state, _originalStates) {
    applyZoomTo(state.item);
  }
}


/*
 * X axis zoom to FixedExpression, Y axis zoom to FixedExpression.
 */
void ZoomFixedExpressionCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(_fixed);
}


/*
 * X axis zoom to Range.
 */
void ZoomXRangeCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(QRect(_fixed.x(), item->projectionRect().y(), _fixed.width(), item->projectionRect().height()));
}


/*
 * Y axis zoom to Range.
 */
void ZoomYRangeCommand::applyZoomTo(PlotItem *item) {
  item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(QRect(item->projectionRect().x(), _fixed.y(), item->projectionRect().width(), _fixed.height()));
}


/*
 * X axis zoom to Auto, Y axis zoom to AutoBorder.
 */
void ZoomMaximumCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
  item->yAxis()->setAxisZoomMode(PlotAxis::AutoBorder);
  item->setProjectionRect(item->computedProjectionRect());
}


/*
 * X axis zoom to Auto, Y axis zoom to SpikeInsensitive.
 */
void ZoomMaxSpikeInsensitiveCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
  item->yAxis()->setAxisZoomMode(PlotAxis::SpikeInsensitive);
  item->setProjectionRect(item->computedProjectionRect());
}


/*
 * X axis zoom to Auto, Y axis zoom to Mean Centered.
 */
void ZoomYMeanCenteredCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
  item->yAxis()->setAxisZoomMode(PlotAxis::MeanCentered);
  item->setProjectionRect(item->computedProjectionRect());
}


/*
 * X axis zoom to auto, Y zoom not changed.
 */
void ZoomXMaximumCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
  QRectF compute = item->computedProjectionRect();
  item->setProjectionRect(QRectF(compute.x(),
                           item->projectionRect().y(),
                           compute.width(),
                           item->projectionRect().height()));
}

/*
 * X axis zoom changed to fixed and shifted to right:
 *       new_xmin = xmin + (xmax - xmin)*0.10;
 *       new_xmax = xmax + (xmax – xmin)*0.10;
 */
void ZoomXRightCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dx = (item->xMax() - item->xMin())*0.10;
  if (item->xAxis()->axisLog()) { 
    compute.setLeft(pow(10, item->xMin() + dx));
    compute.setRight(pow(10, item->xMax() + dx));
  } else {
    compute.setLeft(compute.left() + dx);
    compute.setRight(compute.right() + dx);
  }

  item->setProjectionRect(compute);
}

/*
 * X axis zoom changed to fixed and shifted to :
 *       new_xmin = xmin - (xmax - xmin)*0.10;
 *       new_xmax = xmax - (xmax – xmin)*0.10;
 */
void ZoomXLeftCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dx = (item->xMax() - item->xMin())*0.10;
  if (item->xAxis()->axisLog()) { 
    compute.setLeft(pow(10, item->xMin() - dx));
    compute.setRight(pow(10, item->xMax() - dx));
  } else {
    compute.setLeft(compute.left() - dx);
    compute.setRight(compute.right() - dx);
  }

  item->setProjectionRect(compute);
}

/*
 * X axis zoom changed to fixed and increased:
 *       new_xmin = xmin - (xmax - xmin)*0.25;
 *       new_xmax = xmax + (xmax – xmin)*0.25;
 */
void ZoomXOutCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dx = (item->xMax() - item->xMin())*0.25;
  if (item->xAxis()->axisLog()) { 
    compute.setLeft(pow(10, item->xMin() - dx));
    compute.setRight(pow(10, item->xMax() + dx));
  } else {
    compute.setLeft(compute.left() - dx);
    compute.setRight(compute.right() + dx);
  }

  item->setProjectionRect(compute);
//   item->update();
}


/*
 * X axis zoom changed to fixed and decreased:
 *       new_xmin = xmin + (xmax - xmin)*0.1666666;
 *       new_xmax = xmax - (xmax – xmin)*0.1666666;
 */
void ZoomXInCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dx = (item->xMax() - item->xMin())*0.1666666;
  if (item->xAxis()->axisLog()) { 
    compute.setLeft(pow(10, item->xMin() + dx));
    compute.setRight(pow(10, item->xMax() - dx));
  } else {
    compute.setLeft(compute.left() + dx);
    compute.setRight(compute.right() - dx);
  }

  item->setProjectionRect(compute);
}


/*
 * Normalize X axis to Y axis: Given the current plot aspect ratio, change
 * the X axis range to have the same units per mm as the Y axis range. Particularly
 * useful for images.
 */
void ZoomNormalizeXToYCommand::applyZoomTo(PlotItem *item) {
  QRectF compute = item->projectionRect();
  qreal mean = compute.center().x();
  qreal range = item->plotRect().width() * compute.height() / item->plotRect().height();

  compute.setLeft(mean - (range / 2.0));
  compute.setRight(mean + (range / 2.0));

  item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(compute);
}


/*
 * When zoomed in in X, auto zoom Y, only
 * counting points within the current X range. (eg, curve goes from x=0 to 100, but
 * we are zoomed in to x = 30 to 40. Adjust Y zoom to include all points with x
 * values between 30 and 40.
 */
void ZoomYLocalMaximumCommand::applyZoomTo(PlotItem *item) {
  qreal minimum = item->yAxis()->axisLog() ? 0.0 : -0.1;
  qreal maximum = 0.1;
  item->computedRelationalMax(minimum, maximum);

  item->computeBorder(Qt::Vertical, minimum, maximum);

  item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();
  compute.setTop(minimum);
  compute.setBottom(maximum);

  item->setProjectionRect(compute);
}


/*
 * Y axis zoom to auto, X zoom not changed.
 */
void ZoomYMaximumCommand::applyZoomTo(PlotItem *item) {
  item->yAxis()->setAxisZoomMode(PlotAxis::Auto);
  QRectF compute = item->computedProjectionRect();
  item->setProjectionRect(QRectF(item->projectionRect().x(),
                           compute.y(),
                           item->projectionRect().width(),
                           compute.height()));
}


/*
 * Y axis zoom up. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin + (ymax - ymin)*0.1;
 *             new_ymax = ymax + (ymax - ymin)*0.1;
 */
void ZoomYUpCommand::applyZoomTo(PlotItem *item) {
  if (item->yAxis()->axisZoomMode() != PlotAxis::MeanCentered)
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dy = (item->yMax() - item->yMin())*0.1;
  if (item->yAxis()->axisLog()) { 
    compute.setTop(pow(10, item->yMin() + dy));
    compute.setBottom(pow(10, item->yMax() + dy));
  } else {
    compute.setTop(compute.top() + dy);
    compute.setBottom(compute.bottom() + dy);
  }

  item->setProjectionRect(compute);
}


/*
 * Y axis zoom down. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin - (ymax - ymin)*0.10;
 *             new_ymax = ymax - (ymax - ymin)*0.10;
 */
void ZoomYDownCommand::applyZoomTo(PlotItem *item) {
  if (item->yAxis()->axisZoomMode() != PlotAxis::MeanCentered)
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dy = (item->yMax() - item->yMin())*0.1;
  if (item->yAxis()->axisLog()) { 
    compute.setTop(pow(10, item->yMin() - dy));
    compute.setBottom(pow(10, item->yMax() - dy));
  } else {
    compute.setTop(compute.top() - dy);
    compute.setBottom(compute.bottom() - dy);
  }

  item->setProjectionRect(compute);
}


/*
 * Y axis zoom increased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin - (ymax - ymin)*0.25;
 *             new_ymax = ymax + (ymax - ymin)*0.25;
 */
void ZoomYOutCommand::applyZoomTo(PlotItem *item) {
  if (item->yAxis()->axisZoomMode() != PlotAxis::MeanCentered)
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dy = (item->yMax() - item->yMin())*0.25;
  if (item->yAxis()->axisLog()) { 
    compute.setTop(pow(10, item->yMin() - dy));
    compute.setBottom(pow(10, item->yMax() + dy));
  } else {
    compute.setTop(compute.top() - dy);
    compute.setBottom(compute.bottom() + dy);
  }

  item->setProjectionRect(compute);
//   item->update();
}


/*
 * Y axis zoom decreased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin + (ymax - ymin)*0.1666666;
 *             new_ymax = ymax - (ymax – ymin)*0.1666666;
 */
void ZoomYInCommand::applyZoomTo(PlotItem *item) {
  if (item->yAxis()->axisZoomMode() != PlotAxis::MeanCentered)
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

  QRectF compute = item->projectionRect();

  qreal dy = (item->yMax() - item->yMin())*0.1666666;
  if (item->yAxis()->axisLog()) { 
    compute.setTop(pow(10, item->yMin() + dy));
    compute.setBottom(pow(10, item->yMax() - dy));
  } else {
    compute.setTop(compute.top() + dy);
    compute.setBottom(compute.bottom() - dy);
  }

  item->setProjectionRect(compute);
//   item->update();
}


/*
 * Normalize Y axis to X axis: Given the current plot aspect ratio,
 * change the Y axis range to have the same units per mm as the X axis range.
 * Particularly useful for images.
 */
void ZoomNormalizeYToXCommand::applyZoomTo(PlotItem *item) {
  QRectF compute = item->projectionRect();
  qreal mean = compute.center().y();
  qreal range = item->plotRect().height() * compute.width() / item->plotRect().width();

  compute.setTop(mean - (range / 2.0));
  compute.setBottom(mean + (range / 2.0));

  item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(compute);
}


/*
 * Switch the X Axis to a Log Scale.
 */
void ZoomXLogCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisLog(_enableLog);
  item->setProjectionRect(item->computedProjectionRect()); //need to recompute
}


/*
 * Switch the Y Axis to a Log Scale.
 */
void ZoomYLogCommand::applyZoomTo(PlotItem *item) {
  item->yAxis()->setAxisLog(_enableLog);
  item->setProjectionRect(item->computedProjectionRect()); //need to recompute
}


}

// vim: ts=2 sw=2 et
