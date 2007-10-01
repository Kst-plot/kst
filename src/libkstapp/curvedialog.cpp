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

#include "curvedialog.h"

#include "dialogpage.h"

#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"

namespace Kst {

CurveTab::CurveTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Curve"));
}


CurveTab::~CurveTab() {
}


CurveDialog::CurveDialog(KstObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Curve"));
  else
    setWindowTitle(tr("New Curve"));

  _curveTab = new CurveTab(this);
  addDataTab(_curveTab);

  //FIXME need to do validation to enable/disable ok button...
}


CurveDialog::~CurveDialog() {
}


QString CurveDialog::tagName() const {
  return DataDialog::tagName();
}


KstObjectPtr CurveDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


KstObjectPtr CurveDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
