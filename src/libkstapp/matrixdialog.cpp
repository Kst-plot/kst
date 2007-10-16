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

#include "matrixdialog.h"

#include "dialogpage.h"
#include "datasourcedialog.h"

#include "datamatrix.h"
#include "generatedmatrix.h"

#include "datacollection.h"
#include "dataobjectcollection.h"

#include "matrixdefaults.h"
#include "defaultprimitivenames.h"

#include <QDir>

namespace Kst {

MatrixTab::MatrixTab(QWidget *parent)
  : DataTab(parent), _mode(DataMatrix) {

  setupUi(this);
  setTabTitle(tr("Matrix"));

  connect(_readFromSource, SIGNAL(toggled(bool)),
          this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)),
          this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()),
          this, SLOT(showConfigWidget()));

  _fileName->setFile(QDir::currentPath());
  _fileName->setFile(matrixDefaults.dataSource());

  //FIXME need a solution for replacing kio for this...
  _connect->setVisible(false);
}


MatrixTab::~MatrixTab() {
}


DataSourcePtr MatrixTab::dataSource() const {
  return _dataSource;
}


void MatrixTab::setDataSource(DataSourcePtr dataSource) {
  _dataSource = dataSource;
}


QString MatrixTab::file() const {
  return _fileName->file();
}


void MatrixTab::setFile(const QString &file) {
  _fileName->setFile(file);
}


QString MatrixTab::field() const {
  return _field->currentText();
}


void MatrixTab::setField(const QString &field) {
  _field->setCurrentIndex(_field->findText(field));
}


void MatrixTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


void MatrixTab::readFromSourceChanged() {

  if (_readFromSource->isChecked())
    setMatrixMode(DataMatrix);
  else
    setMatrixMode(GeneratedMatrix);

   _dataSourceGroup->setEnabled(_readFromSource->isChecked());
   _dataRangeGroup->setEnabled(_readFromSource->isChecked());
   _gradientGroup->setEnabled(!_readFromSource->isChecked());
   _scalingGroup->setEnabled(!_readFromSource->isChecked());
}


void MatrixTab::fileNameChanged(const QString &file) {
  QFileInfo info(file);
  if (!info.exists() || !info.isFile())
    return;

  _field->clear();

  dataSourceList.lock().readLock();
  _dataSource = dataSourceList.findReusableFileName(file);
  dataSourceList.lock().unlock();

  if (!_dataSource) {
    _dataSource = DataSource::loadSource(file, QString());
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

  _dataSource->unlock();
}


void MatrixTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
}


MatrixDialog::MatrixDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Matrix"));
  else
    setWindowTitle(tr("New Matrix"));

  _matrixTab = new MatrixTab(this);
  addDataTab(_matrixTab);

  //FIXME need to do validation to enable/disable ok button...
}


MatrixDialog::~MatrixDialog() {
}


QString MatrixDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr MatrixDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


ObjectPtr MatrixDialog::createNewDataMatrix() const {
  qDebug() << "createNewDataMatrix" << endl;
  return 0;
}


ObjectPtr MatrixDialog::createNewGeneratedMatrix() const {
  qDebug() << "createNewGeneratedMatrix" << endl;
  return 0;
}


ObjectPtr MatrixDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
