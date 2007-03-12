/***************************************************************************
                    kstviewmatricesdialog.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2005 The University of British Columbia
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

// includes files for Qt
#include <qlayout.h>
#include <qpushbutton.h>
#include <q3table.h>

// include files for KDE
#include <klocale.h>

// application specific includes
#include "kstmatrix.h"
#include "kstdatacollection.h"
#include "kstviewmatricesdialog.h"
#include "matrixselector.h"

KstViewMatricesDialogI::KstViewMatricesDialogI(QWidget* parent, Qt::WindowFlags fl)
: QDialog(parent, fl) {
  _tableMatrices = new KstMatrixTable(this, "tableMatrices");
  _tableMatrices->setNumRows(0);
  _tableMatrices->setNumCols(5);
  _tableMatrices->setReadOnly(true);
  _tableMatrices->setSorting(false);
  _tableMatrices->setSelectionMode(Q3Table::Single);
  vboxLayout->insertWidget(1, _tableMatrices);

  connect(Cancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(matrixSelector, SIGNAL(selectionChanged(const QString&)), this, SLOT(matrixChanged(const QString&)));
  connect(matrixSelector, SIGNAL(newMatrixCreated(const QString&)), this, SLOT(matrixChanged(const QString&)));

  languageChange();
}


KstViewMatricesDialogI::~KstViewMatricesDialogI() {
}


bool KstViewMatricesDialogI::hasContent() const {
  return !KST::matrixList.isEmpty();
}


void KstViewMatricesDialogI::updateViewMatricesDialog() {
  matrixSelector->update();
  QString matrix = matrixSelector->selectedMatrix();
  _tableMatrices->setMatrix(matrix);
  updateViewMatricesDialog(matrix);
}


void KstViewMatricesDialogI::updateViewMatricesDialog(const QString& matrixName) {
  int needed = 0;
  
  KST::matrixList.lock().readLock();
  KstMatrixPtr matrix = *KST::matrixList.findTag(matrixName);
  KST::matrixList.lock().unlock();
  if (matrix) {
    matrix->readLock();
    
    needed = matrix->xNumSteps();
    if (needed != _tableMatrices->numCols()) {
      _tableMatrices->setNumCols(needed);
    }    
    
    needed = matrix->yNumSteps();
    if (needed != _tableMatrices->numRows()) {
      _tableMatrices->setNumRows(needed);
    }  
        
    matrix->unlock();
  }
}


void KstViewMatricesDialogI::showViewMatricesDialog() {
  updateViewMatricesDialog();
  updateDefaults(0);
  show();
  raise();
}


void KstViewMatricesDialogI::matrixChanged(const QString& matrix) {
  updateViewMatricesDialog(matrix);
  _tableMatrices->setMatrix(matrix);
  _tableMatrices->update();
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KstViewMatricesDialogI::languageChange() {
  setWindowTitle(i18n("View Matrix Values"));
}


void KstViewMatricesDialogI::updateDefaults(int index) {
  Q_UNUSED(index)
}

#include "kstviewmatricesdialog.moc"
// vim: ts=2 sw=2 et
