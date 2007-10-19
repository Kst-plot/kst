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

#include "csddialog.h"

#include "dialogpage.h"

#include "psd.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"

namespace Kst {

CSDTab::CSDTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Cumulative Spectral Decay"));

}


CSDTab::~CSDTab() {
}


VectorPtr CSDTab::vector() const {
  return _vector->selectedVector();
}


CurvePlacement* CSDTab::curvePlacement() const {
  return _curvePlacement;
}


FFTOptions* CSDTab::FFTOptionsWidget() const {
  return _FFTOptions;
}


CSDDialog::CSDDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Cumulative Spectral Decay"));
  else
    setWindowTitle(tr("New Cumulative Spectral Decay"));

  _CSDTab = new CSDTab(this);
  addDataTab(_CSDTab);

  //FIXME need to do validation to enable/disable ok button...
}


CSDDialog::~CSDDialog() {
}


QString CSDDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr CSDDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


ObjectPtr CSDDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
