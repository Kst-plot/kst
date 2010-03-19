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
#include <stdio.h>
#include "vectordialog.h"

#include "dialogpage.h"
#include "datasourcedialog.h"
#include "editmultiplewidget.h"

#include "datavector.h"
#include "generatedvector.h"

#include "datacollection.h"
#include "document.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "dialogdefaults.h"

#include <QDir>
#include <QThreadPool>

namespace Kst {

VectorTab::VectorTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent), _mode(DataVector), _store(store), _initField(QString()), _requestID(0) {

  setupUi(this);
  setTabTitle(tr("Vector"));

  connect(_generatedVectorGroup, SIGNAL(clicked(bool)), this, SLOT(generateClicked()));
  connect(_dataVectorGroup, SIGNAL(clicked(bool)), this, SLOT(readFromSourceClicked()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));
  connect(_field, SIGNAL(editTextChanged(const QString &)), this, SIGNAL(fieldChanged()));

  connect(_dataRange, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_numberOfSamples, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_from, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_to, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));

  // embed data range in the data source box
  _dataRange->groupBox2->setFlat(true);
  _dataRange->groupBox2->setTitle("");
  int top_margin;
  _dataRange->groupBox2->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _dataRange->groupBox2->layout()->setContentsMargins(0,top_margin,0,0); 

  _connect->setVisible(false);
}


VectorTab::~VectorTab() {
}


DataSourcePtr VectorTab::dataSource() const {
  return _dataSource;
}


void VectorTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


void VectorTab::setVectorMode(VectorMode mode) {
  _mode = mode;
  _dataVectorGroup->setChecked(mode == DataVector);
  _dataRange->setEnabled(mode == DataVector);
  _generatedVectorGroup->setChecked(mode == GeneratedVector);
}


QString VectorTab::file() const {
  return _fileName->file();
}


void VectorTab::setFile(const QString &file) {
  QFileInfo info(file);
  _fileName->setFile(file);
}


QString VectorTab::field() const {
  return _field->currentText();
}


void VectorTab::setField(const QString &field) {
  _initField = field; // for delayed index setting
  _field->setCurrentIndex(_field->findText(field));
}


void VectorTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


DataRange *VectorTab::dataRange() const {
  return _dataRange;
}


qreal VectorTab::from() const {
  return _from->text().toDouble();
}


bool VectorTab::fromDirty() const {
  return (!_from->text().isEmpty());
}


void VectorTab::setFrom(qreal from) {
  _from->setText(QString::number(from));
}


qreal VectorTab::to() const {
  return _to->text().toDouble();
}


bool VectorTab::toDirty() const {
  return (!_to->text().isEmpty());
}


void VectorTab::setTo(qreal to) {
  _to->setText(QString::number(to));
}


int VectorTab::numberOfSamples() const {
  return _numberOfSamples->value();
}


bool VectorTab::numberOfSamplesDirty() const {
  return (!_numberOfSamples->text().isEmpty());
}


void VectorTab::setNumberOfSamples(int numberOfSamples) {
  _numberOfSamples->setValue(numberOfSamples);
}

void VectorTab::generateClicked() {
  if (_generatedVectorGroup->isChecked())
    setVectorMode(GeneratedVector);
  else
    setVectorMode(DataVector);

  emit sourceChanged();
}

void VectorTab::readFromSourceClicked() {
  if (_dataVectorGroup->isChecked())
    setVectorMode(DataVector);
  else
    setVectorMode(GeneratedVector);

  emit sourceChanged();
}

void VectorTab::hideGeneratedOptions() {
  _generatedVectorGroup->setVisible(false);
  _dataVectorGroup->setCheckable(false);
  _dataVectorGroup->setTitle("");
  _dataVectorGroup->setFlat(true);
  int top_margin;
  _dataVectorGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _dataVectorGroup->layout()->setContentsMargins(0,top_margin,0,0); 
}


void VectorTab::hideDataOptions() {
  _dataVectorGroup->setVisible(false);
  _dataRange->setVisible(false);
  _generatedVectorGroup->setCheckable(false);
  _generatedVectorGroup->setTitle("");
  _generatedVectorGroup->setFlat(true);
  int top_margin;
  _generatedVectorGroup->layout()->getContentsMargins(NULL,&top_margin,NULL,NULL);
  _generatedVectorGroup->layout()->setContentsMargins(0,top_margin,0,0); 
}


void VectorTab::clearTabValues() {
  _numberOfSamples->clear();
  _from->clear();
  _to->clear();
  _dataRange->clearValues();
}


void VectorTab::enableSingleEditOptions(bool enabled) {
  _dataVectorGroup->setEnabled(enabled);
}


void VectorTab::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);
  _field->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->fieldList());
  if (!_initField.isEmpty()) {
    setField(_initField);
  }
  _field->setEditable(!_dataSource->fieldListIsComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  _dataSource->unlock();
  emit sourceChanged();
}


void VectorTab::fileNameChanged(const QString &file) {
  _field->clear();
  _field->setEnabled(false);
  _configure->setEnabled(false);
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


void VectorTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
  fileNameChanged(_dataSource->fileName());
}

VectorDialog::VectorDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Vector"));
  else
    setWindowTitle(tr("New Vector"));

  Q_ASSERT(_document);
  _vectorTab = new VectorTab(_document->objectStore(), this);
  addDataTab(_vectorTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_vectorTab, SIGNAL(sourceChanged()), this, SLOT(updateButtons()));
  connect(_vectorTab, SIGNAL(fieldChanged()), this, SLOT(updateButtons()));

  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_vectorTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


VectorDialog::~VectorDialog() {
}


void VectorDialog::editMultipleMode() {
  _vectorTab->enableSingleEditOptions(false);
  _vectorTab->clearTabValues();
}


void VectorDialog::editSingleMode() {
  _vectorTab->enableSingleEditOptions(true);
   configureTab(dataObject());
}


void VectorDialog::updateButtons() {

  bool valid = _vectorTab->vectorMode() == VectorTab::GeneratedVector || !_vectorTab->field().isEmpty();
  // FIXME: add stricter validity testing.
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
}


void VectorDialog::configureTab(ObjectPtr vector) {
  if (!vector) {
    _vectorTab->dataRange()->loadWidgetDefaults();
    _vectorTab->setFile(_dialogDefaults->value("vector/datasource",_vectorTab->file()).toString());
    _vectorTab->setFrom(_dialogDefaults->value("genVector/min",-10).toInt());
    _vectorTab->setTo(_dialogDefaults->value("genVector/max",10).toInt());
    _vectorTab->setNumberOfSamples(_dialogDefaults->value("genVector/length",1000).toInt());
  } else if (DataVectorPtr dataVector = kst_cast<DataVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::DataVector);
    _vectorTab->setFile(dataVector->dataSource()->fileName());
    _vectorTab->setDataSource(dataVector->dataSource());
    _vectorTab->setField(dataVector->field());
    _vectorTab->dataRange()->setRange(dataVector->numFrames());
    _vectorTab->dataRange()->setStart(dataVector->startFrame());
    _vectorTab->dataRange()->setCountFromEnd(dataVector->countFromEOF());
    _vectorTab->dataRange()->setReadToEnd(dataVector->readToEOF());
    _vectorTab->dataRange()->setSkip(dataVector->skip());
    _vectorTab->dataRange()->setDoSkip(dataVector->doSkip());
    _vectorTab->dataRange()->setDoFilter(dataVector->doAve());
    _vectorTab->hideGeneratedOptions();
    if (_editMultipleWidget) {
      DataVectorList objects = _document->objectStore()->getObjects<DataVector>();
      _editMultipleWidget->clearObjects();
      foreach(DataVectorPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::GeneratedVector);
    _vectorTab->setFrom(generatedVector->min());
    _vectorTab->setTo(generatedVector->max());
    _vectorTab->setNumberOfSamples(generatedVector->length());
    _vectorTab->hideDataOptions();
    if (_editMultipleWidget) {
      DataVectorList objects = _document->objectStore()->getObjects<DataVector>();
      _editMultipleWidget->clearObjects();
      foreach(DataVectorPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


ObjectPtr VectorDialog::createNewDataObject() {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    return createNewDataVector();
  case VectorTab::GeneratedVector:
    return createNewGeneratedVector();
  default:
    return 0;
  }
}


ObjectPtr VectorDialog::createNewDataVector() {
  const DataSourcePtr dataSource = _vectorTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  const QString field = _vectorTab->field();
  const DataRange *dataRange = _vectorTab->dataRange();

  Q_ASSERT(_document && _document->objectStore());

  DataVectorPtr vector = _document->objectStore()->createObject<DataVector>();

  vector->writeLock();
  vector->change(dataSource, field,
      dataRange->countFromEnd() ? -1 : int(dataRange->start()),
      dataRange->readToEnd() ? -1 : int(dataRange->range()),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());

  if (DataDialog::tagStringAuto()) {
     vector->setDescriptiveName(QString());
  } else {
     vector->setDescriptiveName(DataDialog::tagString());
  }

  setDataVectorDefaults(vector);
  _vectorTab->dataRange()->setWidgetDefaults();

  vector->registerChange();
  vector->unlock();

  _dataObjectName = vector->Name();

  return vector;
}


ObjectPtr VectorDialog::createNewGeneratedVector() {
  const qreal from = _vectorTab->from();
  const qreal to = _vectorTab->to();
  const int numberOfSamples = _vectorTab->numberOfSamples();

  Q_ASSERT(_document && _document->objectStore());
  GeneratedVectorPtr vector = _document->objectStore()->createObject<GeneratedVector>();
  vector->changeRange(from, to, numberOfSamples);

  setGenVectorDefaults(vector);

  if (DataDialog::tagStringAuto()) {
     vector->setDescriptiveName(QString());
  } else {
     vector->setDescriptiveName(DataDialog::tagString());
  }

  _dataObjectName = vector->Name();

  return vector;
}


ObjectPtr VectorDialog::editExistingDataObject() const {
  if (DataVectorPtr dataVector = kst_cast<DataVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      const DataRange *dataRange = _vectorTab->dataRange();
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        DataVectorPtr vector = kst_cast<DataVector>(_document->objectStore()->retrieveObject(objectName));
        if (vector) {
          int start = dataRange->startDirty() ? dataRange->start() : vector->startFrame();
          int range = dataRange->rangeDirty() ?  dataRange->range() : vector->numFrames();
          int skip = dataRange->skipDirty() ?  dataRange->skip() : vector->skip();

          if (dataRange->countFromEndDirty()) {
              start = dataRange->countFromEnd() ? -1 : dataRange->start();
              range = dataRange->readToEnd() ? -1 : dataRange->range();
          }

          bool doSkip = dataRange->doSkipDirty() ?  dataRange->doSkip() : vector->doSkip();
          bool doAve = dataRange->doFilterDirty() ?  dataRange->doFilter() : vector->doAve();
          vector->writeLock();
          vector->changeFrames(start, range, skip, doSkip, doAve);
          vector->registerChange();
          vector->unlock();
        }
      }
    } else {
      const DataSourcePtr dataSource = _vectorTab->dataSource();

      //FIXME better validation than this please...
      if (!dataSource)
        return 0;

      const QString field = _vectorTab->field();
      const DataRange *dataRange = _vectorTab->dataRange();

      dataVector->writeLock();
      dataVector->change(dataSource, field,
        dataRange->countFromEnd() ? -1 : int(dataRange->start()),
        dataRange->readToEnd() ? -1 : int(dataRange->range()),
        dataRange->skip(),
        dataRange->doSkip(),
        dataRange->doFilter());

      if (DataDialog::tagStringAuto()) {
        dataVector->setDescriptiveName(QString());
      } else {
        dataVector->setDescriptiveName(DataDialog::tagString());
      }

      dataVector->registerChange();
      dataVector->unlock();

      setDataVectorDefaults(dataVector);
      _vectorTab->dataRange()->setWidgetDefaults();
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        GeneratedVectorPtr vector = kst_cast<GeneratedVector>(_document->objectStore()->retrieveObject(objectName));
        if (vector) {
          double min = _vectorTab->fromDirty() ? _vectorTab->from() : vector->min();
          double max = _vectorTab->toDirty() ?  _vectorTab->to() : vector->max();
          double length = _vectorTab->numberOfSamplesDirty() ?  _vectorTab->numberOfSamples() : vector->length();
          vector->writeLock();
          vector->changeRange(min, max, length);
          vector->registerChange();
          vector->unlock();
        }
      }
    } else {
      const qreal from = _vectorTab->from();
      const qreal to = _vectorTab->to();
      const int numberOfSamples = _vectorTab->numberOfSamples();
      generatedVector->writeLock();
      generatedVector->changeRange(from, to, numberOfSamples);
      if (DataDialog::tagStringAuto()) {
        generatedVector->setDescriptiveName(QString());
      } else {
        generatedVector->setDescriptiveName(DataDialog::tagString());
      }
      generatedVector->registerChange();
      generatedVector->unlock();
      setGenVectorDefaults(generatedVector);
    }
  }

  return dataObject();
}

}

// vim: ts=2 sw=2 et
