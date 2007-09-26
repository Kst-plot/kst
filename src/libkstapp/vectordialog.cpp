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

#include "vectordialog.h"

#include "dialogpage.h"
#include "datasourcedialog.h"

#include "kstrvector.h"
#include "kstsvector.h"

#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"

#include "kstvectordefaults.h"
#include "defaultprimitivenames.h"

#include <QDir>

namespace Kst {

VectorTab::VectorTab(QWidget *parent)
  : DataTab(parent), _mode(DataVector) {

  setupUi(this);
  setTabTitle(tr("Vector"));

  connect(_readFromSource, SIGNAL(toggled(bool)),
          this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)),
          this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()),
          this, SLOT(showConfigWidget()));

  _fileName->setFile(QDir::currentPath());
  //_fileName->setFile(KST::vectorDefaults.dataSource());

  //FIXME need a solution for replacing kio for this...
  _connect->setVisible(false);
}


VectorTab::~VectorTab() {
}


KstDataSourcePtr VectorTab::dataSource() const {
  return _dataSource;
}


void VectorTab::setDataSource(KstDataSourcePtr dataSource) {
  _dataSource = dataSource;
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


void VectorTab::readFromSourceChanged() {

  if (_readFromSource->isChecked())
    setVectorMode(DataVector);
  else
    setVectorMode(GeneratedVector);

  _rvectorGroup->setEnabled(_readFromSource->isChecked());
  _dataRange->setEnabled(_readFromSource->isChecked());
  _svectorGroup->setEnabled(!_readFromSource->isChecked());
}


void VectorTab::fileNameChanged(const QString &file) {
  QFileInfo info(file);
  if (!info.exists() || !info.isFile())
    return;

  _field->clear();

  KST::dataSourceList.lock().readLock();
  _dataSource = KST::dataSourceList.findReusableFileName(file);
  KST::dataSourceList.lock().unlock();

  if (!_dataSource) {
    _dataSource = KstDataSource::loadSource(file, QString());
  }

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
}


void VectorTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
}


VectorDialog::VectorDialog(QWidget *parent)
  : DataDialog(parent) {

  setWindowTitle(tr("New Vector"));

  _vectorTab = new VectorTab(this);
  addDataTab(_vectorTab);
}


VectorDialog::VectorDialog(KstObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  setWindowTitle(tr("Edit Vector"));

  _vectorTab = new VectorTab(this);
  addDataTab(_vectorTab);

  //FIXME need to do validation to enable/disable ok button...
}


VectorDialog::~VectorDialog() {
}


QString VectorDialog::tagName() const {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    {
      QString tagName = DataDialog::tagName();
      tagName.replace(defaultTag(), _vectorTab->field());
      return KST::suggestVectorName(tagName);
    }
  case VectorTab::GeneratedVector:
    {
      if (DataDialog::tagName() == defaultTag()) {
        const qreal from = _vectorTab->from();
        const qreal to = _vectorTab->to();
        return KST::suggestVectorName(QString("(%1..%2)").arg(from).arg(to));
      }
    }
  default:
    return DataDialog::tagName();
  }
}


KstObjectPtr VectorDialog::createNewDataObject() const {
  switch(_vectorTab->vectorMode()) {
  case VectorTab::DataVector:
    return createNewDataVector();
  case VectorTab::GeneratedVector:
    return createNewGeneratedVector();
  default:
    return 0;
  }
}


KstObjectPtr VectorDialog::createNewDataVector() const {
  const KstDataSourcePtr dataSource = _vectorTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  const QString field = _vectorTab->field();
  const DataRange *dataRange = _vectorTab->dataRange();
  const KstObjectTag tag = KstObjectTag(tagName(), dataSource->tag(), false);

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

  KstRVectorPtr vector = new KstRVector(
      dataSource, field, tag,
      dataRange->countFromEnd() ? -1 : int(dataRange->start()),
      dataRange->readToEnd() ? -1 : int(dataRange->range()),
      dataRange->skip(),
      dataRange->doSkip(),
      dataRange->doFilter());

  vector->writeLock();
  vector->update(0);
  vector->unlock();

  return static_cast<KstObjectPtr>(vector);
}


KstObjectPtr VectorDialog::createNewGeneratedVector() const {
  const qreal from = _vectorTab->from();
  const qreal to = _vectorTab->to();
  const int numberOfSamples = _vectorTab->numberOfSamples();
  const KstObjectTag tag = KstObjectTag(tagName(), KstObjectTag::globalTagContext);

//   qDebug() << "Creating new generated vector ===>"
//            << "\n\tfrom:" << from
//            << "\n\tto:" << to
//            << "\n\tnumberOfSamples:" << numberOfSamples
//            << "\n\ttag:" << tag.tag()
//            << endl;

  KstSVectorPtr vector = new KstSVector(from, to, numberOfSamples, tag);
  return static_cast<KstObjectPtr>(vector);
}


KstObjectPtr VectorDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
