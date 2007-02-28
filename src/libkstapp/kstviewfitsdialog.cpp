/***************************************************************************
                    kstviewfitsdialog.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 The University of British Columbia
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

#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtable.h>
#include <qtimer.h>

#include <klocale.h>

#include "kstcplugin.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstviewfitsdialog.h"

KstViewFitsDialogI::KstViewFitsDialogI(QWidget* parent,
                                             const char* name,
                                             bool modal,
                                             WFlags fl) 
: KstViewFitsDialog(parent, name, modal, fl) {

  tableFits = new KstFitTable(this, "tableFits");
  tableFits->setNumRows(0);
  tableFits->setNumCols(1);
  tableFits->setReadOnly(true);
  tableFits->setSorting(false);
  tableFits->setSelectionMode(QTable::Single);
  layout2->addWidget(tableFits, 2, 0);

  connect(Cancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(_comboBoxFits, SIGNAL(activated(const QString&)), this, SLOT(fitChanged(const QString&)));

  tableFits->setReadOnly(true);
}

KstViewFitsDialogI::~KstViewFitsDialogI() {
}


bool KstViewFitsDialogI::hasContent() const {
  bool content = false;
  KstCPluginList fits = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  KstCPluginList::ConstIterator it = fits.begin();
  for (; it != fits.end(); ++it) {
    (*it)->readLock();
    content = (*it)->plugin()->data()._isFit ? true : content;
    (*it)->unlock();
  }
  return content;
}


void KstViewFitsDialogI::fillComboBox(const QString& str) {
  QString fitName = str;
  bool changed = false;
  
  _comboBoxFits->clear();
  KstCPluginList fits = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  for (uint i = 0; i < fits.count(); i++) {
    KstCPluginPtr fit = fits[i];
    fit->readLock();
    if (fit->plugin()->data()._isFit) {
      _comboBoxFits->insertItem(fit->tagName());
      if (fitName == fit->tagName() || fitName.isEmpty()) {
        _comboBoxFits->setCurrentItem(_comboBoxFits->count() - 1);
        if (fitName.isEmpty()) {
          fitName = fit->tagName();
        }
        changed = true;
        fitChanged(fitName);
      }
    }
    fit->unlock();
  }

  if (!changed) {
    fitChanged(_comboBoxFits->currentText());
  }
}

void KstViewFitsDialogI::updateViewFitsDialog() {
  if (_comboBoxFits->listBox()->isVisible()) {
    QTimer::singleShot(250, this, SLOT(updateViewFitsDialog()));
  } else {
    QString old;
    if (_comboBoxFits->count() > 0) {
      int idx = _comboBoxFits->currentItem();
      old = _comboBoxFits->text(idx);
    }
    fillComboBox(old);
  }
}

void KstViewFitsDialogI::showViewFitsDialog(const QString& fit) {
  fillComboBox(fit);
}

void KstViewFitsDialogI::showViewFitsDialog() {
  updateViewFitsDialog();
  updateDefaults(0);

  show();
  raise();
}

void KstViewFitsDialogI::fitChanged(const QString& strFit) {
  double* params = 0L;
  double* covars = 0L;
  double chi2Nu = 0.0;
  int numParams = 0;
  int numCovars = 0;

  KstCPluginList fits = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  KstCPluginPtr plugin = *(fits.findTag(strFit));
  if (plugin) {
    plugin->readLock();
    const KstScalarMap& scalars = plugin->outputScalars();
    KstScalarPtr scalarChi2Nu = scalars["chi^2/nu"];
    if (scalarChi2Nu) {
      scalarChi2Nu->readLock();
      chi2Nu = scalarChi2Nu->value();
      scalarChi2Nu->unlock();
    }

    const KstVectorMap& vectors = plugin->outputVectors();
    KstVectorPtr vectorParam = vectors["Parameters"];
    if (vectorParam) {
      vectorParam->readLock();
      KstVectorPtr vectorCovar = vectors["Covariance"];
      if (vectorCovar) {
        vectorCovar->readLock();
        numParams = vectorParam->length();
        numCovars = vectorCovar->length();

        if (numParams > 0 && numCovars > 0) {
          params = new double[numParams];
          covars = new double[numCovars];

          for (int i = 0; i < numParams; i++) {
            params[i] = vectorParam->value(i);
          }

          for (int i = 0; i < numCovars; i++) {
            covars[i] = vectorCovar->value(i);
          }
        }
        vectorCovar->unlock();
      }
      vectorParam->unlock();
    }
    plugin->unlock();
  }

  tableFits->setParameters(params, numParams, covars, numCovars, chi2Nu);

  if (numParams > 0) {
    tableFits->horizontalHeader()->setLabel(0, i18n("Value"));
    tableFits->horizontalHeader()->setLabel(1, i18n("Covariance:"));

    tableFits->verticalHeader()->setLabel(numParams+0, "---");
    tableFits->verticalHeader()->setLabel(numParams+1, i18n("Chi^2/Nu"));

    if (plugin) {
      plugin->readLock();
      KstSharedPtr<Plugin> pluginBase = plugin->plugin();
      if (pluginBase) {
        textLabelFit->setText(pluginBase->data()._readableName);
        for (int i = 0; i < numParams; i++) {
          QString parameterName = pluginBase->parameterName(i);
          tableFits->horizontalHeader()->setLabel(i + 2, parameterName);
          tableFits->verticalHeader()->setLabel(i, parameterName);
        }
      }
      plugin->unlock();
    }
  }

  tableFits->update();
}

void KstViewFitsDialogI::updateDefaults(int index) {
  Q_UNUSED(index)
}

#include "kstviewfitsdialog.moc"
// vim: ts=2 sw=2 et
