/**************************************************************************
        kstcurvedifferentiate.cpp - source file: inherits designer dialog
                             -------------------
    begin                :  2005
    copyright            : (C) 2005 by University of British Columbia
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

#include <q3buttongroup.h> 
#include <qcheckbox.h> 
#include <qcombobox.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>

#include <klocale.h>
#include <kiconloader.h>

#include "kst2dplot.h"
#include "kstcurvedifferentiate.h"
#include "kstlinestyle.h"
#include "kstnumbersequence.h"
#include "kstviewwindow.h"

KstCurveDifferentiateI::KstCurveDifferentiateI(QWidget* parent, Qt::WindowFlags fl)
: QDialog(parent, fl) {

  availableListBox->clear();
  selectedListBox->clear();
  availableListBox->insertItem(i18n("Line Color"));  
  availableListBox->insertItem(i18n("Point Style"));  
  availableListBox->insertItem(i18n("Line Style"));
  availableListBox->insertItem(i18n("Line Width"));
  
  connect(Cancel_2, SIGNAL(clicked()), this, SLOT(close()));
  connect(OK_2, SIGNAL(clicked()), this, SLOT(apply()));
  
  // more connections to emulate kactionselector behaviour
  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
  connect(availableListBox, SIGNAL(highlighted(int)), this, SLOT(updateButtons()));
  connect(selectedListBox, SIGNAL(highlighted(int)), this, SLOT(updateButtons()));
  
  _up->setIcon(BarIcon("up"));
  _up->setEnabled(false);
  _down->setIcon(BarIcon("down"));
  _down->setEnabled(false);
  _add->setIcon(BarIcon("forward"));
  _add->setEnabled(false);
  _remove->setIcon(BarIcon("back"));
  _remove->setEnabled(false);

  maxLineWidth->setMaximum(KSTLINEWIDTH_MAX);
  maxLineWidth->setMinimum(1);
  
  _radioButtonRepeatPlot->setChecked(true);
  _radioButtonApplyAllWindows->setChecked(true);
  
  loadProperties();
  setOptions();
  updateCurveDifferentiate();
}


KstCurveDifferentiateI::~KstCurveDifferentiateI() {
  
}


void KstCurveDifferentiateI::updateCurveDifferentiate() {
  updateButtons();
}


void KstCurveDifferentiateI::showCurveDifferentiate() {
  updateCurveDifferentiate();
  show();
  raise();
}


void KstCurveDifferentiateI::updateButtons() {
  int i = selectedListBox->currentItem();
  if (i >= 0 && selectedListBox->isSelected(i)) {
    _remove->setEnabled(true); 
  } else {
    _remove->setEnabled(false);
  }
  
  i = availableListBox->currentItem();
  if (i >= 0 && availableListBox->isSelected(i)) {
    _add->setEnabled(true); 
  } else {
    _add->setEnabled(false);
  }

  _up->setEnabled(selectedListBox->currentItem() > 0);
  _down->setEnabled(selectedListBox->currentItem() >= 0 &&
                    selectedListBox->currentItem() < (int)selectedListBox->count() - 1);
}


void KstCurveDifferentiateI::loadProperties( ) {
  KConfig cfg("kstrc");

  _lineColorOrder = cfg.readNumEntry("DifferentiateLineColor", -1);
  _pointStyleOrder = cfg.readNumEntry("DifferentiatePointStyle", -1);
  _lineStyleOrder = cfg.readNumEntry("DifferentiateLineStyle", -1);
  _lineWidthOrder = cfg.readNumEntry("DifferentiateLineWidth", -1);
  _maxLineWidth = cfg.readNumEntry("DifferentiateMaxLineWidth", 1);
  _pointDensity = cfg.readNumEntry("DifferentiatePointDensity", 0);
  _repeatAcross = cfg.readNumEntry("DifferentiateRepeatAcross", 0);
  _applyTo = cfg.readNumEntry("DifferentiateApplyTo", 0);
}


void KstCurveDifferentiateI::saveProperties( ) {
  KConfig cfg("kstrc", KConfig::NoGlobals);

  cfg.writeEntry("DifferentiateLineColor", _lineColorOrder);
  cfg.writeEntry("DifferentiatePointStyle", _pointStyleOrder);
  cfg.writeEntry("DifferentiateLineStyle", _lineStyleOrder);
  cfg.writeEntry("DifferentiateLineWidth", _lineWidthOrder);
  cfg.writeEntry("DifferentiateMaxLineWidth", _maxLineWidth);
  cfg.writeEntry("DifferentiatePointDensity", _pointDensity);
  cfg.writeEntry("DifferentiateRepeatAcross", _repeatAcross);
  cfg.writeEntry("DifferentiateApplyTo", _applyTo);

  cfg.sync();
}  


void KstCurveDifferentiateI::setOptions( ) {  
  QRadioButton* button;
  
  availableListBox->clear();
  selectedListBox->clear();
  
  if (_lineColorOrder == -1) {
    availableListBox->insertItem(i18n("Line Color"));
  } else {
    selectedListBox->insertItem(i18n("Line Color"), _lineColorOrder);
  }
  if (_pointStyleOrder == -1) {
    availableListBox->insertItem(i18n("Point Style"));
  } else {
    selectedListBox->insertItem(i18n("Point Style"), _pointStyleOrder);
  }
  if (_lineStyleOrder == -1) {
    availableListBox->insertItem(i18n("Line Style"));
  } else {
    selectedListBox->insertItem(i18n("Line Style"), _lineStyleOrder);
  }
  if (_lineWidthOrder == -1) {
    availableListBox->insertItem(i18n("Line Width"));
  } else {
    selectedListBox->insertItem(i18n("Line Width"), _lineWidthOrder);
  }
  
  maxLineWidth->setValue(_maxLineWidth);
  pointDensity->setCurrentIndex(_pointDensity);

  button = (QRadioButton*)_buttonGroupRepeat->find(_repeatAcross);
  if (button) {
    button->setChecked(true);
  }
  button = (QRadioButton*)_buttonGroupApplyTo->find(_applyTo);
  if (button) {
    button->setChecked(true);
  }
}


void KstCurveDifferentiateI::getOptions( ) {
  _lineColorOrder  = selectedListBox->index(selectedListBox->findItem(i18n("Line Color"), Q3ListBox::ExactMatch));
  _pointStyleOrder = selectedListBox->index(selectedListBox->findItem(i18n("Point Style"), Q3ListBox::ExactMatch));
  _lineStyleOrder  = selectedListBox->index(selectedListBox->findItem(i18n("Line Style"), Q3ListBox::ExactMatch));
  _lineWidthOrder  = selectedListBox->index(selectedListBox->findItem(i18n("Line Width"), Q3ListBox::ExactMatch));
   
  _maxLineWidth = maxLineWidth->value();
  _pointDensity = pointDensity->currentIndex();
  
  //FIXME PORT
//   _repeatAcross = _buttonGroupRepeat->selectedId();
//   _applyTo = _buttonGroupApplyTo->selectedId();
}


void KstCurveDifferentiateI::removeButtonClicked() {
  // move from selected to available
  for (uint i = 0; i < selectedListBox->count(); i++) {
    if (selectedListBox->isSelected(i)) {
      availableListBox->insertItem(selectedListBox->text(i));
      selectedListBox->removeItem(i); 
      availableListBox->setSelected((int)availableListBox->count() - 1, true); 
    }  
  }
  updateButtons();
}


void KstCurveDifferentiateI::addButtonClicked() {
  // move from available to selected
  for (uint i = 0; i < availableListBox->count(); i++) {
    if (availableListBox->isSelected(i)) {
      selectedListBox->insertItem(availableListBox->text(i));
      availableListBox->removeItem(i);  
      selectedListBox->setSelected((int)selectedListBox->count() - 1, true);
    }  
  }
  updateButtons();
}


void KstCurveDifferentiateI::upButtonClicked() {
  // move item up
  int i = selectedListBox->currentItem();
  if (i != -1) {
    QString text = selectedListBox->currentText();
    selectedListBox->removeItem(i);
    selectedListBox->insertItem(text, i-1);
    selectedListBox->setSelected(i-1, true);
    updateButtons();
  }
}


void KstCurveDifferentiateI::downButtonClicked() {
  // move item down
  int i = selectedListBox->currentItem();
  if (i != -1) {
    QString text = selectedListBox->currentText();
    selectedListBox->removeItem(i);
    selectedListBox->insertItem(text, i+1);
    selectedListBox->setSelected(i+1, true);
    updateButtons();
  }
}


void KstCurveDifferentiateI::cycleWindow(KstViewWindow *window) {
  KstTopLevelViewPtr tlv = kst_cast<KstTopLevelView>(window->view());
  if (tlv) {         
    Kst2DPlotList plotList = tlv->findChildrenType<Kst2DPlot>(false);
    for (Kst2DPlotList::Iterator it = plotList.begin(); it != plotList.end(); ++it ) {
      if (_repeatAcross == 0) {
        _seqVect[0]->reset();
      }
      
      KstVCurveList vcurves = kstObjectSubList<KstBaseCurve,KstVCurve>((*it)->Curves);
      for (KstVCurveList::Iterator i = vcurves.begin(); i != vcurves.end(); ++i) {
        if (_lineColorOrder > -1) {
          (*i)->setColor(KstColorSequence::entry(_lineColorSeq.current()));
        }
        if (_pointStyleOrder > -1) {
          (*i)->pointType = _pointStyleSeq.current();
          (*i)->setHasPoints(true);
          (*i)->setPointDensity(_pointDensity);
        }
        if (_lineStyleOrder > -1) {
          (*i)->setLineStyle(_lineStyleSeq.current());
        }
        if (_lineWidthOrder > -1) {
          (*i)->setLineWidth(_lineWidthSeq.current());
        }

        (_seqVect[0])->next();        
      }
    }
  }
}


void KstCurveDifferentiateI::apply() {
  KstApp *app = KstApp::inst();
  KstViewWindow *window;
  int maxSequences = 0;
  
  getOptions();
  saveProperties();
  
  _seqVect.clear();
  _seqVect.resize(4);
  
  if (_lineColorOrder > -1) {
    _lineColorSeq.setRange(0, KstColorSequence::count());
    _seqVect.insert(_lineColorOrder, &_lineColorSeq);
    maxSequences++;
  }
  if (_pointStyleOrder > -1) {
    _pointStyleSeq.setRange(0, KSTPOINT_MAXTYPE - 1);
    _seqVect.insert(_pointStyleOrder, &_pointStyleSeq);
    maxSequences++;
  }
  if (_lineStyleOrder > -1) {
    _lineStyleSeq.setRange(0, KSTLINESTYLE_MAXTYPE - 1);
    _seqVect.insert(_lineStyleOrder, &_lineStyleSeq);
    maxSequences++;
  }
  if (_lineWidthOrder > -1) {
    _lineWidthSeq.setRange(1, _maxLineWidth);
    _seqVect.insert(_lineWidthOrder, &_lineWidthSeq);
    maxSequences++;
  }
  
  if (maxSequences > 0) {
    int i;
    
    _seqVect.resize(maxSequences);
    for (i = 0; i < maxSequences-1; i++) {
      _seqVect[i]->hookToNextSequence(_seqVect[i+1]);
    }
    _seqVect[maxSequences-1]->hookToNextSequence(0L);
        
    if (_applyTo == 0) {
      window = dynamic_cast<KstViewWindow*>(app->activeWindow());
      
      if (window) {
        cycleWindow(window);
      }
    } else {
      QList<QWidget*> childViews = app->childViews();
      QListIterator<KMdiChildView*> it(childViews);
      while (it.hasNext()) {
        if (_repeatAcross == 1) {
          _seqVect[0]->reset();
        }

        window = dynamic_cast<KstViewWindow*>(it.next());
        if (window && !window->view()->children().isEmpty()) {
          cycleWindow(window);
        }
        it.next();
      }
    }
  }

  close();
}

#include "kstcurvedifferentiate.moc"
// vim: ts=2 sw=2 et
