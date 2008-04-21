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

  _xAxisTab->setAxisMajorTickSpacing(_plotItem->xAxisMajorTickMode());
  _xAxisTab->setDrawAxisMajorTicks(_plotItem->drawXAxisMajorTicks());
  _xAxisTab->setDrawAxisMajorGridLines(_plotItem->drawXAxisMajorGridLines());
  _xAxisTab->setDrawAxisMinorTicks(_plotItem->drawXAxisMinorTicks());
  _xAxisTab->setDrawAxisMinorGridLines(_plotItem->drawXAxisMinorGridLines());
  _xAxisTab->setAxisMajorGridLineColor(_plotItem->xAxisMajorGridLineColor());
  _xAxisTab->setAxisMinorGridLineColor(_plotItem->xAxisMinorGridLineColor());
  _xAxisTab->setAxisMajorGridLineStyle(_plotItem->xAxisMajorGridLineStyle());
  _xAxisTab->setAxisMinorGridLineStyle(_plotItem->xAxisMinorGridLineStyle());
  _xAxisTab->setLog(_plotItem->xAxisLog());
  _xAxisTab->setReversed(_plotItem->xAxisReversed());
  _xAxisTab->setBaseOffset(_plotItem->xAxisBaseOffset());
  _xAxisTab->setInterpret(_plotItem->xAxisInterpret());
  _xAxisTab->setAxisDisplay(_plotItem->xAxisDisplay());
  _xAxisTab->setAxisInterpretation(_plotItem->xAxisInterpretation());
  _xAxisTab->setAxisMinorTickCount(_plotItem->xAxisMinorTickCount());
  _xAxisTab->setSignificantDigits(_plotItem->xAxisSignificantDigits());

  _yAxisTab->setAxisMajorTickSpacing(_plotItem->yAxisMajorTickMode());
  _yAxisTab->setDrawAxisMajorTicks(_plotItem->drawYAxisMajorTicks());
  _yAxisTab->setDrawAxisMajorGridLines(_plotItem->drawYAxisMajorGridLines());
  _yAxisTab->setDrawAxisMinorTicks(_plotItem->drawYAxisMinorTicks());
  _yAxisTab->setDrawAxisMinorGridLines(_plotItem->drawYAxisMinorGridLines());
  _yAxisTab->setAxisMajorGridLineColor(_plotItem->yAxisMajorGridLineColor());
  _yAxisTab->setAxisMinorGridLineColor(_plotItem->yAxisMinorGridLineColor());
  _yAxisTab->setAxisMajorGridLineStyle(_plotItem->yAxisMajorGridLineStyle());
  _yAxisTab->setAxisMinorGridLineStyle(_plotItem->yAxisMinorGridLineStyle());
  _yAxisTab->setLog(_plotItem->yAxisLog());
  _yAxisTab->setReversed(_plotItem->yAxisReversed());
  _yAxisTab->setBaseOffset(_plotItem->yAxisBaseOffset());
  _yAxisTab->setInterpret(_plotItem->yAxisInterpret());
  _yAxisTab->setAxisDisplay(_plotItem->yAxisDisplay());
  _yAxisTab->setAxisInterpretation(_plotItem->yAxisInterpretation());
  _yAxisTab->setAxisMinorTickCount(_plotItem->yAxisMinorTickCount());
  _yAxisTab->setSignificantDigits(_plotItem->yAxisSignificantDigits());
}


void PlotItemDialog::setupMarkers() {
  Q_ASSERT(_plotItem);

  _xMarkersTab->setPlotMarkers(_plotItem->xAxisPlotMarkers());
  _yMarkersTab->setPlotMarkers(_plotItem->yAxisPlotMarkers());
}


void PlotItemDialog::setupContent() {
  QStringList displayedRelations;
  QStringList availableRelations;
  QStringList allRelations;

  CurveList curves = _store->getObjects<Curve>();
  ImageList images = _store->getObjects<Image>();

  foreach (RelationPtr relation, _plotItem->renderItem(PlotRenderItem::Cartesian)->relationList()) {
    displayedRelations.append(relation->tag().displayString());
  }

  foreach (CurvePtr curve, curves) {
    allRelations.append(curve->tag().displayString());
    if (!displayedRelations.contains(curve->tag().displayString())) {
      availableRelations.append(curve->tag().displayString());
    }
  }

  foreach (ImagePtr image, images) {
    allRelations.append(image->tag().displayString());
    if (!displayedRelations.contains(image->tag().displayString())) {
      availableRelations.append(image->tag().displayString());
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
      curvePage->setPageTitle(curve->tag().displayString());
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
      imagePage->setPageTitle(image->tag().displayString());
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
    currentRelations.append(relation->tag().displayString());
    if (!displayedRelations.contains(relation->tag().displayString())) {
      _plotItem->renderItem(PlotRenderItem::Cartesian)->removeRelation(relation);
      _plotItem->update();
    }
  }

  foreach (QString relationTag, displayedRelations) {
    if (!currentRelations.contains(relationTag)) {
      if (RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(ObjectTag::fromString(relationTag)))) {
        _plotItem->renderItem(PlotRenderItem::Cartesian)->addRelation(relation);
        _plotItem->update();
      }
    }
  }

  updateRelations();
}


void PlotItemDialog::relationChanged() {
  foreach(DialogPage* page, _relationPages) {
    if (CurvePtr curve = kst_cast<Curve>(_store->retrieveObject(ObjectTag::fromString(page->pageTitle())))) {
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

        curve->update(0);
        curve->unlock();
      }
    } else if (ImagePtr image = kst_cast<Image>(_store->retrieveObject(ObjectTag::fromString(page->pageTitle())))) {
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

        image->update(0);
        image->unlock();
      }
    }
  }
}


void PlotItemDialog::xAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->setXAxisMajorTickMode(_xAxisTab->axisMajorTickSpacing());
  _plotItem->setDrawXAxisMajorTicks(_xAxisTab->drawAxisMajorTicks());
  _plotItem->setDrawXAxisMajorGridLines(_xAxisTab->drawAxisMajorGridLines());
  _plotItem->setDrawXAxisMinorTicks(_xAxisTab->drawAxisMinorTicks());
  _plotItem->setDrawXAxisMinorGridLines(_xAxisTab->drawAxisMinorGridLines());
  _plotItem->setXAxisMajorGridLineColor(_xAxisTab->axisMajorGridLineColor());
  _plotItem->setXAxisMinorGridLineColor(_xAxisTab->axisMinorGridLineColor());
  _plotItem->setXAxisMajorGridLineStyle(_xAxisTab->axisMajorGridLineStyle());
  _plotItem->setXAxisMinorGridLineStyle(_xAxisTab->axisMinorGridLineStyle());
  _plotItem->setXAxisLog(_xAxisTab->isLog());
  _plotItem->setXAxisLog(_xAxisTab->isLog());
  _plotItem->setXAxisReversed(_xAxisTab->isReversed());
  _plotItem->setXAxisInterpret(_xAxisTab->isInterpret());
  _plotItem->setXAxisDisplay(_xAxisTab->axisDisplay());
  _plotItem->setXAxisInterpretation(_xAxisTab->axisInterpretation());
  _plotItem->setXAxisBaseOffset(_xAxisTab->isBaseOffset());
  _plotItem->setXAxisMinorTickCount(_xAxisTab->axisMinorTickCount());
  _plotItem->setXAxisSignificantDigits(_xAxisTab->significantDigits());
  _plotItem->setProjectionRect(_plotItem->projectionRect());
}


void PlotItemDialog::yAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->setYAxisMajorTickMode(_yAxisTab->axisMajorTickSpacing());
  _plotItem->setDrawYAxisMajorTicks(_yAxisTab->drawAxisMajorTicks());
  _plotItem->setDrawYAxisMajorGridLines(_yAxisTab->drawAxisMajorGridLines());
  _plotItem->setDrawYAxisMinorTicks(_yAxisTab->drawAxisMinorTicks());
  _plotItem->setDrawYAxisMinorGridLines(_yAxisTab->drawAxisMinorGridLines());
  _plotItem->setYAxisMajorGridLineColor(_yAxisTab->axisMajorGridLineColor());
  _plotItem->setYAxisMinorGridLineColor(_yAxisTab->axisMinorGridLineColor());
  _plotItem->setYAxisMajorGridLineStyle(_yAxisTab->axisMajorGridLineStyle());
  _plotItem->setYAxisMinorGridLineStyle(_yAxisTab->axisMinorGridLineStyle());
  _plotItem->setYAxisLog(_yAxisTab->isLog());
  _plotItem->setYAxisLog(_yAxisTab->isLog());
  _plotItem->setYAxisReversed(_yAxisTab->isReversed());
  _plotItem->setYAxisInterpret(_yAxisTab->isInterpret());
  _plotItem->setYAxisDisplay(_yAxisTab->axisDisplay());
  _plotItem->setYAxisInterpretation(_yAxisTab->axisInterpretation());
  _plotItem->setYAxisBaseOffset(_yAxisTab->isBaseOffset());
  _plotItem->setYAxisMinorTickCount(_yAxisTab->axisMinorTickCount());
  _plotItem->setYAxisSignificantDigits(_yAxisTab->significantDigits());
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
  _plotItem->setXAxisPlotMarkers(_xMarkersTab->plotMarkers());
}


void PlotItemDialog::yAxisPlotMarkersChanged() {
  Q_ASSERT(_plotItem);
  _plotItem->setYAxisPlotMarkers(_yMarkersTab->plotMarkers());
}


}

// vim: ts=2 sw=2 et
