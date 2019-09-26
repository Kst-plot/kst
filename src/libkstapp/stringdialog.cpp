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
#include "stringdialog.h"

#include "application.h"
#include "datasourceconfiguredialog.h"
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

  connect(_generatedStringGroup, SIGNAL(toggled(bool)), this, SLOT(generateClicked()));
  connect(_dataStringGroup, SIGNAL(toggled(bool)), this, SLOT(readFromSourceClicked()));
  connect(_stringValue, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
  connect(_fileName, SIGNAL(changed(QString)), this, SLOT(fileNameChanged(QString)));
  connect(_field, SIGNAL(currentTextChanged(QString)), this, SLOT(fieldChanged(QString)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
  connect(_field, SIGNAL(currentIndexChanged(int)), this, SLOT(textChanged()));
  connect(_frame, SIGNAL(valueChanged(int)), this, SLOT(textChanged()));
  connect(_lastFrame, SIGNAL(clicked(bool)), this, SLOT(enableFrames()));
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


int StringTab::frame() const {
  if (_lastFrame->isChecked()) {
    return (-1);
  } else {
    return _frame->value();
  }
}


void StringTab::setFrame(int frame) {
  if (frame<0) {
    _lastFrame->setChecked(true);
    _frame->setEnabled(false);
  } else {
    _lastFrame->setChecked(false);
    _frame ->setValue(frame);
    _frame->setEnabled(true);
  }
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
  _current_field = field;
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
  setField(_current_field);
  _field->setEditable(!_dataSource->string().isListComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  bool isStringStream = _dataSource->isStringStream(_field->currentText());
  _frame->setVisible(isStringStream);
  _frameLabel->setVisible(isStringStream);
  _lastFrame->setVisible(isStringStream);

  _dataSource->unlock();

  _store->cleanUpDataSourceList();

  emit sourceChanged();
}

void StringTab::enableFrames() {
  _frame->setEnabled(!_lastFrame->isChecked());
}


void StringTab::fileNameChanged(const QString &file) {
  _field->clear();
  _field->setEnabled(false);
  _configure->setEnabled(false);
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString,int)), this, SLOT(sourceValid(QString,int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}

void StringTab::fieldChanged(const QString &field) {
  bool isImageStream = _dataSource->isStringStream(field);
  _frame->setVisible(isImageStream);
  _frameLabel->setVisible(isImageStream);
  _lastFrame->setVisible(isImageStream);
}


void StringTab::showConfigWidget() {
  QPointer<DataSourceConfigureDialog> dialog = new DataSourceConfigureDialog(dataDialog()->editMode(), _dataSource, this);
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
  connect(_stringTab, SIGNAL(destroyed()), kstApp->mainWindow(), SLOT(cleanUpDataSourceList()));
  updateButtons();
}


StringDialog::~StringDialog() {
}


// QString StringDialog::tagString() const {
//   return DataDialog::tagString();
// }


void StringDialog::configureTab(ObjectPtr object) {
  if (DataStringPtr dataString = kst_cast<DataString>(object)) {
    _stringTab->setStringMode(StringTab::DataString);
    _stringTab->setFile(dataString->dataSource()->fileName());
    _stringTab->setDataSource(dataString->dataSource());
    _stringTab->setField(dataString->field());
    _stringTab->hideGeneratedOptions();
    _stringTab->setFrame(dataString->frame());
  } else if (StringPtr string = kst_cast<String>(object)) { // edit value string
    _stringTab->setStringMode(StringTab::GeneratedString);
    _stringTab->hideDataOptions();
    _stringTab->setValue(string->value());
  } else { // new string
    _stringTab->setFile(dialogDefaults().value("String/datasource",_stringTab->file()).toString());
  }
}

void StringDialog::updateButtons() {
  bool enable = !_stringTab->value().isEmpty()
      || !_stringTab->field().isEmpty();

  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(enable);
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

  _stringTab->setDataSource(0L);
  _document->objectStore()->cleanUpDataSourceList();

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
  string->change(dataSource, field, _stringTab->frame());

  if (DataDialog::tagStringAuto()) {
     string->setDescriptiveName(QString());
  } else {
     string->setDescriptiveName(DataDialog::tagString());
  }

  string->registerChange();
  string->unlock();

  _dataObjectName = string->Name();

  //UpdateServer::self()->requestUpdateSignal();

  dialogDefaults().setValue("String/datasource", _stringTab->file());

  return string;
}


ObjectPtr StringDialog::editExistingDataObject() const {
  if (_stringTab->stringMode()==StringTab::GeneratedString) {
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
  } else if (_stringTab->stringMode()==StringTab::DataString) {
    if (DataStringPtr string = kst_cast<DataString>(dataObject())) {
      const DataSourcePtr dataSource = _stringTab->dataSource();

      if (!dataSource)
        return 0;

      const QString field = _stringTab->field();

      string->writeLock();
      if (DataDialog::tagStringAuto()) {
        string->setDescriptiveName(QString());
      } else {
        string->setDescriptiveName(DataDialog::tagString());
      }
      string->change(dataSource, field, _stringTab->frame());
      string->registerChange();
      string->unlock();
      dialogDefaults().setValue("String/datasource", _stringTab->file());
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
