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

#include "equationdialog.h"

#include "dialogpage.h"

#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Equation"));
}


EquationTab::~EquationTab() {
}


EquationDialog::EquationDialog(QWidget *parent)
  : DataDialog(parent) {

  setWindowTitle(tr("New Equation"));

  _equationTab = new EquationTab(this);
  addDataTab(_equationTab);
}


EquationDialog::EquationDialog(KstObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  setWindowTitle(tr("Edit Equation"));

  _equationTab = new EquationTab(this);
  addDataTab(_equationTab);

  //FIXME need to do validation to enable/disable ok button...
}


EquationDialog::~EquationDialog() {
}


QString EquationDialog::tagName() const {
  return DataDialog::tagName();
}


KstObjectPtr EquationDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


KstObjectPtr EquationDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
