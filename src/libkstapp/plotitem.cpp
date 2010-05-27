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

#include "image.h"

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
#include "dialogdefaults.h"

#include <QMenu>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

// Zoom Debugging.  0 Off, 1 On.
#define DEBUG_ZOOM 0

// Label Region Debugging.  0 Off, 1 On.
#define DEBUG_LABEL_REGION 0

// Benchmark drawing
// 0 = None, 1 = PlotItem, 2 = More Details
#define BENCHMARK 0

static const int PLOT_MAXIMIZED_ZORDER = 1000;

namespace Kst {

PlotItem::PlotItem(View *parent)
  : ViewItem(parent), PlotItemInterface(),
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
  _globalFontScale(0.0),
  _numberAxisLabelScaleFactor(1.0),
  _useNumberAxisLabelScale(true),
  _showLegend(false),
  _plotMaximized(false),
  _allowUpdates(true),
  _legend(0),
  _zoomMenu(0),
  _filterMenu(0),
  _fitMenu(0),
  _sharedAxisBoxMenu(0),
  _sharedBox(0),
  _axisLabelsDirty(true),
  _plotPixmapDirty(true),
  _i_per(0)
{
  setTypeName("Plot");
  _initializeShortName();

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

  //parentView()->configurePlotFontDefaults(this);

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

  _undoStack = new QUndoStack(this);

  createActions();

  PlotItemManager::self()->addPlot(this);

  // Set the initial projection.
  setProjectionRect(QRectF(QPointF(-0.1, -0.1), QPointF(0.1, 0.1)));
  renderItem(PlotRenderItem::Cartesian);

  setPlotBordersDirty(true);
  connect(this, SIGNAL(triggerRedraw()), this, SLOT(redrawPlot()));
  connect(this, SIGNAL(geometryChanged()), this, SLOT(setLabelsDirty()));
  connect(this, SIGNAL(updateAxes()), this, SLOT(setPlotPixmapDirty()));
  connect(this, SIGNAL(geometryChanged()), this, SLOT(setPlotPixmapDirty()));

  applyDefaults();
}

void PlotItem::applyDefaults() {
  QFont font;

  // Global Font Settings
  QFont globalfont;
  globalfont.fromString(_dialogDefaults->value("plot/globalFontFamily",globalfont.toString()).toString());
  setGlobalFont(globalfont);
  setGlobalFontScale(ApplicationSettings::self()->defaultFontScale());
  QColor color;
  color = _dialogDefaults->value("plot/globalFontColor", QColor(Qt::black)).value<QColor>();
  setGlobalFontColor(color);

  // Left Font Settings
  leftLabelDetails()->setFontUseGlobal(_dialogDefaults->value("plot/leftFontGlobal", true).toBool());
  font.fromString(_dialogDefaults->value("plot/leftFontFamily",globalfont.toString()).toString());
  leftLabelDetails()->setFont(font);
  leftLabelDetails()->setFontScale(_dialogDefaults->value("plot/leftFontScale",0).toDouble());
  color = _dialogDefaults->value("plot/leftFontColor", QColor(Qt::black)).value<QColor>();
  leftLabelDetails()->setFontColor(color);

  // right Font Settings
  rightLabelDetails()->setFontUseGlobal(_dialogDefaults->value("plot/rightFontGlobal", true).toBool());
  font.fromString(_dialogDefaults->value("plot/rightFontFamily",globalfont.toString()).toString());
  rightLabelDetails()->setFont(font);
  rightLabelDetails()->setFontScale(_dialogDefaults->value("plot/rightFontScale",0).toDouble());
  color = _dialogDefaults->value("plot/rightFontColor", QColor(Qt::black)).value<QColor>();
  rightLabelDetails()->setFontColor(color);

  // top Font Settings
  topLabelDetails()->setFontUseGlobal(_dialogDefaults->value("plot/topFontGlobal", true).toBool());
  font.fromString(_dialogDefaults->value("plot/topFontFamily",globalfont.toString()).toString());
  topLabelDetails()->setFont(font);
  topLabelDetails()->setFontScale(_dialogDefaults->value("plot/topFontScale",0).toDouble());
  color = _dialogDefaults->value("plot/topFontColor", QColor(Qt::black)).value<QColor>();
  topLabelDetails()->setFontColor(color);

  // bottom Font Settings
  bottomLabelDetails()->setFontUseGlobal(_dialogDefaults->value("plot/bottomFontGlobal", true).toBool());
  font.fromString(_dialogDefaults->value("plot/bottomFontFamily",globalfont.toString()).toString());
  bottomLabelDetails()->setFont(font);
  bottomLabelDetails()->setFontScale(_dialogDefaults->value("plot/bottomFontScale",0).toDouble());
  color = _dialogDefaults->value("plot/bottomFontColor", QColor(Qt::black)).value<QColor>();
  bottomLabelDetails()->setFontColor(color);

  // number Font Settings
  numberLabelDetails()->setFontUseGlobal(_dialogDefaults->value("plot/numberFontGlobal", true).toBool());
  font.fromString(_dialogDefaults->value("plot/numberFontFamily",globalfont.toString()).toString());
  numberLabelDetails()->setFont(font);
  numberLabelDetails()->setFontScale(_dialogDefaults->value("plot/numberFontScale",0).toDouble());
  color = _dialogDefaults->value("plot/numberFontColor", QColor(Qt::black)).value<QColor>();
  numberLabelDetails()->setFontColor(color);

  // set the pen
  QPen pen;
  pen.setStyle((Qt::PenStyle)_dialogDefaults->value("plot/strokeStyle", 1).toInt());
  pen.setWidthF(_dialogDefaults->value("plot/strokeWidth",0).toDouble());
  pen.setJoinStyle((Qt::PenJoinStyle)_dialogDefaults->value("plot/strokeJoinStyle",64).toInt());
  pen.setCapStyle((Qt::PenCapStyle)_dialogDefaults->value("plot/strokeCapStyle",16).toInt());
  QBrush brush;
  color = _dialogDefaults->value("plot/strokeBrushColor",QColor(Qt::black)).value<QColor>();
  brush.setColor(color);
  brush.setStyle((Qt::BrushStyle)_dialogDefaults->value("plot/strokeBrushStyle",1).toInt());
  pen.setBrush(brush);
  setPen(pen);

  //set the brush
  bool useGradient = _dialogDefaults->value("plot/fillBrushUseGradient", false).toBool();
  if (useGradient) {
    QStringList stopInfo =
        _dialogDefaults->value("plot/fillBrushGradient", "0,#000000,1,#ffffff,").
        toString().split(',', QString::SkipEmptyParts);
    QLinearGradient gradient(1,0,0,0);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    for (int i = 0; i < stopInfo.size(); i+=2) {
      gradient.setColorAt(stopInfo.at(i).toDouble(), QColor(stopInfo.at(i+1)));
    }
    brush = QBrush(gradient);
  } else {
    color = _dialogDefaults->value("plot/fillBrushColor",QColor(Qt::white)).value<QColor>();
    brush.setColor(color);
    brush.setStyle((Qt::BrushStyle)_dialogDefaults->value("plot/fillBrushStyle",1).toInt());
  }
  setBrush(brush);

  xAxis()->setAxisVisible(_dialogDefaults->value("plot/xAxisVisible", true).toBool());
  xAxis()->setAxisLog(_dialogDefaults->value("plot/xAxisLog", false).toBool());
  xAxis()->setAxisReversed(_dialogDefaults->value("plot/xAxisReversed", false).toBool());
  xAxis()->setAxisAutoBaseOffset(_dialogDefaults->value("plot/xAxisAutoBaseOffset", true).toBool());
  xAxis()->setAxisBaseOffset(_dialogDefaults->value("plot/xAxisBaseOffset", false).toBool());
  xAxis()->setAxisInterpret(_dialogDefaults->value("plot/xAxisInterpret", false).toBool());
  xAxis()->setAxisInterpretation((AxisInterpretationType)_dialogDefaults->value("plot/xAxisInterpretation", 1).toInt());
  xAxis()->setAxisDisplay((AxisDisplayType)_dialogDefaults->value("plot/xAxisDisplay", 4).toInt());
  xAxis()->setAxisMajorTickMode((PlotAxis::MajorTickMode)_dialogDefaults->value("plot/xAxisMajorTickMode", 5).toInt());
  xAxis()->setAxisMinorTickCount(_dialogDefaults->value("plot/xAxisMinorTickCount", 5).toInt());
  xAxis()->setAxisAutoMinorTicks(_dialogDefaults->value("plot/xAxisautoMinorTickCount", true).toBool());
  xAxis()->setDrawAxisMajorTicks(_dialogDefaults->value("plot/xAxisDrawMajorTicks", true).toBool());
  xAxis()->setDrawAxisMinorTicks(_dialogDefaults->value("plot/xAxisDrawMinorTicks", true).toBool());
  xAxis()->setDrawAxisMajorGridLines(_dialogDefaults->value("plot/xAxisDrawMajorGridLines", true).toBool());
  xAxis()->setDrawAxisMinorGridLines(_dialogDefaults->value("plot/xAxisDrawMinorGridLines", false).toBool());

  xAxis()->setAxisMajorGridLineStyle((Qt::PenStyle)_dialogDefaults->value("plot/xAxisDrawMajorGridLineStyle", 2).toInt());
  xAxis()->setAxisMajorGridLineWidth(_dialogDefaults->value("plot/xAxisDrawMajorGridLineWidth",1).toDouble());
  color = _dialogDefaults->value("plot/xAxisDrawMajorGridLineColor", "#a0a0a4").value<QColor>();
  xAxis()->setAxisMajorGridLineColor(color);

  xAxis()->setAxisMinorGridLineStyle((Qt::PenStyle)_dialogDefaults->value("plot/xAxisDrawMinorGridLineStyle", 2).toInt());
  xAxis()->setAxisMinorGridLineWidth(_dialogDefaults->value("plot/xAxisDrawMinorGridLineWidth",1).toDouble());
  color = _dialogDefaults->value("plot/xAxisDrawMinorGridLineColor", "#a0a0a4").value<QColor>();
  xAxis()->setAxisMinorGridLineColor(color);

  xAxis()->setAxisSignificantDigits(_dialogDefaults->value("plot/xAxisSignificantDigits",9).toInt());
  xAxis()->setAxisLabelRotation(_dialogDefaults->value("plot/xAxisRotation",0).toDouble());


  yAxis()->setAxisVisible(_dialogDefaults->value("plot/yAxisVisible", true).toBool());
  yAxis()->setAxisLog(_dialogDefaults->value("plot/yAxisLog", false).toBool());
  yAxis()->setAxisReversed(_dialogDefaults->value("plot/yAxisReversed", false).toBool());
  yAxis()->setAxisAutoBaseOffset(_dialogDefaults->value("plot/yAxisAutoBaseOffset", true).toBool());
  yAxis()->setAxisBaseOffset(_dialogDefaults->value("plot/yAxisBaseOffset", false).toBool());
  yAxis()->setAxisInterpret(_dialogDefaults->value("plot/yAxisInterpret", false).toBool());
  yAxis()->setAxisInterpretation((AxisInterpretationType)_dialogDefaults->value("plot/yAxisInterpretation", 1).toInt());
  yAxis()->setAxisDisplay((AxisDisplayType)_dialogDefaults->value("plot/yAxisDisplay", 4).toInt());
  yAxis()->setAxisMajorTickMode((PlotAxis::MajorTickMode)_dialogDefaults->value("plot/yAxisMajorTickMode", 5).toInt());
  yAxis()->setAxisMinorTickCount(_dialogDefaults->value("plot/yAxisMinorTickCount", 5).toInt());
  yAxis()->setAxisAutoMinorTicks(_dialogDefaults->value("plot/yAxisautoMinorTickCount", true).toBool());
  yAxis()->setDrawAxisMajorTicks(_dialogDefaults->value("plot/yAxisDrawMajorTicks", true).toBool());
  yAxis()->setDrawAxisMinorTicks(_dialogDefaults->value("plot/yAxisDrawMinorTicks", true).toBool());
  yAxis()->setDrawAxisMajorGridLines(_dialogDefaults->value("plot/yAxisDrawMajorGridLines", true).toBool());
  yAxis()->setDrawAxisMinorGridLines(_dialogDefaults->value("plot/yAxisDrawMinorGridLines", false).toBool());

  yAxis()->setAxisMajorGridLineStyle((Qt::PenStyle)_dialogDefaults->value("plot/yAxisDrawMajorGridLineStyle", 2).toInt());
  yAxis()->setAxisMajorGridLineWidth(_dialogDefaults->value("plot/yAxisDrawMajorGridLineWidth",1).toDouble());
  color = _dialogDefaults->value("plot/yAxisDrawMajorGridLineColor", "#a0a0a4").value<QColor>();
  yAxis()->setAxisMajorGridLineColor(color);

  yAxis()->setAxisMinorGridLineStyle((Qt::PenStyle)_dialogDefaults->value("plot/yAxisDrawMinorGridLineStyle", 2).toInt());
  yAxis()->setAxisMinorGridLineWidth(_dialogDefaults->value("plot/yAxisDrawMinorGridLineWidth",1).toDouble());
  color = _dialogDefaults->value("plot/yAxisDrawMinorGridLineColor", "#a0a0a4").value<QColor>();
  yAxis()->setAxisMinorGridLineColor(color);

  yAxis()->setAxisSignificantDigits(_dialogDefaults->value("plot/yAxisSignificantDigits",9).toInt());
  yAxis()->setAxisLabelRotation(_dialogDefaults->value("plot/yAxisRotation",0).toDouble());

}


PlotItem::~PlotItem() {
  delete _xAxis;
  delete _yAxis;
  delete _leftLabelDetails;
  delete _rightLabelDetails;
  delete _topLabelDetails;
  delete _bottomLabelDetails;
  delete _numberLabelDetails;
  delete _zoomMenu;
  delete _filterMenu;

  PlotItemManager::self()->removePlot(this);
}

void PlotItem::_initializeShortName() {
  _shortName = "P"+QString::number(_plotnum);
  if (_plotnum>max_plotnum)
    max_plotnum = _plotnum;
  _plotnum++;

}

QString PlotItem::plotName() const {
  return Name();
}


void PlotItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("plot");
    xml.writeAttribute("tiedxzoom", QVariant(isXTiedZoom()).toString());
    xml.writeAttribute("tiedyzoom", QVariant(isYTiedZoom()).toString());
    xml.writeAttribute("leftlabelvisible", QVariant(_leftLabelDetails->isVisible()).toString());
    xml.writeAttribute("bottomlabelvisible", QVariant(_bottomLabelDetails->isVisible()).toString());
    xml.writeAttribute("rightlabelvisible", QVariant(_rightLabelDetails->isVisible()).toString());
    xml.writeAttribute("toplabelvisible", QVariant(_topLabelDetails->isVisible()).toString());
    xml.writeAttribute("globalfont", QVariant(_globalFont).toString());
    xml.writeAttribute("globalfontscale", QVariant(_globalFontScale).toString());
    xml.writeAttribute("globalfontcolor", QVariant(_globalFontColor).toString());
    xml.writeAttribute("showlegend", QVariant(_showLegend).toString());
    saveNameInfo(xml, PLOTNUM);

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
  PlotItemDialog *editDialog = new PlotItemDialog(this, kstApp->mainWindow());
  editDialog->show();
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

  _zoomTied = new QAction(tr("Zoom Tied"), this);
  _zoomTied->setShortcut(Qt::Key_T);
  _zoomTied->setCheckable(true);
  registerShortcut(_zoomTied);
  connect(_zoomTied, SIGNAL(triggered()), this, SLOT(zoomTied()));

  _zoomXTied = new QAction(tr("Zoom X Tied"), this);
  _zoomXTied->setShortcut(Qt::CTRL+Qt::Key_T);
  _zoomXTied->setCheckable(true);
  registerShortcut(_zoomXTied);
  connect(_zoomXTied, SIGNAL(triggered()), this, SLOT(zoomXTied()));

  _zoomYTied = new QAction(tr("Zoom Y Tied"), this);
  _zoomYTied->setShortcut(Qt::SHIFT+Qt::Key_T);
  _zoomYTied->setCheckable(true);
  registerShortcut(_zoomYTied);
  connect(_zoomYTied, SIGNAL(triggered()), this, SLOT(zoomYTied()));

  _zoomMeanCentered = new QAction(tr("Zoom Mean-centered Y"), this);
  _zoomMeanCentered->setShortcut(Qt::Key_A);
  registerShortcut(_zoomMeanCentered);
  connect(_zoomMeanCentered, SIGNAL(triggered()), this, SLOT(zoomMeanCentered()));

  _zoomXMaximum = new QAction(tr("X-Zoom Maximum"), this);
  _zoomXMaximum->setShortcut(Qt::CTRL+Qt::Key_M);
  registerShortcut(_zoomXMaximum);
  connect(_zoomXMaximum, SIGNAL(triggered()), this, SLOT(zoomXMaximum()));

  _zoomXAutoBorder = new QAction(tr("X-Zoom Auto Border"), this);
  _zoomXAutoBorder->setShortcut(Qt::CTRL+Qt::Key_B);
  registerShortcut(_zoomXAutoBorder);
  connect(_zoomXAutoBorder, SIGNAL(triggered()), this, SLOT(zoomXAutoBorder()));

  _zoomXNoSpike = new QAction(tr("X-Zoom Spike Insensitive"), this);
  //_zoomXNoSpike->setShortcut(Qt::CTRL+Qt::Key_S);
  registerShortcut(_zoomXNoSpike);
  connect(_zoomXNoSpike, SIGNAL(triggered()), this, SLOT(zoomXNoSpike()));

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

  _zoomYAutoBorder = new QAction(tr("Y-Zoom Auto Border"), this);
  _zoomYAutoBorder->setShortcut(Qt::SHIFT+Qt::Key_B);
  registerShortcut(_zoomYAutoBorder);
  connect(_zoomYAutoBorder, SIGNAL(triggered()), this, SLOT(zoomYAutoBorder()));

  _zoomYNoSpike = new QAction(tr("Y-Zoom Spike Insensitive"), this);
  _zoomYNoSpike->setShortcut(Qt::SHIFT+Qt::Key_S);
  registerShortcut(_zoomYNoSpike);
  connect(_zoomYNoSpike, SIGNAL(triggered()), this, SLOT(zoomYNoSpike()));

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

  _adjustImageColorscale = new QAction(tr("Adjust Image Color Scale"), this);
  _adjustImageColorscale->setShortcut(Qt::Key_I);
  registerShortcut(_adjustImageColorscale);
  connect(_adjustImageColorscale, SIGNAL(triggered()), this, SLOT(adjustImageColorScale()));

  createZoomMenu();

  _plotMaximize = new QAction(tr("Maximize Plot"), this);
  _plotMaximize->setShortcut(Qt::Key_Z);
  _plotMaximize->setCheckable(true);
  registerShortcut(_plotMaximize);
  connect(_plotMaximize, SIGNAL(triggered()), this, SLOT(plotMaximize()));

  
  _shareBoxShareX = new QAction(tr("Share Plots on X-Axis"), this);
  _shareBoxShareX->setShortcut(Qt::Key_X);
  _shareBoxShareX->setCheckable(true);
  registerShortcut(_shareBoxShareX);
  connect(_shareBoxShareX, SIGNAL(triggered()), this, SIGNAL(shareXAxisTriggered()));

  _shareBoxShareY = new QAction(tr("Share Plots on Y-Axis"), this);
  _shareBoxShareY->setShortcut(Qt::Key_Y);
  _shareBoxShareY->setCheckable(true);
  registerShortcut(_shareBoxShareY);
  connect(_shareBoxShareY, SIGNAL(triggered()), this, SIGNAL(shareYAxisTriggered()));

  _breakSharedBox = new QAction(tr("Break Shared Axis Box"), this);
  _breakSharedBox->setShortcut(Qt::Key_B);
  registerShortcut(_breakSharedBox);
  connect(_breakSharedBox, SIGNAL(triggered()), this, SIGNAL(breakShareTriggered()));
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
  _zoomMenu->addAction(_zoomMeanCentered);
  _zoomMenu->addAction(_zoomTied);
  _zoomMenu->addAction(_adjustImageColorscale);

  _zoomMenu->addSeparator();

  _zoomMenu->addAction(_zoomXTied);
  _zoomMenu->addAction(_zoomXMaximum);
  _zoomMenu->addAction(_zoomXAutoBorder);
  _zoomMenu->addAction(_zoomXNoSpike);
  _zoomMenu->addAction(_zoomXRight);
  _zoomMenu->addAction(_zoomXLeft);
  _zoomMenu->addAction(_zoomXOut);
  _zoomMenu->addAction(_zoomXIn);
  _zoomMenu->addAction(_zoomNormalizeXtoY);
  _zoomMenu->addAction(_zoomLogX);

  _zoomMenu->addSeparator();

  _zoomMenu->addAction(_zoomYTied);
  _zoomMenu->addAction(_zoomYLocalMaximum);
  _zoomMenu->addAction(_zoomYMaximum);
  _zoomMenu->addAction(_zoomYAutoBorder);
  _zoomMenu->addAction(_zoomYNoSpike);
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


void PlotItem::createSharedAxisBoxMenu() {
  if (_sharedAxisBoxMenu) {
    delete _shareBoxShareX;
  }
  _sharedAxisBoxMenu = new QMenu;
  _sharedAxisBoxMenu->setTitle(tr("Shared Axis Box Settings"));

  _sharedAxisBoxMenu->addAction(_shareBoxShareX);
  _sharedAxisBoxMenu->addAction(_shareBoxShareY);
}




void PlotItem::addToMenuForContextEvent(QMenu &menu) {
  if (parentItem() && isInSharedAxisBox() && _sharedBox) {
    if (parentView()->viewMode() == View::Data) {

      menu.addMenu(_sharedAxisBoxMenu);

      menu.addAction(_breakSharedBox);

      _shareBoxShareX->setChecked(_sharedBox->isXAxisShared());
      _shareBoxShareY->setChecked(_sharedBox->isYAxisShared());
    }
  }

  if (parentView()->viewMode() == View::Data) {
    _plotMaximize->setChecked(_plotMaximized);
    menu.addAction(_plotMaximize);
  }

  _zoomLogX->setChecked(xAxis()->axisLog());
  _zoomLogY->setChecked(yAxis()->axisLog());

  _zoomTied->setChecked(isTiedZoom());
  _zoomXTied->setChecked(isXTiedZoom());
  _zoomYTied->setChecked(isYTiedZoom());

  _zoomPrevious->setVisible(!isInSharedAxisBox());
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


bool PlotItem::handleChangedInputs(qint64 serial) {
  if (!_allowUpdates) {
    return false;
  }

  // decide if the inputs have changed
  bool no_change = true;

  if (_serialOfLastChange==Forced) {
    no_change = false;
  } else {
    foreach (PlotRenderItem *renderer, renderItems()) {
      foreach (RelationPtr relation, renderer->relationList()) {
        if (relation->serialOfLastChange() > _serialOfLastChange) {
          no_change = false;
        }
      }
    }
  }

  if (no_change) {
    return false;
  }

  _serialOfLastChange = serial;

  QRectF compute = computedProjectionRect();
  QRectF newProjectionRec = projectionRect();

  if (isInSharedAxisBox()) {
    // Need to update the box's projectionRect.
    sharedAxisBox()->updateZoomForDataUpdate();
  }
  if ((xAxis()->axisZoomMode() == PlotAxis::Auto) ||
        (xAxis()->axisZoomMode() == PlotAxis::MeanCentered) ||
        (xAxis()->axisZoomMode() == PlotAxis::AutoBorder) ||
        (xAxis()->axisZoomMode() == PlotAxis::SpikeInsensitive)) {

    newProjectionRec.setLeft(compute.x());
    newProjectionRec.setWidth(compute.width());
  }

  if ((yAxis()->axisZoomMode() == PlotAxis::AutoBorder) ||
      (yAxis()->axisZoomMode() == PlotAxis::Auto) ||
      (yAxis()->axisZoomMode() == PlotAxis::SpikeInsensitive) ||
      (yAxis()->axisZoomMode() == PlotAxis::MeanCentered)) {
    newProjectionRec.setTop(compute.y());
    newProjectionRec.setHeight(compute.height());
  }

  setProjectionRect(newProjectionRec);

#if 0
  else {
        setProjectionRect(QRectF(compute.x(),
              projectionRect().y(),
              compute.width(),
              projectionRect().height()));
      }
    } else if (yAxis()->axisZoomMode() == PlotAxis::Auto) {
      setProjectionRect(QRectF(projectionRect().x(),
            compute.y(),
            projectionRect().width(),
            compute.height()));
    } else {
      update();
    }
  }
#endif

  setLabelsDirty();

  return true;
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


void PlotItem::updatePlotPixmap() {
#if BENCHMARK
  QTime bench_time;
  bench_time.start();
#endif

  _plotPixmapDirty = false;
  if (maskedByMaximization()) {
    return;
  }

  QPixmap pixmap(rect().width()+1, rect().height()+1);
  pixmap.fill(Qt::transparent);
  QPainter pixmapPainter(&pixmap);

  //pixmapPainter.setRenderHint(QPainter::Antialiasing);

  pixmapPainter.save();
  if (rect().topLeft() != QPointF(0, 0)) {
    pixmapPainter.translate(-rect().topLeft());
  }
  paintPixmap(&pixmapPainter);
  pixmapPainter.restore();
  
  _plotPixmap = pixmap;
#if BENCHMARK
  int i = bench_time.elapsed();
  qDebug() << "Total Time to update plot pixmap " << (void *)this << ": " << i << "ms";
#endif
}


void PlotItem::paint(QPainter *painter) {
  if (maskedByMaximization()) {
    return;
  }

#if BENCHMARK
  QTime bench_time;
  bench_time.start();
#endif
  if (parentView()->isPrinting()) {
    paintPixmap(painter);
  } else {
    if (_plotPixmapDirty && rect().isValid()) {
      updatePlotPixmap();
    }

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect());
    painter->restore();
    painter->drawPixmap(rect().topLeft(), _plotPixmap);
  }
#if BENCHMARK
  int i = bench_time.elapsed();
  qDebug() << "Total Time to paint " << (void *)this << ": " << i << "ms" << endl;
#endif
}


void PlotItem::paintPixmap(QPainter *painter) {
  if ((parentView()->plotBordersDirty() || (creationState() == ViewItem::InProgress)) && rect().isValid()) {
    ViewGridLayout::standardizePlotMargins(this, painter);
    setPlotBordersDirty(false);
  }

#if BENCHMARK
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
  bool xTicksUpdated = xAxis()->ticksUpdated();
  bool yTicksUpdated = yAxis()->ticksUpdated();
  if (xTicksUpdated ||yTicksUpdated) {
    resetScaleAxisLabels();
  }

  painter->setFont(numberLabelDetails()->calculatedFont());

#if BENCHMARK
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
#if BENCHMARK
    b_2 = benchtmp.elapsed();
#endif

  paintPlot(painter, xTicksUpdated, yTicksUpdated);
#if BENCHMARK
    b_3 = benchtmp.elapsed();
#endif

  paintTickLabels(painter);
#if BENCHMARK
    b_4 = benchtmp.elapsed();
#endif

  paintPlotMarkers(painter);

#if DEBUG_LABEL_REGION
  //  qDebug() << "=============> topLabel:" << topLabel() << endl;
#endif
  paintTopLabel(painter);

#if BENCHMARK
    b_5 = benchtmp.elapsed();
#endif

  painter->restore();
#if BENCHMARK
  int i = bench_time.elapsed();
  qDebug() << "Painting Plot Pixmap " << (void *)this << ": " << i << "ms";
  if (b_1 > 0)       qDebug() << "            Setup: " << b_1 << "ms";
  if (b_2 - b_1 > 0) qDebug() << "           Labels: " << (b_2 - b_1) << "ms";
  if (b_3 - b_2 > 0) qDebug() << "             Plot: " << (b_3 - b_2) << "ms";
  if (b_4 - b_3 > 0) qDebug() << "      Tick Labels: " << (b_4 - b_3) << "ms";
  if (b_5 - b_4 > 0) qDebug() << "          Markers: " << (b_5 - b_4) << "ms";
#endif
}


void PlotItem::paintPlot(QPainter *painter, bool xUpdated, bool yUpdated) {
  if (xUpdated) {
    xAxis()->validateDrawingRegion(painter);
    updateXAxisLines();
    updateXAxisLabels(painter);
  }
  if (yUpdated) {
    yAxis()->validateDrawingRegion(painter);
    updateYAxisLines();
    updateYAxisLabels(painter);
  }
  if (isUseAxisScale()) {
    QFont font(painter->font());
    qreal pointSize = qMax((font.pointSizeF() * _numberAxisLabelScaleFactor), ApplicationSettings::self()->minimumFontSize());
    
    font.setPointSizeF(pointSize);
    painter->setFont(font);
  }

  if (_axisLabelsDirty) {
    if (!xUpdated) {
      updateXAxisLabels(painter);
    }
    if (!yUpdated) {
      updateYAxisLabels(painter);
    }
  }
#if BENCHMARK > 1
  QTime bench_time, benchtmp;
  int b_1 = 0, b_2 = 0, b_3 = 0, b_4 = 0, b_5 = 0;
  bench_time.start();
  benchtmp.start();
#endif
  paintMajorGridLines(painter);
#if BENCHMARK > 1
    b_1 = benchtmp.elapsed();
#endif
  paintMinorGridLines(painter);
#if BENCHMARK > 1
    b_2 = benchtmp.elapsed();
#endif
  painter->save();
  painter->setBrush(Qt::NoBrush);
  painter->setPen(pen());
  painter->drawRect(plotRect());
#if BENCHMARK > 1
    b_3 = benchtmp.elapsed();
#endif
  paintMajorTicks(painter);
#if BENCHMARK > 1
    b_4 = benchtmp.elapsed();
#endif
  paintMinorTicks(painter);
#if BENCHMARK > 1
    b_5 = benchtmp.elapsed();
#endif
  painter->restore();

#if BENCHMARK > 1
  int i = bench_time.elapsed();
  qDebug() << "Painting Plot - PaintPlot " << (void *)this << ": " << i << "ms";
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

  int rotation = _xAxis->axisLabelRotation();
  painter->save();
  QTransform t;
  t.rotate(rotation);

  QMapIterator<qreal, QString> xLabelIt(_xAxis->axisLabels());
  while (xLabelIt.hasNext()) {
    xLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, xLabelIt.value());
    QPointF p;
    if (rotation == 0) {
      p = QPointF(mapXToPlot(xLabelIt.key()), plotRect().bottom() + bound.height()*0.5 + _calculatedAxisMarginVLead);
      bound.moveCenter(p);
    } else {
      if (rotation < 0) {
        qreal theta = (rotation >-30 ? rotation : -30);
        qreal right = mapXToPlot(xLabelIt.key()) - bound.height() * (sin(theta*M_PI/180.0));
        bound = t.mapRect(bound);
        p = QPointF(right, plotRect().bottom() + _calculatedAxisMarginVLead);
        bound.moveTopRight(p);
      } else {
        qreal theta = (rotation <30 ? rotation : 30);
        qreal left = mapXToPlot(xLabelIt.key()) - bound.height() * (sin(theta*M_PI/180.0));
        bound = t.mapRect(bound);
        p = QPointF(left, plotRect().bottom() + _calculatedAxisMarginVLead);
        bound.moveTopLeft(p);
      }
    }

    if (rect().left() > bound.left()) {
      bound.setLeft(rect().left());
    }

    if (rect().right() >= bound.right()) { // if no overflow to the right...
      CachedPlotLabel label;
      label.bound = bound;
      label.value = xLabelIt.value();
      _xPlotLabels.append(label);
    }
  }
  painter->restore();

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
  setPlotPixmapDirty();
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

  int rotation = _yAxis->axisLabelRotation();

  QTransform t;
  t.rotate(rotation);

  QMapIterator<qreal, QString> yLabelIt(_yAxis->axisLabels());
  while (yLabelIt.hasNext()) {
    yLabelIt.next();

    QRectF bound = painter->boundingRect(QRectF(), flags, yLabelIt.value());
    QPointF p;
    if (rotation < 0) {
      qreal theta = (rotation >-45.0) ? rotation : -45.0;
      qreal top;
      if (rotation >-89) {
        top = mapYToPlot(yLabelIt.key()) - bound.height()*0.5*cos(theta*2.0*M_PI/180.0);
      } else {
        top = mapYToPlot(yLabelIt.key()) - bound.width()*0.5;
      }
      bound = t.mapRect(bound);
      bound.moveRight(plotRect().left() - _calculatedAxisMarginHLead);
      bound.moveTop(top);
    } else if (rotation > 0) {
      qreal theta = (rotation <45.0) ? rotation : 45.0;
      qreal bottom;
      if (rotation < 89) {
        bottom = mapYToPlot(yLabelIt.key()) + bound.height()*0.5*cos(theta*2.0*M_PI/180.0);
      } else {
        bottom = mapYToPlot(yLabelIt.key()) + bound.width()*0.5;
      }
      bound = t.mapRect(bound);
      bound.moveRight(plotRect().left() - _calculatedAxisMarginHLead);
      bound.moveBottom(bottom);
    } else { // no rotation.
      p = QPointF(plotRect().left() - _calculatedAxisMarginHLead,
                  mapYToPlot(yLabelIt.key()) - bound.height() * 0.5);
      bound.moveTopRight(p);
    }

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
    painter->setPen(QPen(QBrush(_xAxis->axisMajorGridLineColor()), _xAxis->axisMajorGridLineWidth(), _xAxis->axisMajorGridLineStyle()));
    painter->drawLines(_xMajorGridLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMajorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMajorGridLineColor()), _yAxis->axisMajorGridLineWidth(), _yAxis->axisMajorGridLineStyle()));
    painter->drawLines(_yMajorGridLines);
    painter->restore();
  }
}


void PlotItem::paintMinorGridLines(QPainter *painter) {
  if (xAxis()->drawAxisMinorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_xAxis->axisMinorGridLineColor()), _xAxis->axisMinorGridLineWidth(), _xAxis->axisMinorGridLineStyle()));
    painter->drawLines(_xMinorGridLines);
    painter->restore();
  }

  if (yAxis()->drawAxisMinorGridLines()) {
    painter->save();
    painter->setPen(QPen(QBrush(_yAxis->axisMinorGridLineColor()), _yAxis->axisMinorGridLineWidth(), _yAxis->axisMinorGridLineStyle()));
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


void PlotItem::scaleAxisLabels(qreal scaleFactor) {
  _numberAxisLabelScaleFactor = qMin(_numberAxisLabelScaleFactor, scaleFactor);
}


void PlotItem::resetScaleAxisLabels() {
  _numberAxisLabelScaleFactor = 1.0;
}


bool PlotItem::isUseAxisScale() const {
  return _useNumberAxisLabelScale;
}


void PlotItem::setUseAxisScale(bool useScale) {
  if (_useNumberAxisLabelScale != useScale) {
    _useNumberAxisLabelScale = useScale;
    setPlotPixmapDirty();
  }
}


void PlotItem::paintBottomTickLabels(QPainter *painter) {
  int flags = Qt::TextSingleLine/* | Qt::AlignCenter*/;

  painter->save();
  painter->setPen(_numberLabelDetails->fontColor());

  int rotation = _xAxis->axisLabelRotation();
  foreach(CachedPlotLabel label, _xPlotLabels) {
    QRectF bound = label.bound;
    if (_numberAxisLabelScaleFactor<0.9999) {
      bound.translate( bound.width() *(1.0-_numberAxisLabelScaleFactor)*0.5, 0.0);
    }
    if (rotation != 0) {
      painter->save();
      QTransform t;
      t.rotate(-1*rotation);
      painter->rotate(rotation);

      painter->drawText(t.mapRect(bound), Qt::TextSingleLine | Qt::AlignCenter, label.value);
      painter->restore();
    } else {
      painter->drawText(bound, flags, label.value);
    }
  }
  painter->restore();

}


void PlotItem::paintLeftTickLabels(QPainter *painter) {
  int flags = Qt::TextSingleLine | Qt::AlignVCenter | Qt::AlignRight;

  painter->save();
  painter->setPen(_numberLabelDetails->fontColor());

  int rotation = _yAxis->axisLabelRotation();

  foreach(CachedPlotLabel label, _yPlotLabels) {
    if (label.baseLabel) {
      painter->save();
      QTransform t;
      t.rotate(90.0);
      painter->rotate(-90.0);

      painter->drawText(t.mapRect(label.bound), flags, label.value);
      painter->restore();
    } else {
      if (rotation != 0) {
        painter->save();
        QTransform t;
        t.rotate(-1*rotation);
        painter->rotate(rotation);

        painter->drawText(t.mapRect(label.bound), flags, label.value);
        painter->restore();
      } else {
        painter->drawText(label.bound, flags, label.value);
      }
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
  emit updatePlotRect();
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
  emit triggerRedraw();
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


void PlotItem::setTiedZoom(bool tiedXZoom, bool tiedYZoom, bool checkAllTied) {
  if ((_isXTiedZoom == tiedXZoom) && (_isYTiedZoom == tiedYZoom))
    return;

  bool wasTiedZoom = isTiedZoom();

  _isXTiedZoom = tiedXZoom;
  _isYTiedZoom = tiedYZoom;

  if (isTiedZoom() && !wasTiedZoom) {
    PlotItemManager::self()->addTiedZoomPlot(this, checkAllTied);
  } else if (!isTiedZoom() && wasTiedZoom) {
    PlotItemManager::self()->removeTiedZoomPlot(this);
  }

  //FIXME ugh, this is expensive, but need to redraw the checkboxes...
  update();
}


bool PlotItem::isInSharedAxisBox() const {
  return _isInSharedAxisBox;
}


void PlotItem::setInSharedAxisBox(bool inSharedBox) {
  _isInSharedAxisBox = inSharedBox;
  setLockParent(inSharedBox);
}


SharedAxisBoxItem* PlotItem::sharedAxisBox() const {
  return _sharedBox;
}


void PlotItem::setSharedAxisBox(SharedAxisBoxItem* parent) {
  if (parent) {
    if (isTiedZoom()) {
      setTiedZoom(false, false);
    }
    setInSharedAxisBox(true);
    setAllowedGripModes(0);
    setFlags(0);
    setParent(parent);
    setBrush(Qt::transparent);

    _sharedBox = parent;
    createSharedAxisBoxMenu();
  } else {
    setSupportsTiedZoom(true);
    setInSharedAxisBox(false);
    setAllowedGripModes(Move | Resize | Rotate);
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIsFocusable);
    setParent(0);
    setBrush(Qt::white);

    _sharedBox = 0;
  }
}


void PlotItem::setPlotBordersDirty(bool dirty) {
  if (isInSharedAxisBox() && dirty && _sharedBox) {
    _sharedBox->setDirty();
  } else {
    parentView()->setPlotBordersDirty(dirty);
  }
  if (dirty) {
    setPlotRectsDirty();
  }
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
  if (!leftLabelDetails()->isAuto()) {
    return leftLabelDetails()->text();
  } else {
    return autoLeftLabel();
  }
}


QString PlotItem::autoLeftLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    QString label = renderer->leftLabel();
    if (!label.isEmpty())
      return label;
  }
  return QString();
}


QString PlotItem::bottomLabel() const {
  if (!bottomLabelDetails()->isAuto()) {
    return bottomLabelDetails()->text();
  } else {
    return autoBottomLabel();
  }
}


QString PlotItem::autoBottomLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    QString label = renderer->bottomLabel();
    if (!label.isEmpty())
      return label;
  }
  return QString();
}


QString PlotItem::rightLabel() const {
  if (!rightLabelDetails()->isAuto()) {
    return rightLabelDetails()->text();
  } else {
    return autoRightLabel();
  }
}


QString PlotItem::autoRightLabel() const {
  foreach (PlotRenderItem *renderer, renderItems()) {
    QString label = renderer->rightLabel();
    if (!label.isEmpty())
      return label;
  }
  return QString();
}


QString PlotItem::topLabel() const {
  if (!topLabelDetails()->isAuto()) {
    return topLabelDetails()->text();
  } else {
    return autoTopLabel();
  }
}


QString PlotItem::autoTopLabel() const {
  if (showLegend()) {
    return QString();
  } else {
    QString label;
    int count = renderItems().count();
    for (int i=0; i<count; i++) {
      label += renderItems().at(i)->topLabel();
      if (i<count - 1) {
        label += " ";
      }
    }
    if (label == leftLabel()) {
      return QString();
    } else {
      return label;
    }
  }
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
  if (topLabelMargin()>0) {
    return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), topLabelMargin());
  } else {
    return QRectF(0.0, 0.0, width() - leftLabelMargin() - rightLabelMargin(), _calculatedTopLabelHeight);
  }
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

    if (_leftLabel.rc) {
      delete _leftLabel.rc;
    }

    Label::RenderContext *rc = new Label::RenderContext(leftLabelDetails()->calculatedFont(), 0);
    QFontMetrics fm(leftLabelDetails()->calculatedFont());
    rc->y = fm.ascent();
    Label::renderLabel(*rc, parsed->chunk);

    QTransform t;
    t.translate(rect().left(),plotRect().center().y() + rc->x/2);
    t.rotate(-90.0);

    connect(rc, SIGNAL(labelDirty()), this, SLOT(setLeftLabelDirty()));
    connect(rc, SIGNAL(labelDirty()), this, SLOT(redrawPlot()));

    _leftLabel.rc = rc;
    _leftLabel.transform = t;
    _leftLabel.valid = true;

    delete parsed;
  }
}


void PlotItem::paintLeftLabel(QPainter *painter) {
  if (!_leftLabelDetails->isVisible() || leftLabel().isEmpty())
    return;

  generateLeftLabel();

  if (_leftLabel.valid) {
    painter->save();
    painter->setTransform(_leftLabel.transform, true);
    Label::paintLabel(*_leftLabel.rc, painter);
    painter->restore();
  }
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
        Qt::AlignCenter, leftLabel());
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

    if (_bottomLabel.rc) {
      delete _bottomLabel.rc;
    }

    Label::RenderContext *rc = new Label::RenderContext(bottomLabelDetails()->calculatedFont(), 0);
    QFontMetrics fm(bottomLabelDetails()->calculatedFont());
    rc->y = fm.ascent();
    Label::renderLabel(*rc, parsed->chunk);

    QTransform t;
    t.translate(plotRect().center().x() - rc->x / 2, plotAxisRect().bottom());

    connect(rc, SIGNAL(labelDirty()), this, SLOT(setBottomLabelDirty()));
    connect(rc, SIGNAL(labelDirty()), this, SLOT(redrawPlot()));

    _bottomLabel.rc = rc;
    _bottomLabel.transform = t;
    _bottomLabel.valid = true;

    delete parsed;
  }
}


void PlotItem::paintBottomLabel(QPainter *painter) {
  if (!_bottomLabelDetails->isVisible() || bottomLabel().isEmpty())
    return;

  generateBottomLabel();

  if (_bottomLabel.valid) {
    painter->save();
    painter->setTransform(_bottomLabel.transform, true);
    Label::paintLabel(*_bottomLabel.rc, painter);
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
        Qt::AlignCenter, bottomLabel());
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
  if (parsed && rightLabelRect().isValid()) {
    parsed->chunk->attributes.color = _rightLabelDetails->fontColor();

    if (_rightLabel.parsed) {
      delete _rightLabel.parsed;
    }

    Label::RenderContext *rc = new Label::RenderContext(rightLabelDetails()->calculatedFont(), 0);
    QFontMetrics fm(rightLabelDetails()->calculatedFont());
    rc->y = fm.ascent();
    Label::renderLabel(*rc, parsed->chunk);

    QTransform t;
    t.translate(rect().right(), plotRect().center().y() - rc->x/2);

    t.rotate(90.0);

    connect(rc, SIGNAL(labelDirty()), this, SLOT(setRightLabelDirty()));
    connect(rc, SIGNAL(labelDirty()), this, SLOT(redrawPlot()));

    _rightLabel.rc = rc;
    _rightLabel.transform = t;
    _rightLabel.valid = true;

    delete parsed;
  }
}


void PlotItem::paintRightLabel(QPainter *painter) {
  if (!_rightLabelDetails->isVisible() || rightLabel().isEmpty())
    return;

  generateRightLabel();

  if (_rightLabel.valid) {
    painter->save();
    painter->setTransform(_rightLabel.transform, true);
    Label::paintLabel(*_rightLabel.rc, painter);
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
        Qt::AlignCenter, rightLabel());
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
  if (parsed && topLabelRect().isValid()) {
    parsed->chunk->attributes.color = _topLabelDetails->fontColor();

    if (_topLabel.rc) {
      delete _topLabel.rc;
    }

    Label::RenderContext *rc = new Label::RenderContext(topLabelDetails()->calculatedFont(), 0);
    QFontMetrics fm(topLabelDetails()->calculatedFont());
    rc->y = fm.ascent();
    Label::renderLabel(*rc, parsed->chunk);

    QTransform t;
    if (_topLabelDetails->isVisible()) {
      t.translate(plotRect().center().x() - rc->x / 2, rect().top());
    } else {
      t.translate(plotRect().center().x() - rc->x / 2, rect().top() + topLabelRect().height()/2);
    }
    connect(rc, SIGNAL(labelDirty()), this, SLOT(setTopLabelDirty()));
    connect(rc, SIGNAL(labelDirty()), this, SLOT(redrawPlot()));

    _topLabel.rc = rc;
    _topLabel.transform = t;
    _topLabel.valid = true;

    delete parsed;
    }
}


void PlotItem::paintTopLabel(QPainter *painter) {
  if (topLabel().isEmpty())
    return;

  generateTopLabel();
  if (_topLabel.valid) {
    painter->save();
    painter->setTransform(_topLabel.transform, true);
    Label::paintLabel(*_topLabel.rc, painter);
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

  painter->save();

  painter->setFont(topLabelDetails()->calculatedFont());

  QRectF topLabelBound = painter->boundingRect(topLabelRect(),
      Qt::AlignCenter, topLabel());

  painter->restore();

  _calculatedTopLabelHeight = topLabelBound.height();

  if (!_topLabelDetails->isVisible()) {
    _calculatedTopLabelMargin = 0;
  } else {
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

      int rotation = _xAxis->axisLabelRotation();
      QTransform t;
      t.rotate(rotation);

      if (rotation != 0) {
        bound.setHeight(t.mapRect(bound).height());
      }

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
  _calculatedAxisMarginROverflow = 2*ViewItem::sizeOfGrip().width();

  painter->restore();
}

/** This function calculates and sets three things:
      _calculatedAxisMarginHLead: spacing between left of plotRect and right of axis labels
      _calculatedAxisMarginVOverflow: topmost axis number extension beyond plotRect 
      _calculatedAxisMarginWidth: the width of the widest axis number
*/
void PlotItem::calculateLeftTickLabelBound(QPainter *painter) {
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

      int rotation = _yAxis->axisLabelRotation();
      QTransform t;
      t.rotate(rotation);

      if (rotation != 0) {
        bound.setWidth(t.mapRect(bound).width());
        bound.setHeight(t.mapRect(bound).height());
      }

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
    _legend->setPos(rect().x() + width()*0.15, rect().y() + height()*0.15);
    _legend->updateRelativeSize();
  }
  return _legend;
}


void PlotItem::setShowLegend(const bool show) {
  if (show != _showLegend) {
    legend()->setVisible(show);
    _showLegend = show;
  }
}


bool PlotItem::tryShortcut(const QString &keySequence) {
  return ViewItem::tryShortcut(keySequence);
}


void PlotItem::setProjectionRect(const QRectF &rect, bool forceAxisUpdate) {
  if (!(/*_projectionRect == rect ||*/ rect.isEmpty() || !rect.isValid())) {

    _projectionRect = rect;
    setPlotBordersDirty(true);
    emit updateAxes();
    update(); //slow, but need to update everything...
  } else if (forceAxisUpdate) {
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
    qreal min = maximum;
    qreal max = minimum;
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
  if (event->button() == Qt::LeftButton) {
    if (checkBox().contains(event->pos())) {
      setTiedZoom(!isTiedZoom(), !isTiedZoom());
    } else if (parentView()->viewMode() == View::Data) {
      edit();
    } else {
      ViewItem::mousePressEvent(event);
    }
  } 
}


QPainterPath PlotItem::checkBox() const {
  if (!isInSharedAxisBox() || (sharedAxisBox() && sharedAxisBox()->isXAxisShared() && sharedAxisBox()->isYAxisShared())) {
    return ViewItem::checkBox();
  } else {
    QRectF bound = selectBoundingRect();
    QRectF grip;
    if (sharedAxisBox()->isXAxisShared()) {
      grip = QRectF(QPointF(_calculatedPlotRect.topRight().x() + sizeOfGrip().width() * .25, _calculatedPlotRect.topRight().y() - sizeOfGrip().height() * -.25), sizeOfGrip());
    } else if (sharedAxisBox()->isYAxisShared()) {
      grip = QRectF(QPointF(_calculatedPlotRect.topRight().x() - sizeOfGrip().width() * 1.25, _calculatedPlotRect.topRight().y() - sizeOfGrip().height() * 1.25), sizeOfGrip());
    }
    QPainterPath path;
    path.addEllipse(grip);
    return path;
  }
}


QPainterPath PlotItem::tiedZoomCheck() const {
  if (!isInSharedAxisBox() || (sharedAxisBox() && sharedAxisBox()->isXAxisShared() && sharedAxisBox()->isYAxisShared())) {
    return ViewItem::tiedZoomCheck();
  } else {
    QRectF bound = selectBoundingRect();
    QRectF grip;
    if (sharedAxisBox()->isXAxisShared()) {
      grip = QRectF(QPointF(_calculatedPlotRect.topRight().x() + sizeOfGrip().width() * .25, _calculatedPlotRect.topRight().y() - sizeOfGrip().height() * -.25), sizeOfGrip());
    } else if (sharedAxisBox()->isYAxisShared()) {
      grip = QRectF(QPointF(_calculatedPlotRect.topRight().x() - sizeOfGrip().width() * 1.25, _calculatedPlotRect.topRight().y() - sizeOfGrip().height() * 1.25), sizeOfGrip());
    }
    QPainterPath path;
    if (isXTiedZoom() && isYTiedZoom()) {
      path.addEllipse(grip);
    } else if (isXTiedZoom()) {
      path.moveTo(grip.center());
      path.arcTo(grip, 225, 180);
    } else if (isYTiedZoom()) {
      path.moveTo(grip.center());
      path.arcTo(grip, 45, 180);
    }
    return path;
  }
}


bool PlotItem::supportsTiedZoom() const {
  if (isInSharedAxisBox()) {
    if (sharedAxisBox() && sharedAxisBox()->isXAxisShared() && sharedAxisBox()->isYAxisShared() &&
        sharedAxisBox()->keyPlot() == this) {
      return true;
    }
  }
  return ViewItem::supportsTiedZoom();
}


void PlotItem::setAllowUpdates(bool allowed) {
  _allowUpdates = allowed;
  UpdateManager::self()->doUpdates(true);
}


void PlotItem::plotMaximize() {
  if (!_plotMaximized && parentView()->viewMode() != View::Data) {
    return;
  }

  if (_plotMaximized) {
    double x_rescale;
    double y_rescale;

    x_rescale = parentView()->sceneRect().width()/_plotMaximizedSourceParentRect.width();
    y_rescale = parentView()->sceneRect().height()/_plotMaximizedSourceParentRect.height();
    _plotMaximizedSourceRect.setWidth(_plotMaximizedSourceRect.width()*x_rescale);
    _plotMaximizedSourceRect.setHeight(_plotMaximizedSourceRect.height()*y_rescale);
    _plotMaximizedSourcePosition.setX(_plotMaximizedSourcePosition.x()*x_rescale);
    _plotMaximizedSourcePosition.setY(_plotMaximizedSourcePosition.y()*y_rescale);
    xAxis()->setAxisVisible(_plotMaximizedBottomVisible);
    yAxis()->setAxisVisible(_plotMaximizedLeftVisible);
    _leftLabelDetails->setVisible(_plotMaximizedLeftVisible);
    _bottomLabelDetails->setVisible(_plotMaximizedBottomVisible);
    _rightLabelDetails->setVisible(_plotMaximizedRightVisible);
    _topLabelDetails->setVisible(_plotMaximizedTopVisible);
    _plotMaximized = false;
    PlotItemManager::self()->removeFocusPlot(this);
    setParent(_plotMaximizedSourceParent);
    setPos(_plotMaximizedSourcePosition);
    setViewRect(_plotMaximizedSourceRect);
    setZValue(_plotMaximizedSourceZValue);
    parentView()->setChildMaximized(false);
    parentView()->setFontRescale(1.0);
  } else {
    _plotMaximizedBottomVisible = _bottomLabelDetails->isVisible();
    xAxis()->setAxisVisible(true);
    _bottomLabelDetails->setVisible(true);
    _plotMaximizedLeftVisible = _leftLabelDetails->isVisible();
    yAxis()->setAxisVisible(true);
    _leftLabelDetails->setVisible(true);
    _plotMaximizedRightVisible = _rightLabelDetails->isVisible();
    _rightLabelDetails->setVisible(true);
    _plotMaximizedTopVisible = _topLabelDetails->isVisible();
    _topLabelDetails->setVisible(true);
    _plotMaximized = true;
    _plotMaximizedSourcePosition = pos();
    _plotMaximizedSourceRect = viewRect();
    _plotMaximizedSourceZValue = zValue();
    _plotMaximizedSourceParent = parentViewItem();
    _plotMaximizedSourceParentRect = parentView()->sceneRect();

    setParent(0);
    setPos(0, 0);
    setViewRect(parentView()->sceneRect());
    setZValue(PLOT_MAXIMIZED_ZORDER);
    PlotItemManager::self()->setFocusPlot(this);

    double rescale = double(parentView()->sceneRect().width() +
                            parentView()->sceneRect().height())/
                     double(_plotMaximizedSourceRect.width() + _plotMaximizedSourceRect.height());
    parentView()->setChildMaximized(true);
    parentView()->setFontRescale(rescale);
  }
  if (isInSharedAxisBox()) {
    parentView()->setPlotBordersDirty(true);
  }
  setPlotBordersDirty();
}


void PlotItem::zoomFixedExpression(const QRectF &projection, bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomFixedExpression" << projection << "current" << projectionRect();
#endif
  if (projection.isValid()) {
    if (isInSharedAxisBox() && !force) {
      sharedAxisBox()->zoomFixedExpression(projection, this);
    } else {
      ZoomCommand *cmd = new ZoomFixedExpressionCommand(this, projection, force);
      _undoStack->push(cmd);
      cmd->redo();
    }
  }
}


void PlotItem::zoomXRange(const QRectF &projection, bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRange" << projection << endl;
#endif
  if (projection.isValid()) {
    if (isInSharedAxisBox() && !force) {
      sharedAxisBox()->zoomXRange(projection, this);
    } else {
      ZoomCommand *cmd = new ZoomXRangeCommand(this, projection, force);
      _undoStack->push(cmd);
      cmd->redo();
    }
  }
}


void PlotItem::zoomYRange(const QRectF &projection, bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYRange" << projection << endl;
#endif
  if (projection.isValid()) {
    if (isInSharedAxisBox() && !force) {
      sharedAxisBox()->zoomYRange(projection, this);
    } else {
      ZoomCommand *cmd = new ZoomYRangeCommand(this, projection, force);
      _undoStack->push(cmd);
      cmd->redo();
    }
  }
}


void PlotItem::zoomMaximum(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomMaximum" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomMaximum(this);
  } else {
    ZoomCommand *cmd = new ZoomMaximumCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}

void PlotItem::adjustImageColorScale() {
  const double per[] = {0.0, 0.0001, 0.001, 0.005, 0.02};
  const int length = sizeof(per) / sizeof(double);

  if (++_i_per >= length) {
    _i_per = 0;
  }
  foreach (PlotRenderItem *renderer, renderItems()) {
    foreach (RelationPtr relation, renderer->relationList()) {
      if (ImagePtr image = kst_cast<Image>(relation)) {
        image->writeLock();
        image->setThresholdToSpikeInsensitive(per[_i_per]);
        image->registerChange();
        image->unlock();
      }
    }
  }
  UpdateManager::self()->doUpdates(true);
}

void PlotItem::zoomMaxSpikeInsensitive(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomMaxSpikeInsensitive" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomMaxSpikeInsensitive(this);
  } else {
    ZoomCommand *cmd = new ZoomMaxSpikeInsensitiveCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomPrevious() {
#if DEBUG_ZOOM
  qDebug() << "zoomPrevious" << endl;
#endif
  if (!isInSharedAxisBox()) {
    if (_undoStack->canUndo()) {
      QAction *undoAction = _undoStack->createUndoAction(this);
      if (undoAction) {
        undoAction->activate(QAction::Trigger);
      }
    }
  }
}


void PlotItem::zoomTied() {
#if DEBUG_ZOOM
  qDebug() << "zoomTied" << endl;
#endif
  setTiedZoom(!isTiedZoom(), !isTiedZoom());
}


void PlotItem::zoomXTied() {
#if DEBUG_ZOOM
  qDebug() << "zoomXTied" << endl;
#endif
  setTiedZoom(!isXTiedZoom(), isYTiedZoom());
}


void PlotItem::zoomYTied() {
#if DEBUG_ZOOM
  qDebug() << "zoomYTied" << endl;
#endif
  setTiedZoom(isXTiedZoom(), !isYTiedZoom());
}


void PlotItem::zoomMeanCentered(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomMeanCentered" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomMeanCentered(this);
  } else {
    ZoomCommand *cmd = new ZoomMeanCenteredCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYMeanCentered(qreal dY, bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYMeanCentered" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYMeanCentered(this);
  } else {
    ZoomCommand *cmd = new ZoomYMeanCenteredCommand(this, dY, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXMeanCentered(qreal dX, bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXMeanCentered" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXMeanCentered(this);
  } else {
    ZoomCommand *cmd = new ZoomXMeanCenteredCommand(this, dX, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXMaximum(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXMaximum" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXMaximum(this);
  } else {
    ZoomCommand *cmd = new ZoomXMaximumCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXNoSpike(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXNoSpike" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXNoSpike(this);
  } else {
    ZoomCommand *cmd = new ZoomXNoSpikeCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXAutoBorder(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXAutoBorder" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXAutoBorder(this);
  } else {
    ZoomCommand *cmd = new ZoomXAutoBorderCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXRight(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRight" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXRight(this);
  } else {
    ZoomCommand *cmd = new ZoomXRightCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXLeft(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXLeft" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXLeft(this);
  } else {
    ZoomCommand *cmd = new ZoomXLeftCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXOut(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXOut" << endl;
#endif
  resetSelectionRect();
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXOut(this);
  } else {
    ZoomCommand *cmd = new ZoomXOutCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomXIn(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomXIn" << endl;
#endif
  resetSelectionRect();
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomXIn(this);
  } else {
    ZoomCommand *cmd = new ZoomXInCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomNormalizeXtoY(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeXtoY" << endl;
#endif

  if (xAxis()->axisLog() || yAxis()->axisLog())
    return; //apparently we don't want to do anything here according to kst2dplot...

  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomNormalizeXtoY(this);
  } else {
    ZoomCommand *cmd = new ZoomNormalizeXToYCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomLogX(bool force, bool autoLog, bool enableLog) {
#if DEBUG_ZOOM
  qDebug() << "zoomLogX" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomLogX(this);
  } else {
    bool log = enableLog;
    if (autoLog) {
      log = !xAxis()->axisLog();
    }
    ZoomCommand *cmd = new ZoomXLogCommand(this, log, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYLocalMaximum(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYLocalMaximum" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYLocalMaximum(this);
  } else {
    ZoomCommand *cmd = new ZoomYLocalMaximumCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYMaximum(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYMaximum" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYMaximum(this);
  } else {
    ZoomCommand *cmd = new ZoomYMaximumCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYNoSpike(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYNoSpike" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYNoSpike(this);
  } else {
    ZoomCommand *cmd = new ZoomYNoSpikeCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYAutoBorder(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYAutoBorder" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYAutoBorder(this);
  } else {
    ZoomCommand *cmd = new ZoomYAutoBorderCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYUp(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYUp" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYUp(this);
  } else {
    ZoomCommand *cmd = new ZoomYUpCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYDown(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYDown" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYDown(this);
  } else {
    ZoomCommand *cmd = new ZoomYDownCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYOut(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYOut" << endl;
#endif
  resetSelectionRect();
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYOut(this);
  } else {
    ZoomCommand *cmd = new ZoomYOutCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomYIn(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomYIn" << endl;
#endif
  resetSelectionRect();
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomYIn(this);
  } else {
    ZoomCommand *cmd = new ZoomYInCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomNormalizeYtoX(bool force) {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeYtoX" << endl;
#endif

  if (xAxis()->axisLog() || yAxis()->axisLog())
    return; //apparently we don't want to do anything here according to kst2dplot...

  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomNormalizeYtoX(this);
  } else {
    ZoomCommand *cmd = new ZoomNormalizeYToXCommand(this, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
}


void PlotItem::zoomLogY(bool force, bool autoLog, bool enableLog) {
#if DEBUG_ZOOM
  qDebug() << "zoomLogY" << endl;
#endif
  if (isInSharedAxisBox() && !force) {
    sharedAxisBox()->zoomLogY(this);
  } else {
    bool log = enableLog;
    if (autoLog) {
      log = !yAxis()->axisLog();
    }
    ZoomCommand *cmd = new ZoomYLogCommand(this, log, force);
    _undoStack->push(cmd);
    cmd->redo();
  }
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
  setLabelsDirty();
  setAxisLabelsDirty();
  setPlotPixmapDirty();
}

void PlotItem::saveAsDialogDefaults() const {
  _dialogDefaults->setValue("plot/globalFontFamily", QVariant(globalFont()).toString());
  ApplicationSettings::self()->setDefaultFontScale(globalFontScale());
  _dialogDefaults->setValue("plot/globalFontColor", QVariant(globalFontColor()).toString());

  leftLabelDetails()->saveAsDialogDefaults(QString("plot/leftFont"));
  rightLabelDetails()->saveAsDialogDefaults(QString("plot/rightFont"));
  topLabelDetails()->saveAsDialogDefaults(QString("plot/topFont"));
  bottomLabelDetails()->saveAsDialogDefaults(QString("plot/bottomFont"));
  numberLabelDetails()->saveAsDialogDefaults(QString("plot/numberFont"));

  // Save stroke...
  QPen p = pen();
  QBrush b = p.brush();

  _dialogDefaults->setValue("plot/strokeStyle", QVariant(p.style()).toString());
  _dialogDefaults->setValue("plot/strokeWidth", p.widthF());
  _dialogDefaults->setValue("plot/strokeJoinStyle", QVariant(p.joinStyle()).toString());
  _dialogDefaults->setValue("plot/strokeCapStyle", QVariant(p.capStyle()).toString());
  _dialogDefaults->setValue("plot/strokeBrushColor", QVariant(b.color()).toString());
  _dialogDefaults->setValue("plot/strokeBrushStyle", QVariant(b.style()).toString());

  // Save the brush
  b = brush();
  _dialogDefaults->setValue("plot/fillBrushColor", QVariant(b.color()).toString());
  _dialogDefaults->setValue("plot/fillBrushStyle", QVariant(b.style()).toString());
  _dialogDefaults->setValue("plot/fillBrushUseGradient", QVariant(bool(b.gradient())).toString());
  if (b.gradient()) {
    QString stopList;
    foreach(QGradientStop stop, b.gradient()->stops()) {
      qreal point = (qreal)stop.first;
      QColor color = (QColor)stop.second;

      stopList += QString::number(point);
      stopList += ",";
      stopList += color.name();
      stopList += ",";
    }
     _dialogDefaults->setValue("plot/fillBrushGradient", stopList);
   }

  _xAxis->saveAsDialogDefaults("plot/xAxis");
  _yAxis->saveAsDialogDefaults("plot/yAxis");
}


PlotLabel::PlotLabel(PlotItem *plotItem) : QObject(),
  _plotItem(plotItem),
  _visible(true),
  _fontUseGlobal(true),
  _isAuto(true) {

  _fontColor = ApplicationSettings::self()->defaultFontColor();
  _fontScale = ApplicationSettings::self()->defaultFontScale();
  _font = _plotItem->parentView()->defaultFont(_fontScale);
}


void PlotLabel::setDetails(const QString &label, bool is_auto,
                           const bool use_global, const QFont &font, 
                           const qreal scale, const QColor &color) {
  if ((label != _text) || (_isAuto != is_auto) ||
      (use_global != _fontUseGlobal) ||
      (font != _font) ||
      (scale != _fontScale) ||
      (color != _fontColor)) {
    _text = label;
    _isAuto = is_auto;
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
    tempFont.setPointSizeF(_plotItem->parentView()->defaultFont(_plotItem->globalFontScale()).pointSizeF());
  } else {
    tempFont = font();
    tempFont.setPointSizeF(_plotItem->parentView()->defaultFont(fontScale()).pointSizeF());
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


QString PlotLabel::text() const {
  return _text;
}


void PlotLabel::setText(const QString &label) {
  if (label == _text) {
    return;
  }
  _text = label;
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
  xml.writeAttribute("overridetext", _text);
  xml.writeAttribute("autolabel", QVariant(_isAuto).toString());
  xml.writeAttribute("font", QVariant(_font).toString());
  xml.writeAttribute("fontscale", QVariant(_fontScale).toString());
  xml.writeAttribute("fontcolor", QVariant(_fontColor).toString());
  xml.writeAttribute("fontuseglobal", QVariant(_fontUseGlobal).toString());
  xml.writeEndElement();
}


bool PlotLabel::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  Q_UNUSED(store);
  bool validTag = true;

  QString primaryTag = xml.name().toString();
  QXmlStreamAttributes attrs = xml.attributes();
  QStringRef av = attrs.value("visible");
  if (!av.isNull()) {
    setVisible(QVariant(av.toString()).toBool());
  }
  av = attrs.value("overridetext");
  if (!av.isNull()) {
    setText(av.toString());
  }
  av = attrs.value("autolabel");
  if (!av.isNull()) {
    setIsAuto(QVariant(av.toString()).toBool());
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

void PlotLabel::saveAsDialogDefaults(const QString &group) const {
  _dialogDefaults->setValue(group+QString("Global"), fontUseGlobal());
  _dialogDefaults->setValue(group+QString("Family"), QVariant(font()).toString());
  _dialogDefaults->setValue(group+QString("Scale"), fontScale());
  _dialogDefaults->setValue(group+QString("Color"), QVariant(fontColor()).toString());
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
        bool xTiedZoom = false, yTiedZoom = false;
        av = attrs.value("tiedxzoom");
        if (!av.isNull()) {
          xTiedZoom = QVariant(av.toString()).toBool();
        }
        av = attrs.value("tiedyzoom");
        if (!av.isNull()) {
          yTiedZoom = QVariant(av.toString()).toBool();
        }
        rc->setTiedZoom(xTiedZoom, yTiedZoom);
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


ZoomCommand::ZoomCommand(PlotItem *item, const QString &text, bool forced)
    : ViewItemCommand(item, text, false), _plotItem(item) {

  if (!item->isTiedZoom() || forced) {
    _originalStates << item->currentZoomState();
  } else {
    _viewItems = PlotItemManager::tiedZoomViewItems(item);

    QList<PlotItem*> plots = PlotItemManager::tiedZoomPlots(item);
    foreach (PlotItem *plotItem, plots) {
      if (plotItem->isInSharedAxisBox()) {
        if (!_viewItems.contains(plotItem->sharedAxisBox())) {
          _viewItems << plotItem->sharedAxisBox();
        }
      }
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
  bool tiedX = _plotItem->isXTiedZoom();
  bool tiedY = _plotItem->isYTiedZoom();
  foreach (ZoomState state, _originalStates) {
    if (state.item == _plotItem) {
      applyZoomTo(state.item, true, true);
    } else {
      applyZoomTo(state.item, state.item->isXTiedZoom() && tiedX, state.item->isYTiedZoom() && tiedY);
    }
  }
  foreach (ViewItem* item, _viewItems) {
    applyZoomTo(item, tiedX, tiedY);
  }

  kstApp->mainWindow()->document()->setChanged(true);
}


/*
 * X axis zoom to FixedExpression, Y axis zoom to FixedExpression.
 */
void ZoomFixedExpressionCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  if (applyX && applyY) {
    item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(_fixed);
  } else if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(QRectF(_fixed.x(), item->projectionRect().y(), _fixed.width(), item->projectionRect().height()));
  } else if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(QRectF(item->projectionRect().x(), _fixed.y(), item->projectionRect().width(), _fixed.height()));
  }
}


void ZoomFixedExpressionCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX && applyY) {
      shareBox->zoomFixedExpression(_fixed, 0);
    } else if (applyX) {
      shareBox->zoomXRange(_fixed, 0);
    } else if (applyY) {
      shareBox->zoomYRange(_fixed, 0);
    }
  }
}


/*
 * X axis zoom to Range.
 */
void ZoomXRangeCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(QRectF(_fixed.x(), item->projectionRect().y(), _fixed.width(), item->projectionRect().height()));
  }
}


void ZoomXRangeCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXRange(_fixed, 0);
    }
  }
}


/*
 * Y axis zoom to Range.
 */
void ZoomYRangeCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(QRectF(item->projectionRect().x(), _fixed.y(), item->projectionRect().width(), _fixed.height()));
  }
}


void ZoomYRangeCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYRange(_fixed, 0);
    }
  }
}


/*
 * X axis zoom to Auto, Y axis zoom to AutoBorder.
 */
void ZoomMaximumCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  if (applyX && applyY) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    item->yAxis()->setAxisZoomMode(PlotAxis::AutoBorder);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(compute);
  } else if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(), item->projectionRect().y(), compute.width(), item->projectionRect().height()));
  } else if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::AutoBorder);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(), compute.y(), item->projectionRect().width(), compute.height()));
  }
}


void ZoomMaximumCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX && applyY) {
      shareBox->zoomMaximum(0);
    } else if (applyX) {
      shareBox->zoomXMaximum(0);
    } else if (applyY) {
      shareBox->zoomYMaximum(0);
    }
  }
}

/*
 * X axis zoom to Auto, Y axis zoom to SpikeInsensitive.
 */
void ZoomMaxSpikeInsensitiveCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  if (applyX && applyY) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    item->yAxis()->setAxisZoomMode(PlotAxis::SpikeInsensitive);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(compute);
  } else if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(), item->projectionRect().y(), compute.width(), item->projectionRect().height()));
  } else if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::SpikeInsensitive);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(), compute.y(), item->projectionRect().width(), compute.height()));
  }
}


void ZoomMaxSpikeInsensitiveCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX && applyY) {
      shareBox->zoomMaxSpikeInsensitive(0);
    } else if (applyX) {
      shareBox->zoomXMaximum(0);
    } else if (applyY) {
      shareBox->zoomYNoSpike(0);
    }
  }
}


/*
 * X axis zoom to Auto, Y axis zoom to Mean Centered.
 */
void ZoomMeanCenteredCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  if (applyX && applyY) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    item->yAxis()->setAxisZoomMode(PlotAxis::MeanCentered);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(compute);
  } else if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(), item->projectionRect().y(), compute.width(), item->projectionRect().height()));
  } else if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::MeanCentered);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(), compute.y(), item->projectionRect().width(), compute.height()));
  }
}


void ZoomMeanCenteredCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX && applyY) {
      shareBox->zoomMeanCentered(0);
    } else if (applyX) {
      shareBox->zoomXMaximum(0);
    } else if (applyY) {
      shareBox->zoomMeanCentered(0);
    }
  }
}

/*
 * X axis zoom unchanged, Y axis zoom to Mean Centered.
 */
void ZoomYMeanCenteredCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);

  if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::MeanCentered);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(), compute.y(), item->projectionRect().width(), _dY));
  }
}


void ZoomYMeanCenteredCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYMeanCentered(0);
    }
  }
}

/*
 * X axis zoom to Mean Centered, Y axis unchanged.
 */
void ZoomXMeanCenteredCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);

  if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::MeanCentered);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(), item->projectionRect().y(), _dX, item->projectionRect().height()));
  }
}


void ZoomXMeanCenteredCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXMeanCentered(0);
    }
  }
}

/*
 * X axis zoom to auto, Y zoom not changed.
 */
void ZoomXMaximumCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::Auto);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(),
          item->projectionRect().y(),
          compute.width(),
          item->projectionRect().height()));
  }
}


void ZoomXMaximumCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXMaximum(0);
    }
  }
}


/*
 * X axis zoom to auto border, Y zoom not changed.
 */
void ZoomXAutoBorderCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::AutoBorder);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(),
          item->projectionRect().y(),
          compute.width(),
          item->projectionRect().height()));
  }
}


void ZoomXAutoBorderCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXAutoBorder(0);
    }
  }
}


/*
 * X axis zoom to no spike, Y zoom not changed.
 */
void ZoomXNoSpikeCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    item->xAxis()->setAxisZoomMode(PlotAxis::SpikeInsensitive);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(compute.x(),
          item->projectionRect().y(),
          compute.width(),
          item->projectionRect().height()));
  }
}


void ZoomXNoSpikeCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXNoSpike(0);
    }
  }
}


/*
 * X axis zoom changed to fixed and shifted to right:
 *       new_xmin = xmin + (xmax - xmin)*0.10;
 *       new_xmax = xmax + (xmax – xmin)*0.10;
 */
void ZoomXRightCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
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
}


void ZoomXRightCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXRight(0);
    }
  }
}

/*
 * X axis zoom changed to fixed and shifted to :
 *       new_xmin = xmin - (xmax - xmin)*0.10;
 *       new_xmax = xmax - (xmax – xmin)*0.10;
 */
void ZoomXLeftCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
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
}


void ZoomXLeftCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXLeft(0);
    }
  }
}


/*
 * X axis zoom changed to fixed and increased:
 *       new_xmin = xmin - (xmax - xmin)*0.25;
 *       new_xmax = xmax + (xmax – xmin)*0.25;
 */
void ZoomXOutCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
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
}


void ZoomXOutCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXOut(0);
    }
  }
}


/*
 * X axis zoom changed to fixed and decreased:
 *       new_xmin = xmin + (xmax - xmin)*0.1666666;
 *       new_xmax = xmax - (xmax – xmin)*0.1666666;
 */
void ZoomXInCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
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
}


void ZoomXInCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomXIn(0);
    }
  }
}


/*
 * Normalize X axis to Y axis: Given the current plot aspect ratio, change
 * the X axis range to have the same units per mm as the Y axis range. Particularly
 * useful for images.
 */
void ZoomNormalizeXToYCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    QRectF compute = item->projectionRect();
    qreal mean = compute.center().x();
    qreal range = item->plotRect().width() * compute.height() / item->plotRect().height();

    compute.setLeft(mean - (range / 2.0));
    compute.setRight(mean + (range / 2.0));

    item->xAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(compute);
  }
}


void ZoomNormalizeXToYCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomNormalizeXtoY(0);
    }
  }
}


/*
 * When zoomed in in X, auto zoom Y, only
 * counting points within the current X range. (eg, curve goes from x=0 to 100, but
 * we are zoomed in to x = 30 to 40. Adjust Y zoom to include all points with x
 * values between 30 and 40.
 */
void ZoomYLocalMaximumCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    qreal minimum = item->yMax();
    qreal maximum = item->yMin();
    item->computedRelationalMax(minimum, maximum);

    item->computeBorder(Qt::Vertical, minimum, maximum);

    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);

    QRectF compute = item->projectionRect();
    compute.setTop(minimum);
    compute.setBottom(maximum);

    item->setProjectionRect(compute);
  }
}


void ZoomYLocalMaximumCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYLocalMaximum(0);
    }
  }
}


/*
 * Y axis zoom to auto, X zoom not changed.
 */
void ZoomYMaximumCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::Auto);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(),
          compute.y(),
          item->projectionRect().width(),
          compute.height()));
  }
}


void ZoomYMaximumCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYMaximum(0);
    }
  }
}


/*
 * Y axis zoom to auto border, X zoom not changed.
 */
void ZoomYAutoBorderCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::AutoBorder);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(),
          compute.y(),
          item->projectionRect().width(),
          compute.height()));
  }
}


void ZoomYAutoBorderCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYAutoBorder(0);
    }
  }
}


/*
 * Y axis zoom to no spike, X zoom not changed.
 */
void ZoomYNoSpikeCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    item->yAxis()->setAxisZoomMode(PlotAxis::SpikeInsensitive);
    QRectF compute = item->computedProjectionRect();
    item->setProjectionRect(QRectF(item->projectionRect().x(),
          compute.y(),
          item->projectionRect().width(),
          compute.height()));
  }
}


void ZoomYNoSpikeCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYNoSpike(0);
    }
  }
}


/*
 * Y axis zoom up. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin + (ymax - ymin)*0.1;
 *             new_ymax = ymax + (ymax - ymin)*0.1;
 */
void ZoomYUpCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
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
}


void ZoomYUpCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYUp(0);
    }
  }
}


/*
 * Y axis zoom down. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin - (ymax - ymin)*0.10;
 *             new_ymax = ymax - (ymax - ymin)*0.10;
 */
void ZoomYDownCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
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
}


void ZoomYDownCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYDown(0);
    }
  }
}


/*
 * Y axis zoom increased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin - (ymax - ymin)*0.25;
 *             new_ymax = ymax + (ymax - ymin)*0.25;
 */
void ZoomYOutCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
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
}


void ZoomYOutCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYOut(0);
    }
  }
}


/*
 * Y axis zoom decreased. If the Y zoom mode is not
 * Mean Centered, change to Fixed (expression).
 *             new_ymin = ymin + (ymax - ymin)*0.1666666;
 *             new_ymax = ymax - (ymax – ymin)*0.1666666;
 */
void ZoomYInCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
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
}


void ZoomYInCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomYIn(0);
    }
  }
}


/*
 * Normalize Y axis to X axis: Given the current plot aspect ratio,
 * change the Y axis range to have the same units per mm as the X axis range.
 * Particularly useful for images.
 */
void ZoomNormalizeYToXCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    QRectF compute = item->projectionRect();
    qreal mean = compute.center().y();
    qreal range = item->plotRect().height() * compute.width() / item->plotRect().width();

    compute.setTop(mean - (range / 2.0));
    compute.setBottom(mean + (range / 2.0));

    item->yAxis()->setAxisZoomMode(PlotAxis::FixedExpression);
    item->setProjectionRect(compute);
  }
}


void ZoomNormalizeYToXCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomNormalizeYtoX(0);
    }
  }
}


/*
 * Switch the X Axis to a Log Scale.
 */
void ZoomXLogCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  if (applyX) {
    item->xAxis()->setAxisLog(_enableLog);
    item->setProjectionRect(item->computedProjectionRect(), true); //need to recompute
  }
}


void ZoomXLogCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyY);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyX) {
      shareBox->zoomLogX(0, false, _enableLog);
    }
  }
}


/*
 * Switch the Y Axis to a Log Scale.
 */
void ZoomYLogCommand::applyZoomTo(PlotItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  if (applyY) {
    item->yAxis()->setAxisLog(_enableLog);
    item->setProjectionRect(item->computedProjectionRect(), true); //need to recompute
  }
}


void ZoomYLogCommand::applyZoomTo(ViewItem *item, bool applyX, bool applyY) {
  Q_UNUSED(applyX);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(item);
  if (shareBox) {
    if (applyY) {
      shareBox->zoomLogY(0, false, _enableLog);
    }
  }
}

}

// vim: ts=2 sw=2 et
