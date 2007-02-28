/***************************************************************************
                      kstimagedialog.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2003 The University of Toronto
                           (C) 2003 C. Barth Netterfield
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <q3vbox.h>

// include files for KDE
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <knuminput.h>

// application specific includes
#include "colorpalettewidget.h"
#include "curveplacementwidget.h"
#include "editmultiplewidget.h"
#include "imagedialogwidget.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstimagedialog.h"
#include "kstuinames.h"
#include "kstviewwindow.h"
#include "matrixselector.h"

QPointer<KstImageDialogI> KstImageDialogI::_inst;

KstImageDialogI *KstImageDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstImageDialogI(KstApp::inst());
  }
  return _inst;
}


KstImageDialogI::KstImageDialogI(QWidget* parent,
                                 const char* name, bool modal, Qt::WFlags fl)
: KstDataDialog(parent, name, modal, fl) {
  _w = new ImageDialogWidget(_contents);
  setMultiple(true);
  connect(_w->_matrix, SIGNAL(newMatrixCreated(const QString&)), this, SIGNAL(modified()));
  connect(_w->_autoThreshold, SIGNAL(clicked()), this, SLOT(calcAutoThreshold()));
  connect(_w->_smartThreshold, SIGNAL(clicked()), this, SLOT(calcSmartThreshold()));
  connect(_w->_colorOnly, SIGNAL(clicked()), this, SLOT(updateGroups()));
  connect(_w->_contourOnly, SIGNAL(clicked()), this, SLOT(updateGroups()));
  connect(_w->_colorAndContour, SIGNAL(clicked()), this, SLOT(updateGroups()));
  connect(_w->_useVariableWeight, SIGNAL(clicked()), this, SLOT(updateEnables()));
  connect(_w->_realTimeAutoThreshold, SIGNAL(clicked()), this, SLOT(updateEnables()));
  
  // for multiple edit mode
  connect(_w->_colorOnly, SIGNAL(clicked()), this, SLOT(setColorOnlyDirty()));
  connect(_w->_contourOnly, SIGNAL(clicked()), this, SLOT(setContourOnlyDirty()));
  connect(_w->_colorAndContour, SIGNAL(clicked()), this, SLOT(setColorAndContourDirty()));
  connect(_w->_realTimeAutoThreshold, SIGNAL(clicked()), this, SLOT(setRealTimeAutoThresholdDirty()));
  connect(_w->_useVariableWeight, SIGNAL(clicked()), this, SLOT(setUseVariableWeightDirty()));
  connect(_w->_contourColor, SIGNAL(clicked()), this, SLOT(setContourColorDirty()));
}


KstImageDialogI::~KstImageDialogI() {
}


void KstImageDialogI::updateWindow() {
  _w->_curvePlacement->update();
}


void KstImageDialogI::fillFieldsForEdit() {
  fillFieldsForEditNoUpdate();

  KstImagePtr ip = kst_cast<KstImage>(_dp);
  if (!ip) {
    return; // shouldn't be needed
  }
  ip->readLock();
  
  // set the type of image
  _w->_colorOnly->setChecked(ip->hasColorMap() && !ip->hasContourMap());
  _w->_contourOnly->setChecked(ip->hasContourMap() && !ip->hasColorMap());
  _w->_colorAndContour->setChecked(ip->hasColorMap() && ip->hasContourMap());

  // set the matrix
  _w->_matrix->setSelection(ip->matrixTag());

  ip->unlock();

  //update the groups and enables
  // won't call fillFieldsForEditNoUpdate again because
  // fillFieldsForEdit is not called in _editMultipleMode
  updateGroups();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstImageDialogI::fillFieldsForEditNoUpdate() {
  KstImagePtr ip = kst_cast<KstImage>(_dp);
  if (!ip) {
    return; // shouldn't be needed
  }

  KstImageList images = kstObjectSubList<KstDataObject, KstImage>(KST::dataObjectList);

  ip->readLock();
  // fill in the tag name
  _tagName->setText(ip->tagName());

  // fill in the other parameters
  _w->_lowerZ->setText(QString::number(ip->lowerThreshold()));
  _w->_upperZ->setText(QString::number(ip->upperThreshold()));
  _w->_realTimeAutoThreshold->setChecked(ip->autoThreshold());

  _w->_colorPalette->refresh(ip->paletteName());
  _w->_numContourLines->setValue(ip->numContourLines());
  _w->_contourColor->setColor(ip->contourColor());
  int tempWeight = ip->contourWeight();
  _w->_useVariableWeight->setChecked(tempWeight == -1);
  if (tempWeight >= 0) {
    _w->_contourWeight->setValue(tempWeight);
  }

  ip->unlock();

  //don't place the image in edits
  _w->_curvePlacement->hide();

  updateEnables();
}


void KstImageDialogI::fillFieldsForNew() {
  KstImageList images = kstObjectSubList<KstDataObject, KstImage>(KST::dataObjectList);

  // set tag name
  _tagName->setText("<New_Image>");

  _w->_colorPalette->refresh();

  // some default values
  _w->_lowerZ->setText("0");
  _w->_upperZ->setText("100");
  _w->_realTimeAutoThreshold->setChecked(true);

  // let the image be placed in plots
  _w->_curvePlacement->update();

  // for some reason the widgets need to be placed from bottom to top
  _w->_imageTypeGroup->hide();
  _w->_contourMapGroup->hide();
  _w->_colorMapGroup->hide();
  _w->_matrixGroup->hide();
  _w->_curvePlacement->show();
  _w->_contourMapGroup->show();
  _w->_colorMapGroup->show();
  _w->_imageTypeGroup->show();
  _w->_matrixGroup->show();

  // use whatever setting was used last
  updateGroups();
  updateEnables();
  _w->_colorPalette->updatePalette(_w->_colorPalette->selectedPalette());
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstImageDialogI::update() {
  _w->_matrix->update();
}


bool KstImageDialogI::newObject() {
  //if matrixCombo is empty then display an error message
  if (_w->_matrix->selectedMatrix().isEmpty()){
    KMessageBox::sorry(this, i18n("Matrix is a 2D grid of numbers, used to create image", "New image not made: define matrix first."));
    return false;
  }

  //do some checks on the inputs
  double lowerZDouble, upperZDouble;
  if (!checkParameters(lowerZDouble, upperZDouble)) {
    return false;
  }

  KST::matrixList.lock().readLock();
  KstMatrixPtr matrix = *KST::matrixList.findTag(_w->_matrix->selectedMatrix());
  KST::matrixList.lock().unlock();
  if (!matrix) {
    KMessageBox::sorry(this, i18n("Matrix is a 2D grid of numbers, used to create image", "Could not find matrix."));
    return false;
  }
  KST::dataObjectList.lock().readLock();
  matrix->readLock();

  //create a unique name
  QString tag_name = KST::suggestImageName(matrix->tag());
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    matrix->unlock();
    KST::dataObjectList.lock().unlock();
    return false;
  }

  KstImagePtr image;
  if (_w->_contourOnly->isChecked()) {
    //need a contour map only
    QColor tempColor = _w->_contourColor->color();
    image = new KstImage(tag_name, matrix, _w->_numContourLines->text().toInt(), tempColor,
                         _w->_useVariableWeight->isChecked() ? -1 : _w->_contourWeight->value());
  } else if (_w->_colorOnly->isChecked()) {
    //need a color map only
    KPalette* newPal = new KPalette(_w->_colorPalette->selectedPalette());
    image = new KstImage(tag_name, matrix, lowerZDouble, upperZDouble,
                         _w->_realTimeAutoThreshold->isChecked(), newPal);
  } else {
    //need both a contour map and colour map
    QColor tempColor = _w->_contourColor->color();
    KPalette* newPal = new KPalette(_w->_colorPalette->selectedPalette());
    image = new KstImage(tag_name, matrix, lowerZDouble, upperZDouble,
                         _w->_realTimeAutoThreshold->isChecked(), newPal,
                         _w->_numContourLines->text().toInt(), tempColor,
                         _w->_useVariableWeight->isChecked() ? -1 : _w->_contourWeight->value());
  }
  matrix->unlock();
  KST::dataObjectList.lock().unlock();
  placeInPlot(image);
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(image.data());
  KST::dataObjectList.lock().unlock();
  image = 0L; // drop the reference
  emit modified();
  return true;
}


bool KstImageDialogI::editSingleObject(KstImagePtr imPtr) {
  KstMatrixPtr pMatrix;
  if (_matrixDirty) {
    //find the pMatrix
    KST::matrixList.lock().readLock();
    pMatrix = *KST::matrixList.findTag(_w->_matrix->selectedMatrix());
    KST::matrixList.lock().unlock();

    if (!pMatrix) {
      KMessageBox::sorry(this, i18n("Matrix is a 2D grid of numbers, used to create image", "Could not find pMatrix."));
      return false;
    }
  } else {
    imPtr->readLock();
    pMatrix = imPtr->matrix();
    imPtr->unlock();
  }

  imPtr->writeLock();

  // if image type was changed, get all parameters from the dialog
  if (_contourOnlyDirty || _colorOnlyDirty || _colorAndContourDirty) {
    double lowerZDouble, upperZDouble;
    if (!checkParameters(lowerZDouble, upperZDouble)) {
      //KMessageBox::sorry(this, i18n("Image type was changed: Lower Z threshold cannot be higher than Upper Z threshold."));
      //pMatrix->unlock();
      imPtr->unlock();
      return false;
    }
    if (_w->_contourOnly->isChecked()) {
      //need a contour map only
      QColor tempColor = _w->_contourColor->color();
      imPtr->changeToContourOnly(imPtr->tagName(), pMatrix, _w->_numContourLines->text().toInt(), tempColor,
                              _w->_useVariableWeight->isChecked() ? -1 : _w->_contourWeight->value());
    } else if (_w->_colorOnly->isChecked()) {
      //need a color map only
      KPalette* newPal = new KPalette(_w->_colorPalette->selectedPalette());
      imPtr->changeToColorOnly(imPtr->tagName(), pMatrix, lowerZDouble, upperZDouble,
                            _w->_realTimeAutoThreshold->isChecked(), newPal);
    } else {
      //need both a contour map and colour map
      QColor tempColor = _w->_contourColor->color();
      KPalette* newPal = new KPalette(_w->_colorPalette->selectedPalette());
      imPtr->changeToColorAndContour(imPtr->tagName(), pMatrix, lowerZDouble, upperZDouble,
                                  _w->_realTimeAutoThreshold->isChecked(), newPal,
                                  _w->_numContourLines->text().toInt(), tempColor,
                                  _w->_useVariableWeight->isChecked() ? -1 : _w->_contourWeight->value());
    }
  } else {
    // get the current or new parameters as required
    QColor pContourColor;
    double pLowerZ, pUpperZ;
    int pNumContours, pContourWeight;
    bool pRealTimeAutoThreshold, pUseVariableWeight;

    if (_lowerZDirty) {
      pLowerZ = _w->_lowerZ->text().toDouble();
    } else {
      pLowerZ = imPtr->lowerThreshold();
    }

    if (_upperZDirty) {
      pUpperZ = _w->_upperZ->text().toDouble();
    } else {
      pUpperZ = imPtr->upperThreshold();
    }

    if (_realTimeAutoThresholdDirty) {
      pRealTimeAutoThreshold = _w->_realTimeAutoThreshold->isChecked();
    } else {
      pRealTimeAutoThreshold = imPtr->autoThreshold();
    }

    if (_numContourLinesDirty) {
      pNumContours = _w->_numContourLines->text().toInt();
    } else {
      pNumContours = imPtr->numContourLines();
    }

    if (_contourWeightDirty) {
      pContourWeight = _w->_contourWeight->value();
    } else {
      pContourWeight = imPtr->contourWeight();
    }

    if (_useVariableWeightDirty) {
      pUseVariableWeight = _w->_useVariableWeight->isChecked();
    } else {
      pUseVariableWeight = imPtr->contourWeight() == -1;
    }

    if (_contourColorDirty) {
      pContourColor = _w->_contourColor->color();
    } else {
      pContourColor = imPtr->contourColor();
    }

    // check parameters for color map
    if (imPtr->hasColorMap()) {
      if (pLowerZ > pUpperZ) {
        //KMessageBox::sorry(this, i18n("The Lower Z threshold cannot be higher than Upper Z threshold."));
        //pMatrix->unlock();
        imPtr->unlock();
        return false;
      }
    }

    // don't change the image type, just change applicable settings for the
    // current image type
    if (imPtr->hasContourMap() && !imPtr->hasColorMap()) {
      imPtr->changeToContourOnly(imPtr->tagName(), pMatrix, pNumContours, pContourColor,
          pUseVariableWeight ? -1 : pContourWeight);
    } else {
      KPalette *palette;
      if (_paletteDirty) {
        palette = new KPalette(_w->_colorPalette->selectedPalette());
      } else {
        palette = imPtr->palette();
      }

      if (imPtr->hasColorMap() && !imPtr->hasContourMap()) {
        imPtr->changeToColorOnly(imPtr->tagName(), pMatrix, pLowerZ, pUpperZ,
            pRealTimeAutoThreshold, palette);
      } else {
        // images always have at least one of color or contour maps
        imPtr->changeToColorAndContour(imPtr->tagName(), pMatrix, pLowerZ, pUpperZ,
            pRealTimeAutoThreshold, palette,
            pNumContours, pContourColor,
            pUseVariableWeight ? -1 : pContourWeight);
      }
    }
  }


  imPtr->unlock();

  return true;
}

bool KstImageDialogI::editObject() {
  KstImageList imList = kstObjectSubList<KstDataObject,KstImage>(KST::dataObjectList);
  
  // if editing multiple objects, edit each one
  if (_editMultipleMode) {
    _numContourLinesDirty = _w->_numContourLines->text() != " ";
    _contourWeightDirty = _w->_contourWeight->text() != " ";
    _paletteDirty = _w->_colorPalette->currentPaletteIndex() != 0;
    _matrixDirty = _w->_matrix->_matrix->currentItem() != 0;
    _lowerZDirty = !_w->_lowerZ->text().isEmpty();
    _upperZDirty = !_w->_upperZ->text().isEmpty();

    bool didEdit = false;
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); ++i) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstImageList::Iterator imIter = imList.findTag(_editMultipleWidget->_objectList->text(i));
        if (imIter == imList.end()) {
          return false;
        }

        KstImagePtr imPtr = *imIter;

        if (!editSingleObject(imPtr)) {
          return false;
        }
        didEdit = true;
      }
    }

    if (!didEdit) {
      KMessageBox::sorry(this, i18n("Select one or more objects to edit."));
      return false;
    }
  } else {
    KstImagePtr ip = kst_cast<KstImage>(_dp);
    // verify that the curve name is unique
    QString tag_name = _tagName->text();
    if (!ip || (tag_name != ip->tagName() && KstData::self()->dataTagNameNotUnique(tag_name))) {
      _tagName->setFocus();
      return false;
    }

    ip->writeLock();
    ip->setTagName(KstObjectTag(tag_name, ip->tag().context())); // FIXME: doesn't allow changing tag context
    ip->unlock();

    // then edit the object
    _colorOnlyDirty = true;
    _contourOnlyDirty = true;
    _colorAndContourDirty = true;
    _paletteDirty = true;
    _lowerZDirty = true;
    _upperZDirty = true;
    _realTimeAutoThresholdDirty = true;
    _numContourLinesDirty = true;
    _contourWeightDirty = true;
    _useVariableWeightDirty = true;
    _contourColorDirty = true;
    if (!editSingleObject(ip)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstImageDialogI::calcAutoThreshold() {
  //make sure an matrix is selected
  if (!_w->_matrix->selectedMatrix().isEmpty()){
    KST::matrixList.lock().readLock();
    KstMatrixPtr matrix = *KST::matrixList.findTag(_w->_matrix->selectedMatrix());
    KST::matrixList.lock().unlock();
    if (matrix) {
      matrix->readLock();
      _w->_lowerZ->setText(QString::number(matrix->minValue()));
      _w->_upperZ->setText(QString::number(matrix->maxValue()));
      matrix->unlock();
    }
  }
}

// This should use a smart (percentile based) algorithm to
// calculate the thresholds.  It will be expensive.
void KstImageDialogI::calcSmartThreshold() {
  //make sure an matrix is selected
  if (!_w->_matrix->selectedMatrix().isEmpty()){
    KST::matrixList.lock().readLock();
    KstMatrixPtr matrix = *KST::matrixList.findTag(_w->_matrix->selectedMatrix());
    KST::matrixList.lock().unlock();
    if (matrix) {
      matrix->readLock();
      double per = _w->_smartThresholdValue->value()/100.0;

      matrix->calcNoSpikeRange(per);
      _w->_lowerZ->setText(QString::number(matrix->minValueNoSpike()));
      _w->_upperZ->setText(QString::number(matrix->maxValueNoSpike()));
      matrix->unlock();
    }
  }
}

void KstImageDialogI::placeInPlot(KstImagePtr image) {
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_w->_curvePlacement->_plotWindow->currentText()));
  if (!w) {
    QString n = KstApp::inst()->newWindow(KST::suggestWinName());
    w = static_cast<KstViewWindow*>(KstApp::inst()->findWindow(n));
  }
  if (w) {
    Kst2DPlotPtr plot;
    if (_w->_curvePlacement->existingPlot()) {
      /* assign image to plot */
      plot = kst_cast<Kst2DPlot>(w->view()->findChild(_w->_curvePlacement->plotName()));
      if (plot) {
        plot->addCurve(KstBaseCurvePtr(image));
      }
    }

    if (_w->_curvePlacement->newPlot()) {
      /* assign image to plot */
      QString name = w->createObject<Kst2DPlot>(KST::suggestPlotName());
      if (_w->_curvePlacement->reGrid()) {
        w->view()->cleanup(_w->_curvePlacement->columns());
      }
      plot = kst_cast<Kst2DPlot>(w->view()->findChild(name));
      if (plot) {
        _w->_curvePlacement->update();
        _w->_curvePlacement->setCurrentPlot(plot->tagName());
        plot->setXScaleMode(AUTO);
        plot->setYScaleMode(AUTO);
        plot->addCurve(KstBaseCurvePtr(image));
        plot->generateDefaultLabels();
      }
    }
  }
}


void KstImageDialogI::updateGroups() {
  _w->_colorMapGroup->setEnabled(_w->_colorOnly->isChecked() || _w->_colorAndContour->isChecked());
  _w->_contourMapGroup->setEnabled(_w->_contourOnly->isChecked() || _w->_colorAndContour->isChecked());

  // if editing multiple, also set some defaults for the newly enabled groups
  if (_editMultipleMode) {
    fillFieldsForEditNoUpdate();
  }
}


void KstImageDialogI::updateEnables() {
  if (!_w->_useVariableWeight->isTristate()) {
    _w->_contourWeight->setEnabled(!_w->_useVariableWeight->isChecked());
  }

  if (!_w->_realTimeAutoThreshold->isTristate()) {
    if (_w->_realTimeAutoThreshold->isChecked()) {
      calcAutoThreshold();
    }
    _w->_lowerZ->setEnabled(!_w->_realTimeAutoThreshold->isChecked());
    _w->_upperZ->setEnabled(!_w->_realTimeAutoThreshold->isChecked());
    _w->_autoThreshold->setEnabled(!_w->_realTimeAutoThreshold->isChecked() && !_editMultipleMode);
    _w->_smartThreshold->setEnabled(!_w->_realTimeAutoThreshold->isChecked() && !_editMultipleMode);
    _w->_smartThresholdValue->setEnabled(!_w->_realTimeAutoThreshold->isChecked() && !_editMultipleMode);
  }
}


bool KstImageDialogI::checkParameters(double& lowerZDouble, double& upperZDouble) {
  if (_w->_colorOnly->isChecked() || _w->_colorAndContour->isChecked()) {
    bool ok1, ok2;
    lowerZDouble = _w->_lowerZ->text().toDouble(&ok1);
    upperZDouble = _w->_upperZ->text().toDouble(&ok2);
    if (!(ok1 && ok2)) {
      if (ok1 || ok2) {
        if (ok1) {
          KMessageBox::sorry(this, i18n("The upper threshold is not a valid decimal number."));
        } else {
          KMessageBox::sorry(this, i18n("The lower threshold is not a valid decimal number."));
        }
      } else {
        KMessageBox::sorry(this, i18n("The upper and lower thresholds are not valid decimal numbers."));
      }
      return false;
    }
    if (lowerZDouble >= upperZDouble) {
      KMessageBox::sorry(this, i18n("The upper threshold must be greater than the lower threshold."));
      return false;
    }
  }
  //for now there is nothing to check for contour maps
  return true;
}


void KstImageDialogI::populateEditMultiple() {
  KstImageList imlist = kstObjectSubList<KstDataObject,KstImage>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(imlist.tagNames());

  // also intermediate state for multiple edit
  _w->_colorOnly->setChecked(false);
  _w->_contourOnly->setChecked(false);
  _w->_colorAndContour->setChecked(false);
  _w->_colorMapGroup->setEnabled(true);
  _w->_contourMapGroup->setEnabled(true);
  _w->_colorPalette->_palette->insertItem("", 0);
  _w->_colorPalette->_palette->setCurrentItem(0);
  _w->_matrix->_matrix->insertItem("",0);
  _w->_matrix->_matrix->setCurrentItem(0);
  _w->_lowerZ->setText("");
  _w->_upperZ->setText("");
  _w->_realTimeAutoThreshold->setTristate(true);
  _w->_realTimeAutoThreshold->setNoChange();
  _w->_autoThreshold->setEnabled(false);
  _w->_numContourLines->setSpecialValueText(" ");
  _w->_numContourLines->setMinValue(_w->_numContourLines->minValue() - 1);
  _w->_numContourLines->setValue(_w->_numContourLines->minValue());
  _w->_contourWeight->setSpecialValueText(" ");
  _w->_contourWeight->setMinValue(_w->_contourWeight->minValue() - 1);
  _w->_contourWeight->setValue(_w->_contourWeight->minValue());
  _w->_useVariableWeight->setTristate(true);
  _w->_useVariableWeight->setNoChange();
  _w->_contourColor->setColor(QColor()); //default color

  _tagName->setText("");
  _tagName->setEnabled(false);

  _w->_lowerZ->setEnabled(true);
  _w->_upperZ->setEnabled(true);
  _w->_contourWeight->setEnabled(true);

  // and clean all the fields
  _colorOnlyDirty = false;
  _contourOnlyDirty = false;
  _colorAndContourDirty = false;
  _paletteDirty = false;
  _lowerZDirty = false;
  _upperZDirty = false;
  _realTimeAutoThresholdDirty = false;
  _numContourLinesDirty = false;
  _contourWeightDirty = false;
  _useVariableWeightDirty = false;
  _contourColorDirty = false;
}


void KstImageDialogI::setRealTimeAutoThresholdDirty() {
  _w->_realTimeAutoThreshold->setTristate(false);
  _realTimeAutoThresholdDirty = true;
  updateEnables();
}


void KstImageDialogI::setUseVariableWeightDirty() {
  _w->_useVariableWeight->setTristate(false);
  _useVariableWeightDirty = true;
  updateEnables();
}


void KstImageDialogI::cleanup() {
  if (_editMultipleMode) {
    _w->_numContourLines->setSpecialValueText(QString::null);
    _w->_numContourLines->setMinValue(_w->_numContourLines->minValue() + 1);
    _w->_contourWeight->setSpecialValueText(QString::null);
    _w->_contourWeight->setMinValue(_w->_contourWeight->minValue() + 1);
    _w->_autoThreshold->setEnabled(true);
  }
}


void KstImageDialogI::setMatrix(const QString& name) {
  _w->_matrix->setSelection(name);
}

#include "kstimagedialog.moc"
// vim: ts=2 sw=2 et
