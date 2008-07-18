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

#include "scalardialog.h"
#include "enodes.h"
#include "document.h"
#include "objectstore.h"

#include <QPushButton>

namespace Kst {

ScalarTab::ScalarTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Scalar"));

  connect(_scalarValue, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged()));
}


ScalarTab::~ScalarTab() {
}


QString ScalarTab::value() const {
  return _scalarValue->text();
}


void ScalarTab::setValue(const QString &value) {
  _scalarValue->setText(value);
}


void ScalarTab::textChanged() {
  emit valueChanged();
}


ScalarDialog::ScalarDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Scalar"));
  else
    setWindowTitle(tr("New Scalar"));

  _scalarTab = new ScalarTab(this);
  addDataTab(_scalarTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  }

  connect(_scalarTab, SIGNAL(valueChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


ScalarDialog::~ScalarDialog() {
}


// QString ScalarDialog::tagString() const {
//   return DataDialog::tagString();
// }


void ScalarDialog::configureTab(ObjectPtr object) {
  if (ScalarPtr scalar = kst_cast<Scalar>(object)) {
    _scalarTab->setValue(QString::number(scalar->value()));
  }
}

void ScalarDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!_scalarTab->value().isEmpty());
}


ObjectPtr ScalarDialog::createNewDataObject(){
  bool ok = false;

  Q_ASSERT(_document && _document->objectStore());

  double value = _scalarTab->value().toDouble(&ok);
  if (!ok) {
    value = Equations::interpret(_document->objectStore(), _scalarTab->value().toLatin1(), &ok);
  }

  if (!ok) {
    return 0; //invalid
  }

//   qDebug() << "Creating new scalar  ===>"
//            << "\n\ttag:" << DataDialog::tagString()
//            << "\n\tvalue:" << value
//            << endl;

  ScalarPtr scalar = _document->objectStore()->createObject<Scalar>();
  scalar->setValue(value);
  scalar->setOrphan(true);
  scalar->setEditable(true);
  scalar->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  scalar->writeLock();
  scalar->update();
  scalar->unlock();

  _dataObjectName = scalar->Name();

  return static_cast<ObjectPtr>(scalar);
}


ObjectPtr ScalarDialog::editExistingDataObject() const {
  if (ScalarPtr scalar = kst_cast<Scalar>(dataObject())) {
    bool ok;
    double value = _scalarTab->value().toDouble(&ok);
    if (!ok) {
      value = Equations::interpret(_document->objectStore(), _scalarTab->value().toLatin1(), &ok);
    }
    scalar->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));
    scalar->writeLock();
    scalar->setValue(value);
    scalar->immediateUpdate();
    scalar->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
