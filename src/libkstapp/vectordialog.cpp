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

  connect(_readFromSource, SIGNAL(toggled(bool)),
          this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)),
          this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()),
          this, SLOT(showConfigWidget()));

  _fileName->setFile(QDir::currentPath());
  //_fileName->setFile(vectorDefaults.dataSource());

  //FIXME need a solution for replacing kio for this...
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


void VectorTab::setFrom(qreal from) {
  _from->setText(QString::number(from));
}


qreal VectorTab::to() const {
  return _to->text().toDouble();
}


void VectorTab::setTo(qreal to) {
  _to->setText(QString::number(to));
}


int VectorTab::numberOfSamples() const {
  return _numberOfSamples->value();
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


void VectorDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_vectorTab->vectorMode() == VectorTab::GeneratedVector || !_vectorTab->field().isEmpty());
}

void VectorDialog::configureTab(ObjectPtr vector) {
  if (!vector) {
    _vectorTab->setFile(Kst::dialogDefaults->value("vector/datasource",_vectorTab->file()).toString());
    _vectorTab->dataRange()->setRange(Kst::dialogDefaults->value("vector/range", 1).toInt());
    _vectorTab->dataRange()->setStart(Kst::dialogDefaults->value("vector/start", 0).toInt());
    _vectorTab->dataRange()->setCountFromEnd(Kst::dialogDefaults->value("vector/countFromEnd",false).toBool());
    _vectorTab->dataRange()->setReadToEnd(Kst::dialogDefaults->value("vector/readToEnd",true).toBool());
    _vectorTab->dataRange()->setSkip(Kst::dialogDefaults->value("vector/skip", 0).toInt());
    _vectorTab->dataRange()->setDoSkip(Kst::dialogDefaults->value("vector/doSkip", false).toBool());
    _vectorTab->dataRange()->setDoFilter(Kst::dialogDefaults->value("vector/doAve",false).toBool());
    _vectorTab->setFrom(Kst::dialogDefaults->value("genVector/min",-10).toInt());
    _vectorTab->setTo(Kst::dialogDefaults->value("genVector/max",10).toInt());
    _vectorTab->setNumberOfSamples(Kst::dialogDefaults->value("genVector/length",1000).toInt());
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
  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(vector)) {
    _vectorTab->setVectorMode(VectorTab::GeneratedVector);
    _vectorTab->setFrom(generatedVector->min());
    _vectorTab->setTo(generatedVector->max());
    _vectorTab->setNumberOfSamples(generatedVector->length());
    _vectorTab->hideDataOptions();
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
//            << "\n\ttag:" << tag.tag()
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

  Kst::setDataVectorDefaults(vector);

#if 0
  DataVectorPtr vector = new DataVector(
      dataSource, field, tag,
      dataRange->countFromEnd() ? -1 : int(dataRange->start()),
      dataRange->readToEnd() ? -1 : int(dataRange->range()),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());
#endif

  vector->update(0);
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

  Kst::setGenVectorDefaults(vector);

//  return static_cast<ObjectPtr>(vector);
  return vector;
}


ObjectPtr VectorDialog::editExistingDataObject() const {
  if (DataVectorPtr dataVector = kst_cast<DataVector>(dataObject())) {
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
    dataVector->update(0);
    dataVector->unlock();

    Kst::setDataVectorDefaults(dataVector);

  } else if (GeneratedVectorPtr generatedVector = kst_cast<GeneratedVector>(dataObject())) {
    const qreal from = _vectorTab->from();
    const qreal to = _vectorTab->to();
    const int numberOfSamples = _vectorTab->numberOfSamples();
    generatedVector->writeLock();
    generatedVector->changeRange(from, to, numberOfSamples);
    generatedVector->unlock();
    Kst::setGenVectorDefaults(generatedVector);
  }
  return dataObject();
}

}
//---------------------------------------------------------------------------
// what follows belongs in dialogdefaults.cpp, but cbn hasn't figured out how
// to add a file to the list of things to get built and linked...
void Kst::setDataVectorDefaults(DataVectorPtr V) {
  //FIXME Do we need a V->readLock() here?
  Kst::dialogDefaults->setValue("vector/datasource", V->filename());
  Kst::dialogDefaults->setValue("vector/range", V->reqNumFrames());
  Kst::dialogDefaults->setValue("vector/start", V->reqStartFrame());
  Kst::dialogDefaults->setValue("vector/countFromEnd", V->countFromEOF());
  Kst::dialogDefaults->setValue("vector/readToEnd", V->readToEOF());
  Kst::dialogDefaults->setValue("vector/skip", V->skip());
  Kst::dialogDefaults->setValue("vector/doSkip", V->doSkip());
  Kst::dialogDefaults->setValue("vector/doAve", V->doAve());
}

//FIXME: move to dialogdefaults.cpp... (How do you add a file to the build system??)
void Kst::setGenVectorDefaults(GeneratedVectorPtr V) {
  //FIXME Do we need a V->readLock() here?
  Kst::dialogDefaults->setValue("genVector/min", V->min());
  Kst::dialogDefaults->setValue("genVector/max", V->max()); 
  Kst::dialogDefaults->setValue("genVector/length", V->length());
}
//---------------------------------------------------------------------------

// vim: ts=2 sw=2 et
