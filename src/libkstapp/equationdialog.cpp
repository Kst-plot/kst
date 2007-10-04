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

#include "datacollection.h"
#include "dataobjectcollection.h"

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Equation"));
}


EquationTab::~EquationTab() {
}


EquationDialog::EquationDialog(Kst::ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Equation"));
  else
    setWindowTitle(tr("New Equation"));

  _equationTab = new EquationTab(this);
  addDataTab(_equationTab);

  //FIXME need to do validation to enable/disable ok button...
}


EquationDialog::~EquationDialog() {
}


QString EquationDialog::tagName() const {
  return DataDialog::tagName();
}


Kst::ObjectPtr EquationDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


Kst::ObjectPtr EquationDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
