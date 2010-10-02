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

#include "datasourcedialog.h"

#include "scalardialog.h"
#include "enodes.h"
#include "document.h"
#include "objectstore.h"
#include "datascalar.h"
#include "vscalar.h"
#include "dialogdefaults.h"
#include "datasourcepluginmanager.h"

#include <QPushButton>
#include <QThreadPool>

namespace Kst {

ScalarTab::ScalarTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent),  _mode(DataScalar), _store(store), _requestID(0) {

  setupUi(this);
  setTabTitle(tr("Scalar"));

  setDataOptions();

  connect(_scalarValue, SIGNAL(textChanged(const QString&)), this, SLOT(entryChanged()));
  connect(_start, SIGNAL(textChanged(const QString&)), this, SLOT(entryChanged()));
  connect(_field, SIGNAL(editTextChanged(const QString&)), this, SLOT(entryChanged()));
  connect(_fieldRV, SIGNAL(editTextChanged(const QString&)), this, SLOT(entryChanged()));
  connect(_readFromSource, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
  connect(_readFromRVector, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
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


void ScalarTab::entryChanged() {
  emit valueChanged();
}


void ScalarTab::readFromSourceChanged() {

  if (_readFromSource->isChecked()) {
    _mode = DataScalar;
    setDataOptions();
  } else if (_readFromRVector->isChecked()) {
    _mode = RVectorScalar;
    setRVOptions();
  } else {
    _mode = GeneratedScalar;
  }
  _dataScalarGroup->setEnabled((_readFromSource->isChecked())||(_readFromRVector->isChecked()));

  bool isRV = _readFromRVector->isChecked();

  label_6->setEnabled(isRV);
  label_7->setEnabled(isRV);
  _start->setEnabled(isRV);
  _countFromEnd->setEnabled(isRV);

 _generatedScalarGroup->setEnabled(_generateX->isChecked());

  emit sourceChanged();
}



void ScalarTab::hideGeneratedOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _readFromRVector->setVisible(false);
  _generatedScalarGroup->setVisible(false);
}


void ScalarTab::hideDataOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _readFromRVector->setVisible(false);
  _dataScalarGroup->setVisible(false);
  _generatedScalarGroup->setEnabled(true);
}

void ScalarTab::setRVOptions() {
 // replace in right order to avoid flicker
 _field->setVisible(false);
 label_5->setVisible(false);
 _fieldRV->setVisible(true);
 label_8->setVisible(true);
 _readFromRVector->setChecked(true);
}


void ScalarTab::setDataOptions() {
  _fieldRV->setVisible(false);
  label_8->setVisible(false);
  _field->setVisible(true);
  label_5->setVisible(true);
  _readFromSource->setChecked(true);
}


DataSourcePtr ScalarTab::dataSource() const {
  return _dataSource;
}


void ScalarTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
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


QString ScalarTab::fieldRV() const {
  return _fieldRV->currentText();
}


void ScalarTab::setFieldRV(const QString &field) {
  _fieldRV->setCurrentIndex(_fieldRV->findText(field));
}

void ScalarTab::setF0(int f0) {
  _start->setText(QString::number(f0));
}

int ScalarTab::F0() const {
  return _start->text().toInt();
}

void ScalarTab::updateDataSource() {
  fileNameChanged(_fileName->file());
}


void ScalarTab::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);

  _field->setEnabled(true);
  _fieldRV->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->scalar().list());
  _field->setEditable(!_dataSource->scalar().isListComplete());
  _fieldRV->addItems(_dataSource->vector().list());
  _fieldRV->setEditable(!_dataSource->vector().isListComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  _dataSource->unlock();
  modified();
  emit sourceChanged();
}



void ScalarTab::fileNameChanged(const QString &file) {
  _field->clear();
  _fieldRV->clear();
  _field->setEnabled(false);
  _fieldRV->setEnabled(false);
  _configure->setEnabled(false);
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


void ScalarTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
  fileNameChanged(_dataSource->fileName());
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
  connect(_scalarTab, SIGNAL(valueChanged()), this, SLOT(modified()));
  updateButtons();
}


ScalarDialog::~ScalarDialog() {
}

void ScalarDialog::configureTab(ObjectPtr object) {
  if (DataScalarPtr dataScalar = kst_cast<DataScalar>(object)) {
    _scalarTab->setFile(dataScalar->dataSource()->fileName());
    _scalarTab->setDataSource(dataScalar->dataSource());
    _scalarTab->setField(dataScalar->field());
    _scalarTab->hideGeneratedOptions();
    _scalarTab->setDataOptions();
  } else if (VScalarPtr vScalar = kst_cast<VScalar>(object)) {
    _scalarTab->setFile(vScalar->dataSource()->fileName());
    _scalarTab->setDataSource(vScalar->dataSource());
    _scalarTab->setFieldRV(vScalar->field());
    _scalarTab->setF0(vScalar->F0());
    _scalarTab->hideGeneratedOptions();
    _scalarTab->setRVOptions();
  } else if (ScalarPtr scalar = kst_cast<Scalar>(object)) { // edit value scalar
    _scalarTab->hideDataOptions();
    _scalarTab->setValue(QString::number(scalar->value()));
  } else { // new scalar
    _scalarTab->setFile(_dialogDefaults->value("vector/datasource",_scalarTab->file()).toString());
  }
}

void ScalarDialog::updateButtons() {
  bool valid=false;

  switch(_scalarTab->scalarMode()) {
  case ScalarTab::DataScalar:
    valid = !_scalarTab->field().isEmpty();
    break;
  case ScalarTab::RVectorScalar:
    valid = !_scalarTab->fieldRV().isEmpty();
    break;
  case ScalarTab::GeneratedScalar:
    valid = !_scalarTab->value().isEmpty();
    break;
  default:
    valid = false;
  }

  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  //_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
}


ObjectPtr ScalarDialog::createNewDataObject() {
  switch(_scalarTab->scalarMode()) {
  case ScalarTab::DataScalar:
    return createNewDataScalar();
  case ScalarTab::RVectorScalar:
    return createNewVScalar();
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

  ScalarPtr scalar = _document->objectStore()->createObject<Scalar>();
  scalar->setValue(value);
  scalar->setOrphan(true);
  scalar->setEditable(true);
  if (DataDialog::tagStringAuto()) {
     scalar->setDescriptiveName(QString());
  } else {
     scalar->setDescriptiveName(DataDialog::tagString());
  }

  scalar->writeLock();
  scalar->registerChange();
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

  if (DataDialog::tagStringAuto()) {
     scalar->setDescriptiveName(QString());
  } else {
     scalar->setDescriptiveName(DataDialog::tagString());
  }

  scalar->registerChange();
  scalar->unlock();

  _dataObjectName = scalar->Name();

  return scalar;
}


ObjectPtr ScalarDialog::createNewVScalar() {
  const DataSourcePtr dataSource = _scalarTab->dataSource();

  if (!dataSource)
    return 0;

  const QString field = _scalarTab->fieldRV();
  const int f0 = _scalarTab->F0();

  Q_ASSERT(_document && _document->objectStore());

  VScalarPtr scalar = _document->objectStore()->createObject<VScalar>();

  scalar->writeLock();
  scalar->change(dataSource, field, f0);

  if (DataDialog::tagStringAuto()) {
     scalar->setDescriptiveName(QString());
  } else {
     scalar->setDescriptiveName(DataDialog::tagString());
  }

  scalar->registerChange();
  scalar->unlock();

  _dataObjectName = scalar->Name();

  return scalar;
}


ObjectPtr ScalarDialog::editExistingDataObject() const {
  if (DataScalarPtr scalar = kst_cast<DataScalar>(dataObject())) {
    const DataSourcePtr dataSource = _scalarTab->dataSource();

    if (dataSource) {
      const QString field = _scalarTab->field();
      scalar->writeLock();
      scalar->change(dataSource, field);

      if (DataDialog::tagStringAuto()) {
        scalar->setDescriptiveName(QString());
      } else {
        scalar->setDescriptiveName(DataDialog::tagString());
      }

      scalar->registerChange();
      scalar->unlock();
    }
  } else if (VScalarPtr scalar = kst_cast<VScalar>(dataObject())) {
    const DataSourcePtr dataSource = _scalarTab->dataSource();

    if (dataSource) {
      const QString field = _scalarTab->fieldRV();
      int f0 = _scalarTab->F0();;
      scalar->writeLock();
      scalar->change(dataSource, field, f0);

      if (DataDialog::tagStringAuto()) {
        scalar->setDescriptiveName(QString());
      } else {
        scalar->setDescriptiveName(DataDialog::tagString());
      }

      scalar->registerChange();
      scalar->unlock();
    }
  } else   if (ScalarPtr scalar = kst_cast<Scalar>(dataObject())) {
    bool ok;
    double value = _scalarTab->value().toDouble(&ok);
    if (!ok) {
      value = Equations::interpret(_document->objectStore(), _scalarTab->value().toLatin1(), &ok);
    }
    if (DataDialog::tagStringAuto()) {
      scalar->setDescriptiveName(QString());
    } else {
      scalar->setDescriptiveName(DataDialog::tagString());
    }
    scalar->writeLock();
    scalar->setValue(value);
    scalar->registerChange();
    scalar->unlock();
  }

  return dataObject();
}

}

// vim: ts=2 sw=2 et
