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

#include "datadialog.h"

#include "dialogtab.h"
#include "dialogpage.h"

#include "kstdataobject.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

namespace Kst {

DataDialog::DataDialog(QWidget *parent)
  : Dialog(parent), _dataObject(0) {

  createGui();
}

DataDialog::DataDialog(KstObjectPtr dataObject, QWidget *parent)
  : Dialog(parent), _dataObject(dataObject) {

  createGui();
}


DataDialog::~DataDialog() {
}

void DataDialog::createGui() {

  buttonBox()->button(QDialogButtonBox::Apply)->setVisible(false);

  connect(this, SIGNAL(ok()), this, SLOT(slotOk()));

  QWidget *box = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(box);
  layout->setContentsMargins(0, -1, 0, -1);

  QLabel *name = new QLabel(tr("Unique Name:"), box);
  QLineEdit *edit = new QLineEdit(box);

  if (_dataObject)
    edit->setText(_dataObject->tagName());

  layout->addWidget(name);
  layout->addWidget(edit);

  box->setLayout(layout);
}

void DataDialog::addDataTab(DialogTab *tab) {
  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tab->tabTitle());
  page->addDialogTab(tab);
  addDialogPage(page);
}

void DataDialog::slotOk() {
  KstObjectPtr ptr;
  if (!dataObject())
    ptr = createNewDataObject();
  else
    ptr = editExistingDataObject();
  setDataObject(ptr);
}

}

// vim: ts=2 sw=2 et
