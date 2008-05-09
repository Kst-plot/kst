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
#include "dataobjectcollection.h"
#include "document.h"
#include "objectstore.h"

#include "dialogdefaults.h"
//#include "vectordefaults.h"

#include <QDir>

namespace Kst {

VectorTab::VectorTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent), _mode(DataVector), _store(store) {

  setupUi(this);
  setTabTitle(tr("Vector"));

  connect(_readFromSource, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));

  connect(_dataRange, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_numberOfSamples, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_from, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_to, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  _fileName->setFile(QDir::currentPath());

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
  _readFromSource->setChecked(mode == DataVector);
  _generateX->setChecked(mode == GeneratedVector);
}


QString VectorTab::file() const {
  return _fileName->file();
}


void VectorTab::setFile(const QString &file) {
  _fileName->setFile(file);
}


QString VectorTab::field() const {
  return _field->currentText();
}


void VectorTab::setField(const QString &field) {
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


void VectorTab::readFromSourceChanged() {

  if (_readFromSource->isChecked())
    setVectorMode(DataVector);
  else
    setVectorMode(GeneratedVector);

  _dataVectorGroup->setEnabled(_readFromSource->isChecked());
  _dataRange->setEnabled(_readFromSource->isChecked());
  _generatedVectorGroup->setEnabled(!_readFromSource->isChecked());

  emit sourceChanged();
}


void VectorTab::hideGeneratedOptions() {
  _sourceGroup->setVisible(false);
  _generatedVectorGroup->setVisible(false);
  setMaximumHeight(300);
}


void VectorTab::hideDataOptions() {
  _sourceGroup->setVisible(false);
  _dataVectorGroup->setVisible(false);
  _dataRange->setVisible(false);
  setMaximumHeight(150);
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


void VectorTab::fileNameChanged(const QString &file) {
  QFileInfo info(file);
  if (!info.exists() || !info.isFile())
    return;

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

  _field->addItems(_dataSource->fieldList());
  _field->setEditable(!_dataSource->fieldListIsComplete());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  //FIXME deal with time...
  //_dataRange->setAllowTime(ds->supportsTimeConversions());

  _dataSource->unlock();
  emit sourceChanged();
}


void VectorTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
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
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_vectorTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


VectorDialog::~VectorDialog() {
}


QString VectorDialog::tagString() const {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    {
      QString tagString = DataDialog::tagString();
      tagString.replace(defaultTagString(), _vectorTab->field());
      return tagString;
    }
  case VectorTab::GeneratedVector:
    {
      if (DataDialog::tagString() == defaultTagString()) {
        const qreal from = _vectorTab->from();
        const qreal to = _vectorTab->to();
        return QString("(%1..%2)").arg(from).arg(to);
      }
    }
  default:
    return DataDialog::tagString();
  }
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
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_vectorTab->vectorMode() == VectorTab::GeneratedVector || !_vectorTab->field().isEmpty());
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
      QStringList objectList;
      DataVectorList objects = _document->objectStore()->getObjects<DataVector>();
      foreach(DataVectorPtr object, objects) {
        objectList.append(object->tag().displayString());
      }
      _editMultipleWidget->addObjects(objectList);
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::GeneratedVector);
    _vectorTab->setFrom(generatedVector->min());
    _vectorTab->setTo(generatedVector->max());
    _vectorTab->setNumberOfSamples(generatedVector->length());
    _vectorTab->hideDataOptions();
    if (_editMultipleWidget) {
      QStringList objectList;
      GeneratedVectorList objects = _document->objectStore()->getObjects<GeneratedVector>();
      foreach(GeneratedVectorPtr object, objects) {
        objectList.append(object->tag().displayString());
      }
      _editMultipleWidget->addObjects(objectList);
    }
  }
}


ObjectPtr VectorDialog::createNewDataObject() const {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    return createNewDataVector();
  case VectorTab::GeneratedVector:
    return createNewGeneratedVector();
  default:
    return 0;
  }
}


ObjectPtr VectorDialog::createNewDataVector() const {
  const DataSourcePtr dataSource = _vectorTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  const QString field = _vectorTab->field();
  const DataRange *dataRange = _vectorTab->dataRange();

  Q_ASSERT(_document && _document->objectStore());
  const ObjectTag tag = _document->objectStore()->suggestObjectTag<DataVector>(tagString(), dataSource->tag());

//   qDebug() << "Creating new data vector ===>"
//            << "\n\tfileName:" << dataSource->fileName()
//            << "\n\tfileType:" << dataSource->fileType()
//            << "\n\tfield:" << field
//            << "\n\ttag:" << tag.displayString()
// 	   << "\n\ttagString:" << tagString()
//            << "\n\tstart:" << (dataRange->countFromEnd() ? -1 : int(dataRange->start()))
//            << "\n\trange:" << (dataRange->readToEnd() ? -1 : int(dataRange->range()))
//            << "\n\tskip:" << dataRange->skip()
//            << "\n\tdoSkip:" << (dataRange->doSkip() ? "true" : "false")
//            << "\n\tdoFilter:" << (dataRange->doFilter() ? "true" : "false")
//            << endl;

  DataVectorPtr vector = _document->objectStore()->createObject<DataVector>(tag);

  vector->writeLock();
  vector->change(dataSource, field,
      dataRange->countFromEnd() ? -1 : int(dataRange->start()),
      dataRange->readToEnd() ? -1 : int(dataRange->range()),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());

  vector->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  setDataVectorDefaults(vector);
  _vectorTab->dataRange()->setWidgetDefaults();

#if 0
  DataVectorPtr vector = new DataVector(
      dataSource, field, tag,
      dataRange->countFromEnd() ? -1 : int(dataRange->start()),
      dataRange->readToEnd() ? -1 : int(dataRange->range()),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());
#endif

  vector->update();
  vector->unlock();

//  return static_cast<ObjectPtr>(vector);
  return vector;
}


ObjectPtr VectorDialog::createNewGeneratedVector() const {
  const qreal from = _vectorTab->from();
  const qreal to = _vectorTab->to();
  const int numberOfSamples = _vectorTab->numberOfSamples();
  Q_ASSERT(_document && _document->objectStore());
  const ObjectTag tag = _document->objectStore()->suggestObjectTag<GeneratedVector>(tagString(), ObjectTag::globalTagContext);

//   qDebug() << "Creating new generated vector ===>"
//            << "\n\tfrom:" << from
//            << "\n\tto:" << to
//            << "\n\tnumberOfSamples:" << numberOfSamples
//            << "\n\ttag:" << tag.tag()
//            << endl;

  Q_ASSERT(_document && _document->objectStore());
  GeneratedVectorPtr vector = _document->objectStore()->createObject<GeneratedVector>(tag);
  vector->changeRange(from, to, numberOfSamples);

  setGenVectorDefaults(vector);

  vector->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

//  return static_cast<ObjectPtr>(vector);
  return vector;
}


ObjectPtr VectorDialog::editExistingDataObject() const {
  if (DataVectorPtr dataVector = kst_cast<DataVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      const DataRange *dataRange = _vectorTab->dataRange();
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectTag, objects) {
        DataVectorPtr vector = kst_cast<DataVector>(_document->objectStore()->retrieveObject(ObjectTag::fromString(objectTag)));
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
          vector->update();
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

      dataVector->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

//  Disable until new tag system is fully implemented.
//
//       if (dataVector->tag().name()!=tagString()) {
// 	//FIXME: needs a gaurd against being not unique
// 	dataVector->tag().setName(tagString()); 
//       }

      dataVector->update();
      dataVector->unlock();

      setDataVectorDefaults(dataVector);
      _vectorTab->dataRange()->setWidgetDefaults();
    }
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectTag, objects) {
        GeneratedVectorPtr vector = kst_cast<GeneratedVector>(_document->objectStore()->retrieveObject(ObjectTag::fromString(objectTag)));
        if (vector) {
          double min = _vectorTab->fromDirty() ? _vectorTab->from() : vector->min();
          double max = _vectorTab->toDirty() ?  _vectorTab->to() : vector->max();
          double length = _vectorTab->numberOfSamplesDirty() ?  _vectorTab->numberOfSamples() : vector->length();
          vector->writeLock();
          vector->changeRange(min, max, length);
          vector->update();
          vector->unlock();
        }
      }
    } else {
      const qreal from = _vectorTab->from();
      const qreal to = _vectorTab->to();
      const int numberOfSamples = _vectorTab->numberOfSamples();
      generatedVector->writeLock();
      generatedVector->changeRange(from, to, numberOfSamples);
      generatedVector->unlock();
      setGenVectorDefaults(generatedVector);
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
