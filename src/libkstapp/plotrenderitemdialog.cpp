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

#include "plotrenderitemdialog.h"

#include "contenttab.h"
#include "axistab.h"
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

PlotRenderItemDialog::PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _plotItem(item) {

  _store = kstApp->mainWindow()->document()->objectStore();

  _labelTab = new LabelTab(_plotItem->plotItem(), this);
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
}


PlotRenderItemDialog::~PlotRenderItemDialog() {
}


void PlotRenderItemDialog::setupLabels() {
  Q_ASSERT(_plotItem);

  _labelTab->setLeftLabel(_plotItem->plotItem()->leftLabelOverride());
  _labelTab->setBottomLabel(_plotItem->plotItem()->bottomLabelOverride());
  _labelTab->setTopLabel(_plotItem->plotItem()->topLabelOverride());
  _labelTab->setRightLabel(_plotItem->plotItem()->rightLabelOverride());

  _labelTab->setLeftLabelFont(_plotItem->plotItem()->leftLabelFont());
  _labelTab->setBottomLabelFont(_plotItem->plotItem()->bottomLabelFont());
  _labelTab->setTopLabelFont(_plotItem->plotItem()->topLabelFont());
  _labelTab->setRightLabelFont(_plotItem->plotItem()->rightLabelFont());
}


void PlotRenderItemDialog::setupAxis() {
  Q_ASSERT(_plotItem);

  _xAxisTab->setAxisMajorTickSpacing(_plotItem->plotItem()->xAxisMajorTickMode());
  _xAxisTab->setDrawAxisMajorTicks(_plotItem->plotItem()->drawXAxisMajorTicks());
  _xAxisTab->setDrawAxisMajorGridLines(_plotItem->plotItem()->drawXAxisMajorGridLines());
  _xAxisTab->setDrawAxisMinorTicks(_plotItem->plotItem()->drawXAxisMinorTicks());
  _xAxisTab->setDrawAxisMinorGridLines(_plotItem->plotItem()->drawXAxisMinorGridLines());
  _xAxisTab->setAxisMajorGridLineColor(_plotItem->plotItem()->xAxisMajorGridLineColor());
  _xAxisTab->setAxisMinorGridLineColor(_plotItem->plotItem()->xAxisMinorGridLineColor());
  _xAxisTab->setAxisMajorGridLineStyle(_plotItem->plotItem()->xAxisMajorGridLineStyle());
  _xAxisTab->setAxisMinorGridLineStyle(_plotItem->plotItem()->xAxisMinorGridLineStyle());
  _xAxisTab->setLog(_plotItem->isXAxisLog());
  _xAxisTab->setReversed(_plotItem->plotItem()->xAxisReversed());
  _xAxisTab->setBaseOffset(_plotItem->plotItem()->xAxisBaseOffset());
  _xAxisTab->setInterpret(_plotItem->plotItem()->xAxisInterpret());
  _xAxisTab->setAxisDisplay(_plotItem->plotItem()->xAxisDisplay());
  _xAxisTab->setAxisInterpretation(_plotItem->plotItem()->xAxisInterpretation());

  _yAxisTab->setAxisMajorTickSpacing(_plotItem->plotItem()->yAxisMajorTickMode());
  _yAxisTab->setDrawAxisMajorTicks(_plotItem->plotItem()->drawYAxisMajorTicks());
  _yAxisTab->setDrawAxisMajorGridLines(_plotItem->plotItem()->drawYAxisMajorGridLines());
  _yAxisTab->setDrawAxisMinorTicks(_plotItem->plotItem()->drawYAxisMinorTicks());
  _yAxisTab->setDrawAxisMinorGridLines(_plotItem->plotItem()->drawYAxisMinorGridLines());
  _yAxisTab->setAxisMajorGridLineColor(_plotItem->plotItem()->yAxisMajorGridLineColor());
  _yAxisTab->setAxisMinorGridLineColor(_plotItem->plotItem()->yAxisMinorGridLineColor());
  _yAxisTab->setAxisMajorGridLineStyle(_plotItem->plotItem()->yAxisMajorGridLineStyle());
  _yAxisTab->setAxisMinorGridLineStyle(_plotItem->plotItem()->yAxisMinorGridLineStyle());
  _yAxisTab->setLog(_plotItem->isYAxisLog());
  _yAxisTab->setReversed(_plotItem->plotItem()->yAxisReversed());
  _yAxisTab->setBaseOffset(_plotItem->plotItem()->yAxisBaseOffset());
  _yAxisTab->setInterpret(_plotItem->plotItem()->yAxisInterpret());
  _yAxisTab->setAxisDisplay(_plotItem->plotItem()->yAxisDisplay());
  _yAxisTab->setAxisInterpretation(_plotItem->plotItem()->yAxisInterpretation());

}

void PlotRenderItemDialog::setupContent() {
  QStringList displayedRelations;
  QStringList availableRelations;

  CurveList curves = _store->getObjects<Curve>();
  ImageList images = _store->getObjects<Image>();

  foreach (RelationPtr relation, _plotItem->relationList()) {
    displayedRelations.append(relation->tag().displayString());
  }

  foreach (CurvePtr curve, curves) {
    if (!displayedRelations.contains(curve->tag().displayString())) {
      availableRelations.append(curve->tag().displayString());
    }
  }

  foreach (ImagePtr image, images) {
    if (!displayedRelations.contains(image->tag().displayString())) {
      availableRelations.append(image->tag().displayString());
    }
  }

  _contentTab->setDisplayedRelations(displayedRelations);
  _contentTab->setAvailableRelations(availableRelations);
}


void PlotRenderItemDialog::addRelations() {
  foreach (RelationPtr relation, _plotItem->relationList()) {
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


void PlotRenderItemDialog::updateRelations() {
  foreach(DialogPage* page, _relationPages) {
    removeDialogPage(page);
  }
  _relationPages.clear();

  addRelations();
}


void PlotRenderItemDialog::contentChanged() {
  relationChanged();

  QStringList currentRelations;
  QStringList displayedRelations = _contentTab->displayedRelations();

  foreach (RelationPtr relation, _plotItem->relationList()) {
    currentRelations.append(relation->tag().displayString());
    if (!displayedRelations.contains(relation->tag().displayString())) {
      _plotItem->removeRelation(relation);
      _plotItem->plotItem()->update();
    }
  }

  foreach (QString relationTag, displayedRelations) {
    if (!currentRelations.contains(relationTag)) {
      if (RelationPtr relation = kst_cast<Relation>(_store->retrieveObject(ObjectTag::fromString(relationTag)))) {
        _plotItem->addRelation(relation);
        _plotItem->plotItem()->update();
      }
    }
  }

  updateRelations();
}


void PlotRenderItemDialog::relationChanged() {
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


void PlotRenderItemDialog::xAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->plotItem()->setXAxisMajorTickMode(_xAxisTab->axisMajorTickSpacing());
  _plotItem->plotItem()->setDrawXAxisMajorTicks(_xAxisTab->drawAxisMajorTicks());
  _plotItem->plotItem()->setDrawXAxisMajorGridLines(_xAxisTab->drawAxisMajorGridLines());
  _plotItem->plotItem()->setDrawXAxisMinorTicks(_xAxisTab->drawAxisMinorTicks());
  _plotItem->plotItem()->setDrawXAxisMinorGridLines(_xAxisTab->drawAxisMinorGridLines());
  _plotItem->plotItem()->setXAxisMajorGridLineColor(_xAxisTab->axisMajorGridLineColor());
  _plotItem->plotItem()->setXAxisMinorGridLineColor(_xAxisTab->axisMinorGridLineColor());
  _plotItem->plotItem()->setXAxisMajorGridLineStyle(_xAxisTab->axisMajorGridLineStyle());
  _plotItem->plotItem()->setXAxisMinorGridLineStyle(_xAxisTab->axisMinorGridLineStyle());
  _plotItem->plotItem()->setXAxisLog(_xAxisTab->isLog());
  _plotItem->setXAxisLog(_xAxisTab->isLog());
  _plotItem->plotItem()->setXAxisReversed(_xAxisTab->isReversed());
  _plotItem->plotItem()->setXAxisInterpret(_xAxisTab->isInterpret());
  _plotItem->plotItem()->setXAxisDisplay(_xAxisTab->axisDisplay());
  _plotItem->plotItem()->setXAxisInterpretation(_xAxisTab->axisInterpretation());
  _plotItem->plotItem()->setXAxisBaseOffset(_xAxisTab->isBaseOffset());

}


void PlotRenderItemDialog::yAxisChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->plotItem()->setYAxisMajorTickMode(_yAxisTab->axisMajorTickSpacing());
  _plotItem->plotItem()->setDrawYAxisMajorTicks(_yAxisTab->drawAxisMajorTicks());
  _plotItem->plotItem()->setDrawYAxisMajorGridLines(_yAxisTab->drawAxisMajorGridLines());
  _plotItem->plotItem()->setDrawYAxisMinorTicks(_yAxisTab->drawAxisMinorTicks());
  _plotItem->plotItem()->setDrawYAxisMinorGridLines(_yAxisTab->drawAxisMinorGridLines());
  _plotItem->plotItem()->setYAxisMajorGridLineColor(_yAxisTab->axisMajorGridLineColor());
  _plotItem->plotItem()->setYAxisMinorGridLineColor(_yAxisTab->axisMinorGridLineColor());
  _plotItem->plotItem()->setYAxisMajorGridLineStyle(_yAxisTab->axisMajorGridLineStyle());
  _plotItem->plotItem()->setYAxisMinorGridLineStyle(_yAxisTab->axisMinorGridLineStyle());
  _plotItem->plotItem()->setYAxisLog(_yAxisTab->isLog());
  _plotItem->setYAxisLog(_yAxisTab->isLog());
  _plotItem->plotItem()->setYAxisReversed(_yAxisTab->isReversed());
  _plotItem->plotItem()->setYAxisInterpret(_yAxisTab->isInterpret());
  _plotItem->plotItem()->setYAxisDisplay(_yAxisTab->axisDisplay());
  _plotItem->plotItem()->setYAxisInterpretation(_yAxisTab->axisInterpretation());
  _plotItem->plotItem()->setYAxisBaseOffset(_yAxisTab->isBaseOffset());
}


void PlotRenderItemDialog::labelsChanged() {
  Q_ASSERT(_plotItem);

  _plotItem->plotItem()->setLeftLabelOverride(_labelTab->leftLabel());
  _plotItem->plotItem()->setBottomLabelOverride(_labelTab->bottomLabel());
  _plotItem->plotItem()->setRightLabelOverride(_labelTab->rightLabel());
  _plotItem->plotItem()->setTopLabelOverride(_labelTab->topLabel());

  _plotItem->plotItem()->setLeftLabelFont(_labelTab->leftLabelFont());
  _plotItem->plotItem()->setRightLabelFont(_labelTab->rightLabelFont());
  _plotItem->plotItem()->setTopLabelFont(_labelTab->topLabelFont());
  _plotItem->plotItem()->setBottomLabelFont(_labelTab->bottomLabelFont());

}

}

// vim: ts=2 sw=2 et
