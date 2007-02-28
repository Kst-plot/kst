/***************************************************************************
                    kstviewvectorsdialog_i.cpp  -  Part of KST
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

#include <qlayout.h>
#include <qpushbutton.h>
#include <q3table.h>

#include <klocale.h>

#include "kstviewvectorsdialog_i.h"
#include "vectorselector.h"

KstViewVectorsDialogI::KstViewVectorsDialogI(QWidget* parent,
                                             const char* name,
                                             bool modal,
                                             Qt::WFlags fl)
: KstViewVectorsDialog(parent, name, modal, fl) {
  tableVectors = new KstVectorTable(this, "tableVectors");
  tableVectors->setNumRows(0);
  tableVectors->setNumCols(1);
  tableVectors->setReadOnly(true);
  tableVectors->setSorting(false);
  tableVectors->setSelectionMode(Q3Table::Single);
  layout2->insertWidget(1, tableVectors);

  connect(Cancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(vectorSelector, SIGNAL(selectionChanged(const QString&)), this, SLOT( vectorChanged(const QString&)));
  connect(vectorSelector, SIGNAL(newVectorCreated(const QString&)), this, SLOT(vectorChanged(const QString&)));

  if (tableVectors->numCols() != 1) {  
    for (; 0 < tableVectors->numCols(); ) {
      tableVectors->removeColumn(0);
    }
    tableVectors->insertColumns(0, 1);
  }

  tableVectors->setReadOnly(true);
  languageChange();
}


KstViewVectorsDialogI::~KstViewVectorsDialogI() {
}


bool KstViewVectorsDialogI::hasContent() const {
  return !KST::vectorList.isEmpty();
}


void KstViewVectorsDialogI::updateViewVectorsDialog() {
  vectorSelector->update();
  QString vector = vectorSelector->selectedVector();
  tableVectors->setVector(vector);
  updateViewVectorsDialog(vector);
}


void KstViewVectorsDialogI::updateViewVectorsDialog(const QString& vectorName) {
  int needed = 0;
  KST::vectorList.lock().readLock();
  KstVectorPtr vector = *KST::vectorList.findTag(vectorName);
  KST::vectorList.lock().unlock();
  if (vector) {
    vector->readLock();
    needed = vector->length();
    vector->unlock();
  }

  if (needed != tableVectors->numRows()) {
    tableVectors->setNumRows(needed);
  }
  QRect rect = tableVectors->horizontalHeader()->rect();
  tableVectors->setColumnWidth(0, rect.width());
}


void KstViewVectorsDialogI::showViewVectorsDialog() {
  updateViewVectorsDialog();
  updateDefaults(0);
  show();
  raise();
}


void KstViewVectorsDialogI::vectorChanged(const QString& vector) {
  updateViewVectorsDialog(vector);
  tableVectors->setVector(vector);
  tableVectors->update();
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void KstViewVectorsDialogI::languageChange() {
  setCaption(i18n("View Vector Values"));
  tableVectors->horizontalHeader()->setLabel(0, i18n("Values"));
  KstViewVectorsDialog::languageChange();
}


void KstViewVectorsDialogI::updateDefaults(int index) {
  Q_UNUSED(index)
}

#include "kstviewvectorsdialog_i.moc"
// vim: ts=2 sw=2 et
