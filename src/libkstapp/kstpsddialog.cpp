/***************************************************************************
                       kstpsddialog.cpp  -  Part of KST
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
#include <qcheckbox.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qspinbox.h>
#include <q3vbox.h>

// include files for KDE
#include <kcombobox.h>
#include <knuminput.h>
#include <kmessagebox.h>

// application specific inclues
#include "kstfftoptions.h"
#include "curveappearancewidget.h"
#include "curveplacementwidget.h"
#include "editmultiplewidget.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstobjectdefaults.h"
#include "kstpsddialog.h"
#include "kstuinames.h"
#include "kstvcurve.h"
#include "kstviewwindow.h"
#include "vectorselector.h"

const QString& KstPsdDialogI::defaultTag = KGlobal::staticQString("<Auto Name>");

QPointer<KstPsdDialogI> KstPsdDialogI::_inst = 0L;

KstPsdDialogI *KstPsdDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstPsdDialogI(KstApp::inst());
  }
  return _inst;
}


KstPsdDialogI::KstPsdDialogI(QWidget* parent, Qt::WindowFlags fl)
: KstDataDialog(parent, fl) {

  _w = new Ui::KstPsdDialog;
  _w->setupUi(_contents);

  setMultiple(true);
  connect(_w->_vector, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
  
  //for multiple edit mode
  connect(_w->_kstFFTOptions->Apodize, SIGNAL(clicked()), this, SLOT(setApodizeDirty()));
  connect(_w->_kstFFTOptions->RemoveMean, SIGNAL(clicked()), this, SLOT(setRemoveMeanDirty()));
  connect(_w->_kstFFTOptions->Interleaved, SIGNAL(clicked()), this, SLOT(setInterleavedDirty()));
  connect(_w->_kstFFTOptions->InterpolateHoles, SIGNAL(clicked()), this, SLOT(setInterpolateHolesDirty()));
}


KstPsdDialogI::~KstPsdDialogI() {
  delete _w;
}


void KstPsdDialogI::updateWindow() {
  _w->_curvePlacement->update();
}


void KstPsdDialogI::fillFieldsForEdit() {
  KstPSDPtr pp = kst_cast<KstPSD>(_dp);
  if (!pp) {
    return; // shouldn't be needed
  }

  pp->readLock();

  _tagName->setText(pp->tagName());

  _w->_vector->setSelection(pp->vTag());

  // set sample rate, Units, FFT len, and vector units
  _w->_kstFFTOptions->FFTLen->setValue(pp->len());
  _w->_kstFFTOptions->SampRate->setText(QString::number(pp->freq()));

  _w->_kstFFTOptions->VectorUnits->setText(pp->vUnits());
  _w->_kstFFTOptions->RateUnits->setText(pp->rUnits());
  _w->_kstFFTOptions->Apodize->setChecked(pp->apodize());
  _w->_kstFFTOptions->ApodizeFxn->setCurrentIndex(pp->apodizeFxn());
  _w->_kstFFTOptions->Sigma->setValue(pp->gaussianSigma());
  _w->_kstFFTOptions->RemoveMean->setChecked(pp->removeMean());
  _w->_kstFFTOptions->Interleaved->setChecked(pp->average());
  _w->_kstFFTOptions->Output->setCurrentIndex(pp->output());
  _w->_kstFFTOptions->InterpolateHoles->setChecked(pp->interpolateHoles());

  pp->unlock();

  _w->_curveAppearance->hide();
  _w->_curvePlacement->hide();
  _legendText->hide();
  _legendLabel->hide();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstPsdDialogI::fillFieldsForNew() {
  _tagName->setText(defaultTag);
  _legendText->setText(defaultTag);
  _legendText->show();
  _legendLabel->show();
  KST::objectDefaults.sync();

  _w->_curvePlacement->update();
  _w->_kstFFTOptions->update();

  // for some reason the lower widget needs to be shown first to prevent overlapping?
  _w->_curveAppearance->hide();
  _w->_curvePlacement->show();
  _w->_curveAppearance->show();
  _w->_curveAppearance->reset();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstPsdDialogI::update() {
  _w->_curvePlacement->update();
  _w->_vector->update();
}


/* returns true if succesful */
bool KstPsdDialogI::newObject() {
  QString tag_name = _tagName->text();
  if (tag_name == defaultTag) {
    tag_name = KST::suggestPSDName(KstObjectTag::fromString(_w->_vector->selectedVector()));
  }

  // verify that the curve name is unique
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    return false;
  }

  if (_w->_vector->selectedVector().isEmpty()) {
    KMessageBox::sorry(this, i18n("New PSD not made: define vectors first."));
    return false;
  }

  KST::vectorList.lock().readLock();
  KstVectorPtr p = *KST::vectorList.findTag(_w->_vector->selectedVector());
  KST::vectorList.lock().unlock();
  if (!p) {
    kstdFatal() << "Bug in kst: the vector field (PSD) refers to "
                << "a non existant vector...." << endl;
  }

  // create the psd curve
  if (!_w->_kstFFTOptions->checkValues()) {
    return false;
  } else {
    p->readLock();
    KstPSDPtr psd = new KstPSD(tag_name, p,
                            _w->_kstFFTOptions->SampRate->text().toDouble(),
                            _w->_kstFFTOptions->Interleaved->isChecked(),
                            _w->_kstFFTOptions->FFTLen->text().toInt(),
                            _w->_kstFFTOptions->Apodize->isChecked(),
                            _w->_kstFFTOptions->RemoveMean->isChecked(),
                            _w->_kstFFTOptions->VectorUnits->text(),
                            _w->_kstFFTOptions->RateUnits->text(),
                            ApodizeFunction(_w->_kstFFTOptions->ApodizeFxn->currentIndex()),
                            _w->_kstFFTOptions->Sigma->value(),
                            PSDType(_w->_kstFFTOptions->Output->currentIndex()));
    psd->setInterpolateHoles(_w->_kstFFTOptions->InterpolateHoles->isChecked());
    p->unlock();

    KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(psd->tag(),true), psd->vX(), psd->vY(), 0L, 0L, 0L, 0L, _w->_curveAppearance->color());
    vc->setHasPoints(_w->_curveAppearance->showPoints());
    vc->setHasLines(_w->_curveAppearance->showLines());
    vc->setHasBars(_w->_curveAppearance->showBars());
    vc->pointType = _w->_curveAppearance->pointType();
    vc->setLineWidth(_w->_curveAppearance->lineWidth());
    vc->setLineStyle(_w->_curveAppearance->lineStyle());
    vc->setBarStyle(_w->_curveAppearance->barStyle());
    vc->setPointDensity(_w->_curveAppearance->pointDensity());

    QString legend_text = _legendText->text();
    if (legend_text == defaultTag) {
      vc->setLegendText(QString::null);
    } else {
      vc->setLegendText(legend_text);
    }

    Kst2DPlotPtr plot;
    KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_w->_curvePlacement->_plotWindow->currentText()));
    if (!w) {
      QString n = KstApp::inst()->newWindow(KST::suggestWinName());
      w = static_cast<KstViewWindow*>(KstApp::inst()->findWindow(n));
    }
    if (w) {
      if (_w->_curvePlacement->existingPlot()) {
        // assign curve to plot
        plot = kst_cast<Kst2DPlot>(w->view()->findChild(_w->_curvePlacement->plotName()));
        if (plot) {
          plot->addCurve(vc.data());
        }
      }

      if (_w->_curvePlacement->newPlot()) {
        // assign curve to plot
        QString name = w->createObject<Kst2DPlot>(KST::suggestPlotName());
        if (_w->_curvePlacement->reGrid()) {
          w->view()->cleanup(_w->_curvePlacement->columns());
        }
        plot = kst_cast<Kst2DPlot>(w->view()->findChild(name));
        if (plot) {
          plot->setXAxisInterpretation(false, KstAxisInterpretation(), KstAxisDisplay());
          plot->setYAxisInterpretation(false, KstAxisInterpretation(), KstAxisDisplay());
          _w->_curvePlacement->update();
          _w->_curvePlacement->setCurrentPlot(plot->tagName());
          plot->addCurve(vc.data());
          plot->generateDefaultLabels();
        }
      }
    }
    KST::dataObjectList.lock().writeLock();
    KST::dataObjectList.append(psd.data());
    KST::dataObjectList.append(vc.data());
    KST::dataObjectList.lock().unlock();
    psd = 0L;
    vc = 0L;
    emit modified();
  }
  return true;
}


bool KstPsdDialogI::editSingleObject(KstPSDPtr psPtr) {
  psPtr->writeLock();

  KST::vectorList.lock().readLock();
  KstVectorPtr v = *KST::vectorList.findTag(_w->_vector->selectedVector());
  KST::vectorList.lock().unlock();

  if (v) { // Can be null if edit multiple and it wasn't changed
    psPtr->setVector(v);
  }

  // get the values that need to be checked for consistency
  double pSampRate;
  int pFFTLen;
  
  if (_sampRateDirty) {
    pSampRate = _w->_kstFFTOptions->SampRate->text().toDouble();
  } else {
    pSampRate = psPtr->freq();
  }
    
  if (_fFTLenDirty) {
    pFFTLen = _w->_kstFFTOptions->FFTLen->text().toInt();
  } else {
    pFFTLen = psPtr->len();
  }
  
  if (!_w->_kstFFTOptions->checkGivenValues(pSampRate, pFFTLen)) {
    psPtr->unlock();
    return false;
  }

  if (_sampRateDirty) {
    psPtr->setFreq(_w->_kstFFTOptions->SampRate->text().toDouble());
  }
  
  if (_fFTLenDirty) {
    psPtr->setLen(_w->_kstFFTOptions->FFTLen->text().toInt());
  }

  if (_vectorUnitsDirty) {
    psPtr->setVUnits(_w->_kstFFTOptions->VectorUnits->text());
  }

  if (_rateUnitsDirty) {
    psPtr->setRUnits(_w->_kstFFTOptions->RateUnits->text());
  }

  if (_apodizeDirty) {
    psPtr->setApodize(_w->_kstFFTOptions->Apodize->isChecked());
  }

  if (_apodizeFxnDirty) {
    psPtr->setApodizeFxn(ApodizeFunction(_w->_kstFFTOptions->ApodizeFxn->currentIndex()));
  }

  if (_gaussianSigmaDirty) {
    psPtr->setGaussianSigma(_editMultipleMode ? _w->_kstFFTOptions->Sigma->value() - 1 : 
                                                _w->_kstFFTOptions->Sigma->value());
  }

  if (_removeMeanDirty) {
    psPtr->setRemoveMean(_w->_kstFFTOptions->RemoveMean->isChecked());
  }

  if (_interleavedDirty) {
    psPtr->setAverage(_w->_kstFFTOptions->Interleaved->isChecked());
  }

  if (_outputDirty) {
    psPtr->setOutput(PSDType(_w->_kstFFTOptions->Output->currentIndex()));
  }

  if (_interpolateHolesDirty) {
    psPtr->setInterpolateHoles(_w->_kstFFTOptions->InterpolateHoles->isChecked());
  }

  psPtr->unlock();
  return true;
}


// returns true if succesful
bool KstPsdDialogI::editObject() {
  // if the user selected no vector, treat it as non-dirty
  _vectorDirty = _w->_vector->_vector->currentIndex() != 0;
  _apodizeDirty = _w->_kstFFTOptions->ApodizeFxn->currentIndex() != 0;
  _fFTLenDirty = _w->_kstFFTOptions->FFTLen->text() != " ";
  _sampRateDirty = !_w->_kstFFTOptions->SampRate->text().isEmpty();
  _vectorUnitsDirty = !_w->_kstFFTOptions->VectorUnits->text().isEmpty();
  _rateUnitsDirty = !_w->_kstFFTOptions->RateUnits->text().isEmpty();
  _outputDirty = !_w->_kstFFTOptions->Output->currentIndex() != 0;
  KstPSDList psList = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList);
  
  // if editing multiple objects, edit each one
  if (_editMultipleMode) { 
    bool didEdit = false;
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); i++) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstPSDList::Iterator psIter = psList.findTag(_editMultipleWidget->_objectList->text(i));
        if (psIter == psList.end()) {
          return false;
        }
          
        KstPSDPtr psPtr = *psIter;
        
        if (!editSingleObject(psPtr)) {
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
    KstPSDPtr pp = kst_cast<KstPSD>(_dp);
    // verify that the name is unique
    QString tag_name = _tagName->text();
    if (!pp || (tag_name != pp->tagName() && KstData::self()->dataTagNameNotUnique(tag_name))) {
      _tagName->setFocus();
      return false;
    }
    
    pp->writeLock();
    pp->setTagName(KstObjectTag(tag_name, pp->tag().context())); // FIXME: doesn't allow changing tag context
    pp->unlock();
    
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
    _outputDirty = true;
    _interpolateHolesDirty = true;
    if (!editSingleObject(pp)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstPsdDialogI::populateEditMultiple() {
  KstPSDList pslist = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(pslist.tagNames());

  // also intermediate state for multiple edit
  _w->_vector->_vector->insertItem(0, "");
  _w->_vector->_vector->setCurrentIndex(0);
  _w->_kstFFTOptions->ApodizeFxn->insertItem(0, "");
  _w->_kstFFTOptions->ApodizeFxn->setCurrentIndex(0);
  _w->_kstFFTOptions->Apodize->setCheckState(Qt::PartiallyChecked);
  _w->_kstFFTOptions->RemoveMean->setCheckState(Qt::PartiallyChecked);
  _w->_kstFFTOptions->Interleaved->setCheckState(Qt::PartiallyChecked);
  _w->_kstFFTOptions->InterpolateHoles->setCheckState(Qt::PartiallyChecked);
  _w->_kstFFTOptions->SampRate->setText("");
  _w->_kstFFTOptions->VectorUnits->setText("");
  _w->_kstFFTOptions->RateUnits->setText("");
  _w->_kstFFTOptions->FFTLen->setMinimum(_w->_kstFFTOptions->FFTLen->minimum() - 1);
  _w->_kstFFTOptions->FFTLen->setSpecialValueText(" ");
  _w->_kstFFTOptions->FFTLen->setValue(_w->_kstFFTOptions->FFTLen->minimum());
  _w->_kstFFTOptions->Sigma->setMinimum(_w->_kstFFTOptions->Sigma->minimum() - 0.01);
  _w->_kstFFTOptions->Sigma->setSpecialValueText(" ");
  _w->_kstFFTOptions->Sigma->setValue(_w->_kstFFTOptions->Sigma->minimum());
  _w->_kstFFTOptions->Output->insertItem(0, "");
  _w->_kstFFTOptions->Output->setCurrentIndex(0);
  
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
  _interpolateHolesDirty = false;
}


void KstPsdDialogI::setApodizeDirty() {
  _w->_kstFFTOptions->Apodize->setTristate(false);
  _apodizeDirty = true;
}


void KstPsdDialogI::setRemoveMeanDirty() {
  _w->_kstFFTOptions->RemoveMean->setTristate(false);
  _removeMeanDirty = true;
}


void KstPsdDialogI::setInterpolateHolesDirty() {
  _w->_kstFFTOptions->InterpolateHoles->setTristate(false);
  _interpolateHolesDirty = true;
}


void KstPsdDialogI::setInterleavedDirty() {
  _w->_kstFFTOptions->Interleaved->setTristate(false);
  _interleavedDirty = true;
  // also set the FFTLen to be dirty, as presumably the user will think it
  // has been edited
  _fFTLenDirty = true;
}


void KstPsdDialogI::cleanup() {
  if (_editMultipleMode) {
    _w->_kstFFTOptions->Sigma->setMinimum(_w->_kstFFTOptions->Sigma->minimum() + 0.01);
    _w->_kstFFTOptions->Sigma->setSpecialValueText(QString::null);
    _w->_kstFFTOptions->FFTLen->setMinimum(_w->_kstFFTOptions->FFTLen->minimum() + 1);
    _w->_kstFFTOptions->FFTLen->setSpecialValueText(QString::null);
    _w->_kstFFTOptions->ApodizeFxn->removeItem(0);
    _w->_kstFFTOptions->Output->removeItem(0);
  }
}


void KstPsdDialogI::setVector(const QString& name) {
  _w->_vector->setSelection(name);
}

#include "kstpsddialog.moc"
// vim: ts=2 sw=2 et
