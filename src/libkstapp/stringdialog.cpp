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
#include "stringdialog.h"
#include "enodes.h"
#include "document.h"
#include "objectstore.h"
#include "datastring.h"
#include "dialogdefaults.h"

#include <QPushButton>

namespace Kst {

StringTab::StringTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent),  _mode(DataString), _store(store){

  setupUi(this);
  setTabTitle(tr("String"));

  connect(_stringValue, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged()));
  connect(_readFromSource, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
}


StringTab::~StringTab() {
}


QString StringTab::value() const {
  return _stringValue->text();
}


void StringTab::setValue(const QString &value) {
  _stringValue->setText(value);
}


void StringTab::textChanged() {
  emit valueChanged();
}

void StringTab::readFromSourceChanged() {

  if (_readFromSource->isChecked())
    setStringMode(DataString);
  else
    setStringMode(GeneratedString);

  _dataStringGroup->setEnabled(_readFromSource->isChecked());
  _generatedStringGroup->setEnabled(!_readFromSource->isChecked());

  emit sourceChanged();
}



void StringTab::hideGeneratedOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _generatedStringGroup->setVisible(false);
}


void StringTab::hideDataOptions() {
  _readFromSource->setVisible(false);
  _generateX->setVisible(false);
  _dataStringGroup->setVisible(false);
  _generatedStringGroup->setEnabled(true);
}


DataSourcePtr StringTab::dataSource() const {
  return _dataSource;
}


void StringTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


void StringTab::setStringMode(StringMode mode) {
  _mode = mode;
  _readFromSource->setChecked(mode == DataString);
  _generateX->setChecked(mode == GeneratedString);
}


QString StringTab::file() const {
  return _fileName->file();
}


void StringTab::setFile(const QString &file) {
  _fileName->setFile(file);
}


QString StringTab::field() const {
  return _field->currentText();
}


void StringTab::setField(const QString &field) {
  _field->setCurrentIndex(_field->findText(field));
}


void StringTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


void StringTab::updateDataSource() {
  fileNameChanged(_fileName->file());
}


void StringTab::fileNameChanged(const QString &file) {
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

  _field->addItems(_dataSource->stringList());
  _field->setEditable(!_dataSource->stringListIsComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  //FIXME deal with time...
  //_dataRange->setAllowTime(ds->supportsTimeConversions());

  _dataSource->unlock();
  emit sourceChanged();
}


void StringTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
}


StringDialog::StringDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit String"));
  else
    setWindowTitle(tr("New String"));

  _stringTab = new StringTab(_document->objectStore(), this);
  addDataTab(_stringTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_stringTab, SIGNAL(valueChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


StringDialog::~StringDialog() {
}


// QString StringDialog::tagString() const {
//   return DataDialog::tagString();
// }


void StringDialog::configureTab(ObjectPtr object) {
  if (DataStringPtr dataString = kst_cast<DataString>(object)) {
    _stringTab->setFile(dataString->dataSource()->fileName());
    _stringTab->setDataSource(dataString->dataSource());
    _stringTab->setField(dataString->field());
    _stringTab->hideGeneratedOptions();
  } else if (StringPtr string = kst_cast<String>(object)) { // edit value string
    _stringTab->hideDataOptions();
    _stringTab->setValue(string->value());
  } else { // new string
    _stringTab->setFile(_dialogDefaults->value("vector/datasource",_stringTab->file()).toString());
  }
}

void StringDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!_stringTab->value().isEmpty()
                                                      || !_stringTab->field().isEmpty());
}


ObjectPtr StringDialog::createNewDataObject() {
  switch(_stringTab->stringMode()) {
  case StringTab::DataString:
    return createNewDataString();
  case StringTab::GeneratedString:
    return createNewGeneratedString();
  default:
    return 0;
  }
}

ObjectPtr StringDialog::createNewGeneratedString(){

  Q_ASSERT(_document && _document->objectStore());

  QString value = _stringTab->value();

//   qDebug() << "Creating new string  ===>"
//            << "\n\ttag:" << DataDialog::tagString()
//            << "\n\tvalue:" << value
//            << endl;

  StringPtr string = _document->objectStore()->createObject<String>();
  string->setValue(value);
  string->setOrphan(true);
  string->setEditable(true);
  string->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  string->writeLock();
  string->update();
  string->unlock();

  _dataObjectName = string->Name();

  return static_cast<ObjectPtr>(string);
}


ObjectPtr StringDialog::createNewDataString() {
  const DataSourcePtr dataSource = _stringTab->dataSource();

  if (!dataSource)
    return 0;

  const QString field = _stringTab->field();

  Q_ASSERT(_document && _document->objectStore());

  DataStringPtr string = _document->objectStore()->createObject<DataString>();

  string->writeLock();
  string->change(dataSource, field);

  string->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  string->update();
  string->unlock();

  _dataObjectName = string->Name();

  return string;
}


ObjectPtr StringDialog::editExistingDataObject() const {
  if (StringPtr string = kst_cast<String>(dataObject())) {
    QString value = _stringTab->value();
    string->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));
    string->writeLock();
    string->setValue(value);
    string->immediateUpdate();
    string->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
