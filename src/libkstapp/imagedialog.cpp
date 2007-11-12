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

  connect(_matrix, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}


ImageTab::~ImageTab() {
}


void ImageTab::selectionChanged() {
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


bool ImageTab::colorOnly() const {
  return _colorOnly->isChecked();
}


bool ImageTab::contourOnly() const {
  return _contourOnly->isChecked();
}


bool ImageTab::colorAndContour() const {
  return _colorAndContour->isChecked();
}


MatrixPtr ImageTab::matrix() const {
  return _matrix->selectedMatrix();
}


double ImageTab::lowerZ() const {
  return _lowerZ->text().toDouble();
}


double ImageTab::upperZ() const {
  return _upperZ->text().toDouble();
}


int ImageTab::numberOfContourLines() const {
  return _numContourLines->value();
}


int ImageTab::contourWeight() const {
  return _contourWeight->value();
}


QColor ImageTab::contourColor() const {
  return _contourColor->color();
}

void ImageTab::realTimeAutoThresholdToggled(const bool checked) {
  _lowerZ->setEnabled(!checked);
  _upperZ->setEnabled(!checked);
  _autoThreshold->setEnabled(!checked);
  _smartThreshold->setEnabled(!checked);
  _smartThresholdValue->setEnabled(!checked);
}

void ImageTab::updateEnabled(const bool checked) {
  Q_UNUSED(checked);
  _colorMapGroup->setEnabled(_colorOnly->isChecked() || _colorAndContour->isChecked());
  _contourMapGroup->setEnabled(_contourOnly->isChecked() || _colorAndContour->isChecked());
}


void ImageTab::setObjectStore(ObjectStore *store) {
  _matrix->setObjectStore(store);
}


ImageDialog::ImageDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Image"));
  else
    setWindowTitle(tr("New Image"));

  _imageTab = new ImageTab(this);
  addDataTab(_imageTab);

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


ObjectPtr ImageDialog::createNewDataObject() const {

  Q_ASSERT(_document && _document->objectStore());
  ObjectTag tag = _document->objectStore()->suggestObjectTag<Image>(tagString(), ObjectTag::globalTagContext);
  ImagePtr image = _document->objectStore()->createObject<Image>(tag);

  if (_imageTab->colorOnly()) {
    image->changeToColorOnly(_imageTab->matrix(),
        _imageTab->lowerZ(),
        _imageTab->upperZ(),
        _imageTab->realTimeAutoThreshold(),
        _imageTab->colorPalette()->selectedPalette());
  } else if (_imageTab->contourOnly()) {
    image->changeToContourOnly(_imageTab->matrix(),
        _imageTab->numberOfContourLines(),
        _imageTab->contourColor(),
        _imageTab->contourWeight());
  } else {
    image->changeToColorAndContour(_imageTab->matrix(),
        _imageTab->lowerZ(),
        _imageTab->upperZ(),
        _imageTab->realTimeAutoThreshold(),
        _imageTab->colorPalette()->selectedPalette(),
        _imageTab->numberOfContourLines(),
        _imageTab->contourColor(),
        _imageTab->contourWeight());
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
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
