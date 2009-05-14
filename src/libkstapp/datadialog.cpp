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
#include <QCheckBox>
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
#include "application.h"

namespace Kst {

DataDialog::DataDialog(Kst::ObjectPtr dataObject, QWidget *parent)
  : Dialog(parent), _dataObject(dataObject), _modified(false) {

  _dataObjectName = QString();

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

  if (_mode == New) {
    buttonBox()->button(QDialogButtonBox::Apply)->setVisible(false);
  }

  connect(this, SIGNAL(apply()), this, SLOT(slotApply()));

  QWidget *extension = extensionWidget();

  QVBoxLayout *extensionLayout = new QVBoxLayout(extension);
  extensionLayout->setContentsMargins(0, -1, 0, -1);

  _editMultipleWidget = new EditMultipleWidget();
  extensionLayout->addWidget(_editMultipleWidget);

  extension->setLayout(extensionLayout);

  QWidget *box = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(box);

  _nameLabel = new QLabel(tr("Name:"), box);
  _tagString = new QLineEdit(box);
  connect(_tagString, SIGNAL(textChanged(QString)), this, SLOT(modified()));
  _nameLabel->setBuddy(_tagString);

  _shortName = new QLabel(QString(), box);

  _tagStringAuto = new QCheckBox(tr("Auto","automatic"), box);
  connect(_tagStringAuto, SIGNAL(toggled(bool)), _tagString, SLOT(setDisabled(bool)));

  QPushButton *button = new QPushButton(tr("Edit Multiple >>"));
  connect(button, SIGNAL(clicked()), this, SLOT(slotEditMultiple()));

  if (_dataObject) {
    setTagString(_dataObject->descriptiveName());
    setShortName(_dataObject->shortName());
    _tagStringAuto->setChecked(!_dataObject->descriptiveNameIsManual());
  } else {
    _tagStringAuto->setChecked(true);
    setTagString(QString());
    button->setVisible(false);
  }

  layout->addWidget(_nameLabel);
  layout->addWidget(_tagString);
  layout->addWidget(_shortName);
  layout->addWidget(_tagStringAuto);
  layout->addWidget(button);

  box->setLayout(layout);

}


QString DataDialog::tagString() const {
  return _tagString->text();
}


void DataDialog::setTagString(const QString &tagString) {
  _tagString->setText(tagString);
}

void DataDialog::setShortName(const QString &name) {
  _shortName->setText("("+name+")");
}

bool DataDialog::tagStringAuto() const {
  return _tagStringAuto->isChecked();
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


void DataDialog::slotApply() {
  Kst::ObjectPtr ptr;
  if (!dataObject())
    ptr = createNewDataObject();
  else
    ptr = editExistingDataObject();
  setDataObject(ptr);
  kstApp->mainWindow()->document()->setChanged(true);
  clearModified();
}


void DataDialog::modified() {
  _modified = true;
  updateApplyButton();
}


void DataDialog::clearModified() {
  _modified = false;
  updateApplyButton();
}


void DataDialog::updateApplyButton() {
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(_modified);
}


void DataDialog::slotEditMultiple() {
  int currentWidth = width();
  int extensionWidth = extensionWidget()->width();
  if (extensionWidth<204) extensionWidth = 204; // FIXME: magic number hack...
  extensionWidget()->setVisible(!extensionWidget()->isVisible());
  //_tagString->setEnabled(!extensionWidget()->isVisible());
  if (!extensionWidget()->isVisible()) {
    _tagString->setVisible(true);
    _shortName->setVisible(true);
    _tagStringAuto->setVisible(true);
    _nameLabel->setVisible(true);
    setMinimumWidth(currentWidth - extensionWidth);
    resize(currentWidth - extensionWidth, height());
    _mode = Edit;
    emit editSingleMode();
  } else {
    _tagString->setVisible(false);
    _shortName->setVisible(false);
    _tagStringAuto->setVisible(false);
    _nameLabel->setVisible(false);
    setMinimumWidth(currentWidth + extensionWidth);
    resize(currentWidth + extensionWidth, height());
    _mode = EditMultiple;
    emit editMultipleMode();
  }
  clearModified();
}

QString DataDialog::dataObjectName() const {
  return _dataObjectName;
}



}

// vim: ts=2 sw=2 et
