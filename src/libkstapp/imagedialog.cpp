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

#include "imagedialog.h"

#include "dialogpage.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"
#include "palette.h"
#include "image.h"
#include "document.h"
#include "objectstore.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"

namespace Kst {

ImageTab::ImageTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Image"));

  connect(_realTimeAutoThreshold, SIGNAL(toggled(const bool&)), this, SLOT(realTimeAutoThresholdToggled(const bool&)));
  connect(_colorOnly, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_colorAndContour, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_contourOnly, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_autoThreshold, SIGNAL(clicked()), this, SLOT(calculateAutoThreshold()));
  connect(_smartThreshold, SIGNAL(clicked()), this, SLOT(calculateSmartThreshold()));

  connect(_matrix, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}


ImageTab::~ImageTab() {
}


void ImageTab::selectionChanged() {
  _autoThreshold->setEnabled(_matrix->selectedMatrix() && !realTimeAutoThreshold());
  _smartThreshold->setEnabled(_matrix->selectedMatrix() && !realTimeAutoThreshold());
  emit optionsChanged();
}


CurvePlacement* ImageTab::curvePlacement() const {
  return _curvePlacement;
}


ColorPalette* ImageTab::colorPalette() const {
  return _colorPalette;
}


bool ImageTab::realTimeAutoThreshold() const {
  return _realTimeAutoThreshold->isChecked();
}


void ImageTab::setRealTimeAutoThreshold(const bool realTimeAutoThreshold) {
  _realTimeAutoThreshold->setChecked(realTimeAutoThreshold);
}


bool ImageTab::colorOnly() const {
  return _colorOnly->isChecked();
}


void ImageTab::setColorOnly(const bool colorOnly) {
  _colorOnly->setChecked(colorOnly);
}


bool ImageTab::contourOnly() const {
  return _contourOnly->isChecked();
}


void ImageTab::setContourOnly(const bool contourOnly) {
  _contourOnly->setChecked(contourOnly);
}


bool ImageTab::colorAndContour() const {
  return _colorAndContour->isChecked();
}


void ImageTab::setColorAndContour(const bool colorAndContour) {
  _colorAndContour->setChecked(colorAndContour);
}


bool ImageTab::useVariableLineWeight() const {
  return _useVariableWeight->isChecked();
}


void ImageTab::setUseVariableLineWeight(const bool useVariableLineWeight) {
  _useVariableWeight->setChecked(useVariableLineWeight);
}


MatrixPtr ImageTab::matrix() const {
  return _matrix->selectedMatrix();
}


void ImageTab::setMatrix(const MatrixPtr matrix) {
  _matrix->setSelectedMatrix(matrix);
}


double ImageTab::lowerThreshold() const {
  return _lowerThreshold->text().toDouble();
}


void ImageTab::setLowerThreshold(const double lowerThreshold) {
  _lowerThreshold->setText(QString::number(lowerThreshold));
}


double ImageTab::upperThreshold() const {
  return _upperThreshold->text().toDouble();
}


void ImageTab::setUpperThreshold(const double upperThreshold) {
  _upperThreshold->setText(QString::number(upperThreshold));
}


int ImageTab::numberOfContourLines() const {
  return _numContourLines->value();
}


void ImageTab::setNumberOfContourLines(const int numberOfContourLines) {
  _numContourLines->setValue(numberOfContourLines);
}


int ImageTab::contourWeight() const {
  return _contourWeight->value();
}


void ImageTab::setContourWeight(const int contourWeight) {
  _contourWeight->setValue(contourWeight);
}


QColor ImageTab::contourColor() const {
  return _contourColor->color();
}


void ImageTab::setContourColor(const QColor contourColor) {
  _contourColor->setColor(contourColor);
}


void ImageTab::realTimeAutoThresholdToggled(const bool checked) {
  _lowerThreshold->setEnabled(!checked);
  _upperThreshold->setEnabled(!checked);
  _smartThresholdValue->setEnabled(!checked);
  _autoThreshold->setEnabled(_matrix->selectedMatrix() && !checked);
  _smartThreshold->setEnabled(_matrix->selectedMatrix() && !checked);
}


void ImageTab::updateEnabled(const bool checked) {
  Q_UNUSED(checked);
  _colorMapGroup->setEnabled(_colorOnly->isChecked() || _colorAndContour->isChecked());
  _contourMapGroup->setEnabled(_contourOnly->isChecked() || _colorAndContour->isChecked());
}


void ImageTab::calculateAutoThreshold() {
  MatrixPtr matrix = _matrix->selectedMatrix();
  if (matrix) {
    matrix->readLock();
    _lowerThreshold->setText(QString::number(matrix->minValue()));
    _upperThreshold->setText(QString::number(matrix->maxValue()));
    matrix->unlock();
  }
}


void ImageTab::calculateSmartThreshold() {
  MatrixPtr matrix = _matrix->selectedMatrix();
  if (matrix) {
    matrix->readLock();
    double per = _smartThresholdValue->text().toDouble()/100.0;

    matrix->calcNoSpikeRange(per);
    _lowerThreshold->setText(QString::number(matrix->minValueNoSpike()));
    _upperThreshold->setText(QString::number(matrix->maxValueNoSpike()));
    matrix->unlock();
  }
}


void ImageTab::setObjectStore(ObjectStore *store) {
  _matrix->setObjectStore(store);
  _store = store;
}


void ImageTab::hidePlacementOptions() {
  _curvePlacement->setVisible(false);
  setMaximumHeight(335);
}


ImageDialog::ImageDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Image"));
  else
    setWindowTitle(tr("New Image"));

  _imageTab = new ImageTab(this);
  addDataTab(_imageTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  }

  connect(_imageTab, SIGNAL(optionsChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


ImageDialog::~ImageDialog() {
}


QString ImageDialog::tagString() const {
  return DataDialog::tagString();
}


void ImageDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_imageTab->matrix());
}


void ImageDialog::setMatrix(MatrixPtr matrix) {
  _imageTab->setMatrix(matrix);
}


void ImageDialog::configureTab(ObjectPtr object) {
  if (ImagePtr image = kst_cast<Image>(object)) {
    _imageTab->setMatrix(image->matrix());

    if (image->hasContourMap() && image->hasColorMap()) {
      _imageTab->setColorAndContour(true);
      _imageTab->setNumberOfContourLines(image->numContourLines());
      _imageTab->setContourColor(image->contourColor());
      _imageTab->setContourWeight(image->contourWeight());
      _imageTab->setLowerThreshold(image->lowerThreshold());
      _imageTab->setUpperThreshold(image->upperThreshold());
      _imageTab->setRealTimeAutoThreshold(image->autoThreshold());
      _imageTab->colorPalette()->setPalette(image->paletteName());
      _imageTab->setUseVariableLineWeight(image->contourWeight() == -1);

    } else if (image->hasContourMap()) {
      _imageTab->setContourOnly(true);
      _imageTab->setNumberOfContourLines(image->numContourLines());
      _imageTab->setContourColor(image->contourColor());
      _imageTab->setContourWeight(image->contourWeight());
      _imageTab->setUseVariableLineWeight(image->contourWeight() == -1);
    } else {
      _imageTab->setColorOnly(true);
      _imageTab->setLowerThreshold(image->lowerThreshold());
      _imageTab->setUpperThreshold(image->upperThreshold());
      _imageTab->setRealTimeAutoThreshold(image->autoThreshold());
      _imageTab->colorPalette()->setPalette(image->paletteName());
    }

    _imageTab->hidePlacementOptions();
  }
}


ObjectPtr ImageDialog::createNewDataObject() const {

  Q_ASSERT(_document && _document->objectStore());
  ObjectTag tag = _document->objectStore()->suggestObjectTag<Image>(tagString(), ObjectTag::globalTagContext);
  ImagePtr image = _document->objectStore()->createObject<Image>(tag);

  if (_imageTab->colorOnly()) {
    image->changeToColorOnly(_imageTab->matrix(),
        _imageTab->lowerThreshold(),
        _imageTab->upperThreshold(),
        _imageTab->realTimeAutoThreshold(),
        _imageTab->colorPalette()->selectedPalette());
  } else if (_imageTab->contourOnly()) {
    image->changeToContourOnly(_imageTab->matrix(),
        _imageTab->numberOfContourLines(),
        _imageTab->contourColor(),
        _imageTab->useVariableLineWeight() ? -1 : _imageTab->contourWeight());
  } else {
    image->changeToColorAndContour(_imageTab->matrix(),
        _imageTab->lowerThreshold(),
        _imageTab->upperThreshold(),
        _imageTab->realTimeAutoThreshold(),
        _imageTab->colorPalette()->selectedPalette(),
        _imageTab->numberOfContourLines(),
        _imageTab->contourColor(),
        _imageTab->useVariableLineWeight() ? -1 : _imageTab->contourWeight());
  }

  image->writeLock();
  image->update(0);
  image->unlock();

  PlotItem *plotItem = 0;
  switch (_imageTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_imageTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _imageTab->curvePlacement()->createLayout(),
        _imageTab->curvePlacement()->appendToLayout());
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      break;
    }
  default:
    break;
  }

  PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
  //TODO  Adam, is this the correct way to draw an image?  It runs very slow.
  renderItem->addRelation(kst_cast<Relation>(image));
  plotItem->update();

  return ObjectPtr(image.data());
}


ObjectPtr ImageDialog::editExistingDataObject() const {
  if (ImagePtr image = kst_cast<Image>(dataObject())) {
    image->writeLock();
    if (_imageTab->colorOnly()) {
      image->changeToColorOnly(_imageTab->matrix(),
          _imageTab->lowerThreshold(),
          _imageTab->upperThreshold(),
          _imageTab->realTimeAutoThreshold(),
          _imageTab->colorPalette()->selectedPalette());
    } else if (_imageTab->contourOnly()) {
      image->changeToContourOnly(_imageTab->matrix(),
          _imageTab->numberOfContourLines(),
          _imageTab->contourColor(),
          _imageTab->useVariableLineWeight() ? -1 : _imageTab->contourWeight());
    } else {
      image->changeToColorAndContour(_imageTab->matrix(),
          _imageTab->lowerThreshold(),
          _imageTab->upperThreshold(),
          _imageTab->realTimeAutoThreshold(),
          _imageTab->colorPalette()->selectedPalette(),
          _imageTab->numberOfContourLines(),
          _imageTab->contourColor(),
          _imageTab->useVariableLineWeight() ? -1 : _imageTab->contourWeight());
    }

    image->update(0);
    image->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
