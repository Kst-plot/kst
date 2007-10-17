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


uint MatrixTab::nX() const {
  return _nX->text().toInt();
}


void MatrixTab::setNX(uint nX) {
  _nX->setValue(nX);
}


uint MatrixTab::nY() const {
  return _nY->text().toInt();
}


void MatrixTab::setNY(uint nY) {
  _nY->setValue(nY);
}


double MatrixTab::minX() const {
  return _minX->text().toDouble();
}


void MatrixTab::setMinX(double minX) {
  _minX->setText(QString::number(minX));
}


double MatrixTab::minY() const {
  return _minY->text().toDouble();
}


void MatrixTab::setMinY(double minY) {
  _minY->setText(QString::number(minY));
}


double MatrixTab::stepX() const {
  return _xStep->text().toDouble();
}


void MatrixTab::setStepX(double stepX) {
  _xStep->setText(QString::number(stepX));
}


double MatrixTab::stepY() const {
  return _yStep->text().toDouble();
}


void MatrixTab::setStepY(double stepY) {
  _yStep->setText(QString::number(stepY));
}


double MatrixTab::gradientZAtMin() const {
  return _gradientZAtMin->text().toDouble();
}


void MatrixTab::setGradientZAtMin(double gradientZAtMin) {
  _gradientZAtMin->setText(QString::number(gradientZAtMin));
}


double MatrixTab::gradientZAtMax() const {
  return _gradientZAtMax->text().toDouble();
}


void MatrixTab::setGradientZAtMax(double gradientZAtMax) {
  _gradientZAtMax->setText(QString::number(gradientZAtMax));
}


bool MatrixTab::xDirection() const {
  return _gradientX->isChecked();
}


void MatrixTab::setXDirection(bool xDirection) {
  _gradientX->setChecked(xDirection);
  _gradientY->setChecked(!xDirection);
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
  switch(_matrixTab->matrixMode()) {
  case MatrixTab::DataMatrix:
    return createNewDataMatrix();
  case MatrixTab::GeneratedMatrix:
    return createNewGeneratedMatrix();
  default:
    return 0;
  }
}


ObjectPtr MatrixDialog::createNewDataMatrix() const {
  qDebug() << "createNewDataMatrix" << endl;
  return 0;
}


ObjectPtr MatrixDialog::createNewGeneratedMatrix() const {
  const uint nX = _matrixTab->nX();
  const uint nY = _matrixTab->nY();
  const double minX = _matrixTab->minX();
  const double minY = _matrixTab->minY();
  const double stepX = _matrixTab->stepX();
  const double stepY = _matrixTab->stepY();
  const double gradZMin = _matrixTab->gradientZAtMin();
  const double gradZMax = _matrixTab->gradientZAtMax();
  const bool xDirection =  _matrixTab->xDirection();
  const ObjectTag tag = ObjectTag(tagName(), ObjectTag::globalTagContext);

//    qDebug() << "Creating new generated matrix ===>"
//             << "\n\ttag:" << tag.tag()
//             << "\n\tnX:" << nX
//             << "\n\tnY:" << nY
//             << "\n\tminX:" << minX
//             << "\n\tminY:" << minY
//             << "\n\tstepX:" << stepX
//             << "\n\tstepY:" << stepY
//             << "\n\tgradZMin:" << gradZMin
//             << "\n\tgradZMax:" << gradZMax
//             << "\n\txDirection:" << xDirection
//             << endl;

  GeneratedMatrixPtr matrix = new GeneratedMatrix(tag, nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
  return static_cast<ObjectPtr>(matrix);
}


ObjectPtr MatrixDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
