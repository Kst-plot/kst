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
#include "stringdialog.h"
#include "enodes.h"
#include "document.h"
#include "objectstore.h"
#include "datastring.h"
#include "dialogdefaults.h"
#include "datasourcepluginmanager.h"

#include <QPushButton>
#include <QThreadPool>

namespace Kst {

StringTab::StringTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent),  _mode(DataString), _store(store), _requestID(0) {

  setupUi(this);
  setTabTitle(tr("String"));

  _fileNameLabel->setBuddy(_fileName->_fileEdit);

  connect(_generatedStringGroup, SIGNAL(clicked(bool)), this, SLOT(generateClicked()));
  connect(_dataStringGroup, SIGNAL(clicked(bool)), this, SLOT(readFromSourceClicked()));
  connect(_stringValue, SIGNAL(textChanged(const QString&)), this, SLOT(textChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
}


StringTab::~StringTab() {
}


void StringTab::generateClicked() {
  if (_generatedStringGroup->isChecked())
    setStringMode(GeneratedString);
  else
    setStringMode(DataString);

  emit sourceChanged();
}

void StringTab::readFromSourceClicked() {
  if (_dataStringGroup->isChecked())
    setStringMode(DataString);
  else
    setStringMode(GeneratedString);

  emit sourceChanged();
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


void StringTab::hideGeneratedOptions() {
  _generatedStringGroup->setVisible(false);
  _dataStringGroup->setCheckable(false);
  _dataStringGroup->setTitle("");
  _dataStringGroup->setFlat(true);
  int top_margin;
  _dataStringGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _dataStringGroup->layout()->setContentsMargins(0,top_margin,0,0); 

}


void StringTab::hideDataOptions() {
  _dataStringGroup->setVisible(false);
  _generatedStringGroup->setCheckable(false);
  _generatedStringGroup->setTitle("");
  _generatedStringGroup->setFlat(true);
  int top_margin;
  _generatedStringGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _generatedStringGroup->layout()->setContentsMargins(0,top_margin,0,0); 
}


DataSourcePtr StringTab::dataSource() const {
  return _dataSource;
}


void StringTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


void StringTab::setStringMode(StringMode mode) {
  _mode = mode;
  _dataStringGroup->setChecked(mode == DataString);
  _generatedStringGroup->setChecked(mode == GeneratedString);
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


void StringTab::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);

  _field->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->string().list());
  _field->setEditable(!_dataSource->string().isListComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  _dataSource->unlock();
  emit sourceChanged();
//   emit valueChanged();
}


void StringTab::fileNameChanged(const QString &file) {
  _field->clear();
  _field->setEnabled(false);
  _configure->setEnabled(false);
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


void StringTab::showConfigWidget() {
  QPointer<DataSourceDialog> dialog = new DataSourceDialog(dataDialog()->editMode(), _dataSource, this);
  if ( dialog->exec() == QDialog::Accepted ) {
    fileNameChanged(_dataSource->fileName());
  }
  delete dialog;
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
  connect(_stringTab, SIGNAL(sourceChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


StringDialog::~StringDialog() {
}


// QString StringDialog::tagString() const {
//   return DataDialog::tagString();
// }


void StringDialog::configureTab(ObjectPtr object) {
  if (DataStringPtr dataString = kst_cast<DataString>(object)) {
    _stringTab->setFile(dataString->dp()->dataSource()->fileName());
    _stringTab->setDataSource(dataString->dp()->dataSource());
    _stringTab->setField(dataString->dp()->field());
    _stringTab->hideGeneratedOptions();
  } else if (StringPtr string = kst_cast<String>(object)) { // edit value string
    _stringTab->hideDataOptions();
    _stringTab->setValue(string->value());
  } else { // new string
    _stringTab->setFile(_dialogDefaults->value("String/datasource",_stringTab->file()).toString());
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
  if (DataDialog::tagStringAuto()) {
     string->setDescriptiveName(QString());
  } else {
     string->setDescriptiveName(DataDialog::tagString());
  }

  string->writeLock();
  string->registerChange();
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

  if (DataDialog::tagStringAuto()) {
     string->setDescriptiveName(QString());
  } else {
     string->setDescriptiveName(DataDialog::tagString());
  }

  string->registerChange();
  string->unlock();

  _dataObjectName = string->Name();

  return string;
}


ObjectPtr StringDialog::editExistingDataObject() const {
  if (StringPtr string = kst_cast<String>(dataObject())) {
    QString value = _stringTab->value();
    if (DataDialog::tagStringAuto()) {
      string->setDescriptiveName(QString());
    } else {
      string->setDescriptiveName(DataDialog::tagString());
    }
    string->writeLock();
    string->setValue(value);
    string->registerChange();
    string->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
