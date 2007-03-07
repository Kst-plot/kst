/***************************************************************************
                      ksthsdialog.cpp  -  Part of KST
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
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <q3listbox.h>
#include <q3vbox.h>

// include files for KDE
#include <kcombobox.h>
#include "ksdebug.h"
#include <kmessagebox.h>

// application specific includes
#include "ksthsdialog.h"
#include "curveappearancewidget.h"
#include "curveplacementwidget.h"
#include "editmultiplewidget.h"
#include "histogramdialogwidget.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstsettings.h"
#include "kstuinames.h"
#include "kstvcurve.h"
#include "kstviewwindow.h"
#include "vectorselector.h"

#include "ui_ksthsdialog4.h"

const QString& KstHsDialogI::defaultTag = KGlobal::staticQString("<Auto Name>");

QPointer<KstHsDialogI> KstHsDialogI::_inst;

KstHsDialogI *KstHsDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstHsDialogI(KstApp::inst());
  }
  return _inst;
}


KstHsDialogI::KstHsDialogI(QWidget* parent, Qt::WindowFlags fl)
: KstDataDialog(parent, fl) {
  _w = new HistogramDialogWidget(_contents);
  setMultiple(true);
  connect(_w->AutoBin, SIGNAL(clicked()), this, SLOT(autoBin()));
  connect(_w->_vector, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
  connect(_w->_realTimeAutoBin, SIGNAL(clicked()), this, SLOT(updateButtons()));

  // signals for multiple edit mode
  connect(_w->_realTimeAutoBin, SIGNAL(clicked()), this, SLOT(setRealTimeAutoBinDirty()));
  connect(_w->NormIsPercent, SIGNAL(clicked()), this, SLOT(setNormIsPercentDirty()));
  connect(_w->NormIsFraction, SIGNAL(clicked()), this, SLOT(setNormIsFractionDirty()));
  connect(_w->PeakIs1, SIGNAL(clicked()), this, SLOT(setPeakIs1Dirty()));
  connect(_w->NormIsNumber, SIGNAL(clicked()), this, SLOT(setNormIsNumberDirty()));

  QColor qc = _w->_curveAppearance->color();
  _w->_curveAppearance->setValue(false, false, true, qc, 0, 0, 0, 1, 0);
}


KstHsDialogI::~KstHsDialogI() {
}


void KstHsDialogI::updateWindow() {
  _w->_curvePlacement->update();
}


void KstHsDialogI::fillFieldsForEdit() {
  KstHistogramPtr hp = kst_cast<KstHistogram>(_dp);
  if (!hp) {
    return; // shouldn't be needed
  }

  hp->readLock();

  _tagName->setText(hp->tagName());

  _w->_vector->setSelection(hp->vTag());

  _w->N->setValue(hp->nBins());
  _w->Min->setText(QString::number(hp->vX()->min() - (hp->width()/2.0)));
  _w->Max->setText(QString::number(hp->vX()->max() + (hp->width()/2.0)));
  _w->_realTimeAutoBin->setChecked(hp->realTimeAutoBin());

  if (hp->isNormPercent()) {
    _w->NormIsPercent->setChecked(true);
  } else if (hp->isNormFraction()) {
    _w->NormIsFraction->setChecked(true);
  } else if (hp->isNormOne()) {
    _w->PeakIs1->setChecked(true);
  } else {
    _w->NormIsNumber->setChecked(true);
  }


  hp->unlock();
  updateButtons();

  // can't edit curve props from here....
  _w->_curveAppearance->hide();
  _w->_curvePlacement->hide();
  _legendText->hide();
  _legendLabel->hide();

  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstHsDialogI::fillFieldsForNew() {
  // set tag name
  _tagName->setText(defaultTag);
  _legendText->setText(defaultTag);
  _legendText->show();
  _legendLabel->show();

  // set the curve placement window
  _w->_curvePlacement->update();

  // for some reason the lower widget needs to be shown first to prevent overlapping?
  _w->_curveAppearance->hide();
  _w->_curvePlacement->show();
  _w->_curveAppearance->show();
  _w->_curveAppearance->reset();

  QColor qc = _w->_curveAppearance->color();
  _w->_curveAppearance->setValue(false, false, true, qc, 0, 0, 0, 1, 0);

  _w->_realTimeAutoBin->setChecked(false);
  updateButtons();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstHsDialogI::update() {
  _w->_curvePlacement->update();
  _w->_vector->update();
}


bool KstHsDialogI::newObject() {
  QString tag_name = _tagName->text();
  if (tag_name == defaultTag) {
    tag_name = KST::suggestHistogramName(KstObjectTag::fromString(_w->_vector->selectedVector()));
  }

  // verify that the curve name is unique
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    return false;
  }

  if (_w->_vector->selectedVector().isEmpty()) {
    KMessageBox::sorry(this, i18n("New Histogram not made: define vectors first."));
    return false;
  }

  // find max and min
  double new_min = _w->Min->text().toDouble();
  double new_max = _w->Max->text().toDouble();
  if (new_max < new_min) {
    double m = new_max;
    new_max = new_min;
    new_min = m;
  }

  if (new_max == new_min) {
    KMessageBox::sorry(this, i18n("Max and Min can not be equal."));
    return false;
  }

  int new_n_bins = _w->N->text().toInt();
  if (new_n_bins < 1) {
    KMessageBox::sorry(this, i18n("You must have one or more bins in a histogram."));
    return false;
  }

  KstHsNormType new_norm_mode;
  if (_w->NormIsPercent->isChecked()) {
    new_norm_mode = KST_HS_PERCENT;
  } else if (_w->NormIsFraction->isChecked()) {
    new_norm_mode = KST_HS_FRACTION;
  } else if (_w->PeakIs1->isChecked()) {
    new_norm_mode = KST_HS_MAX_ONE;
  } else {
    new_norm_mode = KST_HS_NUMBER;
  }

  KstHistogramPtr hs;

  KST::vectorList.lock().readLock();
  KstVectorPtr vp = *KST::vectorList.findTag(_w->_vector->selectedVector());
  KST::vectorList.lock().unlock();
  if (!vp) {
    kstdFatal() << "Bug in kst: the Vector field (Hs) refers to "
                << " a non existant vector..." << endl;
  }

  vp->readLock();
  hs = new KstHistogram(tag_name, vp, new_min, new_max,
                        new_n_bins, new_norm_mode);
  vp->unlock();
  hs->setRealTimeAutoBin(_w->_realTimeAutoBin->isChecked());

  KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(hs->tag(), true), hs->vX(), hs->vY(), 0L, 0L, 0L, 0L, _w->_curveAppearance->color());

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
    vc->setLegendText(QString(""));
  } else {
    vc->setLegendText(legend_text);
  }

  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_w->_curvePlacement->_plotWindow->currentText()));
  if (!w) {
    QString n = KstApp::inst()->newWindow(KST::suggestWinName());
    w = static_cast<KstViewWindow*>(KstApp::inst()->findWindow(n));
  }
  if (w) {
    Kst2DPlotPtr plot;
    if (_w->_curvePlacement->existingPlot()) {
      /* assign curve to plot */
      plot = kst_cast<Kst2DPlot>(w->view()->findChild(_w->_curvePlacement->plotName()));
      if (plot) {
        plot->addCurve(vc.data());
      }
    }

    if (_w->_curvePlacement->newPlot()) {
      /* assign curve to plot */
      QString name = w->createObject<Kst2DPlot>(KST::suggestPlotName());
      if (_w->_curvePlacement->reGrid()) {
        w->view()->cleanup(_w->_curvePlacement->columns());
      }
      plot = kst_cast<Kst2DPlot>(w->view()->findChild(name));
      if (plot) {
        _w->_curvePlacement->update();
        _w->_curvePlacement->setCurrentPlot(plot->tagName());
        plot->addCurve(vc.data());
        plot->generateDefaultLabels();
      }
    }
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(hs.data());
  KST::dataObjectList.append(vc.data());
  KST::dataObjectList.lock().unlock();

  hs = 0L;
  vc = 0L;
  emit modified();

  return true;
}


bool KstHsDialogI::editSingleObject(KstHistogramPtr hsPtr) {
  // find max and min
  double new_min;
  double new_max;

  hsPtr->readLock();
  new_min = hsPtr->xMin();
  new_max = hsPtr->xMax();
  hsPtr->unlock();

  if (_minDirty) {
    new_min = _w->Min->text().toDouble();
  }

  if (_maxDirty) {
    new_max = _w->Max->text().toDouble();
  }

  if (new_max < new_min) {
    double m = new_max;
    new_max = new_min;
    new_min = m;
  }

  if (new_max == new_min) {
    KMessageBox::sorry(this, i18n("Max and Min can not be equal."));
    _w->Min->setFocus();
    return false;
  }

  int new_n_bins = _w->N->text().toInt();
  if (_nDirty && new_n_bins < 1) {
    KMessageBox::sorry(this, i18n("You must have one or more bins in a histogram."));
    _w->N->setFocus();
    return false;
  }

  if (_vectorDirty) {
    KST::vectorList.lock().readLock();
    hsPtr->setVector(*KST::vectorList.findTag(_w->_vector->selectedVector()));
    KST::vectorList.lock().unlock();
  }

  hsPtr->writeLock();

  if (_nDirty) {
    hsPtr->setNBins(new_n_bins);
  }

  if (_minDirty || _maxDirty) {
    hsPtr->setXRange(new_min, new_max);
  }

  if (_realTimeAutoBinDirty) {
    hsPtr->setRealTimeAutoBin(_w->_realTimeAutoBin->isChecked());
  }

  if (_normIsPercentDirty || _normIsFractionDirty || _peakIs1Dirty || _normIsNumberDirty) {
    if (_w->NormIsPercent->isChecked()) {
      hsPtr->setIsNormPercent();
    } else if (_w->NormIsFraction->isChecked()) {
      hsPtr->setIsNormFraction();
    } else if (_w->PeakIs1->isChecked()) {
      hsPtr->setIsNormOne();
    } else {
      hsPtr->setIsNormNum();
    }
  }

  hsPtr->setDirty();
  hsPtr->unlock();
  return true;
}


bool KstHsDialogI::editObject() {
  KstHistogramList hsList = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList);

  // if editing multiple objects, edit each one
  if (_editMultipleMode) {
    // if the user selected no vector, treat it as non-dirty
    _vectorDirty = _w->_vector->_vector->currentItem() != 0;
    _nDirty = _w->N->text() != " ";
    _minDirty = !_w->Min->text().isEmpty();
    _maxDirty = !_w->Max->text().isEmpty();

    bool didEdit = false;
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); i++) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstHistogramList::Iterator hsIter = hsList.findTag(_editMultipleWidget->_objectList->text(i));
        if (hsIter == hsList.end()) {
          return false;
        }

        KstHistogramPtr hsPtr = *hsIter;

        if (!editSingleObject(hsPtr)) {
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
    KstHistogramPtr hp = kst_cast<KstHistogram>(_dp);
    // verify that the curve name is unique
    QString tag_name = _tagName->text();
    if (!hp || (tag_name != hp->tagName() && KstData::self()->dataTagNameNotUnique(tag_name))) {
      _tagName->setFocus();
      return false;
    }
    hp->setTagName(KstObjectTag(tag_name, hp->tag().context())); // FIXME: doesn't allow changing tag context

    // then edit the object
    _vectorDirty = true;
    _minDirty = true;
    _maxDirty = true;
    _nDirty = true;
    _realTimeAutoBinDirty = true;
    _normIsPercentDirty = true;
    _normIsFractionDirty = true;
    _peakIs1Dirty = true;
    _normIsNumberDirty = true;
    if (!editSingleObject(hp)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstHsDialogI::autoBin() {
  KstReadLocker ml(&KST::vectorList.lock());

  if (!KST::vectorList.isEmpty()) {
    KstVectorList::Iterator i = KST::vectorList.findTag(_w->_vector->selectedVector());
    double max, min;
    int n;

    if (i == KST::vectorList.end()) {
      kstdFatal() << "Bug in kst: the Vector field in hsdialog refers to "
                  << "a non existant vector..." << endl;
    }
    (*i)->readLock(); // Hmm should we really lock here?  AutoBin should I think
    KstHistogram::AutoBin(KstVectorPtr(*i), &n, &max, &min);
    (*i)->unlock();

    _w->N->setValue(n);
    _w->Min->setText(QString::number(min));
    _w->Max->setText(QString::number(max));
  }
}


void KstHsDialogI::updateButtons() {
  if (!_editMultipleMode && _w->_realTimeAutoBin->isChecked()) {
    autoBin();
  }

  _w->Min->setEnabled(!_w->_realTimeAutoBin->isChecked());
  _w->Max->setEnabled(!_w->_realTimeAutoBin->isChecked());
  _w->N->setEnabled(!_w->_realTimeAutoBin->isChecked());
  _w->AutoBin->setEnabled(!_w->_realTimeAutoBin->isChecked() && !_editMultipleMode);
}


void KstHsDialogI::populateEditMultiple() {
  KstHistogramList hslist = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(hslist.tagNames());

  // also intermediate state for multiple edit
  _w->Min->setText("");
  _w->Max->setText("");

  _w->N->setMinValue(_w->N->minValue() - 1);
  _w->N->setSpecialValueText(" ");
  _w->N->setValue(_w->N->minValue());

  _w->_vector->_vector->insertItem("", 0);
  _w->_vector->_vector->setCurrentItem(0);
  _w->_realTimeAutoBin->setTristate(true);
  _w->_realTimeAutoBin->setNoChange();

  _w->NormIsPercent->setChecked(false);
  _w->NormIsFraction->setChecked(false);
  _w->PeakIs1->setChecked(false);
  _w->NormIsNumber->setChecked(false);
  _tagName->setText("");
  _tagName->setEnabled(false);
  _w->AutoBin->setEnabled(false);

  _w->Min->setEnabled(true);
  _w->Max->setEnabled(true);

  // and clean all the fields
  _minDirty = false;
  _maxDirty = false;
  _nDirty = false;
  _vectorDirty = false;
  _realTimeAutoBinDirty = false;
  _normIsPercentDirty = false;
  _normIsFractionDirty = false;
  _peakIs1Dirty = false;
  _normIsNumberDirty = false;
}


void KstHsDialogI::setRealTimeAutoBinDirty() {
  _w->_realTimeAutoBin->setTristate(false);
  _realTimeAutoBinDirty = true;
}


void KstHsDialogI::cleanup() {
  if (_editMultipleMode) {
    _w->N->setMinValue(_w->N->minValue() + 1);
    _w->N->setSpecialValueText(QString::null);
  }
}


void KstHsDialogI::setVector(const QString& name) {
  _w->_vector->setSelection(name);
}


#include "ksthsdialog.moc"
// vim: ts=2 sw=2 et
