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
#include "markerstab.h"
#include "labeltab.h"
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

namespace Kst {

PlotItemDialog::PlotItemDialog(PlotItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _plotItem(item) {

  _store = kstApp->mainWindow()->document()->objectStore();

  _labelTab = new LabelTab(_plotItem, this);
  DialogPage *labelPage = new DialogPage(this);
  labelPage->setPageTitle(tr("Labels"));
  labelPage->addDialogTab(_labelTab);
  addDialogPage(labelPage);
  connect(_labelTab, SIGNAL(apply()), this, SLOT(labelsChanged()));

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
  setupLabels();
  setupMarkers();
}


PlotItemDialog::~PlotItemDialog() {
}


void PlotItemDialog::setupLabels() {
  Q_ASSERT(_plotItem);

  _labelTab->setLeftLabel(_plotItem->leftLabelOverride());
  _labelTab->setBottomLabel(_plotItem->bottomLabelOverride());
  _labelTab->setTopLabel(_plotItem->topLabelOverride());
  _labelTab->setRightLabel(_plotItem->rightLabelOverride());

  _labelTab->setLeftLabelFont(_plotItem->leftLabelFont());
  _labelTab->setBottomLabelFont(_plotItem->bottomLabelFont());
  _labelTab->setTopLabelFont(_plotItem->topLabelFont());
  _labelTab->setRightLabelFont(_plotItem->rightLabelFont());

  _labelTab->setLeftLabelFontScale(_plotItem->leftLabelFontScale());
  _labelTab->setBottomLabelFontScale(_plotItem->bottomLabelFontScale());
  _labelTab->setTopLabelFontScale(_plotItem->topLabelFontScale());
  _labelTab->setRightLabelFontScale(_plotItem->rightLabelFontScale());
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

  CurveList curves = _store->getObjects<Curve>();
  ImageList images = _store->getObjects<Image>();

  foreach (RelationPtr relation, _plotItem->renderItem(PlotRenderItem::Cartesian)->relationList()) {
    displayedRelations.append(relation->Name());
  }

  foreach (CurvePtr curve, curves) {
    allRelations.append(curve->Name());
    if (!displayedRelations.contains(curve->Name())) {
      availableRelations.append(curve->Name());
    }
  }

  foreach (ImagePtr image, images) {
    allRelations.append(image->Name());
    if (!displayedRelations.contains(image->Name())) {
      availableRelations.append(image->Name());
    }
  }

  _contentTab->setDisplayedRelations(displayedRelations);
  _contentTab->setAvailableRelations(availableRelations);
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

        curve->update();
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

        image->update();
        image->unlock();
      }
    }
  }
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
  _plotItem->xAxis()->setAxisMinorTickCount(_xAxisTab->axisMinorTickCount());
  _plotItem->xAxis()->setAxisSignificantDigits(_xAxisTab->significantDigits());
  _plotItem->setProjectionRect(_plotItem->projectionRect());
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
  _plotItem->yAxis()->setAxisBaseOffset(_yAxisTab->isBaseOffset());
  _plotItem->yAxis()->setAxisMinorTickCount(_yAxisTab->axisMinorTickCount());
  _plotItem->yAxis()->setAxisSignificantDigits(_yAxisTab->significantDigits());
  _plotItem->setProjectionRect(_plotItem->projectionRect());
}


void PlotItemDialog::labelsChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->setLeftLabelOverride(_labelTab->leftLabel());
  _plotItem->setBottomLabelOverride(_labelTab->bottomLabel());
  _plotItem->setRightLabelOverride(_labelTab->rightLabel());
  _plotItem->setTopLabelOverride(_labelTab->topLabel());

  _plotItem->setLeftLabelFont(_labelTab->leftLabelFont());
  _plotItem->setRightLabelFont(_labelTab->rightLabelFont());
  _plotItem->setTopLabelFont(_labelTab->topLabelFont());
  _plotItem->setBottomLabelFont(_labelTab->bottomLabelFont());

  _plotItem->setLeftLabelFontScale(_labelTab->leftLabelFontScale());
  _plotItem->setRightLabelFontScale(_labelTab->rightLabelFontScale());
  _plotItem->setTopLabelFontScale(_labelTab->topLabelFontScale());
  _plotItem->setBottomLabelFontScale(_labelTab->bottomLabelFontScale());

}


void PlotItemDialog::xAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  _plotItem->xAxis()->setAxisPlotMarkers(_xMarkersTab->plotMarkers());
}


void PlotItemDialog::yAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  _plotItem->yAxis()->setAxisPlotMarkers(_yMarkersTab->plotMarkers());
}


}

// vim: ts=2 sw=2 et
