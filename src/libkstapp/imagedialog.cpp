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

#include "imagedialog.h"

#include "dialogpage.h"
#include "editmultiplewidget.h"

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

#include "datacollection.h"

namespace Kst {

ImageTab::ImageTab(QWidget *parent)
  : DataTab(parent), _modeDirty(false) {

  setupUi(this);
  setTabTitle(tr("Image"));

  connect(_realTimeAutoThreshold, SIGNAL(toggled(const bool&)), this, SLOT(realTimeAutoThresholdToggled(const bool&)));
  connect(_colorOnly, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_colorAndContour, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_contourOnly, SIGNAL(toggled(const bool&)), this, SLOT(updateEnabled(const bool&)));
  connect(_autoThreshold, SIGNAL(clicked()), this, SLOT(calculateAutoThreshold()));
  connect(_smartThreshold, SIGNAL(clicked()), this, SLOT(calculateSmartThreshold()));

  connect(_matrix, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  connect(_matrix, SIGNAL(selectionChanged()), this, SIGNAL(modified()));
  connect(_colorOnly, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_colorAndContour, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_contourOnly, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_lowerThreshold, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_upperThreshold, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_realTimeAutoThreshold, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));

  connect(_contourColor, SIGNAL(changed(const QColor&)), this, SIGNAL(modified()));
  connect(_numContourLines, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_contourWeight, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_useVariableWeight, SIGNAL(clicked()), this, SIGNAL(modified()));

  connect(_colorPalette, SIGNAL(selectionChanged()), this, SIGNAL(modified()));

  connect(_colorOnly, SIGNAL(clicked()), this, SLOT(modeChanged()));
  connect(_contourOnly, SIGNAL(clicked()), this, SLOT(modeChanged()));
  connect(_colorAndContour, SIGNAL(clicked()), this, SLOT(modeChanged()));

  _matrixLabel->setBuddy(_matrix->_matrix);
}


ImageTab::~ImageTab() {
}


void ImageTab::selectionChanged() {
  _autoThreshold->setEnabled(_matrix->selectedMatrix() && !realTimeAutoThreshold());
  _smartThreshold->setEnabled(_matrix->selectedMatrix() && !realTimeAutoThreshold());
  emit optionsChanged();
}


void ImageTab::modeChanged() {
  _modeDirty = true;
}


void ImageTab::resetModeDirty() {
  _modeDirty = false;
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


bool ImageTab::realTimeAutoThresholdDirty() const {
  return _realTimeAutoThreshold->checkState() != Qt::PartiallyChecked;
}


void ImageTab::setRealTimeAutoThreshold(const bool realTimeAutoThreshold) {
  _realTimeAutoThreshold->setChecked(realTimeAutoThreshold);
}


bool ImageTab::colorOnly() const {
  return _colorOnly->isChecked();
}


void ImageTab::setColorOnly(const bool colorOnly) {
  _colorOnly->setChecked(colorOnly);
  resetModeDirty();
}


bool ImageTab::contourOnly() const {
  return _contourOnly->isChecked();
}


void ImageTab::setContourOnly(const bool contourOnly) {
  _contourOnly->setChecked(contourOnly);
  resetModeDirty();
}


bool ImageTab::colorAndContour() const {
  return _colorAndContour->isChecked();
}


void ImageTab::setColorAndContour(const bool colorAndContour) {
  _colorAndContour->setChecked(colorAndContour);
  resetModeDirty();
}


bool ImageTab::modeDirty() const {
  return _modeDirty;
}


bool ImageTab::useVariableLineWeight() const {
  return _useVariableWeight->isChecked();
}


bool ImageTab::useVariableLineWeightDirty() const {
  return _useVariableWeight->checkState() != Qt::PartiallyChecked;
}


void ImageTab::setUseVariableLineWeight(const bool useVariableLineWeight) {
  _useVariableWeight->setChecked(useVariableLineWeight);
}


MatrixPtr ImageTab::matrix() const {
  return _matrix->selectedMatrix();
}


bool ImageTab::matrixDirty() const {
  return _matrix->selectedMatrixDirty();
}


void ImageTab::setMatrix(const MatrixPtr matrix) {
  _matrix->setSelectedMatrix(matrix);
}


double ImageTab::lowerThreshold() const {
  return _lowerThreshold->text().toDouble();
}


bool ImageTab::lowerThresholdDirty() const {
  return !_lowerThreshold->text().isEmpty();
}


void ImageTab::setLowerThreshold(const double lowerThreshold) {
  _lowerThreshold->setText(QString::number(lowerThreshold));
}


double ImageTab::upperThreshold() const {
  return _upperThreshold->text().toDouble();
}


bool ImageTab::upperThresholdDirty() const {
  return !_upperThreshold->text().isEmpty();
}


void ImageTab::setUpperThreshold(const double upperThreshold) {
  _upperThreshold->setText(QString::number(upperThreshold));
}


int ImageTab::numberOfContourLines() const {
  return _numContourLines->value();
}


bool ImageTab::numberOfContourLinesDirty() const {
  return !_numContourLines->text().isEmpty();
}


void ImageTab::setNumberOfContourLines(const int numberOfContourLines) {
  _numContourLines->setValue(numberOfContourLines);
}


int ImageTab::contourWeight() const {
  return _contourWeight->value();
}


bool ImageTab::contourWeightDirty() const {
  return !_contourWeight->text().isEmpty();
}


void ImageTab::setContourWeight(const int contourWeight) {
  _contourWeight->setValue(contourWeight);
}


QColor ImageTab::contourColor() const {
  return _contourColor->color();
}


bool ImageTab::contourColorDirty() const {
  return !_contourColor->colorDirty();
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
}


void ImageTab::clearTabValues() {
  _matrix->clearSelection();
  _lowerThreshold->clear();
  _upperThreshold->clear();
  _numContourLines->clear();
  _contourWeight->clear();
  _contourColor->clearSelection();
  _colorPalette->clearSelection();
  _realTimeAutoThreshold->setCheckState(Qt::PartiallyChecked);
  _useVariableWeight->setCheckState(Qt::PartiallyChecked);
  _colorOnly->setChecked(true);
  resetModeDirty();
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
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));
  connect(_imageTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


ImageDialog::~ImageDialog() {
}


// QString ImageDialog::tagString() const {
//   return DataDialog::tagString();
// }


void ImageDialog::editMultipleMode() {
  _imageTab->clearTabValues();
}


void ImageDialog::editSingleMode() {
   configureTab(dataObject());
}


void ImageDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_imageTab->matrix() || (editMode() == EditMultiple));
}


void ImageDialog::setMatrix(MatrixPtr matrix) {
  _imageTab->setMatrix(matrix);
}


void ImageDialog::configureTab(ObjectPtr object) {
  if (!object) {
  } else if (ImagePtr image = kst_cast<Image>(object)) {
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
    if (_editMultipleWidget) {
      ImageList objects = _document->objectStore()->getObjects<Image>();
      _editMultipleWidget->clearObjects();
      foreach(ImagePtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


ObjectPtr ImageDialog::createNewDataObject() {

  Q_ASSERT(_document && _document->objectStore());
  ImagePtr image = _document->objectStore()->createObject<Image>();

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
  if (DataDialog::tagStringAuto()) {
     image->setDescriptiveName(QString());
  } else {
     image->setDescriptiveName(DataDialog::tagString());
  }

  image->writeLock();
  image->registerChange();
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
  case CurvePlacement::NewPlotNewTab:
    _document->createView();
    // fall through to case NewPlot.
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      if (_imageTab->curvePlacement()->scaleFonts()) {
        plotItem->view()->resetPlotFontSizes();
      }
      break;
    }
  default:
    break;
  }

  if (_imageTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(image));
    plotItem->update();

    plotItem->view()->appendToLayout(_imageTab->curvePlacement()->layout(), plotItem, _imageTab->curvePlacement()->gridColumns());
  }

  return ObjectPtr(image.data());
}


ObjectPtr ImageDialog::editExistingDataObject() const {
  if (ImagePtr image = kst_cast<Image>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        ImagePtr image = kst_cast<Image>(_document->objectStore()->retrieveObject(objectName));
        if (image) {
          MatrixPtr matrix = _imageTab->matrixDirty() ? _imageTab->matrix() : image->matrix();
          const double lowerThreshold = _imageTab->lowerThresholdDirty() ? _imageTab->lowerThreshold() : image->lowerThreshold();
          const double upperThreshold = _imageTab->upperThresholdDirty() ? _imageTab->upperThreshold() : image->upperThreshold();

          const bool realTimeAutoThreshold = _imageTab->realTimeAutoThresholdDirty() ? _imageTab->realTimeAutoThreshold() : image->autoThreshold();
          const bool useVariableLineWeight = _imageTab->useVariableLineWeightDirty() ? _imageTab->useVariableLineWeight() : (image->contourWeight() == -1);

          const QString colorPalette = _imageTab->colorPalette()->selectedPaletteDirty() ? _imageTab->colorPalette()->selectedPalette() : image->paletteName();

          const int numberOfContourLines = _imageTab->numberOfContourLinesDirty() ? _imageTab->numberOfContourLines() : image->numContourLines();
          const int contourWeight = _imageTab->contourWeightDirty() ? _imageTab->contourWeight() : image->contourWeight();

          const QColor color = _imageTab->contourColorDirty() ? _imageTab->contourColor() : image->contourColor();

          bool colorMap;
          bool contourMap;
          if (_imageTab->modeDirty()) {
            colorMap = _imageTab->colorOnly() || _imageTab->colorAndContour();
            contourMap = _imageTab->contourOnly() || _imageTab->colorAndContour();
          } else {
            colorMap = image->hasColorMap();
            contourMap = image->hasContourMap();
          }

          image->writeLock();
          if (colorMap && contourMap) {
            image->changeToColorAndContour(matrix,
                lowerThreshold,
                upperThreshold,
                realTimeAutoThreshold,
                colorPalette,
                numberOfContourLines,
                color,
                useVariableLineWeight ? -1 : contourWeight);
          } else if (colorMap) {
            image->changeToColorOnly(matrix,
                lowerThreshold,
                upperThreshold,
                realTimeAutoThreshold,
                colorPalette);
          } else {
            image->changeToContourOnly(matrix,
                numberOfContourLines,
                color,
                useVariableLineWeight ? -1 : contourWeight);
          }

          image->registerChange();
          image->unlock();
        }
      }
    } else {
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
      if (DataDialog::tagStringAuto()) {
        image->setDescriptiveName(QString());
      } else {
        image->setDescriptiveName(DataDialog::tagString());
      }

      image->registerChange();
      image->unlock();
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
