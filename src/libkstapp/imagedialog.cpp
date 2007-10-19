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
}


ImageTab::~ImageTab() {
}


CurvePlacement* ImageTab::curvePlacement() const {
  return _curvePlacement;
}


ColorPalette* ImageTab::colorPalette() const {
  return _colorPalette;
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


ImageDialog::ImageDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Image"));
  else
    setWindowTitle(tr("New Image"));

  _ImageTab = new ImageTab(this);
  addDataTab(_ImageTab);

  //FIXME need to do validation to enable/disable ok button...
}


ImageDialog::~ImageDialog() {
}


QString ImageDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr ImageDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


ObjectPtr ImageDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
