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

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDebug>

#include "datadialog.h"

#include "datatab.h"
#include "dialogpage.h"

#include "editmultiplewidget.h"

#include "dataobject.h"
#include "document.h"
#include "mainwindow.h"

namespace Kst {

DataDialog::DataDialog(Kst::ObjectPtr dataObject, QWidget *parent)
  : Dialog(parent), _defaultTagString("<Auto Name>"), _dataObject(dataObject) {

  if (_dataObject)
    _mode = Edit;
  else
    _mode = New;

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _document = mw->document();
  } else {
    // FIXME: we need a document
    qFatal("ERROR: can't construct a Data Dialog without a document");
  }

  createGui();
}


DataDialog::~DataDialog() {
}


void DataDialog::createGui() {

  buttonBox()->button(QDialogButtonBox::Apply)->setVisible(false);

  connect(this, SIGNAL(ok()), this, SLOT(slotOk()));

  QWidget *extension = extensionWidget();

  QVBoxLayout *extensionLayout = new QVBoxLayout(extension);
  extensionLayout->setContentsMargins(0, -1, 0, -1);

  _editMultipleWidget = new EditMultipleWidget();
  extensionLayout->addWidget(_editMultipleWidget);

  extension->setLayout(extensionLayout);

  QWidget *box = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(box);
  layout->setContentsMargins(0, -1, 0, -1);

  QLabel *label = new QLabel(tr("Unique Name:"), box);
  _tagString = new QLineEdit(box);

  QPushButton *button = new QPushButton(tr("Edit Multiple >>"));
  connect(button, SIGNAL(clicked()), this, SLOT(slotEditMultiple()));

  if (_dataObject) {
    setTagString(_dataObject->tag().tagString()); // FIXME: should this be displayString()?
  } else {
    setTagString(tagString());
    button->setVisible(false);
  }

  layout->addWidget(label);
  layout->addWidget(_tagString);
  layout->addWidget(button);

  box->setLayout(layout);
}


QString DataDialog::tagString() const {
//  const QString tag = _tagString->text();
//  return tag.isEmpty() ? _defaultTagString : tag;
  return _tagString->text();
}


void DataDialog::setTagString(const QString &tagString) {
  _tagString->setText(tagString);
}


void DataDialog::addDataTab(DataTab *tab) {
  Q_ASSERT(tab);
  Q_ASSERT(_document);
  tab->setObjectStore(_document->objectStore());
  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tab->tabTitle());
  page->addDialogTab(tab);
  addDialogPage(page);
}

void DataDialog::slotOk() {
  Kst::ObjectPtr ptr;
  if (!dataObject())
    ptr = createNewDataObject();
  else
    ptr = editExistingDataObject();
  setDataObject(ptr);
}


void DataDialog::slotEditMultiple() {
  int currentWidth = width();
  int extensionWidth = 400;
  extensionWidget()->setVisible(!extensionWidget()->isVisible());
 _tagString->setEnabled(!extensionWidget()->isVisible());
  if (!extensionWidget()->isVisible()) {
    setMaximumWidth(currentWidth - extensionWidth);
    setMinimumWidth(currentWidth - extensionWidth);
    _mode = Edit;
    emit editSingleMode();
  } else {
    setMinimumWidth(currentWidth + extensionWidth);
    setMaximumWidth(currentWidth + extensionWidth);
    _mode = EditMultiple;
    emit editMultipleMode();
  }
}


}

// vim: ts=2 sw=2 et
