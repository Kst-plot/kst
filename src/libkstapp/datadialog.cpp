/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
#include "updatemanager.h"
#include "sessionmodel.h"

namespace Kst {

DataDialog::DataDialog(Kst::ObjectPtr dataObject, QWidget *parent, bool edit_multiple)
  : Dialog(parent), _dataObject(dataObject), _modified(false) {

  _dataObjectName.clear();

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

  createGui(edit_multiple);
}


DataDialog::~DataDialog() {
}

void DataDialog::createGui(bool edit_multiple) {

  if (_mode == New) {
    buttonBox()->button(QDialogButtonBox::Apply)->setVisible(false);
  }

  connect(this, SIGNAL(apply()), this, SLOT(slotApply()));

  QWidget *extension = extensionWidget();

  QVBoxLayout *extensionLayout = new QVBoxLayout(extension);
  extensionLayout->setContentsMargins(0, -1, 0, -1);

  if (edit_multiple) {
    _editMultipleWidget = new EditMultipleWidget();
    extensionLayout->addWidget(_editMultipleWidget);
  } else {
    _editMultipleWidget = 0L;
  }

  extension->setLayout(extensionLayout);

  QWidget *box = topCustomWidget();

  QHBoxLayout *layout = new QHBoxLayout(box);

  _nameLabel = new QLabel(tr("&Name:"), box);
  _tagString = new QLineEdit(box);
  connect(_tagString, SIGNAL(textChanged(QString)), this, SLOT(modified()));
  _nameLabel->setBuddy(_tagString);

  _shortName = new QLabel(QString(), box);

  _tagStringAuto = new QCheckBox(tr("&Auto","automatic"), box);
  connect(_tagStringAuto, SIGNAL(toggled(bool)), _tagString, SLOT(setDisabled(bool)));

  if (edit_multiple) {
    _editMultipleButton = new QPushButton(tr("Edit Multiple >>"));
    _editMultipleButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(_editMultipleButton, SIGNAL(clicked()), this, SLOT(slotEditMultiple()));
  }
  QLabel *spacer = new QLabel();

  if (_dataObject) {
    setTagString(_dataObject->descriptiveName());
    setShortName(_dataObject->shortName());
    _tagStringAuto->setChecked(!_dataObject->descriptiveNameIsManual());
  } else {
    _tagStringAuto->setChecked(true);
    setTagString(QString());
    if (edit_multiple) {
      _editMultipleButton->setVisible(false);
    }
  }

  layout->addWidget(_nameLabel);
  layout->addWidget(_tagString);
  layout->addWidget(_shortName);
  layout->addWidget(_tagStringAuto);
  layout->addWidget(spacer);
  if (edit_multiple) {
    layout->addWidget(_editMultipleButton);
  }

  box->setLayout(layout);

}


QString DataDialog::tagString() const {
  return _tagString->text();
}


void DataDialog::setTagString(const QString &tagString) {
  _tagString->setText(tagString);
}

void DataDialog::setShortName(const QString &name) {
  _shortName->setText('('+name+')');
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
  bool do_session_reset = false;
  if (!dataObject()) {
    ptr = createNewDataObject();
    do_session_reset = true;
  } else {
    ptr = editExistingDataObject();
  }
  setDataObject(ptr);
  UpdateManager::self()->doUpdates();
  kstApp->mainWindow()->document()->setChanged(true);
  if (do_session_reset) {
    kstApp->mainWindow()->document()->session()->triggerReset();
  }
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
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_modified);
}

void DataDialog::editMultiple(const QList<ObjectPtr> &objects) {
  slotEditMultiple();

  QStringList names;
  int n = objects.count();
  for (int i = 0; i < n; i++) {
    names.append(objects.at(i)->Name());
  }

  _editMultipleWidget->selectObjects(names);
}

void DataDialog::slotEditMultiple() {
  int charWidth = fontMetrics().averageCharWidth();

  int currentWidth = width();
  int extensionWidth = extensionWidget()->width();
  if (extensionWidth<charWidth*20) extensionWidth = charWidth*25; // FIXME: magic number hack...
  extensionWidget()->setVisible(!extensionWidget()->isVisible());
  if (!extensionWidget()->isVisible()) {
    _tagString->setVisible(true);
    _shortName->setVisible(true);
    _tagStringAuto->setVisible(true);
    _nameLabel->setVisible(true);
    setMinimumWidth(currentWidth - extensionWidth);
    resize(currentWidth - extensionWidth, height());
    _mode = Edit;
    _editMultipleButton->setText(tr("Edit Multiple >>"));
    emit editSingleMode();
  } else {
    if (currentWidth<charWidth*50) currentWidth = charWidth*80; // FIXME: magic number hack...
    _tagString->setVisible(false);
    _shortName->setVisible(false);
    _tagStringAuto->setVisible(false);
    _nameLabel->setVisible(false);
    setMinimumWidth(currentWidth + extensionWidth);
    resize(currentWidth + extensionWidth, height());
    _mode = EditMultiple;
    _editMultipleButton->setText(tr("<< Edit one %1").arg(_shortName->text()));
    emit editMultipleMode();
  }
  clearModified();
}

QString DataDialog::dataObjectName() const {
  return _dataObjectName;
}



}

// vim: ts=2 sw=2 et
