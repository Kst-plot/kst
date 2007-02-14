/***************************************************************************
                       ksteqdialog_i.cpp  -  Part of KST
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
#include <qlistbox.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qvbox.h>

// include files for KDE
#include <kcombobox.h>
#include "ksdebug.h"
#include <kmessagebox.h>

// application specific includes
#include "curveappearancewidget.h"
#include "curveplacementwidget.h"
#include "editmultiplewidget.h"
#include "eqdialogwidget.h"
#include "eparse-eh.h"
#include "kst2dplot.h"
#include "kstdataobjectcollection.h"
#include "kstdefaultnames.h"
#include "ksteqdialog_i.h"
#include "kstuinames.h"
#include "kstvcurve.h"
#include "kstviewwindow.h"
#include "scalarselector.h"
#include "vectorselector.h"

const QString& KstEqDialogI::defaultTag = KGlobal::staticQString("<Auto Name>");

QGuardedPtr<KstEqDialogI> KstEqDialogI::_inst;

KstEqDialogI *KstEqDialogI::globalInstance() {
  if (!_inst) {
    _inst = new KstEqDialogI(KstApp::inst());
  }
  return _inst;
}


KstEqDialogI::KstEqDialogI(QWidget* parent, const char* name, bool modal, WFlags fl)
: KstDataDialog(parent, name, modal, fl) {
  _w = new EqDialogWidget(_contents);
  setMultiple(true);
  connect(_w->_vectors, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
  connect(_w->_xVectors, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
  connect(_w->_scalars, SIGNAL(newScalarCreated()), this, SIGNAL(modified()));
  
  // for edit multiple mode
  connect(_w->_doInterpolation, SIGNAL(clicked()), this, SLOT(setDoInterpolationDirty()));
  populateFunctionList();
}


KstEqDialogI::~KstEqDialogI() {
}


void KstEqDialogI::updateWindow() {
  _w->_curvePlacement->update();
}


void KstEqDialogI::fillFieldsForEdit() {
  KstEquationPtr ep = kst_cast<KstEquation>(_dp);
  if (!ep) {
    return; // shouldn't be needed
  }

  ep->readLock();
  _tagName->setText(ep->tagName());
  _w->_equation->setText(ep->equation());

  _w->_doInterpolation->setChecked(ep->doInterp());
  if (ep->vXIn()) {
    _w->_xVectors->setSelection(ep->vXIn()->tag().displayString());
  }

  ep->unlock();

  _w->_curveAppearance->hide();
  _w->_curvePlacement->hide();
  _legendText->hide();
  _legendLabel->hide();
  
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstEqDialogI::fillFieldsForNew() {
  KstEquationList eqs = kstObjectSubList<KstDataObject, KstEquation>(KST::dataObjectList);

  /* set tag name */
  _tagName->setText(defaultTag);
  _legendText->setText(defaultTag);
  _legendText->show();
  _legendLabel->show();

  /* set the curve placement window  */
  _w->_curvePlacement->update();

  _w->_equation->clear();

  //for some reason the lower widget needs to be shown first to prevent overlapping?
  _w->_curveAppearance->hide();
  _w->_curvePlacement->show();
  _w->_curveAppearance->show();
  _w->_curveAppearance->reset();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstEqDialogI::update() {
  _w->_curvePlacement->update();
  _w->_vectors->update();
  _w->_xVectors->update();
  _w->_scalars->update();
}


bool KstEqDialogI::newObject() {
  QString tag_name = _tagName->text();
  QString etext = _w->_equation->text();
  etext.remove(QRegExp("[^a-zA-Z0-9\\(\\)\\+\\-\\*/\\%\\^\\|\\&\\!<>=_.]"));
  etext.replace(KstObjectTag::tagSeparator, KstObjectTag::tagSeparatorReplacement);
  if (etext.length() > 12) {
    etext.truncate(12);
    etext += "...";
  }

  if (tag_name == defaultTag) {
    tag_name = KST::suggestEQName(etext);
  }

  /* verify that the curve name is unique */
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    return false;
  }

  if (!checkEntries()) {
    return false;
  }

  KST::vectorList.lock().readLock();
  /* find *V */
  KstVectorPtr vp = *KST::vectorList.findTag(_w->_xVectors->selectedVector());
  if (!vp) {
    kstdFatal() << "Bug in kst: the Vector field (Eq) "
                << "refers to a non-existent vector..." << endl;
  }
  KST::vectorList.lock().unlock();

  /** Create the equation here */
  vp->readLock();
  KstEquationPtr eq = new KstEquation(tag_name, _w->_equation->text(), vp, _w->_doInterpolation->isChecked());
  vp->unlock();

  if (!eq->isValid()) {
    eq = 0L;
    QString parseErrors;
    for (QStringList::ConstIterator i = Equation::errorStack.begin(); i != Equation::errorStack.end(); ++i) {
      parseErrors += *i;
      parseErrors += "\n";
    }

    KMessageBox::detailedSorry(this, i18n("There is an error in the equation you entered."), parseErrors);
    return false;
  }

  KstVCurvePtr vc = new KstVCurve(KST::suggestCurveName(eq->tag(), true), eq->vX(), eq->vY(), 0L, 0L, 0L, 0L, _w->_curveAppearance->color());
  vc->setHasPoints(_w->_curveAppearance->showPoints());
  vc->setHasLines(_w->_curveAppearance->showLines());
  vc->setHasBars(_w->_curveAppearance->showBars());
  vc->setLineWidth(_w->_curveAppearance->lineWidth());
  vc->setLineStyle(_w->_curveAppearance->lineStyle());
  vc->pointType = _w->_curveAppearance->pointType();
  vc->setPointDensity(_w->_curveAppearance->pointDensity());
  vc->setBarStyle(_w->_curveAppearance->barStyle());
  
  QString legend_text = _legendText->text();
  if (legend_text == defaultTag) {
    vc->setLegendText(QString(""));
  } else {
    vc->setLegendText(legend_text);
  }

  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_w->_curvePlacement->_plotWindow->currentText()));
  if (!w) {
    QString n = KstApp::inst()->newWindow(KST::suggestWinName());
    w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(n));
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
  KST::dataObjectList.append(eq.data());
  KST::dataObjectList.append(vc.data());
  KST::dataObjectList.lock().unlock();

  eq = 0L; // drop the reference before we update
  vc = 0L;
  emit modified();
  return true;
}


bool KstEqDialogI::checkEntries() {
  if (_w->_xVectors->selectedVector().isEmpty() && !_editMultipleMode) {
    KMessageBox::sorry(this, i18n("An X vector must be defined first."));
    return false;
  }
  return true;
}


bool KstEqDialogI::editSingleObject(KstEquationPtr eqPtr) {
  eqPtr->writeLock();
  if (!checkEntries()) {
    eqPtr->unlock();
    return false;
  }

  // update the vector only if it is dirty 
  KstVectorPtr vp;
  if (_xVectorsDirty) {
    KST::vectorList.lock().readLock();
    /* find *V */
    KstVectorList::Iterator i = KST::vectorList.findTag(_w->_xVectors->selectedVector());
    if (i == KST::vectorList.end()) {
      kstdFatal() << "Bug in kst: the Vector field (Eq) "
                  << "refers to a non existant vector..." << endl;
    }
    vp = *i;
  } else {
    vp = eqPtr->vX();
  }
  KST::vectorList.lock().unlock();
  
  // update the DoInterpolation only if it is dirty
  if (_doInterpolationDirty) {
    eqPtr->setExistingXVector(vp, _w->_doInterpolation->isChecked());
  } else {
    eqPtr->setExistingXVector(vp, eqPtr->doInterp());
  }
  
  if (_equationDirty) {
    eqPtr->setEquation(_w->_equation->text());
    if (!eqPtr->isValid()) {
      QString parseErrors;
      for (QStringList::ConstIterator i = Equation::errorStack.begin(); i != Equation::errorStack.end(); ++i) {
        parseErrors += *i;
        parseErrors += "\n";
      }
      KMessageBox::detailedSorry(this, i18n("There is an error in the equation you entered."), parseErrors);
      eqPtr->unlock();
      return false;
    }
  }
  eqPtr->unlock();
  return true;
}


bool KstEqDialogI::editObject() {
  KstEquationList eqList = kstObjectSubList<KstDataObject,KstEquation>(KST::dataObjectList);
  
  // if editing multiple objects, edit each one
  if (_editMultipleMode) { 
    // if the user selected no vector, treat it as non-dirty
    _xVectorsDirty = _w->_xVectors->_vector->currentItem() != 0;
    _equationDirty = !_w->_equation->text().isEmpty();
  
    bool didEdit = false;
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); i++) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstEquationList::Iterator eqIter = eqList.findTag(_editMultipleWidget->_objectList->text(i));
        if (eqIter == eqList.end()) {
          return false;
        }
          
        KstEquationPtr eqPtr = *eqIter;
        if (!editSingleObject(eqPtr)) {
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
    // verify that the curve name is unique
    QString tag_name = _tagName->text();
    KstEquationPtr ep = kst_cast<KstEquation>(_dp);
    if (!ep || tag_name != ep->tagName() && KstData::self()->dataTagNameNotUnique(tag_name)) {
      _tagName->setFocus();
      return false;
    }
    
    ep->writeLock();
    ep->setTagName(tag_name);
    ep->unlock();
    
    // then edit the object
    _equationDirty = true;
    _xVectorsDirty = true;
    _doInterpolationDirty = true;
    if (!editSingleObject(ep)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstEqDialogI::populateFunctionList() {
  _w->Operators->clear();
  _w->Operators->insertItem("+");
  _w->Operators->insertItem("-");
  _w->Operators->insertItem("*");
  _w->Operators->insertItem("/");
  _w->Operators->insertItem("%");
  _w->Operators->insertItem("^");
  _w->Operators->insertItem("&");
  _w->Operators->insertItem("|");
  _w->Operators->insertItem("&&");
  _w->Operators->insertItem("||");
  _w->Operators->insertItem("!");
  _w->Operators->insertItem("<");
  _w->Operators->insertItem("<=");
  _w->Operators->insertItem("==");
  _w->Operators->insertItem(">=");
  _w->Operators->insertItem(">");
  _w->Operators->insertItem("!=");
  _w->Operators->insertItem("PI");
  _w->Operators->insertItem("e");
  _w->Operators->insertItem("STEP()");
  _w->Operators->insertItem("ABS()");
  _w->Operators->insertItem("SQRT()");
  _w->Operators->insertItem("CBRT()");
  _w->Operators->insertItem("SIN()");
  _w->Operators->insertItem("COS()");
  _w->Operators->insertItem("TAN()");
  _w->Operators->insertItem("ASIN()");
  _w->Operators->insertItem("ACOS()");
  _w->Operators->insertItem("ATAN()");
  _w->Operators->insertItem("SEC()");
  _w->Operators->insertItem("CSC()");
  _w->Operators->insertItem("COT()");
  _w->Operators->insertItem("SINH()");
  _w->Operators->insertItem("COSH()");
  _w->Operators->insertItem("TANH()");
  _w->Operators->insertItem("EXP()");
  _w->Operators->insertItem("LN()");
  _w->Operators->insertItem("LOG()");
  _w->Operators->insertItem("PLUGIN()");
}


void KstEqDialogI::populateEditMultiple() {
  // list of objects
  KstEquationList eqlist = kstObjectSubList<KstDataObject,KstEquation>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(eqlist.tagNames());

  // also intermediate state for multiple edit
  _w->_xVectors->_vector->insertItem("", 0);
  _w->_xVectors->_vector->setCurrentItem(0);
  _w->_doInterpolation->setTristate(true);
  _w->_doInterpolation->setNoChange();
  _w->_equation->setText("");
  
  // and clean all the fields
  _equationDirty = false;
  _xVectorsDirty = false;
  _doInterpolationDirty = false;
}


void KstEqDialogI::setDoInterpolationDirty() {
  _w->_doInterpolation->setTristate(false); 
  _doInterpolationDirty = true;
}

#include "ksteqdialog_i.moc"
// vim: ts=2 sw=2 et
