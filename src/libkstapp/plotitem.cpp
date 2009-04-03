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

#include "plotitemmanager.h"
#include "plotrenderitem.h"

#include "layoutboxitem.h"
#include "viewgridlayout.h"
#include "debug.h"

#include "application.h"
#include "mainwindow.h"
#include "document.h"
#include "tabwidget.h"
#include "labelrenderer.h"

#include "datacollection.h"
#include "cartesianrenderitem.h"

#include "plotitemdialog.h"
#include "dialoglauncher.h"
#include "sharedaxisboxitem.h"

#include "applicationsettings.h"
#include "updatemanager.h"

#include "math_kst.h"

#include "settings.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

// Zoom Debugging.  0 Off, 1 On.
#define DEBUG_ZOOM 0

// Label Region Debugging.  0 Off, 1 On.
#define DEBUG_LABEL_REGION 0

// Benchmark drawing
// #define BENCHMARK 1

static const int PLOT_MAXIMIZED_ZORDER = 1000;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent),
  _isInSharedAxisBox(false),
  _plotRectsDirty(true),
  _calculatedLeftLabelMargin(0.0),
  _calculatedRightLabelMargin(0.0),
  _calculatedTopLabelMargin(0.0),
  _calculatedBottomLabelMargin(0.0),
  _calculatedLabelMarginWidth(0.0),
  _calculatedLabelMarginHeight(0.0),
  _calculatedAxisMarginWidth(0.0),
  _calculatedAxisMarginHeight(0.0),
  _calculatedAxisMarginVLead(0.0),
  _calculatedAxisMarginHLead(0.0),
  _calculatedAxisMarginROverflow(0.0),
  _calculatedAxisMarginTOverflow(0.0),
  _leftPadding(0.0),
  _bottomPadding(0.0),
  _rightPadding(0.0),
  _topPadding(0.0),
  _showLegend(false),
  _plotMaximized(false),
  _allowUpdates(true),
  _updateDelayed(false),
  _legend(0),
  _axisLabelsDirty(true),
  _zoomMenu(0),
  _filterMenu(0),
  _fitMenu(0),
  _sharedBox(0)
{

  setTypeName("Plot");
  setBrush(Qt::white);

  setSupportsTiedZoom(true);

  _xAxis = new PlotAxis(this, Qt::Horizontal);
  _yAxis = new PlotAxis(this, Qt::Vertical);

  connect(this, SIGNAL(geometryChanged()), _xAxis, SLOT(setTicksUpdated()));
  connect(this, SIGNAL(geometryChanged()), _yAxis, SLOT(setTicksUpdated()));
  connect(this, SIGNAL(geometryChanged()), this, SLOT(setPlotRectsDirty()));

  _leftLabelDetails = new PlotLabel(this);
  _rightLabelDetails = new PlotLabel(this);
  _topLabelDetails = new PlotLabel(this);
  _bottomLabelDetails = new PlotLabel(this);
  _numberLabelDetails = new PlotLabel(this);

  connect(_leftLabelDetails, SIGNAL(labelChanged()), this, SLOT(setPlotBordersDirty()));
  connect(_leftLabelDetails, SIGNAL(labelChanged()), this, SLOT(setLeftLabelDirty()));
  connect(_rightLabelDetails, SIGNAL(labelChanged()), this, SLOT(setPlotBordersDirty()));
  connect(_rightLabelDetails, SIGNAL(labelChanged()), this, SLOT(setRightLabelDirty()));
  connect(_topLabelDetails, SIGNAL(labelChanged()), this, SLOT(setPlotBordersDirty()));
  connect(_topLabelDetails, SIGNAL(labelChanged()), this, SLOT(setTopLabelDirty()));
  connect(_bottomLabelDetails, SIGNAL(labelChanged()), this, SLOT(setPlotBordersDirty()));
  connect(_bottomLabelDetails, SIGNAL(labelChanged()), this, SLOT(setBottomLabelDirty()));
  connect(_numberLabelDetails, SIGNAL(labelChanged()), this, SLOT(setPlotBordersDirty()));
  connect(_numberLabelDetails, SIGNAL(labelChanged()), this, SLOT(setAxisLabelsDirty()));

  _globalFont = parentView()->defaultFont();
  _globalFontColor = ApplicationSettings::self()->defaultFontColor();
  _globalFontScale = ApplicationSettings::self()->defaultFontScale();

  _undoStack = new QUndoStack(this);

  createActions();

  PlotItemManager::self()->addPlot(this);

  // Set the initial projection.
  setProjectionRect(QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)));
  renderItem(PlotRenderItem::Cartesian);

  _shortName = "G"+QString::number(_gnum);
  if (_gnum>max_gnum)
    max_gnum = _gnum;
  _gnum++;

  setPlotBordersDirty(true);
  connect(this, SIGNAL(updatePlotRect()), this, SLOT(redrawPlot()));
  connect(this, SIGNAL(geometryChanged()), this, SLOT(setLabelsDirty()));
}


PlotItem::~PlotItem() {
  delete _xAxis;
  delete _yAxis;
  delete _leftLabelDetails;
  delete _rightLabelDetails;
  delete _topLabelDetails;
  delete _bottomLabelDetails;
  delete _numberLabelDetails;

  PlotItemManager::self()->removePlot(this);
}


QString PlotItem::plotName() const {
  return Name();
}


void PlotItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("plot");
    xml.writeAttribute("tiedzoom", QVariant(isTiedZoom()).toString());
    xml.writeAttribute("leftlabelvisible", QVariant(_leftLabelDetails->isVisible()).toString());
    xml.writeAttribute("bottomlabelvisible", QVariant(_bottomLabelDetails->isVisible()).toString());
    xml.writeAttribute("rightlabelvisible", QVariant(_rightLabelDetails->isVisible()).toString());
    xml.writeAttribute("toplabelvisible", QVariant(_topLabelDetails->isVisible()).toString());
    xml.writeAttribute("globalfont", QVariant(_globalFont).toString());
    xml.writeAttribute("globalfontscale", QVariant(_globalFontScale).toString());
    xml.writeAttribute("globalfontcolor", QVariant(_globalFontColor).toString());
    xml.writeAttribute("showlegend", QVariant(_showLegend).toString());
    saveNameInfo(xml, GNUM);

    ViewItem::save(xml);
    legend()->saveInPlot(xml);

    foreach (PlotRenderItem *renderer, renderItems()) {
      renderer->saveInPlot(xml);
    }
    _xAxis->saveInPlot(xml, QString("xaxis"));
    _yAxis->saveInPlot(xml, QString("yaxis"));
    _leftLabelDetails->saveInPlot(xml, QString("leftlabel"));
    _rightLabelDetails->saveInPlot(xml, QString("rightlabel"));
    _topLabelDetails->saveInPlot(xml, QString("toplabel"));
    _bottomLabelDetails->saveInPlot(xml, QString("bottomlabel"));
    _numberLabelDetails->saveInPlot(xml, QString("numberlabel"));

    xml.writeStartElement("projectionrect");
    xml.writeAttribute("x", QVariant(projectionRect().x()).toString());
    xml.writeAttribute("y", QVariant(projectionRect().y()).toString());
    xml.writeAttribute("width", QVariant(projectionRect().width()).toString());
    xml.writeAttribute("height", QVariant(projectionRect().height()).toString());
    xml.writeEndElement();
    xml.writeEndElement();
  }
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

  _plotMaximize = new QAction(tr("Maximize Plot"), this);
  _plotMaximize->setShortcut(Qt::Key_Z);
  _plotMaximize->setCheckable(true);
  registerShortcut(_plotMaximize);
  connect(_plotMaximize, SIGNAL(triggered()), this, SLOT(plotMaximize()));
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


void PlotItem::createFilterMenu() {
  if (_filterMenu) {
    delete _filterMenu;
  }

  _filterMenu = new QMenu;
  _filterMenu->setTitle(tr("Filter"));

  QAction *action;
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (CurvePtr curve = kst_cast<Curve>(relation)) {
        action = new QAction(relation->Name(), this);
        _filterMenu->addAction(action);
      }
    }
  }
  connect(_filterMenu, SIGNAL(triggered(QAction*)), this, SLOT(showFilterDialog(QAction*)));
}


void PlotItem::createFitMenu() {
  if (_fitMenu) {
    delete _fitMenu;
  }

  _fitMenu = new QMenu;
  _fitMenu->setTitle(tr("Fit"));

  QAction *action;
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (CurvePtr curve = kst_cast<Curve>(relation)) {
        action = new QAction(relation->Name(), this);
        _fitMenu->addAction(action);
      }
    }
  }
  connect(_fitMenu, SIGNAL(triggered(QAction*)), this, SLOT(showFitDialog(QAction*)));
}


void PlotItem::addToMenuForContextEvent(QMenu &menu) {
  if (parentItem() && isInSharedAxisBox() && _sharedBox) {
    if (parentView()->viewMode() == View::Data) {
      QAction *breakSharedBox = new QAction(tr("Break Shared Axis Box"), this);
      breakSharedBox->setShortcut(Qt::Key_B);
      connect(breakSharedBox, SIGNAL(triggered()), _sharedBox, SLOT(breakShare()));
      menu.addAction(breakSharedBox);
    }
  }

  if (parentView()->viewMode() == View::Data) {
    _plotMaximize->setChecked(_plotMaximized);
    menu.addAction(_plotMaximize);
  }

  _zoomLogX->setChecked(xAxis()->axisLog());
  _zoomLogY->setChecked(yAxis()->axisLog());
  menu.addMenu(_zoomMenu);
  if (!DataObject::filterPluginList().empty()) {
    createFilterMenu();
    menu.addMenu(_filterMenu);
  }

  if (!DataObject::fitsPluginList().empty()) {
    createFitMenu();
    menu.addMenu(_fitMenu);
  }
}


void PlotItem::showFilterDialog(QAction* action) {
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (relation->Name() == action->text()) {
        if (CurvePtr curve = kst_cast<Curve>(relation)) {
          DialogLauncher::self()->showBasicPluginDialog(DataObject::filterPluginList().first(), 0, curve->xVector(), curve->yVector(), this);
        }
      }
    }
  }
}


void PlotItem::showFitDialog(QAction* action) {
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (relation->Name() == action->text()) {
        if (CurvePtr curve = kst_cast<Curve>(relation)) {
          DialogLauncher::self()->showBasicPluginDialog(DataObject::fitsPluginList().first(), 0, curve->xVector(), curve->yVector(), this);
        }
      }
    }
  }
}


void PlotItem::redrawPlot() {
  update();
}


void PlotItem::updateObject() {
  if (!_allowUpdates) {
    _updateDelayed = true;
    return;
  }
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "\t\tUP - Updating Plot";
#endif
  if (xAxis()->axisZoomMode() == PlotAxis::Auto) {
    if (yAxis()->axisZoomMode() == PlotAxis::AutoBorder || yAxis()->axisZoomMode() == PlotAxis::Auto
        || yAxis()->axisZoomMode() == PlotAxis::SpikeInsensitive || yAxis()->axisZoomMode() == PlotAxis::MeanCentered) {
#if DEBUG_UPDATE_CYCLE > 1
      qDebug() << "\t\t\tUP - Updating Plot Projection Rect - X and Y Maximum";
#endif
      setProjectionRect(computedProjectionRect());
    } else {
#if DEBUG_UPDATE_CYCLE > 1
      qDebug() << "\t\t\tUP - Updating Plot Projection Rect - X Maximum";
#endif
      QRectF compute = computedProjectionRect();
      setProjectionRect(QRectF(compute.x(),
            projectionRect().y(),
            compute.width(),
            projectionRect().height()));
    }
  } else if (yAxis()->axisZoomMode() == PlotAxis::Auto) {
#if DEBUG_UPDATE_CYCLE > 1
    qDebug() << "\t\t\tUP - Updating Plot Projection Rect - Y Maximum";
#endif
    QRectF compute = computedProjectionRect();
    setProjectionRect(QRectF(projectionRect().x(),
          compute.y(),
          projectionRect().width(),
          compute.height()));
  }
  setLabelsDirty();
  update();
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


void PlotItem::calculateBorders(QPainter *painter) {
  calculateBottomTickLabelBound(painter);
  calculateLeftTickLabelBound(painter);

  calculateLeftLabelMargin(painter);
  calculateRightLabelMargin(painter);
  calculateTopLabelMargin(painter);
  calculateBottomLabelMargin(painter);

  calculateMargins();

  setPlotRectsDirty();
}


void PlotItem::paint(QPainter *painter) {
  if (parentView()->plotBordersDirty() || (creationState() == ViewItem::InProgress)) {
    ViewGridLayout::standardizePlotMargins(this, painter);
    setPlotBordersDirty(false);
  }

#ifdef BENCHMARK
  QTime bench_time, benchtmp;
  int b_1 = 0, b_2 = 0, b_3 = 0, b_4 = 0, b_5 = 0;
  bench_time.start();
  benchtmp.start();
#endif
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->drawRect(rect());
  painter->restore();

  painter->save();
  painter->setFont(numberLabelDetails()->calculatedFont());

#ifdef BENCHMARK
    b_1 = benchtmp.elapsed();
#endif
#if DEBUG_LABEL_REGION
  //  qDebug() << "=============> leftLabel:" << leftLabel() << endl;
#endif
  paintLeftLabel(painter);
#if DEBUG_LABEL_REGION
  //  qDebug() << "=============> bottomLabel:" << bottomLabel() << endl;
#endif
  paintBottomLabel(painter);
#if DEBUG_LABEL_REGION
  //  qDebug() << "=============> rightLabel:" << rightLabel() << endl;
#endif
  paintRightLabel(painter);
#if DEBUG_LABEL_REGION
  //  qDebug() << "=============> topLabel:" << topLabel() << endl;
#endif
  paintTopLabel(painter);
#ifdef BENCHMARK
    b_2 = benchtmp.elapsed();
#endif

  paintPlot(painter);
#ifdef BENCHMARK
    b_3 = benchtmp.elapsed();
#endif

  paintTickLabels(painter);
#ifdef BENCHMARK
    b_4 = benchtmp.elapsed();
#endif

  paintPlotMarkers(painter);
#ifdef BENCHMARK
    b_5 = benchtmp.elapsed();
#endif

  painter->restore();
#ifdef BENCHMARK
  int i = bench_time.elapsed();
  qDebug() << endl << "Painting Plot " << (void *)this << ": " << i << "ms";
  if (b_1 > 0)       qDebug() << "            Setup: " << b_1 << "ms";
  if (b_2 - b_1 > 0) qDebug() << "           Labels: " << (b_2 - b_1) << "ms";
  if (b_3 - b_2 > 0) qDebug() << "             Plot: " << (b_3 - b_2) << "ms";
  if (b_4 - b_3 > 0) qDebug() << "      Tick Labels: " << (b_4 - b_3) << "ms";
  if (b_5 - b_4 > 0) qDebug() << "          Markers: " << (b_5 - b_4) << "ms";
#endif
}


void PlotItem::paintPlot(QPainter *painter) {
  bool xLabelsUpdated = false;
  bool yLabelsUpdated = false;
  if (xAxis()->ticksUpdated()) {
    xAxis()->validateDrawingRegion(painter);
    updateXAxisLines();
    updateXAxisLabels(painter);
    xLabelsUpdated = true;
  }
  if (yAxis()->ticksUpdated()) {
    yAxis()->validateDrawingRegion(painter);
    updateYAxisLines();
    updateYAxisLabels(painter);
    yLabelsUpdated = true;
  }

  if (_axisLabelsDirty) {
    if (!xLabelsUpdated) {
      updateXAxisLabels(painter);
    }
    if (!yLabelsUpdated) {
      updateYAxisLabels(painter);
    }
  }
#ifdef BENCHMARK
  QTime bench_time, benchtmp;
  int b_1 = 0, b_2 = 0, b_3 = 0, b_4 = 0, b_5 = 0;
  bench_time.start();
  benchtmp.start();
#endif
  paintMajorGridLines(painter);
#ifdef BENCHMARK
    b_1 = benchtmp.elapsed();
#endif
  paintMinorGridLines(painter);
#ifdef BENCHMARK
    b_2 = benchtmp.elapsed();
#endif
  painter->save();
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(plotRect());
  painter->restore();
#ifdef BENCHMARK
    b_3 = benchtmp.elapsed();
#endif
  paintMajorTicks(painter);
#ifdef BENCHMARK
    b_4 = benchtmp.elapsed();
#endif
  paintMinorTicks(painter);
#ifdef BENCHMARK
    b_5 = benchtmp.elapsed();
#endif

#ifdef BENCHMARK
  int i = bench_time.elapsed();
  qDebug() << endl << "Painting Plot - PaintPlot " << (void *)this << ": " << i << "ms";
  if (b_1 > 0)       qDebug() << "            Major Lines: " << b_1 << "ms";
  if (b_2 - b_1 > 0) qDebug() << "            Minor Lines: " << (b_2 - b_1) << "ms";
  if (b_3 - b_2 > 0) qDebug() << "                   Rect: " << (b_3 - b_2) << "ms";
  if (b_4 - b_3 > 0) qDebug() << "            Major Ticks: " << (b_4 - b_3) << "ms";
  if (b_5 - b_4 > 0) qDebug() << "            Minor Ticks: " << (b_5 - b_4) << "ms";
#endif
}


void PlotItem::updateXAxisLines() {
  qreal majorTickLength = qMin(rect().width(), rect().height()) * .02; //two percent
  qreal minorTickLength = qMin(rect().width(), rect().height()) * 0.01; //one percent
  QRectF rect = plotRect();

  _xMajorGridLines.clear();
  _xMajorTickLines.clear();
  foreach (qreal x, _xAxis->axisMajorTicks()) {
    QPointF p1 = QPointF(mapXToPlot(x), rect.bottom());
    QPointF p2 = p1 - QPointF(0, rect.height());
    _xMajorGridLines << QLineF(p1, p2);

    p2 = p1 - QPointF(0, majorTickLength);
    _xMajorTickLines << QLineF(p1, p2);

    p1.setY(rect.top());
    p2 = p1 + QPointF(0, majorTickLength);
    _xMajorTickLines << QLineF(p1, p2);
  }
  _xMinorGridLines.clear();
  _xMinorTickLines.clear();
  foreach (qreal x, _xAxis->axisMinorTicks()) {
    QPointF p1 = QPointF(mapXToPlot(x), rect.bottom());
    QPointF p2 = p1 - QPointF(0, rect.height());
    _xMinorGridLines << QLineF(p1, p2);

    p2 = p1 - QPointF(0, minorTickLength);
    _xMinorTickLines << QLineF(p1, p2);

    p1.setY(rect.top());
    p2 = p1 + QPointF(0, minorTickLength);
    _xMinorTickLines << QLineF(p1, p2);
  }
  _xPlotMarkerLines.clear();
  foreach (double x, _xAxis->axisPlotMarkers().markers()) {
    if (x > _xMin && x < _xMax) {
      QPointF p1 = QPointF(mapXToPlot(x), rect.bottom());
      QPointF p2 = p1 - QPointF(0, rect.height());
      _xPlotMarkerLines << QLineF(p1, p2);
    }
  }
}


void PlotItem::updateXAxisLabels(QPainter* painter) {
  int flags = Qt::TextSingleLine | Qt::AlignCenter;
  _xPlotLabels.clear();
  QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
  while (xLabelIt.hasNext()) {
    xLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
    bound.setWidth(bound.width());
    QPointF p = QPointF(mapXToPlot(xLabelIt.key()), plotRect().bottom() + 
        bound.height()*0.5 + _calculatedAxisMarginVLead);
    bound.moveCenter(p);

    if (rect().left() > bound.left()) bound.setLeft(rect().left());
    if (rect().right() < bound.right()) bound.setRight(rect().right());

    CachedPlotLabel label;
    label.bound = bound;
    label.value = xLabelIt.value();
    _xPlotLabels.append(label);
  }

  if (!_xAxis->baseLabel().isEmpty()) {
    QRectF bound = painter->boundingRect(QRectF(), flags, _xAxis->baseLabel());
    QPointF p = QPointF(plotRect().left(), plotRect().bottom() + bound.height() * 2.0 + _calculatedAxisMarginVLead);
    bound.moveBottomLeft(p);

    CachedPlotLabel label;
    label.bound = bound;
    label.value = _xAxis->baseLabel();
    label.baseLabel = true;
    _xPlotLabels.append(label);
  }
}


void PlotItem::setPlotRectsDirty() {
  _plotRectsDirty = true; 
  xAxis()->setTicksUpdated(); 
  yAxis()->setTicksUpdated();
}


void PlotItem::updateYAxisLines() {
  qreal majorTickLength = qMin(rect().width(), rect().height()) * .02; //two percent
  qreal minorTickLength = qMin(rect().width(), rect().height()) * 0.01; //one percent
  QRectF rect = plotRect();

  _yMajorGridLines.clear();
  _yMajorTickLines.clear();
  foreach (qreal y, _yAxis->axisMajorTicks()) {
    QPointF p1 = QPointF(rect.left(), mapYToPlot(y));
    QPointF p2 = p1 + QPointF(rect.width(), 0);
    _yMajorGridLines << QLineF(p1, p2);

    p2 = p1 + QPointF(majorTickLength, 0);
    _yMajorTickLines << QLineF(p1, p2);

    p1.setX(rect.right());
    p2 = p1 - QPointF(majorTickLength, 0);
    _yMajorTickLines << QLineF(p1, p2);
  }

  _yMinorGridLines.clear();
  _yMinorTickLines.clear();
  foreach (qreal y, _yAxis->axisMinorTicks()) {
    QPointF p1 = QPointF(rect.left(), mapYToPlot(y));
    QPointF p2 = p1 + QPointF(rect.width(), 0);
    _yMinorGridLines << QLineF(p1, p2);

    p2 = p1 + QPointF(minorTickLength, 0);
    _yMinorTickLines << QLineF(p1, p2);

    p1.setX(rect.right());
    p2 = p1 - QPointF(minorTickLength, 0);
    _yMinorTickLines << QLineF(p1, p2);
  }

  _yPlotMarkerLines.clear();
  foreach (double y, _yAxis->axisPlotMarkers().markers()) {
    if (y > _yMin && y < _yMax) {
      QPointF p1 = QPointF(rect.left(), mapYToPlot(y));
      QPointF p2 = p1 + QPointF(rect.width(), 0);
      _yPlotMarkerLines << QLineF(p1, p2);
    }
  }
}


void PlotItem::updateYAxisLabels(QPainter* painter) {
  int flags = Qt::TextSingleLine | Qt::AlignCenter;
  _yPlotLabels.clear();
  QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
  while (yLabelIt.hasNext()) {
    yLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
    bound.setWidth(bound.width());
    QPointF p = QPointF(plotRect().left() - (bound.width() / 2.0) - _calculatedAxisMarginHLead, mapYToPlot(yLabelIt.key()));
    bound.moveCenter(p);

    if (rect().top() > bound.top()) bound.setTop(rect().top());
    if (rect().bottom() < bound.bottom()) bound.setBottom(rect().bottom());

    CachedPlotLabel label;
    label.bound = bound;
    label.value = yLabelIt.value();
    _yPlotLabels.append(label);
  }

  if (!_yAxis->baseLabel().isEmpty()) {
    painter->save();
    painter->rotate(-90.0);

    QRectF bound = painter->boundingRect(QRectF(), flags, _yAxis->baseLabel());
    bound = QRectF(bound.x(), bound.bottomRight().y() - bound.width(), bound.height(), bound.width());
    QPointF p = QPointF(rect().left(), plotRect().bottom());
    bound.moveBottomLeft(p);

    CachedPlotLabel label;
    label.bound = bound;
    label.value = _yAxis->baseLabel();
    label.baseLabel = true;
    _yPlotLabels.append(label);
    painter->restore();
  }
}


void PlotItem::paintMajorGridLines(QPainter *painter) {
  if (xAxis()->drawAxisMajorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisMajorGridLineColor()), 1.0, _xAxis->axisMajorGridLineStyle()));
    painter->drawLines(_xMajorGridLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMajorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMajorGridLineColor()), 1.0, _yAxis->axisMajorGridLineStyle()));
    painter->drawLines(_yMajorGridLines);
    painter->restore();
  }
}


void PlotItem::paintMinorGridLines(QPainter *painter) {
  if (xAxis()->drawAxisMinorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisMinorGridLineColor()), 1.0, _xAxis->axisMinorGridLineStyle()));
    painter->drawLines(_xMinorGridLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMinorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMinorGridLineColor()), 1.0, _yAxis->axisMinorGridLineStyle()));
    painter->drawLines(_yMinorGridLines);
    painter->restore();
  }
}


void PlotItem::paintMajorTicks(QPainter *painter) {
  if (xAxis()->drawAxisMajorTicks()) {
    painter->drawLines(_xMajorTickLines);
  }

  if (yAxis()->drawAxisMajorTicks()) {
    painter->drawLines(_yMajorTickLines);
  }
}


void PlotItem::paintMinorTicks(QPainter *painter) {
  if (xAxis()->drawAxisMinorTicks()) {
    painter->drawLines(_xMinorTickLines);
  }

  if (yAxis()->drawAxisMinorTicks()) {
    painter->drawLines(_yMinorTickLines);
  }
}


void PlotItem::paintBottomTickLabels(QPainter *painter) {
  int flags = Qt::TextSingleLine | Qt::AlignCenter;

  painter->save();
  painter->setPen(_numberLabelDetails->fontColor());

  foreach(CachedPlotLabel label, _xPlotLabels) {
    painter->drawText(label.bound, flags, label.value);
  }
  painter->restore();

#if DEBUG_LABEL_REGION
  QRectF xLabelRect;
  foreach(CachedPlotLabel label, _xPlotLabels) {
    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(label.bound);
    } else {
      xLabelRect = label.bound;
    }
  }
  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "Bottom Tick Labels - xLabelRect:" << xLabelRect;
  painter->fillRect(xLabelRect, Qt::green);
  painter->restore();
#endif
}


void PlotItem::paintLeftTickLabels(QPainter *painter) {
  int flags = Qt::TextSingleLine | Qt::AlignVCenter;

  painter->save();
  painter->setPen(_numberLabelDetails->fontColor());

  foreach(CachedPlotLabel label, _yPlotLabels) {
    if (label.baseLabel) {
      painter->save();
      QTransform t;
      t.rotate(90.0);
      painter->rotate(-90.0);

      painter->drawText(t.mapRect(label.bound), flags, label.value);
      painter->restore();
    } else {
      painter->drawText(label.bound, flags, label.value);
    }
  }
  painter->restore();

#if DEBUG_LABEL_REGION
  QRectF yLabelRect;
  foreach(CachedPlotLabel label, _yPlotLabels) {
    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(label.bound);
    } else {
      yLabelRect = label.bound;
    }
  }
  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "Left Tick Labels - yLabelRect:" << yLabelRect;
  painter->fillRect(yLabelRect, Qt::green);
  painter->restore();
#endif
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
  if (!_xPlotMarkerLines.isEmpty()) {
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisPlotMarkers().lineColor()), _xAxis->axisPlotMarkers().lineWidth(), _xAxis->axisPlotMarkers().lineStyle()));
    painter->drawLines(_xPlotMarkerLines);
    painter->restore();
  }
  if (!_yPlotMarkerLines.isEmpty()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisPlotMarkers().lineColor()), _yAxis->axisPlotMarkers().lineWidth(), _yAxis->axisPlotMarkers().lineStyle()));
    painter->drawLines(_yPlotMarkerLines);
    painter->restore();
  }

}


void PlotItem::calculatePlotRects() {
  // Calculate the plotAxisRect first.
  qreal left = _leftLabelDetails->isVisible() ? leftLabelMargin() : 0.0;
  qreal bottom = _bottomLabelDetails->isVisible() ? bottomLabelMargin() : 0.0;
  qreal right = _rightLabelDetails->isVisible() ? rightMarginSize() : 0.0;
  qreal top = _topLabelDetails->isVisible() ? topMarginSize() : 0.0;

  QPointF topLeft(rect().topLeft() + QPointF(left, top));
  QPointF bottomRight(rect().bottomRight() - QPointF(right, bottom));

  _calculatedPlotAxisRect = QRectF(topLeft, bottomRight);

  // Use the PlotAxisRect as basis to calculate PlotRect.
  QRectF plot = _calculatedPlotAxisRect;
  qreal xOffset = _xAxis->isAxisVisible() ? axisMarginHeight() : 0.0;
  qreal yOffset = _yAxis->isAxisVisible() ? axisMarginWidth() : 0.0;
  qreal bottomPadding = _xAxis->isAxisVisible() ? _bottomPadding : 0.0;
  qreal leftPadding = _yAxis->isAxisVisible() ? _leftPadding : 0.0;
  qreal rightPadding = _rightLabelDetails->isVisible() ? _rightPadding : 0.0;
  qreal topPadding = _topLabelDetails->isVisible() ? _topPadding : 0.0;

  plot.setLeft(plot.left() + yOffset + leftPadding);
  plot.setBottom(plot.bottom() - xOffset - bottomPadding);
  plot.setRight(plot.right() - rightPadding);
  plot.setTop(plot.top() + topPadding);
  if (!plot.isValid()) {
    if (plot.width() <= 0) {
      plot.setWidth(0.1);
    }
    if (plot.height() <= 0) {
      plot.setHeight(0.1);
    }
  }
  _calculatedPlotRect = plot;
  _plotRectsDirty = false;
}


QRectF PlotItem::plotAxisRect() {
  if (_plotRectsDirty) {
    calculatePlotRects();
  }
  return _calculatedPlotAxisRect;
}


QRectF PlotItem::plotRect() {
  if (_plotRectsDirty) {
    calculatePlotRects();
  }
  return _calculatedPlotRect;
}


qreal PlotItem::leftMarginSize() const {
  qreal margin = _leftLabelDetails->isVisible() ? leftLabelMargin() : 0.0;
  margin += _yAxis->isAxisVisible() ? axisMarginWidth() : 0.0;

  return margin;
}


qreal PlotItem::bottomMarginSize() const {
  qreal margin = _bottomLabelDetails->isVisible() ? bottomLabelMargin() : 0.0;
  margin += _xAxis->isAxisVisible() ? axisMarginHeight() : 0.0;

  return margin;
}


qreal PlotItem::rightMarginSize() const {
  qreal margin = _rightLabelDetails->isVisible() ? rightLabelMargin() : 0.0;
  if (supportsTiedZoom() && margin < tiedZoomSize().width()) margin = tiedZoomSize().width();
  return margin;
}


qreal PlotItem::topMarginSize() const {
  qreal margin = _topLabelDetails->isVisible() ? topLabelMargin() : 0.0;
  if (supportsTiedZoom() && margin < tiedZoomSize().height()) margin = tiedZoomSize().height();
  return margin;
}


void PlotItem::setPadding(const qreal left, const qreal right, const qreal top, const qreal bottom) {
  if ((left == _leftPadding) && (right == _rightPadding) && (top == _topPadding) && (bottom == _bottomPadding)) {
    return;
  }
  _leftPadding = left;
  _rightPadding = right;
  _topPadding = top;
  _bottomPadding = bottom;
  setPlotRectsDirty();
  updatePlotRect();
}


void PlotItem::setLeftPadding(const qreal padding) {
  if (padding != _leftPadding) {
    _leftPadding = padding;
    setPlotRectsDirty();
  }
}


void PlotItem::setBottomPadding(const qreal padding) {
  if (padding != _bottomPadding) {
    _bottomPadding = padding;
    setPlotRectsDirty();
  }
}


void PlotItem::setRightPadding(const qreal padding) {
  if (padding != _rightPadding) {
    _rightPadding = padding;
    setPlotRectsDirty();
  }
}


void PlotItem::setTopPadding(const qreal padding) {
  if (padding != _topPadding) {
    _topPadding = padding;
    setPlotRectsDirty();
  }
}


QRectF PlotItem::projectionRect() const {
  return _projectionRect;
}


void PlotItem::setTiedZoom(bool tiedZoom, bool checkAllTied) {
  if ((_isInSharedAxisBox && !tiedZoom) || (_isTiedZoom == tiedZoom))
    return;

  _isTiedZoom = tiedZoom;

  if (_isTiedZoom)
    PlotItemManager::self()->addTiedZoomPlot(this, checkAllTied);
  else
    PlotItemManager::self()->removeTiedZoomPlot(this);

  //FIXME ugh, this is expensive, but need to redraw the renderitems checkboxes...
  update();
}


bool PlotItem::isInSharedAxisBox() const {
  return _isInSharedAxisBox;
}


void PlotItem::setInSharedAxisBox(bool inSharedBox) {
  _isInSharedAxisBox = inSharedBox;
  setSupportsTiedZoom(!_isInSharedAxisBox);
  setLockParent(inSharedBox);
}


SharedAxisBoxItem* PlotItem::sharedAxisBox() {
  return _sharedBox;
}


void PlotItem::setSharedAxisBox(SharedAxisBoxItem* parent) {
  if (parent) {
    if (_isTiedZoom) {
      setTiedZoom(false);
    }
    setInSharedAxisBox(true);
    setAllowedGripModes(0);
    setFlags(0);
    setParent(parent);
    setTiedZoom(true);
    setBrush(Qt::transparent);
  } else {
    setInSharedAxisBox(false);
    setTiedZoom(false);
    setAllowedGripModes(Move | Resize | Rotate);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setParent(0);
    setBrush(Qt::white);
  }
  _sharedBox = parent;
}


void PlotItem::setPlotBordersDirty(bool dirty) {
  if (isInSharedAxisBox() && dirty && _sharedBox) {
    _sharedBox->setDirty();
  } else {
    parentView()->setPlotBordersDirty(dirty);
  }
  setPlotRectsDirty();
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


QPointF PlotItem::mapToPlot(const QPointF &point) {
  return QPointF(mapXToPlot(point.x()), mapYToPlot(point.y()));
}


qreal PlotItem::mapXToPlot(const qreal &x) {
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


qreal PlotItem::mapYToPlot(const qreal &y) {
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


QFont PlotItem::globalFont() const {
  return _globalFont;
}


void PlotItem::setGlobalFont(const QFont &font) {
  if (font != _globalFont) {
    _globalFont = font;
    setPlotBordersDirty(true);
    setLabelsDirty();
  }
}


qreal PlotItem::globalFontScale() const {
  return _globalFontScale;
}


void PlotItem::setGlobalFontScale(const qreal scale) {
  if (scale != _globalFontScale) {
    _globalFontScale = scale;
    setPlotBordersDirty(true);
    setLabelsDirty();
  }
}


QColor PlotItem::globalFontColor() const {
  return _globalFontColor;
}


void PlotItem::setGlobalFontColor(const QColor &color) {
  if (color != _globalFontColor) {
    _globalFontColor = color;
    setLabelsDirty();
  }
}


QString PlotItem::leftLabel() const {
  if (!leftLabelDetails()->overrideText().isEmpty()) {
    return leftLabelDetails()->overrideText();
  } else {
    return autoLeftLabel();
  }
}


QString PlotItem::autoLeftLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->leftLabel().isEmpty())
      return renderer->leftLabel();
  }
  return QString();
}


QString PlotItem::bottomLabel() const {
  if (!bottomLabelDetails()->overrideText().isEmpty()) {
    return bottomLabelDetails()->overrideText();
  } else {
    return autoBottomLabel();
  }
}


QString PlotItem::autoBottomLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->bottomLabel().isEmpty())
      return renderer->bottomLabel();
  }
  return QString();
}


QString PlotItem::rightLabel() const {
  if (!rightLabelDetails()->overrideText().isEmpty()) {
    return rightLabelDetails()->overrideText();
  } else {
    return autoRightLabel();
  }
}


QString PlotItem::autoRightLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->rightLabel().isEmpty())
      return renderer->rightLabel();
  }
  return QString();
}


QString PlotItem::topLabel() const {
  if (!topLabelDetails()->overrideText().isEmpty()) {
    return topLabelDetails()->overrideText();
  } else {
    return autoTopLabel();
  }
}


QString PlotItem::autoTopLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    if (!renderer->topLabel().isEmpty())
      return renderer->topLabel();
  }
  return QString();
}


void PlotItem::setTopSuppressed(bool suppressed) {
  _topLabelDetails->setVisible(!suppressed);
}


void PlotItem::setRightSuppressed(bool suppressed) {
  _rightLabelDetails->setVisible(!suppressed);
}


void PlotItem::setLeftSuppressed(bool suppressed) {
  _leftLabelDetails->setVisible(!suppressed);
  _yAxis->setAxisVisible(!suppressed);
}


void PlotItem::setBottomSuppressed(bool suppressed) {
  _bottomLabelDetails->setVisible(!suppressed);
  _xAxis->setAxisVisible(!suppressed);
}


void PlotItem::setLabelsVisible(bool visible) {
  _leftLabelDetails->setVisible(visible);
  _rightLabelDetails->setVisible(visible);
  _topLabelDetails->setVisible(visible);
  _bottomLabelDetails->setVisible(visible);
  _xAxis->setAxisVisible(visible);
  _yAxis->setAxisVisible(visible);
}


qreal PlotItem::labelMarginWidth() const {
  return _calculatedLabelMarginWidth;
}


qreal PlotItem::leftLabelMargin() const {
  return _calculatedLeftLabelMargin;
}


qreal PlotItem::rightLabelMargin() const {
  return _calculatedRightLabelMargin;
}


qreal PlotItem::labelMarginHeight() const {
  return _calculatedLabelMarginHeight;
}


qreal PlotItem::topLabelMargin() const {
  return _calculatedTopLabelMargin;
}



qreal PlotItem::bottomLabelMargin() const {
  return _calculatedBottomLabelMargin;
}


QRectF PlotItem::topLabelRect() const {
  return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), topLabelMargin());
}


QRectF PlotItem::bottomLabelRect() const {
  return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), bottomLabelMargin());
}


QRectF PlotItem::leftLabelRect() const {
  return QRectF(0.0, 0.0, leftLabelMargin(), height() - topLabelMargin() - bottomLabelMargin());
}


QRectF PlotItem::rightLabelRect() const {
  return QRectF(0.0, 0.0, rightLabelMargin(), height() - topLabelMargin() - bottomLabelMargin());
}


void PlotItem::generateLeftLabel() {
  if (!_leftLabel.dirty) {
    return;
  }
  _leftLabel.valid = false;
  _leftLabel.dirty = false;
  Label::Parsed *parsed = Label::parse(leftLabel());
  if (parsed) {
    parsed->chunk->attributes.color = _leftLabelDetails->fontColor();

    QRectF leftLabel = leftLabelRect();
    QPixmap pixmap(leftLabel.height(), leftLabel.width());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(leftLabelDetails()->calculatedFont(), &pixmapPainter);
    QFontMetrics fm(leftLabelDetails()->calculatedFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    leftLabel.moveTopRight(plotAxisRect().topLeft());
    leftLabel.moveBottomLeft(QPointF(leftLabel.bottomLeft().x(), plotRect().center().y()+ rc.x / 2));

    QTransform t;
    t.rotate(-90.0);
    _leftLabel.pixmap = pixmap.transformed(t);
    _leftLabel.rect = leftLabel;
    _leftLabel.location = rc.x;
    if (rc.x > 0) {
      _leftLabel.valid = true;
    }

    delete parsed;
    parsed = 0;
  }
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!_leftLabelDetails->isVisible() || leftLabel().isEmpty())
    return;

  generateLeftLabel();

  if (_leftLabel.valid) {
    painter->save();
    painter->drawPixmap(_leftLabel.rect.topLeft(), _leftLabel.pixmap, QRectF(0, 0, _leftLabel.location, _leftLabel.rect.height()));
    painter->restore();
  }

#if DEBUG_LABEL_REGION
  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabel = leftLabelRect();
  leftLabel.moveTopRight(plotAxisRect().topLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "leftLabel:" << t.mapRect(leftLabel)<< endl;
  painter->fillRect(t.mapRect(leftLabel), Qt::red);
  painter->restore();

  painter->restore();
#endif
}


void PlotItem::calculateLeftLabelMargin(QPainter *painter) {
  if (!_leftLabelDetails->isVisible()) {
    _calculatedLeftLabelMargin = 0;
  } else {
    painter->save();
    QTransform t;
    t.rotate(90.0);
    painter->rotate(-90.0);

    painter->setFont(leftLabelDetails()->calculatedFont());
    QRectF leftLabelBound = painter->boundingRect(t.mapRect(leftLabelRect()),
        Qt::TextWordWrap | Qt::AlignCenter, leftLabel());
    painter->restore();

    _calculatedLeftLabelMargin = leftLabelBound.height();

    //No more than 1/4 the width of the plot
    if (width() < _calculatedLeftLabelMargin * 4)
      _calculatedLeftLabelMargin = width() / 4;
  }
}


void PlotItem::generateBottomLabel() {
  if (!_bottomLabel.dirty) {
    return;
  }
  _bottomLabel.valid = false;
  _bottomLabel.dirty = false;
  Label::Parsed *parsed = Label::parse(bottomLabel());
  if (parsed) {
    parsed->chunk->attributes.color = _bottomLabelDetails->fontColor();

    QRectF bottomLabel = bottomLabelRect();
    QPixmap pixmap(bottomLabel.width(), bottomLabel.height());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(bottomLabelDetails()->calculatedFont(), &pixmapPainter);
    QFontMetrics fm(bottomLabelDetails()->calculatedFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    bottomLabel.moveBottomLeft(QPointF(plotRect().center().x()-rc.x/2, rect().bottomLeft().y()));

    _bottomLabel.pixmap = pixmap;
    _bottomLabel.rect = bottomLabel;
    _bottomLabel.location = rc.x;
    if (rc.x > 0) {
      _bottomLabel.valid = true;
    }

    delete parsed;
    parsed = 0;
  }
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!_bottomLabelDetails->isVisible() || bottomLabel().isEmpty())
    return;

  generateBottomLabel();

  if (_bottomLabel.valid) {
    painter->save();
    painter->drawPixmap(_bottomLabel.rect.topLeft(), _bottomLabel.pixmap, QRectF(0, 0, _bottomLabel.location, _bottomLabel.rect.height()));
    painter->restore();
  }

#if DEBUG_LABEL_REGION
  painter->save();

  QRectF bottomLabel = bottomLabelRect();
  bottomLabel.moveTopLeft(plotAxisRect().bottomLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "bottomLabel:" << bottomLabel;
  painter->fillRect(bottomLabel, Qt::red);
  painter->restore();

  painter->restore();
#endif
}


void PlotItem::calculateBottomLabelMargin(QPainter *painter) {
  if (!_bottomLabelDetails->isVisible()) {
    _calculatedBottomLabelMargin = 0;
  } else {
    painter->save();

    painter->setFont(bottomLabelDetails()->calculatedFont());

    QRectF bottomLabelBound = painter->boundingRect(bottomLabelRect(),
        Qt::TextWordWrap | Qt::AlignCenter, bottomLabel());
    painter->restore();

    _calculatedBottomLabelMargin = bottomLabelBound.height();

    //No more than 1/4 the height of the plot
    if (height() < _calculatedBottomLabelMargin * 4)
      _calculatedLeftLabelMargin = height() / 4;
  }
}


void PlotItem::generateRightLabel() {
  if (!_rightLabel.dirty) {
    return;
  }
  _rightLabel.valid = false;
  _rightLabel.dirty = false;
  Label::Parsed *parsed = Label::parse(rightLabel());
  QRectF rightLabel = rightLabelRect();
  if (parsed && rightLabel.isValid()) {
    parsed->chunk->attributes.color = _rightLabelDetails->fontColor();

    QPixmap pixmap(rightLabel.height(), rightLabel.width());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(rightLabelDetails()->calculatedFont(), &pixmapPainter);
    QFontMetrics fm(rightLabelDetails()->calculatedFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    rightLabel.moveTopLeft(QPointF(plotAxisRect().right(), plotRect().center().y() - rc.x / 2));

    QTransform t;
    t.rotate(90.0);
    _rightLabel.pixmap = pixmap.transformed(t);
    _rightLabel.rect = rightLabel;
    _rightLabel.location = rc.x;
    if (rc.x > 0) {
      _rightLabel.valid = true;
    }

    delete parsed;
    parsed = 0;
  }
}


void PlotItem::paintRightLabel(QPainter *painter) {
  if (!_rightLabelDetails->isVisible() || rightLabel().isEmpty())
    return;

  generateRightLabel();

  if (_rightLabel.valid) {
    painter->save();
    painter->drawPixmap(_rightLabel.rect.topLeft(), _rightLabel.pixmap, QRectF(0, 0, _rightLabel.location, _rightLabel.rect.height()));
    painter->restore();
  }

#if DEBUG_LABEL_REGION
  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  QRectF rightLabel = rightLabelRect();
  rightLabel.moveTopLeft(plotAxisRect().topRight());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "rightLabel:" << t.mapRect(rightLabel)<< endl;
  painter->fillRect(t.mapRect(rightLabel), Qt::red);
  painter->restore();

  painter->restore();
#endif
}


void PlotItem::calculateRightLabelMargin(QPainter *painter) {
  if (!_rightLabelDetails->isVisible()) {
    _calculatedRightLabelMargin = 0;
  } else {
    painter->save();
    QTransform t;
    t.rotate(-90.0);
    painter->rotate(90.0);

    painter->setFont(rightLabelDetails()->calculatedFont());

    QRectF rightLabelBound = painter->boundingRect(t.mapRect(rightLabelRect()),
        Qt::TextWordWrap | Qt::AlignCenter, rightLabel());
    painter->restore();

    _calculatedRightLabelMargin = qMax(_calculatedAxisMarginROverflow, rightLabelBound.height());

    //No more than 1/4 the width of the plot
    if (width() < _calculatedRightLabelMargin * 4)
      _calculatedRightLabelMargin = width() / 4;
  }
}


void PlotItem::generateTopLabel() {
  if (!_topLabel.dirty) {
    return;
  }
  _topLabel.valid = false;
  _topLabel.dirty = false;
  Label::Parsed *parsed = Label::parse(topLabel());
  QRectF topLabel = topLabelRect();
  if (parsed && topLabel.isValid()) {
    parsed->chunk->attributes.color = _topLabelDetails->fontColor();

    QPixmap pixmap(topLabel.width(), topLabel.height());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(topLabelDetails()->calculatedFont(), &pixmapPainter);
    QFontMetrics fm(topLabelDetails()->calculatedFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    topLabel.moveBottomLeft(QPointF(plotRect().center().x()-rc.x/2, plotAxisRect().topLeft().y()));

    _topLabel.pixmap = pixmap;
    _topLabel.rect = topLabel;
    _topLabel.location = rc.x;
    if (rc.x > 0) {
      _topLabel.valid = true;
    }

    delete parsed;
    parsed = 0;
  }
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (!_topLabelDetails->isVisible() || topLabel().isEmpty())
    return;

  generateTopLabel();

  if (_topLabel.valid) {
    painter->save();
    painter->drawPixmap(_topLabel.rect.topLeft(), _topLabel.pixmap, QRectF(0, 0, _topLabel.location, _topLabel.rect.height()));
    painter->restore();
  }

#if DEBUG_LABEL_REGION
  painter->save();

  QRectF topLabel = topLabelRect();
  topLabel.moveBottomLeft(plotAxisRect().topLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "topLabel:" << topLabel;
  painter->fillRect(topLabel, Qt::red);
  painter->restore();

  painter->restore();
#endif
}


void PlotItem::calculateTopLabelMargin(QPainter *painter) {
  if (!_topLabelDetails->isVisible()) {
    _calculatedTopLabelMargin = 0;
  } else {
    painter->save();

    painter->setFont(topLabelDetails()->calculatedFont());

    QRectF topLabelBound = painter->boundingRect(topLabelRect(),
        Qt::TextWordWrap | Qt::AlignCenter, topLabel());

    painter->restore();

    _calculatedTopLabelMargin = qMax(_calculatedAxisMarginTOverflow, topLabelBound.height());

    //No more than 1/4 the height of the plot
    if (height() < _calculatedTopLabelMargin * 4)
      _calculatedTopLabelMargin = height() / 4;
  }
}


void PlotItem::calculateMargins() {
  qreal m = qMax(_calculatedLeftLabelMargin, _calculatedRightLabelMargin);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    m = width() / 4;

  _calculatedLabelMarginWidth = m;

  m = qMax(_calculatedTopLabelMargin, _calculatedBottomLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    m = height() / 4;

  _calculatedLabelMarginHeight = m;
}


qreal PlotItem::axisMarginWidth() const {
  return _calculatedAxisMarginWidth;
}


qreal PlotItem::axisMarginHeight() const {
  return _calculatedAxisMarginHeight;
}

/** This function calculates and sets three things:
      _calculatedAxisMarginVLead: spacing between bottom of plotRect and top of axis labels
      _calculatedAxisMarginROverflow: rightmost axis number extension beyond plotRect 
      _calculatedAxisMarginHeight: the height of the axis numbers
*/
void PlotItem::calculateBottomTickLabelBound(QPainter *painter) {
  qreal inHeight = _calculatedAxisMarginHeight;
  qreal inVLead = _calculatedAxisMarginVLead;
  qreal inROver = _calculatedAxisMarginROverflow;
  QRectF xLabelRect;

  painter->save();

  painter->setFont(numberLabelDetails()->calculatedFont());


  int flags = Qt::TextSingleLine | Qt::AlignCenter;

  _calculatedAxisMarginVLead = painter->fontMetrics().boundingRect('0').height()/2;

  if (_xAxis->isAxisVisible()) {
    // future potential optimization: only get bounds of the rightmost label 
    // but remember: the axis may be reversed.
    QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
    while (xLabelIt.hasNext()) {
      xLabelIt.next();

      QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
      QPointF p(mapXToPlot(xLabelIt.key()), plotRect().bottom() + bound.height() / 2.0 + _calculatedAxisMarginVLead);
      bound.moveCenter(p);

      if (xLabelRect.isValid()) {
        xLabelRect = xLabelRect.united(bound);
      } else {
        xLabelRect = bound;
      }
    }
  }
  xLabelRect.setHeight(xLabelRect.height() + _calculatedAxisMarginVLead);

  if (!_xAxis->baseLabel().isEmpty()) {
    qreal height = painter->boundingRect(QRectF(), flags, _xAxis->baseLabel()).height();
    if (bottomLabelMargin() < height) {
      xLabelRect.setHeight(xLabelRect.height() + (height - bottomLabelMargin()));
    }
  }

  _calculatedAxisMarginHeight = xLabelRect.height();
  if (xLabelRect.right() > plotRect().right()) {
    _calculatedAxisMarginROverflow = qMax(ViewItem::sizeOfGrip().width(), xLabelRect.right() - plotRect().right());
  } else {
    _calculatedAxisMarginROverflow = ViewItem::sizeOfGrip().width();
  }

  painter->restore();
}

/** This function calculates and sets three things:
      _calculatedAxisMarginHLead: spacing between left of plotRect and right of axis labels
      _calculatedAxisMarginVOverflow: topmost axis number extension beyond plotRect 
      _calculatedAxisMarginWidth: the width of the widest axis number
*/
void PlotItem::calculateLeftTickLabelBound(QPainter *painter) {
  qreal inWidth = _calculatedAxisMarginWidth;
  qreal inHLead = _calculatedAxisMarginHLead;
  qreal inTOver = _calculatedAxisMarginTOverflow;

  QRectF yLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignCenter;

  painter->save();

  painter->setFont(numberLabelDetails()->calculatedFont());

  _calculatedAxisMarginHLead = painter->fontMetrics().boundingRect('[').height()/2;

  if (_yAxis->isAxisVisible()) {

    QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
    while (yLabelIt.hasNext()) {
      yLabelIt.next();

      QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
      QPointF p(plotRect().left() - bound.width() / 2.0 - _calculatedAxisMarginHLead, mapYToPlot(yLabelIt.key()));
      bound.moveCenter(p);

      if (yLabelRect.isValid()) {
        yLabelRect = yLabelRect.united(bound);
      } else {
        yLabelRect = bound;
      }
    }
  }

  yLabelRect.setWidth(yLabelRect.width() + _calculatedAxisMarginHLead);
  if (!_yAxis->baseLabel().isEmpty()) {
    qreal height = painter->boundingRect(QRectF(), flags, _yAxis->baseLabel()).height();
    if (leftLabelMargin() < height) {
      yLabelRect.setWidth(yLabelRect.width() + (height - leftLabelMargin()));
    }
  }
  _calculatedAxisMarginWidth = yLabelRect.width();
  if (yLabelRect.top() < plotRect().top()) {
    _calculatedAxisMarginTOverflow = qMax(ViewItem::sizeOfGrip().width(), -yLabelRect.top() + plotRect().top());
  } else {
    _calculatedAxisMarginTOverflow = ViewItem::sizeOfGrip().width();
  }
  painter->restore();
}


bool PlotItem::showLegend() const {
  return _showLegend;
}


LegendItem* PlotItem::legend() {
  if (!_legend) {
    _legend = new LegendItem(this);
    _legend->setVisible(false);
  }
  return _legend;
}


void PlotItem::setShowLegend(const bool show) {
  if (show != _showLegend) {
    legend()->setVisible(show);
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
    setPlotBordersDirty(true);
    emit updateAxes();
    update(); //slow, but need to update everything...
  }
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


void PlotItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (isInSharedAxisBox() && _sharedBox) {
    if (_sharedBox->tryMousePressEvent(this, event)) {
      return;
    }
  }
  if (event->button() == Qt::LeftButton) {
    if (checkBox().contains(event->pos())) {
      setTiedZoom(!isTiedZoom());
    } else if (parentView()->viewMode() == View::Data) {
      edit();
    } else {
      ViewItem::mousePressEvent(event);
    }
  } 
}


void PlotItem::setAllowUpdates(bool allowed) {
  if (allowed == _allowUpdates)
    return;

  _allowUpdates = allowed;
  if (_allowUpdates) {
    if (_updateDelayed) {
      _updateDelayed = false;
      updateObject();
    }
  }
}


void PlotItem::plotMaximize() {
  if (!_plotMaximized && parentView()->viewMode() != View::Data) {
    return;
  }

  if (_plotMaximized) {
    _plotMaximized = false;
    PlotItemManager::self()->removeFocusPlot(this);
    setParent(_plotMaximizedSourceParent);
    setPos(_plotMaximizedSourcePosition);
    setViewRect(_plotMaximizedSourceRect);
    setZValue(_plotMaximizedSourceZValue);
  } else {
    _plotMaximized = true;
    _plotMaximizedSourcePosition = pos();
    _plotMaximizedSourceRect = viewRect();
    _plotMaximizedSourceZValue = zValue();
    _plotMaximizedSourceParent = parentViewItem();

    setParent(0);
    setPos(0, 0);
    setViewRect(parentView()->sceneRect());
    setZValue(PLOT_MAXIMIZED_ZORDER);
    PlotItemManager::self()->setFocusPlot(this);
  }
}


void PlotItem::zoomFixedExpression(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomFixedExpression" << projection << "current" << projectionRect();
#endif
  if (projection.isValid()) {
    ZoomCommand *cmd = new ZoomFixedExpressionCommand(this, projection);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXRange(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRange" << projection << endl;
#endif
  if (projection.isValid()) {
    ZoomCommand *cmd = new ZoomXRangeCommand(this, projection);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYRange(const QRectF &projection) {
#if DEBUG_ZOOM
  qDebug() << "zoomYRange" << projection << endl;
#endif
  if (projection.isValid()) {
    ZoomCommand *cmd = new ZoomYRangeCommand(this, projection);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomGeneral(ZoomState &zoomstate) {
  ZoomCommand *cmd = new ZoomGeneralCommand(this, zoomstate);
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


QString PlotItem::descriptionTip() const {
  QString contents;
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      contents += QString("  %1\n").arg(relation->Name());
    }
  }

  return i18n("Plot: %1 \nContents:\n %2").arg(Name()).arg(contents);
}


QString PlotItem::_automaticDescriptiveName() const {
  QString name = i18n("Empty Plot");
  int n=0;
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (n==0) {
        name = relation->descriptiveName();
      }
      n++;
    }
  }
  if (n>1) {
    name += ", ...";
  }
  return name;
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


void PlotItem::updateChildGeometry(const QRectF &oldParentRect, const QRectF &newParentRect) {
  ViewItem::updateChildGeometry(oldParentRect, newParentRect);
  setPlotBordersDirty(true);
}


PlotLabel::PlotLabel(PlotItem *plotItem) : QObject(),
  _plotItem(plotItem),
  _visible(true),
  _fontUseGlobal(true) {

  _font = _plotItem->parentView()->defaultFont();
  _fontColor = ApplicationSettings::self()->defaultFontColor();
  _fontScale = ApplicationSettings::self()->defaultFontScale();
}


void PlotLabel::setDetails(const QString &label, 
                           const bool use_global, const QFont &font, 
                           const qreal scale, const QColor &color) {
  if ((label != _overrideText) ||
      (use_global != _fontUseGlobal) ||
      (font != _font) ||
      (scale != _fontScale) ||
      (color != _fontColor)) {
    _overrideText = label;
    _fontUseGlobal = use_global;
    _font = font;
    _fontScale = scale;
    _fontColor = color;
    emit labelChanged();
  }
}


QFont PlotLabel::calculatedFont() {
  QFont tempFont;
  if (fontUseGlobal()) {
    tempFont = _plotItem->globalFont();
    tempFont.setPixelSize(_plotItem->parentView()->defaultFont(_plotItem->globalFontScale()).pixelSize());
  } else {
    tempFont = font();
    tempFont.setPixelSize(_plotItem->parentView()->defaultFont(fontScale()).pixelSize());
  }
  return tempFont;
}


bool PlotLabel::isVisible() const {
  return _visible;
}


void PlotLabel::setVisible(bool visible) {
  if (_visible == visible)
    return;

  _visible = visible;
  emit labelChanged();
}


bool PlotLabel::fontUseGlobal() const {
  return _fontUseGlobal; 
}


void PlotLabel::setFontUseGlobal(const bool use_global) {
  if (_fontUseGlobal == use_global)
    return;

  _fontUseGlobal = use_global;
  emit labelChanged();
}


QString PlotLabel::overrideText() const {
  return _overrideText;
}


void PlotLabel::setOverrideText(const QString &label) {
  if (label == _overrideText) {
    return;
  }
  _overrideText = label;
  emit labelChanged();
}


qreal PlotLabel::fontScale() const {
  return _fontScale;
}


void PlotLabel::setFontScale(const qreal scale) {
  if (scale != _fontScale) {
    _fontScale = scale;
    emit labelChanged();
  }
}


QFont PlotLabel::font() const {
  return _font;
}


void PlotLabel::setFont(const QFont &font) {
  if (font != _font) {
    _font = font;
    emit labelChanged();
  }
}


QColor PlotLabel::fontColor() const {
  return _fontColor;
}


void PlotLabel::setFontColor(const QColor &color) {
  if (color != _fontColor) {
    _fontColor = color;
    emit labelChanged();
  }
}


void PlotLabel::saveInPlot(QXmlStreamWriter &xml, QString labelId) {
  xml.writeStartElement("plotlabel");
  xml.writeAttribute("id", labelId);
  xml.writeAttribute("visible", QVariant(_visible).toString());
  xml.writeAttribute("overridetext", _overrideText);
  xml.writeAttribute("font", QVariant(_font).toString());
  xml.writeAttribute("fontscale", QVariant(_fontScale).toString());
  xml.writeAttribute("fontcolor", QVariant(_fontColor).toString());
  xml.writeAttribute("fontuseglobal", QVariant(_fontUseGlobal).toString());
  xml.writeEndElement();
}


bool PlotLabel::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av = attrs.value("visible");
  if (!av.isNull()) {
    setVisible(QVariant(av.toString()).toBool());
  }
  av = attrs.value("overridetext");
  if (!av.isNull()) {
    setOverrideText(av.toString());
  }
  av = attrs.value("fontuseglobal");
  if (!av.isNull()) {
    setFontUseGlobal(QVariant(av.toString()).toBool());
  }
  av = attrs.value("font");
  if (!av.isNull()) {
    QFont font;
    font.fromString(av.toString());
    setFont(font);
  }
  av = attrs.value("fontcolor");
  if (!av.isNull()) {
    setFontColor(QColor(av.toString()));
  }
  av = attrs.value("fontscale");
  if (!av.isNull()) {
    setFontScale(QVariant(av.toString()).toDouble());
  }

  QString expectedEnd;
  while (!(xml.isEndElement() && (xml.name().toString() == primaryTag))) {
    if (xml.isEndElement()) {
      if (xml.name().toString() != expectedEnd) {
        validTag = false;
        break;
      }
    }
    xml.readNext();
  }

  return validTag;
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
      if (!rc && xml.name().toString() == "plot") {
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
          rc->leftLabelDetails()->setVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("bottomlabelvisible");
        if (!av.isNull()) {
          rc->bottomLabelDetails()->setVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("rightlabelvisible");
        if (!av.isNull()) {
          rc->rightLabelDetails()->setVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("toplabelvisible");
        if (!av.isNull()) {
          rc->topLabelDetails()->setVisible(QVariant(av.toString()).toBool());
        }
        av = attrs.value("globalfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setGlobalFont(font);
        }
        av = attrs.value("globalfontscale");
        if (!av.isNull()) {
          rc->setGlobalFontScale(QVariant(av.toString()).toDouble());
        }
        av = attrs.value("globalfontcolor");
        if (!av.isNull()) {
          rc->setGlobalFontColor(QColor(av.toString()));
        }
        av = attrs.value("showlegend");
        if (!av.isNull()) {
          rc->setShowLegend(QVariant(av.toString()).toBool());
        }
        if (attrs.value("descriptiveNameIsManual").toString() == "true") {
          rc->setDescriptiveName(attrs.value("descriptiveName").toString());
        }
        Object::processShortNameIndexAttributes(attrs);

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
      } else if (xml.name().toString() == "plotlabel") {
        Q_ASSERT(rc);
        QXmlStreamAttributes subattrs = xml.attributes();
        QStringRef av = subattrs.value("id");
        if (!av.isNull()) {
          if (av == "leftlabel") {
            rc->leftLabelDetails()->configureFromXml(xml, store);
          } else if (av == "rightlabel") {
            rc->rightLabelDetails()->configureFromXml(xml, store);
          } else if (av == "toplabel") {
            rc->topLabelDetails()->configureFromXml(xml, store);
          } else if (av == "bottomlabel") {
            rc->bottomLabelDetails()->configureFromXml(xml, store);
          } else if (av == "numberlabel") {
            rc->numberLabelDetails()->configureFromXml(xml, store);         }
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
    QList<PlotItem*> plots = PlotItemManager::tiedZoomPlots(item);
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
  kstApp->mainWindow()->document()->setChanged(true);
}


void ZoomCommand::redo() {
  foreach (ZoomState state, _originalStates) {
    applyZoomTo(state.item);
  }

  kstApp->mainWindow()->document()->setChanged(true);
}

/* 
 * Set zoom to arbitrary state: used by the dialog
 */
void ZoomGeneralCommand::applyZoomTo(PlotItem *item) {
  item->xAxis()->setAxisZoomMode((PlotAxis::ZoomMode)_zoomstate.xAxisZoomMode);
  item->yAxis()->setAxisZoomMode((PlotAxis::ZoomMode)_zoomstate.yAxisZoomMode);
  item->setProjectionRect(_zoomstate.projectionRect);
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
  item->setProjectionRect(QRectF(_fixed.x(), item->projectionRect().y(), _fixed.width(), item->projectionRect().height()));
}


/*
 * Y axis zoom to Range.
 */
void ZoomYRangeCommand::applyZoomTo(PlotItem *item) {
  item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
  item->setProjectionRect(QRectF(item->projectionRect().x(), _fixed.y(), item->projectionRect().width(), _fixed.height()));
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
