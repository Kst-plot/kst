/***************************************************************************
                       kstcsddialog.cpp  -  Part of KST
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
#include <qlineedit.h>
#include <q3listbox.h>
#include <qspinbox.h>
#include <q3vbox.h>

// include files for KDE
#include <kcombobox.h>
#include "ksdebug.h"
#include <knuminput.h>
#include <kmessagebox.h>

// application specific inclues
#include "fftoptionswidget.h"
#include "colorpalettewidget.h"
#include "curveappearancewidget.h"
#include "curveplacementwidget.h"
#include "editmultiplewidget.h"
#include "csddialogwidget.h"
#include "kst2dplot.h"
#include "kstcsddialog.h"
#include "kstdataobjectcollection.h"
#include "kstuinames.h"
#include "kstvcurve.h"
#include "kstviewwindow.h"
#include "vectorselector.h"

#include "ui_kstcsddialog4.h"

const QString& KstCsdDialogI::defaultTag = KGlobal::staticQString("<Auto Name>");

QPointer<KstCsdDialogI> KstCsdDialogI::_inst = 0L;

KstCsdDialogI *KstCsdDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstCsdDialogI(KstApp::inst());
  }
  return _inst;
}


KstCsdDialogI::KstCsdDialogI(QWidget* parent, Qt::WindowFlags fl)
: KstDataDialog(parent, fl) {
  _w = new CSDDialogWidget(_contents);
  setMultiple(true);
  connect(_w->_vector, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
 
  //for multiple edit mode
  connect(_w->_kstFFTOptions->Apodize, SIGNAL(clicked()), this, SLOT(setApodizeDirty()));
  connect(_w->_kstFFTOptions->RemoveMean, SIGNAL(clicked()), this, SLOT(setRemoveMeanDirty()));
  connect(_w->_kstFFTOptions->Interleaved, SIGNAL(clicked()), this, SLOT(setInterleavedDirty()));
}


KstCsdDialogI::~KstCsdDialogI() {
}


void KstCsdDialogI::updateWindow() {
}


void KstCsdDialogI::fillFieldsForEdit() {
  KstCSDPtr cp = kst_cast<KstCSD>(_dp);
  if (!cp) {
    return; // shouldn't be needed
  }

  cp->readLock();

  _tagName->setText(cp->tagName());

  _w->_vector->setSelection(cp->vTag());

  // set sample rate, Units, FFT len, and vector units
  _w->_kstFFTOptions->FFTLen->setValue(cp->length());
  _w->_kstFFTOptions->SampRate->setText(QString::number(cp->freq()));
  _w->_kstFFTOptions->VectorUnits->setText(cp->vectorUnits());
  _w->_kstFFTOptions->RateUnits->setText(cp->rateUnits());
  _w->_kstFFTOptions->Apodize->setChecked(cp->apodize());
  _w->_kstFFTOptions->ApodizeFxn->setCurrentItem(cp->apodizeFxn());
  _w->_kstFFTOptions->Sigma->setValue(cp->gaussianSigma());
  _w->_kstFFTOptions->RemoveMean->setChecked(cp->removeMean());
  _w->_kstFFTOptions->Interleaved->setChecked(cp->average());
  _w->_kstFFTOptions->Output->setCurrentItem(cp->output());
  _w->_windowSize->setValue(cp->windowSize());
  _w->_kstFFTOptions->synch();

  cp->unlock();
 
  _w->_imageOptionsGroup->hide();
  _w->_curvePlacement->hide();

  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstCsdDialogI::fillFieldsForNew() {
  _tagName->setText(defaultTag);
  _w->_kstFFTOptions->update();
  
  _w->_colorPalette->refresh();
 
  // update the curve placement widget
  _w->_curvePlacement->update();
  
  _w->_imageOptionsGroup->show();
  _w->_curvePlacement->show();
 
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstCsdDialogI::update() {
  _w->_vector->update();
}


/* returns true if succesful */
bool KstCsdDialogI::newObject() {
  QString tag_name = _tagName->text();
  if (tag_name == defaultTag) {
    tag_name = KST::suggestCSDName(KstObjectTag::fromString(_w->_vector->selectedVector()));
  }

  // verify that the curve name is unique
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    return false;
  }

  if (_w->_vector->selectedVector().isEmpty()) {
    KMessageBox::sorry(this, i18n("New CSD not made: define vectors first."));
    return false;
  }

  KST::vectorList.lock().readLock();
  KstVectorPtr p = *KST::vectorList.findTag(_w->_vector->selectedVector());
  KST::vectorList.lock().unlock();
  if (!p) {
    kstdFatal() << "Bug in kst: the vector field in CSD dialog refers to "
                << "a non existant vector...." << endl;
  }
 
  ApodizeFunction apodizeFxn = ApodizeFunction(_w->_kstFFTOptions->ApodizeFxn->currentItem());
  bool apodize = _w->_kstFFTOptions->Apodize->isChecked();
  double gaussianSigma = _w->_kstFFTOptions->Sigma->value();
  bool removeMean = _w->_kstFFTOptions->RemoveMean->isChecked();
  bool average = _w->_kstFFTOptions->Interleaved->isChecked();
  int windowSize = _w->_windowSize->value();
  int length = _w->_kstFFTOptions->FFTLen->value();
  double freq = _w->_kstFFTOptions->SampRate->text().toDouble();
  PSDType output = PSDType(_w->_kstFFTOptions->Output->currentItem());
  QString vectorUnits = _w->_kstFFTOptions->VectorUnits->text();
  QString rateUnits = _w->_kstFFTOptions->RateUnits->text();
  _w->_kstFFTOptions->synch();
 
  KstCSDPtr csd = new KstCSD(tag_name, p, freq, average, removeMean,
                             apodize, apodizeFxn, windowSize, length, gaussianSigma, output,
                             vectorUnits, rateUnits);
  //csd->setInterpolateHoles(_w->_kstFFTOptions->InterpolateHoles->isChecked());
 
  KstImagePtr image = createImage(csd);
 
  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(csd.data());
  KST::dataObjectList.append(image.data());
  KST::dataObjectList.lock().unlock();

  csd = 0L;
  emit modified();
  return true;
}


KstImagePtr KstCsdDialogI::createImage(KstCSDPtr csd) {
  KPalette* newPal = new KPalette(_w->_colorPalette->selectedPalette());
  csd->readLock();
  KstImagePtr image = new KstImage(csd->tagName()+"-I", csd->outputMatrix(), 0, 1, true, newPal);
  csd->unlock();

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
        plot->addCurve(KstBaseCurvePtr(image));
        plot->generateDefaultLabels();
      }
    }
  }
  return image;
}


bool KstCsdDialogI::editSingleObject(KstCSDPtr csPtr) {
  csPtr->writeLock();

  KST::vectorList.lock().readLock();
  csPtr->setVector(*KST::vectorList.findTag(_w->_vector->selectedVector()));
  KST::vectorList.lock().unlock();

  // get the values that need to be checked for consistency
  double pSampRate;
  int pFFTLen;
 
  if (_sampRateDirty) {
    pSampRate = _w->_kstFFTOptions->SampRate->text().toDouble();
  } else {
    pSampRate = csPtr->freq();
  }
   
  if (_fFTLenDirty) {
    pFFTLen = _w->_kstFFTOptions->FFTLen->text().toInt();
  } else {
    pFFTLen = csPtr->length();
  }
 
  if (!_w->_kstFFTOptions->checkGivenValues(pSampRate, pFFTLen)) {
    csPtr->unlock();
    return false;
  }

  if (_sampRateDirty) {
    csPtr->setFreq(_w->_kstFFTOptions->SampRate->text().toDouble());
  }
 
  if (_fFTLenDirty) {
    csPtr->setLength(_w->_kstFFTOptions->FFTLen->text().toInt());
  }

  if (_apodizeDirty) {
    csPtr->setApodize(_w->_kstFFTOptions->Apodize->isChecked());
  }
 
  if (_apodizeFxnDirty) {
    csPtr->setApodizeFxn(ApodizeFunction(_w->_kstFFTOptions->ApodizeFxn->currentItem()));
  }
 
  if (_gaussianSigmaDirty) {
    csPtr->setGaussianSigma(_editMultipleMode ? _w->_kstFFTOptions->Sigma->value() - 1 :
                                                _w->_kstFFTOptions->Sigma->value());
  }
 
  if (_removeMeanDirty) {
    csPtr->setRemoveMean(_w->_kstFFTOptions->RemoveMean->isChecked());
  }
 
  if (_interleavedDirty) {
    csPtr->setAverage(_w->_kstFFTOptions->Interleaved->isChecked());
  }
 
  if (_windowSizeDirty) {
    csPtr->setWindowSize(_w->_windowSize->value());
  }
 
  if (_rateUnitsDirty) {
    csPtr->setRateUnits(_w->_kstFFTOptions->RateUnits->text());
  }
 
  if (_vectorUnitsDirty) {
    csPtr->setVectorUnits(_w->_kstFFTOptions->VectorUnits->text());
  }
 
  if (_outputDirty) {
    csPtr->setOutput(PSDType(_w->_kstFFTOptions->Output->currentItem()));
  }
    
  csPtr->unlock();
  return true;
}


// returns true if succesful
bool KstCsdDialogI::editObject() {
  // if the user selected no vector, treat it as non-dirty
  _vectorDirty = _w->_vector->_vector->currentItem() != 0;
  _fFTLenDirty = _w->_kstFFTOptions->FFTLen->text() != " ";
  _sampRateDirty = !_w->_kstFFTOptions->SampRate->text().isEmpty();
  _vectorUnitsDirty = !_w->_kstFFTOptions->VectorUnits->text().isEmpty();
  _rateUnitsDirty = !_w->_kstFFTOptions->RateUnits->text().isEmpty();
  _windowSizeDirty = _w->_windowSize->text() != " ";
  _apodizeFxnDirty = _w->_kstFFTOptions->ApodizeFxn->currentItem() != 0;
  _gaussianSigmaDirty = _w->_kstFFTOptions->Sigma->value() != _w->_kstFFTOptions->Sigma->minValue();
  _outputDirty =  _w->_kstFFTOptions->Output->currentItem() != 0;
  
  KstCSDList csList = kstObjectSubList<KstDataObject,KstCSD>(KST::dataObjectList);
 
  // if editing multiple objects, edit each one
  if (_editMultipleMode) {
    bool didEdit = false;
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); i++) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstCSDList::Iterator csIter = csList.findTag(_editMultipleWidget->_objectList->text(i));
        if (csIter == csList.end()) {
          return false;
        }
         
        KstCSDPtr csPtr = *csIter;
       
        if (!editSingleObject(csPtr)) {
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
    KstCSDPtr cp = kst_cast<KstCSD>(_dp);
    // verify that the name is unique
    QString tag_name = _tagName->text();
    if (!cp || (tag_name != cp->tagName() && KstData::self()->dataTagNameNotUnique(tag_name))) {
      _tagName->setFocus();
      return false;
    }
   
    cp->writeLock();
    cp->setTagName(KstObjectTag(tag_name, cp->tag().context())); // FIXME: doesn't allow changing tag context
    cp->unlock();
   
    // then edit the object
    _vectorDirty = true;
    _apodizeDirty = true;
    _apodizeFxnDirty = true;
    _gaussianSigmaDirty = true;
    _removeMeanDirty = true;
    _interleavedDirty = true;
    _sampRateDirty = true;
    _vectorUnitsDirty = true;
    _rateUnitsDirty = true;
    _fFTLenDirty = true;
    _windowSizeDirty = true;
    _outputDirty = true;
    if (!editSingleObject(cp)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstCsdDialogI::populateEditMultiple() {
  KstCSDList csList = kstObjectSubList<KstDataObject,KstCSD>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(csList.tagNames());

  // also intermediate state for multiple edit
  _w->_vector->_vector->insertItem("", 0);
  _w->_vector->_vector->setCurrentItem(0);
  _w->_kstFFTOptions->Apodize->setNoChange();
  _w->_kstFFTOptions->ApodizeFxn->insertItem("", 0);
  _w->_kstFFTOptions->ApodizeFxn->setCurrentItem(0);
  _w->_kstFFTOptions->Sigma->setMinValue(_w->_kstFFTOptions->Sigma->minValue() - 0.01);
  _w->_kstFFTOptions->Sigma->setSpecialValueText(" ");
  _w->_kstFFTOptions->Sigma->setValue(_w->_kstFFTOptions->Sigma->minValue());
  _w->_kstFFTOptions->RemoveMean->setNoChange();
  _w->_kstFFTOptions->Interleaved->setNoChange();
  _w->_kstFFTOptions->SampRate->setText("");
  _w->_kstFFTOptions->VectorUnits->setText("");
  _w->_kstFFTOptions->RateUnits->setText("");
  _w->_kstFFTOptions->FFTLen->setMinValue(_w->_kstFFTOptions->FFTLen->minValue() - 1);
  _w->_kstFFTOptions->FFTLen->setSpecialValueText(" ");
  _w->_kstFFTOptions->FFTLen->setValue(_w->_kstFFTOptions->FFTLen->minValue());
  _w->_kstFFTOptions->Output->insertItem("", 0);
  _w->_kstFFTOptions->Output->setCurrentItem(0);
  _w->_windowSize->setMinValue(_w->_windowSize->minValue() - 1);
  _w->_windowSize->setSpecialValueText(" ");
  _w->_windowSize->setValue(_w->_windowSize->minValue());
 
  _tagName->setText("");
  _tagName->setEnabled(false);
 
  // and clean all the fields
  _vectorDirty = false;
  _apodizeDirty = false;
  _apodizeFxnDirty = false;
  _gaussianSigmaDirty = false;
  _removeMeanDirty = false;
  _interleavedDirty = false;
  _sampRateDirty = false;
  _vectorUnitsDirty = false;
  _rateUnitsDirty = false;
  _fFTLenDirty = false;
  _outputDirty = false;
  _windowSizeDirty = false;
}


void KstCsdDialogI::cleanup() {
  if (_editMultipleMode) {
     _w->_kstFFTOptions->FFTLen->setMinValue(_w->_kstFFTOptions->FFTLen->minValue() + 1);
     _w->_kstFFTOptions->FFTLen->setSpecialValueText(QString::null);
     _w->_kstFFTOptions->Sigma->setMinValue(_w->_kstFFTOptions->Sigma->minValue() + 0.01);
     _w->_kstFFTOptions->Sigma->setSpecialValueText(QString::null);
     _w->_kstFFTOptions->ApodizeFxn->removeItem(0);
     _w->_kstFFTOptions->Output->removeItem(0);
  }
}


void KstCsdDialogI::setApodizeDirty() {
  _apodizeDirty = true;
  _w->_kstFFTOptions->Apodize->setTristate(false);
}


void KstCsdDialogI::setRemoveMeanDirty() {
  _w->_kstFFTOptions->RemoveMean->setTristate(false);
  _removeMeanDirty = true;
}


void KstCsdDialogI::setInterleavedDirty() {
  _w->_kstFFTOptions->Interleaved->setTristate(false);
  _interleavedDirty = true;
  // also set the FFTLen to be dirty, as presumably the user will think it
  // has been edited
  _fFTLenDirty = true;
}


void KstCsdDialogI::setVector(const QString& name) {
  _w->_vector->setSelection(name);
}

#include "kstcsddialog.moc"
// vim: ts=2 sw=2 et
