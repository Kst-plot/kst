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

#include "plotitemdialog.h"

#include "contenttab.h"
#include "axistab.h"
#include "rangetab.h"
#include "markerstab.h"
#include "labeltab.h"
#include "overridelabeltab.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "curve.h"
#include "curvedialog.h"
#include "plotrenderitem.h"
#include "plotitem.h"
#include "image.h"
#include "imagedialog.h"

#include "filltab.h"
#include "stroketab.h"

namespace Kst {

PlotItemDialog::PlotItemDialog(PlotItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _plotItem(item) {

  _store = kstApp->mainWindow()->document()->objectStore();

  setWindowTitle(tr("Edit Plot Item"));

  _labelTab = new LabelTab(_plotItem, this);
  _topLabelTab = new OverrideLabelTab(tr("Top Font"), this);
  _bottomLabelTab = new OverrideLabelTab(tr("Bottom Font"), this);
  _leftLabelTab = new OverrideLabelTab(tr("Left Font"), this);
  _rightLabelTab = new OverrideLabelTab(tr("Right Font"), this);
  _axisLabelTab = new OverrideLabelTab(tr("Axis Font"), this);

  _labelPage = new DialogPageTab(this);
  _labelPage->setPageTitle(tr("Labels"));
  _labelPage->addDialogTab(_labelTab);
  _labelPage->addDialogTab(_topLabelTab);
  _labelPage->addDialogTab(_bottomLabelTab);
  _labelPage->addDialogTab(_leftLabelTab);
  _labelPage->addDialogTab(_rightLabelTab);
  _labelPage->addDialogTab(_axisLabelTab);
  addDialogPage(_labelPage);

  connect(_labelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_topLabelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_bottomLabelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_leftLabelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_rightLabelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_axisLabelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_labelTab, SIGNAL(globalFontUpdate()), this, SLOT(globalFontUpdate()));

  connect(_topLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useTopDefaultChanged(bool)));
  connect(_bottomLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useBottomDefaultChanged(bool)));
  connect(_leftLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useLeftDefaultChanged(bool)));
  connect(_rightLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useRightDefaultChanged(bool)));
  connect(_axisLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useAxisDefaultChanged(bool)));

  _xAxisTab = new AxisTab(this);
  DialogPage *xAxisPage = new DialogPage(this);
  xAxisPage->setPageTitle(tr("x-Axis"));
  xAxisPage->addDialogTab(_xAxisTab);
  addDialogPage(xAxisPage);
  connect(_xAxisTab, SIGNAL(apply()), this, SLOT(xAxisChanged()));

  _yAxisTab = new AxisTab(this);
  DialogPage *yAxisPage = new DialogPage(this);
  yAxisPage->setPageTitle(tr("y-Axis"));
  yAxisPage->addDialogTab(_yAxisTab);
  addDialogPage(yAxisPage);
  connect(_yAxisTab, SIGNAL(apply()), this, SLOT(yAxisChanged()));

  _rangeTab = new RangeTab(_plotItem, this);
  DialogPage *rangePage = new DialogPage(this);
  rangePage->setPageTitle(tr("Range"));
  rangePage->addDialogTab(_rangeTab);
  addDialogPage(rangePage);
  connect(_rangeTab, SIGNAL(apply()), this, SLOT(rangeChanged()));

  _xMarkersTab = new MarkersTab(this);
  DialogPage *xMarkersPage = new DialogPage(this);
  xMarkersPage->setPageTitle(tr("x-Axis markers"));
  xMarkersPage->addDialogTab(_xMarkersTab);
  addDialogPage(xMarkersPage);
  _xMarkersTab->setObjectStore(_store);
  connect(_xMarkersTab, SIGNAL(apply()), this, SLOT(xAxisPlotMarkersChanged()));

  _yMarkersTab = new MarkersTab(this);
  DialogPage *yMarkersPage = new DialogPage(this);
  yMarkersPage->setPageTitle(tr("y-Axis markers"));
  yMarkersPage->addDialogTab(_yMarkersTab);
  addDialogPage(yMarkersPage);
  _yMarkersTab->setObjectStore(_store);
  connect(yMarkersPage, SIGNAL(apply()), this, SLOT(yAxisPlotMarkersChanged()));

  _contentTab = new ContentTab(this);
  connect(_contentTab, SIGNAL(apply()), this, SLOT(contentChanged()));

  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tr("Content"));
  page->addDialogTab(_contentTab);
  addDialogPage(page);

  addRelations();

  setupContent();
  setupAxis();
  setupRange();
  setupLabels();
  setupMarkers();
}


PlotItemDialog::~PlotItemDialog() {
}


void PlotItemDialog::setupLabels() {
  Q_ASSERT(_plotItem);

  _labelTab->setLeftLabel(_plotItem->leftLabel());
  _labelTab->setBottomLabel(_plotItem->bottomLabel());
  _labelTab->setTopLabel(_plotItem->topLabel());
  _labelTab->setRightLabel(_plotItem->rightLabel());

  _labelTab->setLeftLabelAuto(_plotItem->leftLabelDetails()->isAuto());
  _labelTab->setBottomLabelAuto(_plotItem->bottomLabelDetails()->isAuto());
  _labelTab->setTopLabelAuto(_plotItem->topLabelDetails()->isAuto());
  _labelTab->setRightLabelAuto(_plotItem->rightLabelDetails()->isAuto());

  _labelTab->setShowLegend(_plotItem->showLegend());

  _topLabelTab->setUseDefault(_plotItem->topLabelDetails()->fontUseGlobal());
  _topLabelTab->setLabelFont(_plotItem->topLabelDetails()->font());
  _topLabelTab->setLabelFontScale(_plotItem->topLabelDetails()->fontScale());
  _topLabelTab->setLabelColor(_plotItem->topLabelDetails()->fontColor());

  _bottomLabelTab->setUseDefault(_plotItem->bottomLabelDetails()->fontUseGlobal());
  _bottomLabelTab->setLabelFont(_plotItem->bottomLabelDetails()->font());
  _bottomLabelTab->setLabelFontScale(_plotItem->bottomLabelDetails()->fontScale());
  _bottomLabelTab->setLabelColor(_plotItem->bottomLabelDetails()->fontColor());

  _leftLabelTab->setUseDefault(_plotItem->leftLabelDetails()->fontUseGlobal());
  _leftLabelTab->setLabelFont(_plotItem->leftLabelDetails()->font());
  _leftLabelTab->setLabelFontScale(_plotItem->leftLabelDetails()->fontScale());
  _leftLabelTab->setLabelColor(_plotItem->leftLabelDetails()->fontColor());

  _rightLabelTab->setUseDefault(_plotItem->rightLabelDetails()->fontUseGlobal());
  _rightLabelTab->setLabelFont(_plotItem->rightLabelDetails()->font());
  _rightLabelTab->setLabelFontScale(_plotItem->rightLabelDetails()->fontScale());
  _rightLabelTab->setLabelColor(_plotItem->rightLabelDetails()->fontColor());

  _axisLabelTab->setUseDefault(_plotItem->numberLabelDetails()->fontUseGlobal());
  _axisLabelTab->setLabelFont(_plotItem->numberLabelDetails()->font());
  _axisLabelTab->setLabelFontScale(_plotItem->numberLabelDetails()->fontScale());
  _axisLabelTab->setLabelColor(_plotItem->numberLabelDetails()->fontColor());
}


void PlotItemDialog::setupRange() {
  _rangeTab->setupRange();
}


void PlotItemDialog::setupAxis() {
  Q_ASSERT(_plotItem);

  _xAxisTab->setAxisMajorTickSpacing(_plotItem->xAxis()->axisMajorTickMode());
  _xAxisTab->setDrawAxisMajorTicks(_plotItem->xAxis()->drawAxisMajorTicks());
  _xAxisTab->setDrawAxisMajorGridLines(_plotItem->xAxis()->drawAxisMajorGridLines());
  _xAxisTab->setDrawAxisMinorTicks(_plotItem->xAxis()->drawAxisMinorTicks());
  _xAxisTab->setDrawAxisMinorGridLines(_plotItem->xAxis()->drawAxisMinorGridLines());
  _xAxisTab->setAxisMajorGridLineColor(_plotItem->xAxis()->axisMajorGridLineColor());
  _xAxisTab->setAxisMinorGridLineColor(_plotItem->xAxis()->axisMinorGridLineColor());
  _xAxisTab->setAxisMajorGridLineStyle(_plotItem->xAxis()->axisMajorGridLineStyle());
  _xAxisTab->setAxisMinorGridLineStyle(_plotItem->xAxis()->axisMinorGridLineStyle());
  _xAxisTab->setLog(_plotItem->xAxis()->axisLog());
  _xAxisTab->setReversed(_plotItem->xAxis()->axisReversed());
  _xAxisTab->setAutoBaseOffset(_plotItem->xAxis()->axisAutoBaseOffset());
  _xAxisTab->setBaseOffset(_plotItem->xAxis()->axisBaseOffset());
  _xAxisTab->setInterpret(_plotItem->xAxis()->axisInterpret());
  _xAxisTab->setAxisDisplay(_plotItem->xAxis()->axisDisplay());
  _xAxisTab->setAxisInterpretation(_plotItem->xAxis()->axisInterpretation());
  _xAxisTab->setAxisMinorTickCount(_plotItem->xAxis()->axisMinorTickCount());
  _xAxisTab->setSignificantDigits(_plotItem->xAxis()->axisSignificantDigits());

  _yAxisTab->setAxisMajorTickSpacing(_plotItem->yAxis()->axisMajorTickMode());
  _yAxisTab->setDrawAxisMajorTicks(_plotItem->yAxis()->drawAxisMajorTicks());
  _yAxisTab->setDrawAxisMajorGridLines(_plotItem->yAxis()->drawAxisMajorGridLines());
  _yAxisTab->setDrawAxisMinorTicks(_plotItem->yAxis()->drawAxisMinorTicks());
  _yAxisTab->setDrawAxisMinorGridLines(_plotItem->yAxis()->drawAxisMinorGridLines());
  _yAxisTab->setAxisMajorGridLineColor(_plotItem->yAxis()->axisMajorGridLineColor());
  _yAxisTab->setAxisMinorGridLineColor(_plotItem->yAxis()->axisMinorGridLineColor());
  _yAxisTab->setAxisMajorGridLineStyle(_plotItem->yAxis()->axisMajorGridLineStyle());
  _yAxisTab->setAxisMinorGridLineStyle(_plotItem->yAxis()->axisMinorGridLineStyle());
  _yAxisTab->setLog(_plotItem->yAxis()->axisLog());
  _yAxisTab->setReversed(_plotItem->yAxis()->axisReversed());
  _yAxisTab->setAutoBaseOffset(_plotItem->yAxis()->axisAutoBaseOffset());
  _yAxisTab->setBaseOffset(_plotItem->yAxis()->axisBaseOffset());
  _yAxisTab->setInterpret(_plotItem->yAxis()->axisInterpret());
  _yAxisTab->setAxisDisplay(_plotItem->yAxis()->axisDisplay());
  _yAxisTab->setAxisInterpretation(_plotItem->yAxis()->axisInterpretation());
  _yAxisTab->setAxisMinorTickCount(_plotItem->yAxis()->axisMinorTickCount());
  _yAxisTab->setSignificantDigits(_plotItem->yAxis()->axisSignificantDigits());
}


void PlotItemDialog::setupMarkers() {
  Q_ASSERT(_plotItem);

  _xMarkersTab->setPlotMarkers(_plotItem->xAxis()->axisPlotMarkers());
  _yMarkersTab->setPlotMarkers(_plotItem->yAxis()->axisPlotMarkers());
}


void PlotItemDialog::setupContent() {
  QStringList displayedRelations;
  QStringList availableRelations;
  QStringList allRelations;
  QStringList displayedRelationTips;
  QStringList availableRelationTips;
  QStringList allRelationTips;

  CurveList curves = _store->getObjects<Curve>();
  ImageList images = _store->getObjects<Image>();

  foreach (RelationPtr relation, _plotItem->renderItem(PlotRenderItem::Cartesian)->relationList()) {
    displayedRelations.append(relation->Name());
    displayedRelationTips.append(relation->descriptionTip());
  }

  foreach (CurvePtr curve, curves) {
    allRelations.append(curve->Name());
    allRelationTips.append(curve->descriptionTip());
    if (!displayedRelations.contains(curve->Name())) {
      availableRelations.append(curve->Name());
      availableRelationTips.append(curve->descriptionTip());
    }
  }

  foreach (ImagePtr image, images) {
    allRelations.append(image->Name());
    allRelationTips.append(image->descriptionTip());
    if (!displayedRelations.contains(image->Name())) {
      availableRelations.append(image->Name());
      availableRelationTips.append(image->descriptionTip());
    }
  }

  _contentTab->setDisplayedRelations(displayedRelations, displayedRelationTips);
  _contentTab->setAvailableRelations(availableRelations, availableRelationTips);
}


void PlotItemDialog::addRelations() {
  foreach (RelationPtr relation, _plotItem->renderItem(PlotRenderItem::Cartesian)->relationList()) {
    if (CurvePtr curve = kst_cast<Curve>(relation)) {
      CurveTab* curveTab = new CurveTab(this);

      curveTab->setObjectStore(_store);
      curveTab->setXVector(curve->xVector());
      curveTab->setYVector(curve->yVector());
      if (curve->hasXError()) {
        curveTab->setXError(curve->xErrorVector());
      }
      if (curve->hasYError()) {
      curveTab->setYError(curve->yErrorVector());
      }
      if (curve->hasXMinusError()) {
      curveTab->setXMinusError(curve->xMinusErrorVector());
      }
      if (curve->hasYMinusError()) {
        curveTab->setYMinusError(curve->yMinusErrorVector());
      }
      curveTab->curveAppearance()->setColor(curve->color());
      curveTab->curveAppearance()->setShowPoints(curve->hasPoints());
      curveTab->curveAppearance()->setShowLines(curve->hasLines());
      curveTab->curveAppearance()->setShowBars(curve->hasBars());
      curveTab->curveAppearance()->setLineWidth(curve->lineWidth());
      curveTab->curveAppearance()->setLineStyle(curve->lineStyle());
      curveTab->curveAppearance()->setPointType(curve->pointType());
      curveTab->curveAppearance()->setPointDensity(curve->pointDensity());
      curveTab->curveAppearance()->setBarStyle(curve->barStyle());
      curveTab->hidePlacementOptions();

      DialogPage *curvePage = new DialogPage(this);
      curvePage->setPageTitle(curve->Name());
      curvePage->addDialogTab(curveTab);
      addDialogPage(curvePage);
      _relationPages.append(curvePage);
    } else if (ImagePtr image = kst_cast<Image>(relation)) {
      ImageTab* imageTab = new ImageTab(this);
      imageTab->setObjectStore(_store);

      imageTab->setMatrix(image->matrix());

      if (image->hasContourMap() && image->hasColorMap()) {
        imageTab->setColorAndContour(true);
        imageTab->setNumberOfContourLines(image->numContourLines());
        imageTab->setContourColor(image->contourColor());
        imageTab->setContourWeight(image->contourWeight());
        imageTab->setLowerThreshold(image->lowerThreshold());
        imageTab->setUpperThreshold(image->upperThreshold());
        imageTab->setRealTimeAutoThreshold(image->autoThreshold());
        imageTab->colorPalette()->setPalette(image->paletteName());
        imageTab->setUseVariableLineWeight(image->contourWeight() == -1);

      } else if (image->hasContourMap()) {
        imageTab->setContourOnly(true);
        imageTab->setNumberOfContourLines(image->numContourLines());
        imageTab->setContourColor(image->contourColor());
        imageTab->setContourWeight(image->contourWeight());
        imageTab->setUseVariableLineWeight(image->contourWeight() == -1);
      } else {
        imageTab->setColorOnly(true);
        imageTab->setLowerThreshold(image->lowerThreshold());
        imageTab->setUpperThreshold(image->upperThreshold());
        imageTab->setRealTimeAutoThreshold(image->autoThreshold());
        imageTab->colorPalette()->setPalette(image->paletteName());
      }
      imageTab->hidePlacementOptions();

      DialogPage *imagePage = new DialogPage(this);
      imagePage->setPageTitle(image->Name());
      imagePage->addDialogTab(imageTab);
      addDialogPage(imagePage);
      _relationPages.append(imagePage);
    }
  }
}


void PlotItemDialog::updateRelations() {
  foreach(DialogPage* page, _relationPages) {
    removeDialogPage(page);
  }
  _relationPages.clear();

  addRelations();
}


void PlotItemDialog::contentChanged() {
  relationChanged();

  QStringList currentRelations;
  QStringList displayedRelations = _contentTab->displayedRelations();

  foreach (RelationPtr relation, _plotItem->renderItem(PlotRenderItem::Cartesian)->relationList()) {
    currentRelations.append(relation->Name());
    if (!displayedRelations.contains(relation->Name())) {
      _plotItem->renderItem(PlotRenderItem::Cartesian)->removeRelation(relation);
      _plotItem->update();
    }
  }

  foreach (QString relationName, displayedRelations) {
    if (!currentRelations.contains(relationName)) {
      if (RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(relationName))) {
        _plotItem->renderItem(PlotRenderItem::Cartesian)->addRelation(relation);
        _plotItem->update();
      }
    }
  }

  updateRelations();
}


void PlotItemDialog::relationChanged() {
  foreach(DialogPage* page, _relationPages) {
    if (CurvePtr curve = kst_cast<Curve>(_store->retrieveObject(page->pageTitle()))) {
      CurveTab* curveTab = static_cast<CurveTab*>(page->currentWidget());
      if (curveTab) {
        curve->writeLock();
        curve->setXVector(curveTab->xVector());
        curve->setYVector(curveTab->yVector());
        curve->setXError(curveTab->xError());
        curve->setYError(curveTab->yError());
        curve->setXMinusError(curveTab->xMinusError());
        curve->setYMinusError(curveTab->yMinusError());
        curve->setColor(curveTab->curveAppearance()->color());
        curve->setHasPoints(curveTab->curveAppearance()->showPoints());
        curve->setHasLines(curveTab->curveAppearance()->showLines());
        curve->setHasBars(curveTab->curveAppearance()->showBars());
        curve->setLineWidth(curveTab->curveAppearance()->lineWidth());
        curve->setLineStyle(curveTab->curveAppearance()->lineStyle());
        curve->setPointType(curveTab->curveAppearance()->pointType());
        curve->setPointDensity(curveTab->curveAppearance()->pointDensity());
        curve->setBarStyle(curveTab->curveAppearance()->barStyle());

        curve->processUpdate(curve);
        curve->unlock();
      }
    } else if (ImagePtr image = kst_cast<Image>(_store->retrieveObject(page->pageTitle()))) {
      ImageTab* imageTab = static_cast<ImageTab*>(page->currentWidget());
      if (imageTab) {
        image->writeLock();
        if (imageTab->colorOnly()) {
          image->changeToColorOnly(imageTab->matrix(),
              imageTab->lowerThreshold(),
              imageTab->upperThreshold(),
              imageTab->realTimeAutoThreshold(),
              imageTab->colorPalette()->selectedPalette());
        } else if (imageTab->contourOnly()) {
          image->changeToContourOnly(imageTab->matrix(),
              imageTab->numberOfContourLines(),
              imageTab->contourColor(),
              imageTab->useVariableLineWeight() ? -1 : imageTab->contourWeight());
        } else {
          image->changeToColorAndContour(imageTab->matrix(),
              imageTab->lowerThreshold(),
              imageTab->upperThreshold(),
              imageTab->realTimeAutoThreshold(),
              imageTab->colorPalette()->selectedPalette(),
              imageTab->numberOfContourLines(),
              imageTab->contourColor(),
              imageTab->useVariableLineWeight() ? -1 : imageTab->contourWeight());
        }

        image->processUpdate(image);
        image->unlock();
      }
    }
  }
}
  QPointer<PlotItem> item;
  QRectF projectionRect;
  int xAxisZoomMode;
  int yAxisZoomMode;
  bool isXAxisLog;
  bool isYAxisLog;
  qreal xLogBase;
  qreal yLogBase;

void PlotItemDialog::rangeChanged() {
  Q_ASSERT(_plotItem);
  ZoomState zoomstate;
  double x;
  double y;
  double w;
  double h;

  zoomstate = _plotItem->currentZoomState();

  zoomstate.item = _plotItem;
  if (_rangeTab->xMean()) {
    w = (_rangeTab->xRange());
    x = (_plotItem->xMin() + _plotItem->xMax()-w)/2.0;
  } else {
    x = qMin(_rangeTab->xMax(), _rangeTab->xMin());
    w = fabs(_rangeTab->xMax() - _rangeTab->xMin());
  }
  if (_rangeTab->yMean()) {
    h = (_rangeTab->yRange());
    y = (_plotItem->yMax() + _plotItem->yMin()-h)/2;
  } else {
    y = qMin(_rangeTab->yMax(), _rangeTab->yMin());
    h = fabs(_rangeTab->yMax() - _rangeTab->yMin());
  }

  if (w == 0.0) w = 0.2;
  if (h == 0.0) h = 0.2;

  if (_rangeTab->xAuto()) {
  } else if (_rangeTab->xSpike()) {
    zoomstate.xAxisZoomMode = PlotAxis::SpikeInsensitive;
  } else if (_rangeTab->xBorder()) {
    zoomstate.xAxisZoomMode = PlotAxis::AutoBorder;
  } else if (_rangeTab->xMean()) {
    zoomstate.xAxisZoomMode = PlotAxis::MeanCentered;
  } else if (_rangeTab->xFixed()) {
    zoomstate.xAxisZoomMode = PlotAxis::FixedExpression;
  }

  if (_rangeTab->yAuto()) {
    zoomstate.yAxisZoomMode = PlotAxis::Auto;
  } else if (_rangeTab->ySpike()) {
    zoomstate.yAxisZoomMode = PlotAxis::SpikeInsensitive;
  } else if (_rangeTab->yBorder()) {
    zoomstate.yAxisZoomMode = PlotAxis::AutoBorder;
  } else if (_rangeTab->yMean()) {
    zoomstate.yAxisZoomMode = PlotAxis::MeanCentered;
  } else if (_rangeTab->yFixed()) {
    zoomstate.yAxisZoomMode = PlotAxis::FixedExpression;
  }
  zoomstate.projectionRect = QRectF(x,y,w,h);

  _plotItem->zoomGeneral(zoomstate);
}

void PlotItemDialog::xAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->xAxis()->setAxisMajorTickMode(_xAxisTab->axisMajorTickSpacing());
  _plotItem->xAxis()->setDrawAxisMajorTicks(_xAxisTab->drawAxisMajorTicks());
  _plotItem->xAxis()->setDrawAxisMajorGridLines(_xAxisTab->drawAxisMajorGridLines());
  _plotItem->xAxis()->setDrawAxisMinorTicks(_xAxisTab->drawAxisMinorTicks());
  _plotItem->xAxis()->setDrawAxisMinorGridLines(_xAxisTab->drawAxisMinorGridLines());
  _plotItem->xAxis()->setAxisMajorGridLineColor(_xAxisTab->axisMajorGridLineColor());
  _plotItem->xAxis()->setAxisMinorGridLineColor(_xAxisTab->axisMinorGridLineColor());
  _plotItem->xAxis()->setAxisMajorGridLineStyle(_xAxisTab->axisMajorGridLineStyle());
  _plotItem->xAxis()->setAxisMinorGridLineStyle(_xAxisTab->axisMinorGridLineStyle());
  _plotItem->xAxis()->setAxisLog(_xAxisTab->isLog());
  _plotItem->xAxis()->setAxisReversed(_xAxisTab->isReversed());
  _plotItem->xAxis()->setAxisInterpret(_xAxisTab->isInterpret());
  _plotItem->xAxis()->setAxisDisplay(_xAxisTab->axisDisplay());
  _plotItem->xAxis()->setAxisInterpretation(_xAxisTab->axisInterpretation());
  _plotItem->xAxis()->setAxisBaseOffset(_xAxisTab->isBaseOffset());
  _plotItem->xAxis()->setAxisAutoBaseOffset(_xAxisTab->isAutoBaseOffset());
  _plotItem->xAxis()->setAxisMinorTickCount(_xAxisTab->axisMinorTickCount());
  _plotItem->xAxis()->setAxisSignificantDigits(_xAxisTab->significantDigits());
  _plotItem->setProjectionRect(_plotItem->projectionRect(), _plotItem->xAxis()->isDirty());
}


void PlotItemDialog::yAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->yAxis()->setAxisMajorTickMode(_yAxisTab->axisMajorTickSpacing());
  _plotItem->yAxis()->setDrawAxisMajorTicks(_yAxisTab->drawAxisMajorTicks());
  _plotItem->yAxis()->setDrawAxisMajorGridLines(_yAxisTab->drawAxisMajorGridLines());
  _plotItem->yAxis()->setDrawAxisMinorTicks(_yAxisTab->drawAxisMinorTicks());
  _plotItem->yAxis()->setDrawAxisMinorGridLines(_yAxisTab->drawAxisMinorGridLines());
  _plotItem->yAxis()->setAxisMajorGridLineColor(_yAxisTab->axisMajorGridLineColor());
  _plotItem->yAxis()->setAxisMinorGridLineColor(_yAxisTab->axisMinorGridLineColor());
  _plotItem->yAxis()->setAxisMajorGridLineStyle(_yAxisTab->axisMajorGridLineStyle());
  _plotItem->yAxis()->setAxisMinorGridLineStyle(_yAxisTab->axisMinorGridLineStyle());
  _plotItem->yAxis()->setAxisLog(_yAxisTab->isLog());
  _plotItem->yAxis()->setAxisReversed(_yAxisTab->isReversed());
  _plotItem->yAxis()->setAxisInterpret(_yAxisTab->isInterpret());
  _plotItem->yAxis()->setAxisDisplay(_yAxisTab->axisDisplay());
  _plotItem->yAxis()->setAxisInterpretation(_yAxisTab->axisInterpretation());
  _plotItem->yAxis()->setAxisAutoBaseOffset(_yAxisTab->isAutoBaseOffset());
  _plotItem->yAxis()->setAxisBaseOffset(_yAxisTab->isBaseOffset());
  _plotItem->yAxis()->setAxisMinorTickCount(_yAxisTab->axisMinorTickCount());
  _plotItem->yAxis()->setAxisSignificantDigits(_yAxisTab->significantDigits());
  _plotItem->setProjectionRect(_plotItem->projectionRect(), _plotItem->yAxis()->isDirty());
}


void PlotItemDialog::labelsChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->leftLabelDetails()->setDetails(_labelTab->leftLabel(), _labelTab->leftLabelAuto(), _leftLabelTab->useDefault(), _leftLabelTab->labelFont(), _leftLabelTab->labelFontScale(), _leftLabelTab->labelColor());
  _plotItem->bottomLabelDetails()->setDetails(_labelTab->bottomLabel(), _labelTab->bottomLabelAuto(), _bottomLabelTab->useDefault(), _bottomLabelTab->labelFont(), _bottomLabelTab->labelFontScale(), _bottomLabelTab->labelColor());
  _plotItem->rightLabelDetails()->setDetails(_labelTab->rightLabel(), _labelTab->rightLabelAuto(), _rightLabelTab->useDefault(), _rightLabelTab->labelFont(), _rightLabelTab->labelFontScale(), _rightLabelTab->labelColor());
  _plotItem->topLabelDetails()->setDetails(_labelTab->topLabel(), _labelTab->topLabelAuto(), _topLabelTab->useDefault(), _topLabelTab->labelFont(), _topLabelTab->labelFontScale(), _topLabelTab->labelColor());
  _plotItem->numberLabelDetails()->setDetails(QString(), false, _axisLabelTab->useDefault(), _axisLabelTab->labelFont(), _axisLabelTab->labelFontScale(), _axisLabelTab->labelColor());

  _plotItem->setGlobalFont(_labelTab->globalLabelFont());
  _plotItem->setGlobalFontScale(_labelTab->globalLabelFontScale());
  _plotItem->setGlobalFontColor(_labelTab->globalLabelColor());

  _plotItem->setShowLegend(_labelTab->showLegend());
}


void PlotItemDialog::xAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  _plotItem->xAxis()->setAxisPlotMarkers(_xMarkersTab->plotMarkers());
}


void PlotItemDialog::yAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  _plotItem->yAxis()->setAxisPlotMarkers(_yMarkersTab->plotMarkers());
}

void PlotItemDialog::useTopDefaultChanged(bool use) {
  if (use) {
    _labelPage->setTabText(1, tr("Top Font"));
    globalFontUpdate();
  } else {
    _labelPage->setTabText(1, tr("Top Font*"));
  }
}


void PlotItemDialog::useBottomDefaultChanged(bool use) {
  if (use) {
    _labelPage->setTabText(2, tr("Bottom Font"));
    globalFontUpdate();
  } else {
    _labelPage->setTabText(2, tr("Bottom Font*"));
  }
}


void PlotItemDialog::useLeftDefaultChanged(bool use) {
  if (use) {
    _labelPage->setTabText(3, tr("Left Font"));
    globalFontUpdate();
  } else {
    _labelPage->setTabText(3, tr("Left Font*"));
  }
}


void PlotItemDialog::useRightDefaultChanged(bool use) {
  if (use) {
    _labelPage->setTabText(4, tr("Right Font"));
    globalFontUpdate();
  } else {
    _labelPage->setTabText(4, tr("Right Font*"));
  }
}


void PlotItemDialog::useAxisDefaultChanged(bool use) {
  if (use) {
    _labelPage->setTabText(5, tr("Axis Font"));
    globalFontUpdate();
  } else {
    _labelPage->setTabText(5, tr("Axis Font*"));
  }
}


void PlotItemDialog::globalFontUpdate() {
  qreal fontScale = _labelTab->globalLabelFontScale();
  QFont font = _labelTab->globalLabelFont();
  QColor color = _labelTab->globalLabelColor();


  _topLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _bottomLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _leftLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _rightLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _axisLabelTab->setFontSpecsIfDefault(font, fontScale, color);
}

}

// vim: ts=2 sw=2 et
