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

#include "datasourcedialog.h"
#include "scalardialog.h"
#include "enodes.h"
#include "document.h"
#include "objectstore.h"
#include "datascalar.h"
#include "dialogdefaults.h"

#include <QPushButton>

namespace Kst {

ScalarTab::ScalarTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent),  _mode(DataScalar), _store(store){

  setupUi(this);
  setTabTitle(tr("Scalar"));

  connect(_scalarValue, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged()));
  connect(_readFromSource, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
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

void ScalarTab::readFromSourceChanged() {

  if (_readFromSource->isChecked())
    setScalarMode(DataScalar);
  else
    setScalarMode(GeneratedScalar);

  _dataScalarGroup->setEnabled(_readFromSource->isChecked());
  _generatedScalarGroup->setEnabled(!_readFromSource->isChecked());

  emit sourceChanged();
}



void ScalarTab::hideGeneratedOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _generatedScalarGroup->setVisible(false);
}


void ScalarTab::hideDataOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _dataScalarGroup->setVisible(false);
  _generatedScalarGroup->setEnabled(true);
}


DataSourcePtr ScalarTab::dataSource() const {
  return _dataSource;
}


void ScalarTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


void ScalarTab::setScalarMode(ScalarMode mode) {
  _mode = mode;
  _readFromSource->setChecked(mode == DataScalar);
  _generateX->setChecked(mode == GeneratedScalar);
}


QString ScalarTab::file() const {
  return _fileName->file();
}


void ScalarTab::setFile(const QString &file) {
  _fileName->setFile(file);
}


QString ScalarTab::field() const {
  return _field->currentText();
}


void ScalarTab::setField(const QString &field) {
  _field->setCurrentIndex(_field->findText(field));
}


void ScalarTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


void ScalarTab::updateDataSource() {
  fileNameChanged(_fileName->file());
}


void ScalarTab::fileNameChanged(const QString &file) {
  QFileInfo info(file);
  if (!info.exists()) {
    _field->setEnabled(false);
    _configure->setEnabled(false);
    return;
  }

  _field->clear();
  Q_ASSERT(_store);
  _dataSource = DataSource::findOrLoadSource(_store, file);

  if (!_dataSource) {
    _field->setEnabled(false);
    _configure->setEnabled(false);
    return; //Couldn't find a suitable datasource
  }

  _field->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->scalarList());
  _field->setEditable(!_dataSource->scalarListIsComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  //FIXME deal with time...
  //_dataRange->setAllowTime(ds->supportsTimeConversions());

  _dataSource->unlock();
  emit sourceChanged();
}


void ScalarTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
}


ScalarDialog::ScalarDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Scalar"));
  else
    setWindowTitle(tr("New Scalar"));

  _scalarTab = new ScalarTab(_document->objectStore(), this);
  addDataTab(_scalarTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
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
  if (DataScalarPtr dataScalar = kst_cast<DataScalar>(object)) {
    _scalarTab->setFile(dataScalar->dataSource()->fileName());
    _scalarTab->setDataSource(dataScalar->dataSource());
    _scalarTab->setField(dataScalar->field());
    _scalarTab->hideGeneratedOptions();
  } else if (ScalarPtr scalar = kst_cast<Scalar>(object)) { // edit value scalar
    _scalarTab->hideDataOptions();
    _scalarTab->setValue(QString::number(scalar->value()));
  } else { // new scalar
    _scalarTab->setFile(_dialogDefaults->value("vector/datasource",_scalarTab->file()).toString());
  }
}

void ScalarDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!_scalarTab->value().isEmpty()
                                                      || !_scalarTab->field().isEmpty());
}


ObjectPtr ScalarDialog::createNewDataObject() {
  switch(_scalarTab->scalarMode()) {
  case ScalarTab::DataScalar:
    return createNewDataScalar();
  case ScalarTab::GeneratedScalar:
    return createNewGeneratedScalar();
  default:
    return 0;
  }
}

ObjectPtr ScalarDialog::createNewGeneratedScalar(){
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


ObjectPtr ScalarDialog::createNewDataScalar() {
  const DataSourcePtr dataSource = _scalarTab->dataSource();

  if (!dataSource)
    return 0;

  const QString field = _scalarTab->field();

  Q_ASSERT(_document && _document->objectStore());

  DataScalarPtr scalar = _document->objectStore()->createObject<DataScalar>();

  scalar->writeLock();
  scalar->change(dataSource, field);

  scalar->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  scalar->update();
  scalar->unlock();

  _dataObjectName = scalar->Name();

  return scalar;
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
