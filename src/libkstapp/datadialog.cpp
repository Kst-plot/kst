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

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

namespace Kst {

DataDialog::DataDialog(QWidget *parent)
  : Dialog(parent) {

  QWidget *box = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(box);
  layout->setContentsMargins(0, -1, 0, -1);

  QLabel *name = new QLabel(tr("Unique Name:"), box);
  QLineEdit *edit = new QLineEdit(box);
  layout->addWidget(name);
  layout->addWidget(edit);

  box->setLayout(layout);

}


DataDialog::~DataDialog() {
}

void DataDialog::addDataTab(DialogTab *tab) {
  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tab->tabTitle());
  page->addDialogTab(tab);
  addDialogPage(page);
}

}

// vim: ts=2 sw=2 et
