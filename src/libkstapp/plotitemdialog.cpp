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

#include "math_kst.h"

namespace Kst {

PlotItemDialog::PlotItemDialog(PlotItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _plotItem(item), _defaultTagString("<Auto Name>") {

  Q_ASSERT(_plotItem);

  _store = kstApp->mainWindow()->document()->objectStore();

  setWindowTitle(tr("Edit Plot Item"));

  _contentTab = new ContentTab(this);
  connect(_contentTab, SIGNAL(apply()), this, SLOT(contentChanged()));
  DialogPage *contentsPage = new DialogPage(this);
  contentsPage->setPageTitle(tr("Contents"));
  contentsPage->addDialogTab(_contentTab);
  addDialogPage(contentsPage, true);

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
  addDialogPage(_labelPage, true);

  connect(_labelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));
  connect(_labelTab, SIGNAL(globalFontUpdate()), this, SLOT(globalFontUpdate()));

  connect(_topLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useTopDefaultChanged(bool)));
  connect(_bottomLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useBottomDefaultChanged(bool)));
  connect(_leftLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useLeftDefaultChanged(bool)));
  connect(_rightLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useRightDefaultChanged(bool)));
  connect(_axisLabelTab, SIGNAL(useDefaultChanged(bool)), this, SLOT(useAxisDefaultChanged(bool)));

  _rangeTab = new RangeTab(_plotItem, this);
  DialogPage *rangePage = new DialogPage(this);
  rangePage->setPageTitle(tr("Range/Zoom"));
  rangePage->addDialogTab(_rangeTab);
  addDialogPage(rangePage, true);
  connect(_rangeTab, SIGNAL(apply()), this, SLOT(rangeChanged()));

  _xAxisTab = new AxisTab(this);
  DialogPage *xAxisPage = new DialogPage(this);
  xAxisPage->setPageTitle(tr("X-Axis"));
  xAxisPage->addDialogTab(_xAxisTab);
  addDialogPage(xAxisPage, true);
  connect(_xAxisTab, SIGNAL(apply()), this, SLOT(xAxisChanged()));

  _yAxisTab = new AxisTab(this);
  DialogPage *yAxisPage = new DialogPage(this);
  yAxisPage->setPageTitle(tr("Y-Axis"));
  yAxisPage->addDialogTab(_yAxisTab);
  addDialogPage(yAxisPage, true);
  connect(_yAxisTab, SIGNAL(apply()), this, SLOT(yAxisChanged()));

  _xMarkersTab = new MarkersTab(this);
  DialogPage *xMarkersPage = new DialogPage(this);
  xMarkersPage->setPageTitle(tr("X-Axis Markers"));
  xMarkersPage->addDialogTab(_xMarkersTab);
  addDialogPage(xMarkersPage, true);
  _xMarkersTab->setObjectStore(_store);
  connect(_xMarkersTab, SIGNAL(apply()), this, SLOT(xAxisPlotMarkersChanged()));

  _yMarkersTab = new MarkersTab(this);
  DialogPage *yMarkersPage = new DialogPage(this);
  yMarkersPage->setPageTitle(tr("Y-Axis Markers"));
  yMarkersPage->addDialogTab(_yMarkersTab);
  addDialogPage(yMarkersPage, true);
  _yMarkersTab->setObjectStore(_store);
  connect(yMarkersPage, SIGNAL(apply()), this, SLOT(yAxisPlotMarkersChanged()));

  // addRelations(); This tends to clutter the plot dialog, let's test skipping it

  setupContent();
  setupAxis();
  setupRange();
  setupLabels();
  setupMarkers();

  setSupportsMultipleEdit(true);

  if (_plotItem->descriptiveNameIsManual()) {
    setTagString(_plotItem->descriptiveName());
  } else {
    setTagString(_defaultTagString);
  }

  QList<PlotItem*> list = ViewItem::getItems<PlotItem>();
  clearMultipleEditOptions();
  foreach(PlotItem* plot, list) {
    addMultipleEditOption(plot->plotName(), plot->descriptionTip(), plot->shortName());
  }
  
  QList<QList<QListWidgetItem*> > moveItems;
  moveItems << _listWidget->findItems("Appearance", Qt::MatchFixedString);
  moveItems << _listWidget->findItems("Dimensions", Qt::MatchFixedString);
  foreach(const QList<QListWidgetItem*>& found, moveItems) {
    if (found.size() > 0) {
      _listWidget->addItem(_listWidget->takeItem(_listWidget->row(found.first())));
    }
  }

  selectDialogPage(contentsPage);
  _saveAsDefault->show();

  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultiple()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingle()));
  connect(this, SIGNAL(apply()), this, SLOT(slotApply()));

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Make sure the labels are visible, doesn't work right now :-)
}


PlotItemDialog::~PlotItemDialog() {
}


void PlotItemDialog::editMultiple() {
  _xAxisTab->clearTabValues();
  _yAxisTab->clearTabValues();
  _rangeTab->clearTabValues();
  _labelTab->clearTabValues();
  _labelTab->enableSingleEditOptions(false);
  _topLabelTab->clearTabValues();
  _bottomLabelTab->clearTabValues();
  _leftLabelTab->clearTabValues();
  _rightLabelTab->clearTabValues();
  _axisLabelTab->clearTabValues();
  _xMarkersTab->clearTabValues();
  _yMarkersTab->clearTabValues();
  _contentTab->setEnabled(false);
  foreach(DialogPage* page, _relationPages) {
    removeDialogPage(page);
  }
  _relationPages.clear();
}


void PlotItemDialog::editSingle() {
  _contentTab->setEnabled(true);
  updateRelations();
  setupContent();
  setupAxis();
  setupRange();
  setupLabels();
  setupMarkers();
  setAlwaysAllowApply(false);
}


void PlotItemDialog::slotApply() {
  if (editMode() == Single) {
    _plotItem->setDescriptiveName(tagString().remove(_defaultTagString));
  }

  //FIXME: it is not clear that slotApply must be executed last.
  // experimentally, it seems to be...
  if (_saveAsDefault->isChecked()) {
    _plotItem->saveAsDialogDefaults();
  }
}


void PlotItemDialog::setupLabels() {
  Q_ASSERT(_plotItem);

  _labelTab->enableSingleEditOptions(true);

  _labelTab->setLeftLabel(_plotItem->leftLabel());
  _labelTab->setBottomLabel(_plotItem->bottomLabel());
  _labelTab->setTopLabel(_plotItem->topLabel());
  _labelTab->setRightLabel(_plotItem->rightLabel());

  _labelTab->setAutoScaleNumbers(_plotItem->isUseAxisScale());

  _labelTab->setLeftLabelAuto(_plotItem->leftLabelDetails()->isAuto());
  _labelTab->setBottomLabelAuto(_plotItem->bottomLabelDetails()->isAuto());
  _labelTab->setTopLabelAuto(_plotItem->topLabelDetails()->isAuto());
  _labelTab->setRightLabelAuto(_plotItem->rightLabelDetails()->isAuto());

  _labelTab->setShowLegend(_plotItem->showLegend());

  _topLabelTab->enableSingleEditOptions(true);
  _topLabelTab->setUseDefault(_plotItem->topLabelDetails()->fontUseGlobal());
  _topLabelTab->setLabelFont(_plotItem->topLabelDetails()->font());
  _topLabelTab->setLabelFontScale(_plotItem->topLabelDetails()->fontScale());
  _topLabelTab->setLabelColor(_plotItem->topLabelDetails()->fontColor());

  _bottomLabelTab->enableSingleEditOptions(true);
  _bottomLabelTab->setUseDefault(_plotItem->bottomLabelDetails()->fontUseGlobal());
  _bottomLabelTab->setLabelFont(_plotItem->bottomLabelDetails()->font());
  _bottomLabelTab->setLabelFontScale(_plotItem->bottomLabelDetails()->fontScale());
  _bottomLabelTab->setLabelColor(_plotItem->bottomLabelDetails()->fontColor());

  _leftLabelTab->enableSingleEditOptions(true);
  _leftLabelTab->setUseDefault(_plotItem->leftLabelDetails()->fontUseGlobal());
  _leftLabelTab->setLabelFont(_plotItem->leftLabelDetails()->font());
  _leftLabelTab->setLabelFontScale(_plotItem->leftLabelDetails()->fontScale());
  _leftLabelTab->setLabelColor(_plotItem->leftLabelDetails()->fontColor());

  _rightLabelTab->enableSingleEditOptions(true);
  _rightLabelTab->setUseDefault(_plotItem->rightLabelDetails()->fontUseGlobal());
  _rightLabelTab->setLabelFont(_plotItem->rightLabelDetails()->font());
  _rightLabelTab->setLabelFontScale(_plotItem->rightLabelDetails()->fontScale());
  _rightLabelTab->setLabelColor(_plotItem->rightLabelDetails()->fontColor());

  _axisLabelTab->enableSingleEditOptions(true);
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

  _xAxisTab->enableSingleEditOptions(true);
  _xAxisTab->setAxisMajorTickSpacing(_plotItem->xAxis()->axisMajorTickMode());
  _xAxisTab->setDrawAxisMajorTicks(_plotItem->xAxis()->drawAxisMajorTicks());
  _xAxisTab->setDrawAxisMajorGridLines(_plotItem->xAxis()->drawAxisMajorGridLines());
  _xAxisTab->setDrawAxisMinorTicks(_plotItem->xAxis()->drawAxisMinorTicks());
  _xAxisTab->setDrawAxisMinorGridLines(_plotItem->xAxis()->drawAxisMinorGridLines());
  _xAxisTab->setAutoMinorTickCount(_plotItem->xAxis()->axisAutoMinorTicks());
  _xAxisTab->setAxisMajorGridLineColor(_plotItem->xAxis()->axisMajorGridLineColor());
  _xAxisTab->setAxisMinorGridLineColor(_plotItem->xAxis()->axisMinorGridLineColor());
  _xAxisTab->setAxisMajorGridLineWidth(_plotItem->xAxis()->axisMajorGridLineWidth());
  _xAxisTab->setAxisMinorGridLineWidth(_plotItem->xAxis()->axisMinorGridLineWidth());
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
  _xAxisTab->setLabelRotation(_plotItem->xAxis()->axisLabelRotation());

  _yAxisTab->enableSingleEditOptions(true);
  _yAxisTab->setAxisMajorTickSpacing(_plotItem->yAxis()->axisMajorTickMode());
  _yAxisTab->setDrawAxisMajorTicks(_plotItem->yAxis()->drawAxisMajorTicks());
  _yAxisTab->setDrawAxisMajorGridLines(_plotItem->yAxis()->drawAxisMajorGridLines());
  _yAxisTab->setDrawAxisMinorTicks(_plotItem->yAxis()->drawAxisMinorTicks());
  _yAxisTab->setAutoMinorTickCount(_plotItem->yAxis()->axisAutoMinorTicks());
  _yAxisTab->setDrawAxisMinorGridLines(_plotItem->yAxis()->drawAxisMinorGridLines());
  _yAxisTab->setAxisMajorGridLineColor(_plotItem->yAxis()->axisMajorGridLineColor());
  _yAxisTab->setAxisMinorGridLineColor(_plotItem->yAxis()->axisMinorGridLineColor());
  _yAxisTab->setAxisMajorGridLineWidth(_plotItem->yAxis()->axisMajorGridLineWidth());
  _yAxisTab->setAxisMinorGridLineWidth(_plotItem->yAxis()->axisMinorGridLineWidth());
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
  _yAxisTab->setLabelRotation(_plotItem->yAxis()->axisLabelRotation());
}


void PlotItemDialog::setupMarkers() {
  Q_ASSERT(_plotItem);

  _xMarkersTab->enableSingleEditOptions(true);
  _xMarkersTab->setPlotMarkers(_plotItem->xAxis()->axisPlotMarkers());
  _yMarkersTab->enableSingleEditOptions(true);
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
      addDialogPage(curvePage, false);
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
      addDialogPage(imagePage, false);
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

        curve->registerChange();
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

        image->registerChange();
        image->unlock();
      }
    }
  }
}


void PlotItemDialog::rangeChanged() {
  Q_ASSERT(_plotItem);
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveRange(plotItem);
    }
  } else {
    saveRange(_plotItem);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void PlotItemDialog::saveRange(PlotItem *item) {
  ZoomState zoomstate;
  double newXMin, newXMax;
  double newYMin, newYMax;

  zoomstate = item->currentZoomState();

  qreal xRange = _rangeTab->xRangeDirty() ? _rangeTab->xRange() :fabs(item->xMax() - item->xMin());
  qreal xMax = _rangeTab->xMaxDirty() ? _rangeTab->xMax() :item->xMax();
  qreal xMin = _rangeTab->xMinDirty() ? _rangeTab->xMin() :item->xMin();
  if (item->xAxis()->axisLog()) {
    if (xMax != item->xMax()) {
      xMax = pow(10, xMax);
    } else {
      xMax = item->projectionRect().right();
    }
    if (xMin != item->xMin()) {
      xMin = pow(10, xMin);
    } else {
      xMin = item->projectionRect().left();
    }
  }

  qreal yRange = _rangeTab->yRangeDirty() ? _rangeTab->yRange() :fabs(item->yMax() - item->yMin());
  qreal yMax = _rangeTab->yMaxDirty() ? _rangeTab->yMax() :item->yMax();
  qreal yMin = _rangeTab->yMinDirty() ? _rangeTab->yMin() :item->yMin();

  if (item->yAxis()->axisLog()) {
    if (yMax != item->yMax()) {
      yMax = pow(10, yMax);
    } else {
      yMax = item->projectionRect().bottom();
    }
    if (yMin != item->yMin()) {
      yMin = pow(10, yMin);
    } else {
      yMin = item->projectionRect().top();
    }
  }

  zoomstate.item = item;

  if ((_rangeTab->xModeDirty() && _rangeTab->xMean()) || (!_rangeTab->xModeDirty() && zoomstate.xAxisZoomMode == PlotAxis::MeanCentered)) {
    if (item->xAxis()->axisLog()) {
      qreal min = (item->xMin() + item->xMax() - xRange) / 2.0;
      newXMin = pow(10, min);
      newXMax = pow(10, min+xRange);
    } else {
      newXMax = xRange;
      newXMin = (item->xMin() + item->xMax() - newXMax)/2.0;
    }
  } else {
    newXMin = qMin(xMax, xMin);
    newXMax = fabs(xMax - xMin);
  }
  if (newXMax == 0.0) newXMax = 0.2;

  if ((_rangeTab->yModeDirty() && _rangeTab->yMean()) || (!_rangeTab->yModeDirty() && zoomstate.yAxisZoomMode == PlotAxis::MeanCentered)) {
    if (item->yAxis()->axisLog()) {
      qreal min = (item->yMin() + item->yMax() - yRange) / 2.0;
      newYMin = pow(10, min);
      newYMax = pow(10, min+yRange);
    } else {
      newYMax = yRange;
      newYMin = (item->yMin() + item->yMax() - newYMax)/2.0;
    }
  } else {
    newYMin = qMin(yMax, yMin);
    newYMax = fabs(yMax - yMin);
  }
  if (newYMax == 0.0) newYMax = 0.2;

   PlotAxis::ZoomMode xZoomMode = item->yAxis()->axisZoomMode();
   PlotAxis::ZoomMode yZoomMode = item->yAxis()->axisZoomMode();
  if (_rangeTab->xModeDirty()) {
    if (_rangeTab->xAuto()) {
      xZoomMode = PlotAxis::Auto;
    } else if (_rangeTab->xSpike()) {
      xZoomMode = PlotAxis::SpikeInsensitive;
    } else if (_rangeTab->xBorder()) {
      xZoomMode = PlotAxis::AutoBorder;
    } else if (_rangeTab->xMean()) {
      xZoomMode = PlotAxis::MeanCentered;
    } else if (_rangeTab->xFixed()) {
      xZoomMode = PlotAxis::FixedExpression;
    }
  }

  if (_rangeTab->yModeDirty()) {
    if (_rangeTab->yAuto()) {
      yZoomMode = PlotAxis::Auto;
    } else if (_rangeTab->ySpike()) {
      yZoomMode = PlotAxis::SpikeInsensitive;
    } else if (_rangeTab->yBorder()) {
      yZoomMode = PlotAxis::AutoBorder;
    } else if (_rangeTab->yMean()) {
      yZoomMode = PlotAxis::MeanCentered;
    } else if (_rangeTab->yFixed()) {
      yZoomMode = PlotAxis::FixedExpression;
    }
  }

  QRectF newProjectionRect(newXMin, newYMin, newXMax, newYMax);
  if (_rangeTab->xModeDirty()) {
    if (xZoomMode == PlotAxis::Auto) {
      item->zoomXMaximum();
    } else if (xZoomMode == PlotAxis::AutoBorder) {
      item->zoomXAutoBorder();
    } else if (xZoomMode == PlotAxis::SpikeInsensitive) {
      item->zoomXNoSpike();
    }
  }
  if (xZoomMode == PlotAxis::FixedExpression) {
    item->zoomXRange(newProjectionRect);
  }
  if (xZoomMode == PlotAxis::MeanCentered) {
      item->zoomXMeanCentered(_rangeTab->xRange());
  }

  if (_rangeTab->yModeDirty()) {
    if (yZoomMode == PlotAxis::Auto) {
      item->zoomYMaximum();
    } else if (yZoomMode == PlotAxis::AutoBorder) {
      item->zoomYAutoBorder();
    } else if (yZoomMode == PlotAxis::SpikeInsensitive) {
      item->zoomYNoSpike();
    }
  }
  if (yZoomMode == PlotAxis::FixedExpression) {
    item->zoomYRange(newProjectionRect);
  }
  if (yZoomMode == PlotAxis::MeanCentered) {
      item->zoomYMeanCentered(_rangeTab->yRange());
  }
}


void PlotItemDialog::xAxisChanged() {
  Q_ASSERT(_plotItem);
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveAxis(plotItem->xAxis(), _xAxisTab);
      plotItem->setProjectionRect(plotItem->projectionRect(), plotItem->xAxis()->isDirty());
    }
  } else {
    saveAxis(_plotItem->xAxis(), _xAxisTab);
    _plotItem->setProjectionRect(_plotItem->projectionRect(), _plotItem->xAxis()->isDirty());
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void PlotItemDialog::yAxisChanged() {
  Q_ASSERT(_plotItem);
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveAxis(plotItem->yAxis(), _yAxisTab);
      plotItem->setProjectionRect(plotItem->projectionRect(), plotItem->yAxis()->isDirty());
    }
  } else {
    saveAxis(_plotItem->yAxis(), _yAxisTab);
    _plotItem->setProjectionRect(_plotItem->projectionRect(), _plotItem->yAxis()->isDirty());
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void PlotItemDialog::saveAxis(PlotAxis *axis, AxisTab *axisTab) {
  Q_ASSERT(axis);
  if (axisTab->axisMajorTickSpacingDirty()) {
    axis->setAxisMajorTickMode(axisTab->axisMajorTickSpacing());
  }
  if (axisTab->drawAxisMajorTicksDirty()) {
    axis->setDrawAxisMajorTicks(axisTab->drawAxisMajorTicks());
  }
  if (axisTab->drawAxisMajorGridLinesDirty()) {
    axis->setDrawAxisMajorGridLines(axisTab->drawAxisMajorGridLines());
  }
  if (axisTab->drawAxisMinorTicksDirty()) {
    axis->setDrawAxisMinorTicks(axisTab->drawAxisMinorTicks());
  }
  if (axisTab->drawAxisMinorGridLinesDirty()) {
    axis->setDrawAxisMinorGridLines(axisTab->drawAxisMinorGridLines());
  }
  if (axisTab->axisMajorGridLineColorDirty()) {
    axis->setAxisMajorGridLineColor(axisTab->axisMajorGridLineColor());
  }
  if (axisTab->axisMinorGridLineColorDirty()) {
    axis->setAxisMinorGridLineColor(axisTab->axisMinorGridLineColor());
  }
  if (axisTab->axisMajorGridLineWidthDirty()) {
    axis->setAxisMajorGridLineWidth(axisTab->axisMajorGridLineWidth());
  }
  if (axisTab->axisMinorGridLineWidthDirty()) {
    axis->setAxisMinorGridLineWidth(axisTab->axisMinorGridLineWidth());
  }
  if (axisTab->axisMajorGridLineStyleDirty()) {
    axis->setAxisMajorGridLineStyle(axisTab->axisMajorGridLineStyle());
  }
  if (axisTab->axisMinorGridLineStyleDirty()) {
    axis->setAxisMinorGridLineStyle(axisTab->axisMinorGridLineStyle());
  }
  if (axisTab->isLogDirty()) {
    axis->setAxisLog(axisTab->isLog());
  }
  if (axisTab->isReversedDirty()) {
    axis->setAxisReversed(axisTab->isReversed());
  }
  if (axisTab->isInterpretDirty()) {
    axis->setAxisInterpret(axisTab->isInterpret());
  }
  if (axisTab->axisDisplayDirty()) {
    axis->setAxisDisplay(axisTab->axisDisplay());
  }
  if (axisTab->axisInterpretationDirty()) {
    axis->setAxisInterpretation(axisTab->axisInterpretation());
  }
  if (axisTab->isBaseOffsetDirty()) {
    axis->setAxisBaseOffset(axisTab->isBaseOffset());
  }
  if (axisTab->axisMinorTickCountDirty()) {
    axis->setAxisMinorTickCount(axisTab->axisMinorTickCount());
  }
  if (axisTab->isAutoMinorTickCountDirty()) {
    axis->setAxisAutoMinorTicks(axisTab->isAutoMinorTickCount());
  }
  if (axisTab->significantDigitsDirty()) {
    axis->setAxisSignificantDigits(axisTab->significantDigits());
  }
  if (axisTab->isBaseOffsetDirty()) {
    axis->setAxisBaseOffset(axisTab->isBaseOffset());
  }
  if (axisTab->isAutoBaseOffsetDirty()) {
    axis->setAxisAutoBaseOffset(axisTab->isAutoBaseOffset());
  }
  if (axisTab->labelRotationDirty()) {
    axis->setAxisLabelRotation(axisTab->labelRotation());
  }
}


void PlotItemDialog::labelsChanged() {
  Q_ASSERT(_plotItem);
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveLabels(plotItem);
    }
  } else {
    saveLabels(_plotItem);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void PlotItemDialog::saveLabels(PlotItem *item) {
  QString leftLabel = _labelTab->leftLabelDirty() ? _labelTab->leftLabel() :item->leftLabel();
  bool leftLabelAuto = _labelTab->leftLabelAutoDirty() ? _labelTab->leftLabelAuto() :item->leftLabelDetails()->isAuto();
  bool leftUseDefault = _leftLabelTab->useDefaultDirty() ? _leftLabelTab->useDefault() :item->leftLabelDetails()->fontUseGlobal();
  QFont leftFont = _leftLabelTab->labelFontDirty() ?
                   _leftLabelTab->labelFont(item->leftLabelDetails()->font()) :item->leftLabelDetails()->font();
  qreal leftFontScale = _leftLabelTab->labelFontScaleDirty() ? _leftLabelTab->labelFontScale() :item->leftLabelDetails()->fontScale();
  QColor leftFontColor = _leftLabelTab->labelColorDirty() ? _leftLabelTab->labelColor() :item->leftLabelDetails()->fontColor();

  QString bottomLabel = _labelTab->bottomLabelDirty() ? _labelTab->bottomLabel() :item->bottomLabel();
  bool bottomLabelAuto = _labelTab->bottomLabelAutoDirty() ? _labelTab->bottomLabelAuto() :item->bottomLabelDetails()->isAuto();
  bool bottomUseDefault = _bottomLabelTab->useDefaultDirty() ? _bottomLabelTab->useDefault() :item->bottomLabelDetails()->fontUseGlobal();
  QFont bottomFont = _bottomLabelTab->labelFontDirty() ?
                     _bottomLabelTab->labelFont(item->bottomLabelDetails()->font()) :item->bottomLabelDetails()->font();
  qreal bottomFontScale = _bottomLabelTab->labelFontScaleDirty() ? _bottomLabelTab->labelFontScale() :item->bottomLabelDetails()->fontScale();
  QColor bottomFontColor = _bottomLabelTab->labelColorDirty() ? _bottomLabelTab->labelColor() :item->bottomLabelDetails()->fontColor();

  QString rightLabel = _labelTab->rightLabelDirty() ? _labelTab->rightLabel() :item->rightLabel();
  bool rightLabelAuto = _labelTab->rightLabelAutoDirty() ? _labelTab->rightLabelAuto() :item->rightLabelDetails()->isAuto();
  bool rightUseDefault = _rightLabelTab->useDefaultDirty() ? _rightLabelTab->useDefault() :item->rightLabelDetails()->fontUseGlobal();
  QFont rightFont = _rightLabelTab->labelFontDirty() ?
                    _rightLabelTab->labelFont(item->rightLabelDetails()->font()) :item->rightLabelDetails()->font();
  qreal rightFontScale = _rightLabelTab->labelFontScaleDirty() ? _rightLabelTab->labelFontScale() :item->rightLabelDetails()->fontScale();
  QColor rightFontColor = _rightLabelTab->labelColorDirty() ? _rightLabelTab->labelColor() :item->rightLabelDetails()->fontColor();

  QString topLabel = _labelTab->topLabelDirty() ? _labelTab->topLabel() :item->topLabel();
  bool topLabelAuto = _labelTab->topLabelAutoDirty() ? _labelTab->topLabelAuto() :item->topLabelDetails()->isAuto();
  bool topUseDefault = _topLabelTab->useDefaultDirty() ? _topLabelTab->useDefault() :item->topLabelDetails()->fontUseGlobal();
  QFont topFont = _topLabelTab->labelFontDirty() ?
                  _topLabelTab->labelFont(item->topLabelDetails()->font()) :item->topLabelDetails()->font();
  qreal topFontScale = _topLabelTab->labelFontScaleDirty() ? _topLabelTab->labelFontScale() :item->topLabelDetails()->fontScale();
  QColor topFontColor = _topLabelTab->labelColorDirty() ? _topLabelTab->labelColor() :item->topLabelDetails()->fontColor();

  bool axisUseDefault = _axisLabelTab->useDefaultDirty() ? _axisLabelTab->useDefault() :item->numberLabelDetails()->fontUseGlobal();
  QFont axisFont = _axisLabelTab->labelFontDirty() ?
                   _axisLabelTab->labelFont(item->numberLabelDetails()->font()) :item->numberLabelDetails()->font();
  qreal axisFontScale = _axisLabelTab->labelFontScaleDirty() ? _axisLabelTab->labelFontScale() :item->numberLabelDetails()->fontScale();
  QColor axisFontColor = _axisLabelTab->labelColorDirty() ? _axisLabelTab->labelColor() :item->numberLabelDetails()->fontColor();

  QFont globalFont = _labelTab->globalLabelFontDirty() ? _labelTab->globalLabelFont(item->globalFont()) :item->globalFont();
  qreal globalFontScale = _labelTab->globalLabelFontScaleDirty() ? _labelTab->globalLabelFontScale() :item->globalFontScale();
  QColor globalFontColor = _labelTab->globalLabelColorDirty() ? _labelTab->globalLabelColor() :item->globalFontColor();
  bool showLegend = _labelTab->showLegendDirty() ? _labelTab->showLegend() :item->showLegend();
  bool axisLabelScale = _labelTab->autoScaleNumbersDirty() ? _labelTab->autoScaleNumbers() :item->isUseAxisScale();

  item->leftLabelDetails()->setDetails(leftLabel, leftLabelAuto, leftUseDefault, leftFont, leftFontScale, leftFontColor);
  item->bottomLabelDetails()->setDetails(bottomLabel, bottomLabelAuto, bottomUseDefault, bottomFont, bottomFontScale, bottomFontColor);
  item->rightLabelDetails()->setDetails(rightLabel, rightLabelAuto, rightUseDefault, rightFont, rightFontScale, rightFontColor);
  item->topLabelDetails()->setDetails(topLabel, topLabelAuto, topUseDefault, topFont, topFontScale, topFontColor);
  item->numberLabelDetails()->setDetails(QString(), false, axisUseDefault, axisFont, axisFontScale, axisFontColor);

  item->setGlobalFont(globalFont);
  item->setGlobalFontScale(globalFontScale);
  item->setGlobalFontColor(globalFontColor);
  item->setUseAxisScale(axisLabelScale);

  item->setShowLegend(showLegend);
}


void PlotItemDialog::xAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  if (!_xMarkersTab->markersDirty()) {
    return;
  }

  PlotMarkers markers = _xMarkersTab->plotMarkers();
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveMarkers(plotItem->xAxis(), markers);
    }
  } else {
    saveMarkers(_plotItem->xAxis(), markers);
  }
  kstApp->mainWindow()->document()->setChanged(true);
}


void PlotItemDialog::yAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  if (!_yMarkersTab->markersDirty()) {
    return;
  }

  PlotMarkers markers = _yMarkersTab->plotMarkers();
  if (editMode() == Multiple) {
    foreach(ViewItem* item, selectedMultipleEditObjects()) {
      PlotItem* plotItem = (PlotItem*)item;
      saveMarkers(plotItem->yAxis(), markers);
    }
  } else {
    saveMarkers(_plotItem->yAxis(), markers);
  }
  kstApp->mainWindow()->document()->setChanged(true);

}


void PlotItemDialog::saveMarkers(PlotAxis *axis, PlotMarkers &markers) {
  axis->setAxisPlotMarkers(markers);
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
  QFont font = _labelTab->globalLabelFont(QFont());
  QColor color = _labelTab->globalLabelColor();


  _topLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _bottomLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _leftLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _rightLabelTab->setFontSpecsIfDefault(font, fontScale, color);
  _axisLabelTab->setFontSpecsIfDefault(font, fontScale, color);
}

}

// vim: ts=2 sw=2 et
