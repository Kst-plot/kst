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
#include "document.h"
#include "tabwidget.h"
#include "labelrenderer.h"

#include "datacollection.h"
#include "cartesianrenderitem.h"

#include "plotitemdialog.h"
#include "dialoglauncher.h"
#include "sharedaxisboxitem.h"

#include "applicationsettings.h"

#include "math_kst.h"

#include "settings.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

// Zoom Debugging.  0 Off, 1 On.
#define DEBUG_ZOOM 0

// Label Region Debugging.  0 Off, 1 On.
#define DEBUG_LABEL_REGION 0

// FIXME:no magic numbers in pixels
static qreal BOTTOM_MARGIN = 0.0;
static qreal LEFT_MARGIN = 0.0;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent),
  _isInSharedAxisBox(false),
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
  _zoomMenu(0),
  _filterMenu(0),
  _fitMenu(0)
{

  setName("Plot");
  setZValue(PLOT_ZVALUE);
  setBrush(Qt::white);

  setSupportsTiedZoom(true);

  _xAxis = new PlotAxis(this, Qt::Horizontal);
  _yAxis = new PlotAxis(this, Qt::Vertical);

  _globalFont = parentView()->defaultFont();
  _leftLabelFont = parentView()->defaultFont();
  _bottomLabelFont = parentView()->defaultFont();
  _topLabelFont = parentView()->defaultFont();
  _rightLabelFont = parentView()->defaultFont();
  _numberLabelFont = parentView()->defaultFont();

  _globalFontColor = ApplicationSettings::self()->defaultFontColor();
  _leftLabelFontColor = ApplicationSettings::self()->defaultFontColor();
  _bottomLabelFontColor = ApplicationSettings::self()->defaultFontColor();
  _topLabelFontColor = ApplicationSettings::self()->defaultFontColor();
  _rightLabelFontColor = ApplicationSettings::self()->defaultFontColor();
  _numberLabelFontColor = ApplicationSettings::self()->defaultFontColor();

  _globalFontScale = ApplicationSettings::self()->defaultFontScale();
  _leftLabelFontScale = ApplicationSettings::self()->defaultFontScale();
  _bottomLabelFontScale = ApplicationSettings::self()->defaultFontScale();
  _topLabelFontScale = ApplicationSettings::self()->defaultFontScale();
  _rightLabelFontScale = ApplicationSettings::self()->defaultFontScale();
  _numberLabelFontScale = ApplicationSettings::self()->defaultFontScale();

  _leftFontUseGlobal = true;
  _rightFontUseGlobal = true;
  _topFontUseGlobal = true;
  _bottomFontUseGlobal = true;
  _numberFontUseGlobal = true;

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

  parentView()->setPlotBordersDirty(true);

  connect(this, SIGNAL(marginsChanged()), this, SLOT(marginsUpdated()));
}


PlotItem::~PlotItem() {
  delete _xAxis;
  delete _yAxis;
  PlotItemManager::self()->removePlot(this);
}


QString PlotItem::plotName() const {
  return Name();
}


void PlotItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("plot");
    xml.writeAttribute("tiedzoom", QVariant(isTiedZoom()).toString());
    xml.writeAttribute("leftlabelvisible", QVariant(_isLeftLabelVisible).toString());
    xml.writeAttribute("bottomlabelvisible", QVariant(_isBottomLabelVisible).toString());
    xml.writeAttribute("rightlabelvisible", QVariant(_isRightLabelVisible).toString());
    xml.writeAttribute("toplabelvisible", QVariant(_isTopLabelVisible).toString());
    xml.writeAttribute("globalfont", QVariant(_globalFont).toString());
    xml.writeAttribute("globalfontscale", QVariant(_globalFontScale).toString());
    xml.writeAttribute("globalfontcolor", QVariant(_globalFontColor).toString());
    xml.writeAttribute("leftlabeloverride", _leftLabelOverride);
    xml.writeAttribute("leftlabeluseglobal", QVariant(_leftFontUseGlobal).toString());
    xml.writeAttribute("leftlabelfont", QVariant(_leftLabelFont).toString());
    xml.writeAttribute("leftlabelfontscale", QVariant(_leftLabelFontScale).toString());
    xml.writeAttribute("leftlabelfontcolor", QVariant(_leftLabelFontColor).toString());
    xml.writeAttribute("bottomlabeloverride", _bottomLabelOverride);
    xml.writeAttribute("bottomlabeluseglobal", QVariant(_bottomFontUseGlobal).toString());
    xml.writeAttribute("bottomlabelfont", QVariant(_bottomLabelFont).toString());
    xml.writeAttribute("bottomlabelfontscale", QVariant(_bottomLabelFontScale).toString());
    xml.writeAttribute("bottomlabelfontcolor", QVariant(_bottomLabelFontColor).toString());
    xml.writeAttribute("toplabeloverride", _topLabelOverride);
    xml.writeAttribute("toplabeluseglobal", QVariant(_topFontUseGlobal).toString());
    xml.writeAttribute("toplabelfont", QVariant(_topLabelFont).toString());
    xml.writeAttribute("toplabelfontscale", QVariant(_topLabelFontScale).toString());
    xml.writeAttribute("toplabelfontcolor", QVariant(_topLabelFontColor).toString());
    xml.writeAttribute("rightlabeloverride", _rightLabelOverride);
    xml.writeAttribute("rightlabeluseglobal", QVariant(_rightFontUseGlobal).toString());
    xml.writeAttribute("rightlabelfont", QVariant(_rightLabelFont).toString());
    xml.writeAttribute("rightlabelfontscale", QVariant(_rightLabelFontScale).toString());
    xml.writeAttribute("rightlabelfontcolor", QVariant(_rightLabelFontColor).toString());
    xml.writeAttribute("numberlabelfont", QVariant(_numberLabelFont).toString());
    xml.writeAttribute("numberlabeluseglobal", QVariant(_numberFontUseGlobal).toString());
    xml.writeAttribute("numberlabelfontscale", QVariant(_numberLabelFontScale).toString());
    xml.writeAttribute("numberlabelfontcolor", QVariant(_numberLabelFontColor).toString());
    xml.writeAttribute("showlegend", QVariant(_showLegend).toString());
    saveNameInfo(xml, GNUM);

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
  if (parentItem()) {
    if (SharedAxisBoxItem *sharedBox = qgraphicsitem_cast<SharedAxisBoxItem*>(parentItem())) {
      if (parentView()->viewMode() == View::Data) {
        QAction *breakSharedBox = new QAction(tr("Break Shared Axis Box"), this);
        breakSharedBox->setShortcut(Qt::Key_B);
        connect(breakSharedBox, SIGNAL(triggered()), sharedBox, SLOT(breakShare()));
        menu.addAction(breakSharedBox);
      }
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


void PlotItem::updateObject() {
  if (!_allowUpdates) {
    _updateDelayed = true;
    return;
  }
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "UP - Updating Plot";
#endif
  if (xAxis()->axisZoomMode() == PlotAxis::Auto) {
    if (yAxis()->axisZoomMode() == PlotAxis::AutoBorder || yAxis()->axisZoomMode() == PlotAxis::Auto
        || yAxis()->axisZoomMode() == PlotAxis::SpikeInsensitive || yAxis()->axisZoomMode() == PlotAxis::MeanCentered) {
#if DEBUG_UPDATE_CYCLE > 1
      qDebug() << "UP - Updating Plot Projection Rect - X and Y Maximum";
#endif
      setProjectionRect(computedProjectionRect());
    } else {
#if DEBUG_UPDATE_CYCLE > 1
      qDebug() << "UP - Updating Plot Projection Rect - X Maximum";
#endif
      QRectF compute = computedProjectionRect();
      setProjectionRect(QRectF(compute.x(),
            projectionRect().y(),
            compute.width(),
            projectionRect().height()));
    }
  } else if (yAxis()->axisZoomMode() == PlotAxis::Auto) {
#if DEBUG_UPDATE_CYCLE > 1
    qDebug() << "UP - Updating Plot Projection Rect - Y Maximum";
#endif
    QRectF compute = computedProjectionRect();
    setProjectionRect(QRectF(projectionRect().x(),
          compute.y(),
          projectionRect().width(),
          compute.height()));
  }
  update();
}


void PlotItem::marginsUpdated() {
  //ViewGridLayout::standardizePlotMargins(this);
  //qDebug() << "---Margins updated called";
  if (isInSharedAxisBox() && parentItem()) {
    if (SharedAxisBoxItem *sharedBox = qgraphicsitem_cast<SharedAxisBoxItem*>(parentItem())) {
      sharedBox->sharePlots();
    }
  }
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

  setCalculatedLeftLabelMargin(calculateLeftLabelBound(painter).width());
  setCalculatedRightLabelMargin(calculateRightLabelBound(painter).width());
  setCalculatedTopLabelMargin(calculateTopLabelBound(painter).height());
  setCalculatedBottomLabelMargin(calculateBottomLabelBound(painter).height());
}


void PlotItem::paint(QPainter *painter) {
  painter->save();
  painter->setPen(Qt::NoPen);
  painter->drawRect(rect());
  painter->restore();

  painter->save();
  painter->setFont(calculatedNumberLabelFont());

  // FIXME: the plot size calculations need to be separated from the 
  // painting to avoid n^2 or worse behavior.

  if (parentView()->plotBordersDirty() || (creationState() == ViewItem::InProgress)) {
    ViewGridLayout::standardizePlotMargins(this, painter);
    parentView()->setPlotBordersDirty(false);
  }

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

  painter->save();
  painter->setPen(_numberLabelFontColor);

  QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
  while (xLabelIt.hasNext()) {
    xLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
    bound.setWidth(bound.width());
    QPointF p = QPointF(mapXToPlot(xLabelIt.key()), plotRect().bottom() + 
        bound.height()*0.5 + _calculatedAxisMarginVLead);
    bound.moveCenter(p);
    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    if (rect().left() > bound.left()) bound.setLeft(rect().left());
    if (rect().right() < bound.right()) bound.setRight(rect().right());

    painter->drawText(bound, flags, xLabelIt.value());
  }

  if (!_xAxis->baseLabel().isEmpty()) {
    QRectF bound = painter->boundingRect(QRectF(), flags, _xAxis->baseLabel());
    QPointF p = QPointF(plotRect().left(), plotRect().bottom() + bound.height() * 2.0 + _calculatedAxisMarginVLead);
    bound.moveBottomLeft(p);

    if (xLabelRect.isValid()) {
      xLabelRect = xLabelRect.united(bound);
    } else {
      xLabelRect = bound;
    }

    painter->drawText(bound, flags, _xAxis->baseLabel());
  }
  _xLabelRect = xLabelRect;
  painter->restore();

#if DEBUG_LABEL_REGION
  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "Bottom Tick Labels - xLabelRect:" << xLabelRect;
  painter->fillRect(xLabelRect, Qt::green);
  painter->restore();
#endif
}


void PlotItem::paintLeftTickLabels(QPainter *painter) {

  QRectF yLabelRect;
  int flags = Qt::TextSingleLine | Qt::AlignVCenter;

  painter->save();
  painter->setPen(_numberLabelFontColor);
  QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
  while (yLabelIt.hasNext()) {
    yLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
    bound.setWidth(bound.width());
    QPointF p = QPointF(plotRect().left() - (bound.width() / 2.0) - _calculatedAxisMarginHLead, mapYToPlot(yLabelIt.key()));
    bound.moveCenter(p);

    if (yLabelRect.isValid()) {
      yLabelRect = yLabelRect.united(bound);
    } else {
      yLabelRect = bound;
    }

    if (rect().top() > bound.top()) bound.setTop(rect().top());
    if (rect().bottom() < bound.bottom()) bound.setBottom(rect().bottom());

    painter->drawText(bound, flags, yLabelIt.value());
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
  painter->restore();

#if DEBUG_LABEL_REGION
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
  qreal right = isRightLabelVisible() ? rightMarginSize() : 0.0;
  qreal top = isTopLabelVisible() ? topMarginSize() : 0.0;

  QPointF topLeft(rect().topLeft() + QPointF(left, top));
  QPointF bottomRight(rect().bottomRight() - QPointF(right, bottom));

  return QRectF(topLeft, bottomRight);
}


QRectF PlotItem::plotRect() const {
  //the PlotRenderItems use this to set their rects
  QRectF plot = plotAxisRect();
  qreal xOffset = _xAxis->isAxisVisible() ? axisMarginHeight() : 0.0;
  qreal yOffset = _yAxis->isAxisVisible() ? axisMarginWidth() : 0.0;
  qreal bottomPadding = _xAxis->isAxisVisible() ? _bottomPadding : 0.0;
  qreal leftPadding = _yAxis->isAxisVisible() ? _leftPadding : 0.0;
  qreal rightPadding = isRightLabelVisible() ? _rightPadding : 0.0;
  qreal topPadding = isTopLabelVisible() ? _topPadding : 0.0;

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
  return plot;
}


qreal PlotItem::leftMarginSize() const {
  qreal margin = isLeftLabelVisible() ? leftLabelMargin() : 0.0;
  margin += _yAxis->isAxisVisible() ? axisMarginWidth() : 0.0;

  return margin;
}


qreal PlotItem::bottomMarginSize() const {
  qreal margin = isBottomLabelVisible() ? bottomLabelMargin() : 0.0;
  margin += _xAxis->isAxisVisible() ? axisMarginHeight() : 0.0;

  return margin;
}


qreal PlotItem::rightMarginSize() const {
  qreal margin = isRightLabelVisible() ? rightLabelMargin() : 0.0;
  if (supportsTiedZoom() && margin < tiedZoomSize().width()) margin = tiedZoomSize().width();
  return margin;
}


qreal PlotItem::topMarginSize() const {
  qreal margin = isTopLabelVisible() ? topLabelMargin() : 0.0;
  if (supportsTiedZoom() && margin < tiedZoomSize().height()) margin = tiedZoomSize().height();
  return margin;
}


void PlotItem::setLeftPadding(const qreal padding) {
  _leftPadding = padding;
}


void PlotItem::setBottomPadding(const qreal padding) {
  _bottomPadding = padding;
}


void PlotItem::setRightPadding(const qreal padding) {
  _rightPadding = padding;
}


void PlotItem::setTopPadding(const qreal padding) {
  _topPadding = padding;
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


void PlotItem::setSharedAxisBox(ViewItem* parent) {
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
  parentView()->setPlotBordersDirty(true);
}


QFont PlotItem::topLabelFont() const {
  return _topLabelFont;
}


void PlotItem::setTopLabelFont(const QFont &font) {
  _topLabelFont = font;
  parentView()->setPlotBordersDirty(true);
}


QFont PlotItem::globalFont() const {
  return _globalFont;
}


void PlotItem::setGlobalFont(const QFont &font) {
  _globalFont = font;
}


QFont PlotItem::leftLabelFont() const {
  return _leftLabelFont;
}


void PlotItem::setLeftLabelFont(const QFont &font) {
  _leftLabelFont = font;
  parentView()->setPlotBordersDirty(true);
}


QFont PlotItem::bottomLabelFont() const {
  return _bottomLabelFont;
}


void PlotItem::setBottomLabelFont(const QFont &font) {
  _bottomLabelFont = font;
  parentView()->setPlotBordersDirty(true);
}


QFont PlotItem::numberLabelFont() const {
  return _numberLabelFont;
}


void PlotItem::setNumberLabelFont(const QFont &font) {
  _numberLabelFont = font;
  parentView()->setPlotBordersDirty(true);
}


qreal PlotItem::rightLabelFontScale() const {
  return _rightLabelFontScale;
}


void PlotItem::setRightLabelFontScale(const qreal scale) {
  _rightLabelFontScale = scale;
  parentView()->setPlotBordersDirty(true);
}


qreal PlotItem::globalFontScale() const {
  return _globalFontScale;
}


void PlotItem::setGlobalFontScale(const qreal scale) {
  _globalFontScale = scale;
}


qreal PlotItem::leftLabelFontScale() const {
  return _leftLabelFontScale;
}


void PlotItem::setLeftLabelFontScale(const qreal scale) {
  _leftLabelFontScale = scale;
  parentView()->setPlotBordersDirty(true);
}


qreal PlotItem::topLabelFontScale() const {
  return _topLabelFontScale;
}


void PlotItem::setTopLabelFontScale(const qreal scale) {
  _topLabelFontScale = scale;
  parentView()->setPlotBordersDirty(true);
}


qreal PlotItem::bottomLabelFontScale() const {
  return _bottomLabelFontScale;
}


void PlotItem::setBottomLabelFontScale(const qreal scale) {
  _bottomLabelFontScale = scale;
  parentView()->setPlotBordersDirty(true);
}


qreal PlotItem::numberLabelFontScale() const {
  return _numberLabelFontScale;
}


void PlotItem::setNumberLabelFontScale(const qreal scale) {
  _numberLabelFontScale = scale;
  parentView()->setPlotBordersDirty(true);
}


QColor PlotItem::globalFontColor() const {
  return _globalFontColor;
}


void PlotItem::setGlobalFontColor(const QColor &color) {
  _globalFontColor = color;
}


QColor PlotItem::leftLabelFontColor() const {
  return _leftLabelFontColor;
}


void PlotItem::setLeftLabelFontColor(const QColor &color) {
  _leftLabelFontColor = color;
}


QColor PlotItem::rightLabelFontColor() const {
  return _rightLabelFontColor;
}


void PlotItem::setRightLabelFontColor(const QColor &color) {
  _rightLabelFontColor = color;
}


QColor PlotItem::topLabelFontColor() const {
  return _topLabelFontColor;
}


void PlotItem::setTopLabelFontColor(const QColor &color) {
  _topLabelFontColor = color;
}


QColor PlotItem::bottomLabelFontColor() const {
  return _bottomLabelFontColor;
}


void PlotItem::setBottomLabelFontColor(const QColor &color) {
  _bottomLabelFontColor = color;
}


QColor PlotItem::numberLabelFontColor() const {
  return _numberLabelFontColor;
}


void PlotItem::setNumberLabelFontColor(const QColor &color) {
  _numberLabelFontColor = color;
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
  parentView()->setPlotBordersDirty(true);
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
  parentView()->setPlotBordersDirty(true);
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
  parentView()->setPlotBordersDirty(true);
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
  parentView()->setPlotBordersDirty(true);
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
  parentView()->setPlotBordersDirty(true);
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
  parentView()->setPlotBordersDirty(true);
//   emit marginsChanged();
}


bool PlotItem::isBottomLabelVisible() const {
  return _isBottomLabelVisible;
}


void PlotItem::setBottomLabelVisible(bool visible) {
  if (_isBottomLabelVisible == visible)
    return;

  _isBottomLabelVisible = visible;
  parentView()->setPlotBordersDirty(true);
//   emit marginsChanged();
}


bool PlotItem::isRightLabelVisible() const {
  return _isRightLabelVisible;
}


void PlotItem::setRightLabelVisible(bool visible) {
  if (_isRightLabelVisible == visible)
    return;

  _isRightLabelVisible = visible;
  parentView()->setPlotBordersDirty(true);
//   emit marginsChanged();
}


bool PlotItem::isTopLabelVisible() const {
  return _isTopLabelVisible;
}


void PlotItem::setTopLabelVisible(bool visible) {
  if (_isTopLabelVisible == visible)
    return;

  _isTopLabelVisible = visible;
  parentView()->setPlotBordersDirty(true);
//   emit marginsChanged();
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
/*  if (before != this->calculatedLeftLabelMargin()) {
    emit marginsChanged();
  }*/
}


qreal PlotItem::calculatedRightLabelMargin() const {
  qreal m = qMax(_calculatedAxisMarginROverflow, _calculatedRightLabelMargin);

  //No more than 1/4 the width of the plot
  if (width() < m * 4)
    return width() / 4;

  return m;
}


void PlotItem::setCalculatedRightLabelMargin(qreal margin) {
  qreal before = this->calculatedRightLabelMargin();
  _calculatedRightLabelMargin = margin;
/*  if (before != this->calculatedRightLabelMargin()) {
    emit marginsChanged();
  }*/
}


qreal PlotItem::calculatedLabelMarginHeight() const {
  qreal m = qMax(_calculatedTopLabelMargin, _calculatedBottomLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


qreal PlotItem::calculatedTopLabelMargin() const {
  qreal m = qMax(_calculatedAxisMarginTOverflow, _calculatedTopLabelMargin);

  //No more than 1/4 the height of the plot
  if (height() < m * 4)
    return height() / 4;

  return m;
}


void PlotItem::setCalculatedTopLabelMargin(qreal margin) {
  qreal before = this->calculatedTopLabelMargin();
  _calculatedTopLabelMargin = margin;
/*  if (before != this->calculatedTopLabelMargin()) {
    emit marginsChanged();
  }*/
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
/*  if (before != this->calculatedBottomLabelMargin()) {
    emit marginsChanged();
  }*/
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
  QFont font;
  if (leftFontUseGlobal()) {
    font = _globalFont;
    font.setPixelSize(parentView()->defaultFont(_globalFontScale).pixelSize());
  } else {
    font = _leftLabelFont;
    font.setPixelSize(parentView()->defaultFont(_leftLabelFontScale).pixelSize());
  }
  return font;
}


QFont PlotItem::calculatedRightLabelFont() {
  QFont font;
  if (rightFontUseGlobal()) {
    font = _globalFont;
    font.setPixelSize(parentView()->defaultFont(_globalFontScale).pixelSize());
  } else {
    font = _rightLabelFont;
    font.setPixelSize(parentView()->defaultFont(_rightLabelFontScale).pixelSize());
  }
  return font;
}


QFont PlotItem::calculatedTopLabelFont() {
  QFont font;
  if (topFontUseGlobal()) {
    font = _globalFont;
    font.setPixelSize(parentView()->defaultFont(_globalFontScale).pixelSize());
  } else {
    font = _topLabelFont;
    font.setPixelSize(parentView()->defaultFont(_topLabelFontScale).pixelSize());
  }
  return font;
}


QFont PlotItem::calculatedBottomLabelFont() {
  QFont font;
  if (bottomFontUseGlobal()) {
    font = _globalFont;
    font.setPixelSize(parentView()->defaultFont(_globalFontScale).pixelSize());
  } else {
    font = _bottomLabelFont;
    font.setPixelSize(parentView()->defaultFont(_bottomLabelFontScale).pixelSize());
  }
  return font;
}


QFont PlotItem::calculatedNumberLabelFont() {
  QFont font;
  if (numberFontUseGlobal()) {
    font = _globalFont;
    font.setPixelSize(parentView()->defaultFont(_globalFontScale).pixelSize());
  } else {
    font = _numberLabelFont;
    font.setPixelSize(parentView()->defaultFont(_numberLabelFontScale).pixelSize());
  }
  return font;
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!isLeftLabelVisible() || leftLabelOverride().isEmpty())
    return;

  Label::Parsed *parsed = Label::parse(leftLabelOverride());
  if (parsed) {

    parsed->chunk->attributes.color = _leftLabelFontColor;

    QRectF leftLabel = leftLabelRect(false);
    QPixmap pixmap(leftLabel.height(), leftLabel.width());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(calculatedLeftLabelFont(), &pixmapPainter);
    QFontMetrics fm(calculatedLeftLabelFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    leftLabel.moveTopRight(plotAxisRect().topLeft());
    leftLabel.moveBottomLeft(QPointF(leftLabel.bottomLeft().x(), plotRect().center().y()+ rc.x / 2));

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

#if DEBUG_LABEL_REGION
  painter->save();
  QTransform t;
  t.rotate(90.0);
  painter->rotate(-90.0);

  QRectF leftLabel = leftLabelRect(false);
  leftLabel.moveTopRight(plotAxisRect().topLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "leftLabel:" << t.mapRect(leftLabel)<< endl;
  painter->fillRect(t.mapRect(leftLabel), Qt::red);
  painter->restore();

  painter->restore();
#endif
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

    parsed->chunk->attributes.color = _bottomLabelFontColor;

    QRectF bottomLabel = bottomLabelRect(false);
    QPixmap pixmap(bottomLabel.width(), bottomLabel.height());
    pixmap.fill(Qt::transparent);
    QPainter pixmapPainter(&pixmap);

    Label::RenderContext rc(calculatedBottomLabelFont(), &pixmapPainter);
    QFontMetrics fm(calculatedBottomLabelFont());
    rc.y = fm.ascent();
    Label::renderLabel(rc, parsed->chunk);

    bottomLabel.moveBottomLeft(QPointF(plotRect().center().x()-rc.x/2, rect().bottomLeft().y()));

    if (rc.x > 0)
      painter->drawPixmap(bottomLabel.topLeft(), pixmap, QRectF(0, 0, rc.x, bottomLabel.height()));

    painter->restore();
    delete parsed;
    parsed = 0;
  }

#if DEBUG_LABEL_REGION
  painter->save();

  QRectF bottomLabel = bottomLabelRect(false);
  bottomLabel.moveTopLeft(plotAxisRect().bottomLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "bottomLabel:" << bottomLabel;
  painter->fillRect(bottomLabel, Qt::red);
  painter->restore();

  painter->restore();
#endif
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

    parsed->chunk->attributes.color = _rightLabelFontColor;

    if (rightLabel.isValid()) {
      QPixmap pixmap(rightLabel.height(), rightLabel.width());
      pixmap.fill(Qt::transparent);
      QPainter pixmapPainter(&pixmap);

      Label::RenderContext rc(calculatedRightLabelFont(), &pixmapPainter);
      QFontMetrics fm(calculatedRightLabelFont());
      rc.y = fm.ascent();
      Label::renderLabel(rc, parsed->chunk);

      rightLabel.moveTopLeft(QPointF(plotAxisRect().right(), plotRect().center().y() - rc.x / 2));

      painter->save();
      QTransform t;
      t.rotate(-90.0);
      painter->rotate(90.0);

      if (rc.x > 0)
        painter->drawPixmap(t.mapRect(rightLabel).topLeft(), pixmap, QRectF(0, 0, rc.x, rightLabel.height()));

      painter->restore();
    }

    delete parsed;
    parsed = 0;
  }

#if DEBUG_LABEL_REGION
  painter->save();
  QTransform t;
  t.rotate(-90.0);
  painter->rotate(90.0);

  QRectF rightLabel = rightLabelRect(false);
  rightLabel.moveTopLeft(plotAxisRect().topRight());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "rightLabel:" << t.mapRect(rightLabel)<< endl;
  painter->fillRect(t.mapRect(rightLabel), Qt::red);
  painter->restore();

  painter->restore();
#endif
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

    parsed->chunk->attributes.color = _topLabelFontColor;

    QRectF topLabel = topLabelRect(false);
    if (topLabel.isValid()) {
      QPixmap pixmap(topLabel.width(), topLabel.height());
      pixmap.fill(Qt::transparent);
      QPainter pixmapPainter(&pixmap);

      Label::RenderContext rc(calculatedTopLabelFont(), &pixmapPainter);
      QFontMetrics fm(calculatedTopLabelFont());
      rc.y = fm.ascent();
      Label::renderLabel(rc, parsed->chunk);

      topLabel.moveTopLeft(QPointF(plotRect().center().x()-rc.x/2, 0));
      if (rc.x > 0)
        painter->drawPixmap(topLabel.topLeft(), pixmap, QRectF(0, 0, rc.x, topLabel.height()));
    }
    painter->restore();

    delete parsed;
    parsed = 0;
  }

#if DEBUG_LABEL_REGION
  painter->save();

  QRectF topLabel = topLabelRect(false);
  topLabel.moveBottomLeft(plotAxisRect().topLeft());

  painter->save();
  painter->setOpacity(0.3);
  qDebug() << "topLabel:" << topLabel;
  painter->fillRect(topLabel, Qt::red);
  painter->restore();

  painter->restore();
#endif
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
//   if (before != this->calculatedAxisMarginWidth()) {
//     emit marginsChanged();
//   }
}


qreal PlotItem::calculatedAxisMarginHeight() const {
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

  painter->setFont(calculatedNumberLabelFont());


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
    if (calculatedBottomLabelMargin() < height) {
      xLabelRect.setHeight(xLabelRect.height() + (height - calculatedBottomLabelMargin()));
    }
  }

  _calculatedAxisMarginHeight = xLabelRect.height();
  if (xLabelRect.right() > plotRect().right()) {
    _calculatedAxisMarginROverflow = qMax(ViewItem::sizeOfGrip().width(), xLabelRect.right() - plotRect().right());
  } else {
    _calculatedAxisMarginROverflow = ViewItem::sizeOfGrip().width();
  }

  painter->restore();
/*  if ((inHeight != _calculatedAxisMarginHeight) 
       || (inVLead != _calculatedAxisMarginVLead) 
       || (inROver != _calculatedAxisMarginROverflow)) {
    emit marginsChanged();
  }*/
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

  painter->setFont(calculatedNumberLabelFont());

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
    if (calculatedLeftLabelMargin() < height) {
      yLabelRect.setWidth(yLabelRect.width() + (height - calculatedLeftLabelMargin()));
    }
  }
  _calculatedAxisMarginWidth = yLabelRect.width();
  if (yLabelRect.top() < plotRect().top()) {
    _calculatedAxisMarginTOverflow = qMax(ViewItem::sizeOfGrip().width(), -yLabelRect.top() + plotRect().top());
  } else {
    _calculatedAxisMarginTOverflow = ViewItem::sizeOfGrip().width();
  }
  painter->restore();
/*  if ((inWidth != _calculatedAxisMarginWidth) 
       || (inHLead != _calculatedAxisMarginHLead) 
       || (inTOver != _calculatedAxisMarginTOverflow)) {
     emit marginsChanged();
  }*/
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
    parentView()->setPlotBordersDirty(true);
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


void PlotItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (isInSharedAxisBox()) {
    if (SharedAxisBoxItem *sharedBox = qgraphicsitem_cast<SharedAxisBoxItem*>(parentItem())) {
      if (sharedBox->tryMousePressEvent(this, event)) {
        return;
      }
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
    setZValue(1000);
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
        av = attrs.value("leftlabeloverride");
        if (!av.isNull()) {
          rc->setLeftLabelOverride(av.toString());
        }
        av = attrs.value("leftlabeluseglobal");
        if (!av.isNull()) {
          rc->setLeftFontUseGlobal(av.toString().toInt());
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
        av = attrs.value("leftlabelfontcolor");
        if (!av.isNull()) {
          rc->setLeftLabelFontColor(QColor(av.toString()));
        }
        av = attrs.value("bottomlabeloverride");
        if (!av.isNull()) {
          rc->setBottomLabelOverride(av.toString());
        }
        av = attrs.value("bottomlabeluseglobal");
        if (!av.isNull()) {
          rc->setBottomFontUseGlobal(av.toString().toInt());
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
        av = attrs.value("bottomlabelfontcolor");
        if (!av.isNull()) {
          rc->setBottomLabelFontColor(QColor(av.toString()));
        }

        av = attrs.value("toplabeloverride");
        if (!av.isNull()) {
          rc->setTopLabelOverride(av.toString());
        }
        av = attrs.value("toplabeluseglobal");
        if (!av.isNull()) {
          rc->setTopFontUseGlobal(av.toString().toInt());
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
        av = attrs.value("toplabelfontcolor");
        if (!av.isNull()) {
          rc->setTopLabelFontColor(QColor(av.toString()));
        }

        av = attrs.value("rightlabeloverride");
        if (!av.isNull()) {
          rc->setRightLabelOverride(av.toString());
        }
        av = attrs.value("rightlabeluseglobal");
        if (!av.isNull()) {
          rc->setRightFontUseGlobal(av.toString().toInt());
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
        av = attrs.value("rightlabelfontcolor");
        if (!av.isNull()) {
          rc->setRightLabelFontColor(QColor(av.toString()));
        }
        av = attrs.value("numberlabeluseglobal");
        if (!av.isNull()) {
          rc->setNumberFontUseGlobal(av.toString().toInt());
        }
        av = attrs.value("numberlabelfont");
        if (!av.isNull()) {
          QFont font;
          font.fromString(av.toString());
          rc->setNumberLabelFont(font);
        }
        av = attrs.value("numberlabelfontscale");
        if (!av.isNull()) {
          rc->setNumberLabelFontScale(QVariant(av.toString()).toDouble());
        }
        av = attrs.value("numberlabelfontcolor");
        if (!av.isNull()) {
          rc->setNumberLabelFontColor(QColor(av.toString()));
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

  if (PlotItem *plotItem = qobject_cast<PlotItem*>(_item)) {
    if (!plotItem->isTiedZoom()) {
      ViewGridLayout::resetSharedPlots(plotItem);
    }
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
