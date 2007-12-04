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

#include "document.h"
#include "objectstore.h"

#include "dialogdefaults.h"

#include <QDir>

namespace Kst {

MatrixTab::MatrixTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent), _mode(DataMatrix), _store(store) {

  setupUi(this);
  setTabTitle(tr("Matrix"));

  connect(_readFromSource, SIGNAL(toggled(bool)),
          this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)),
          this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()),
          this, SLOT(showConfigWidget()));

  connect(_xStartCountFromEnd, SIGNAL(clicked()), this, SLOT(xStartCountFromEndClicked()));
  connect(_yStartCountFromEnd, SIGNAL(clicked()), this, SLOT(yStartCountFromEndClicked()));
  connect(_xNumStepsReadToEnd, SIGNAL(clicked()), this, SLOT(xNumStepsReadToEndClicked()));
  connect(_yNumStepsReadToEnd, SIGNAL(clicked()), this, SLOT(yNumStepsReadToEndClicked()));

  connect(_readFromSource, SIGNAL(clicked()), this, SLOT(updateEnables()));
  connect(_generateGradient, SIGNAL(clicked()), this, SLOT(updateEnables()));
  connect(_doSkip, SIGNAL(clicked()), this, SLOT(updateEnables()));

  _fileName->setFile(QDir::currentPath());
//  _fileName->setFile(matrixDefaults.dataSource());  // huh?

  //FIXME need a solution for replacing kio for this...
  _connect->setVisible(false);
}


MatrixTab::~MatrixTab() {
}

void MatrixTab::xStartCountFromEndClicked() {
  _xNumStepsReadToEnd->setChecked(_xNumStepsReadToEnd->isChecked() && !_xStartCountFromEnd->isChecked());
  _xStart->setEnabled(!_xStartCountFromEnd->isChecked());
  _xNumSteps->setEnabled(!_xNumStepsReadToEnd->isChecked());
}


void MatrixTab::xNumStepsReadToEndClicked() {
  _xStartCountFromEnd->setChecked(_xStartCountFromEnd->isChecked() && !_xNumStepsReadToEnd->isChecked());
  _xNumSteps->setEnabled(!_xNumStepsReadToEnd->isChecked());
  _xStart->setEnabled(!_xStartCountFromEnd->isChecked());
}


void MatrixTab::yStartCountFromEndClicked() {
  _yNumStepsReadToEnd->setChecked(_yNumStepsReadToEnd->isChecked() && !_yStartCountFromEnd->isChecked());
  _yStart->setEnabled(!_yStartCountFromEnd->isChecked());
  _yNumSteps->setEnabled(!_yNumStepsReadToEnd->isChecked());
}


void MatrixTab::yNumStepsReadToEndClicked() {
  _yStartCountFromEnd->setChecked(_yStartCountFromEnd->isChecked() && !_yNumStepsReadToEnd->isChecked());
  _yNumSteps->setEnabled(!_yNumStepsReadToEnd->isChecked());
  _yStart->setEnabled(!_yStartCountFromEnd->isChecked());
}


void MatrixTab::updateEnables() {
  _dataSourceGroup->setEnabled(_readFromSource->isChecked());
  _dataRangeGroup->setEnabled(_readFromSource->isChecked());
  _gradientGroup->setEnabled(_generateGradient->isChecked());
  _scalingGroup->setEnabled(_generateGradient->isChecked());

  if (_dataRangeGroup->isEnabled()) {
    _skip->setEnabled(_doSkip->isChecked());
    _doAve->setEnabled(_doSkip->isChecked());
    xStartCountFromEndClicked();
    xNumStepsReadToEndClicked();
    yStartCountFromEndClicked();
    yNumStepsReadToEndClicked();
  }
}


void MatrixTab::hideGeneratedOptions() {
  _sourceGroup->setVisible(false);
  _gradientGroup->setVisible(false);
  _scalingGroup->setVisible(false);
  setMaximumHeight(300);
}


void MatrixTab::hideDataOptions() {
  _sourceGroup->setVisible(false);
  _dataSourceGroup->setVisible(false);
  _dataRangeGroup->setVisible(false);
  setMaximumHeight(200);
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


int MatrixTab::xStart() const {
  return _xStart->text().toInt();
}


void MatrixTab::setXStart(int xStart) {
  _xStart->setValue(xStart);
}


int MatrixTab::yStart() const {
  return _yStart->text().toInt();
}


void MatrixTab::setYStart(int yStart) {
  _yStart->setValue(yStart);
}


int MatrixTab::xNumSteps() const {
  return _xNumSteps->text().toInt();
}


void MatrixTab::setXNumSteps(int xNumSteps) {
  _xNumSteps->setValue(xNumSteps);
}


int MatrixTab::yNumSteps() const {
  return _yNumSteps->text().toInt();
}


void MatrixTab::setYNumSteps(int yNumSteps) {
  _yNumSteps->setValue(yNumSteps);
}


int MatrixTab::skip() const {
  return _skip->text().toInt();
}


void MatrixTab::setSkip(int skip) {
  _skip ->setValue(skip);
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


bool MatrixTab::doAve() const {
  return _doAve->isChecked();
}


void MatrixTab::setDoAve(bool doAve) {
  _doAve->setChecked(doAve);
}


bool MatrixTab::doSkip() const {
  return _doSkip->isChecked();
}


void MatrixTab::setDoSkip(bool doSkip) {
  _doSkip->setChecked(doSkip);
}


bool MatrixTab::xStartCountFromEnd() const {
  return _xStartCountFromEnd->isChecked();
}


void MatrixTab::setXStartCountFromEnd(bool xStartCountFromEnd) {
  _xStartCountFromEnd->setChecked(xStartCountFromEnd);
}


bool MatrixTab::yStartCountFromEnd() const {
  return _yStartCountFromEnd->isChecked();
}


void MatrixTab::setYStartCountFromEnd(bool yStartCountFromEnd) {
  _yStartCountFromEnd->setChecked(yStartCountFromEnd);
}


bool MatrixTab::xReadToEnd() const {
  return _xNumStepsReadToEnd->isChecked();
}


void MatrixTab::setXReadToEnd(bool xReadToEnd) {
  _xNumStepsReadToEnd->setChecked(xReadToEnd);
}


bool MatrixTab::yReadToEnd() const {
  return _yNumStepsReadToEnd->isChecked();
}


void MatrixTab::setYReadToEnd(bool yReadToEnd) {
  _yNumStepsReadToEnd->setChecked(yReadToEnd);
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
  emit sourceChanged();
}


void MatrixTab::setMatrixMode(MatrixMode mode) {
  _mode = mode;
  _readFromSource->setChecked(mode == DataMatrix);
  _generateGradient->setChecked(mode == GeneratedMatrix);
}


void MatrixTab::fileNameChanged(const QString &file) {
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

  _field->addItems(_dataSource->matrixList());
  _field->setEditable(!_dataSource->fieldListIsComplete() && !_dataSource->matrixList().empty());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  _dataSource->unlock();

  emit sourceChanged();
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

  Q_ASSERT(_document && _document->objectStore());
  _matrixTab = new MatrixTab(_document->objectStore(), this);
  addDataTab(_matrixTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_matrixTab, SIGNAL(sourceChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


MatrixDialog::~MatrixDialog() {
}


QString MatrixDialog::tagString() const {
  return DataDialog::tagString();
}


void MatrixDialog::configureTab(ObjectPtr matrix) {
  if (!matrix) {
    _matrixTab->setMatrixMode(MatrixTab::DataMatrix);
    _matrixTab->setFile(_dialogDefaults->value("matrix/datasource",_matrixTab->file()).toString());

    _matrixTab->setXStartCountFromEnd(_dialogDefaults->value("matrix/xCountFromEnd",false).toBool());
    _matrixTab->setYStartCountFromEnd(_dialogDefaults->value("matrix/yCountFromEnd",false).toBool());
    _matrixTab->setXReadToEnd(_dialogDefaults->value("matrix/xReadToEnd",false).toBool());
    _matrixTab->setYReadToEnd(_dialogDefaults->value("matrix/yReadToEnd",false).toBool());

    _matrixTab->setXNumSteps(_dialogDefaults->value("matrix/xNumSteps",1000).toInt());
    _matrixTab->setYNumSteps(_dialogDefaults->value("matrix/yNumSteps",1000).toInt());
    _matrixTab->setXStart(_dialogDefaults->value("matrix/reqXStart",0).toInt());
    _matrixTab->setYStart(_dialogDefaults->value("matrix/reqYStart",0).toInt());

  } else if (DataMatrixPtr dataMatrix = kst_cast<DataMatrix>(matrix)) {
    _matrixTab->setMatrixMode(MatrixTab::DataMatrix);
    _matrixTab->setFile(dataMatrix->dataSource()->fileName());
    _matrixTab->setDataSource(dataMatrix->dataSource());
    _matrixTab->setField(dataMatrix->field());
    _matrixTab->setXStartCountFromEnd(dataMatrix->xCountFromEnd());
    _matrixTab->setYStartCountFromEnd(dataMatrix->yCountFromEnd());
    _matrixTab->setXNumSteps(dataMatrix->xNumSteps());
    _matrixTab->setYNumSteps(dataMatrix->yNumSteps());

    _matrixTab->setXStart(dataMatrix->reqXStart());
    _matrixTab->setYStart(dataMatrix->reqYStart());
    _matrixTab->setXReadToEnd(dataMatrix->xReadToEnd());
    _matrixTab->setYReadToEnd(dataMatrix->yReadToEnd());
    _matrixTab->setSkip(dataMatrix->skip());
    _matrixTab->setDoSkip(dataMatrix->doSkip());
    _matrixTab->setDoAve(dataMatrix->doAverage());
    _matrixTab->hideGeneratedOptions();
  } else if (GeneratedMatrixPtr generatedMatrix = kst_cast<GeneratedMatrix>(matrix)) {
    _matrixTab->setMatrixMode(MatrixTab::GeneratedMatrix);
    _matrixTab->setNX(generatedMatrix->xNumSteps());
    _matrixTab->setNY(generatedMatrix->yNumSteps());
    _matrixTab->setMinX(generatedMatrix->minX());
    _matrixTab->setMinY(generatedMatrix->minY());
    _matrixTab->setStepX(generatedMatrix->xStepSize());
    _matrixTab->setStepY(generatedMatrix->yStepSize());
    _matrixTab->setGradientZAtMin(generatedMatrix->gradZMin());
    _matrixTab->setGradientZAtMax(generatedMatrix->gradZMax());
    _matrixTab->setXDirection(generatedMatrix->xDirection());
    _matrixTab->hideDataOptions();
  }
}


void MatrixDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_matrixTab->matrixMode() == MatrixTab::GeneratedMatrix || !_matrixTab->field().isEmpty());
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
 const DataSourcePtr dataSource = _matrixTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  const QString field = _matrixTab->field();
  const ObjectTag tag = ObjectTag(tagString(), dataSource->tag(), false);
  const int skip = _matrixTab->skip();
  const bool doAve = _matrixTab->doAve();
  const bool doSkip = _matrixTab->doSkip();
  const int xStart = _matrixTab->xStartCountFromEnd() ? -1 : _matrixTab->xStart();
  const int yStart = _matrixTab->yStartCountFromEnd() ? -1 : _matrixTab->yStart();
  const int xNumSteps = _matrixTab->xReadToEnd() ? -1 : _matrixTab->xNumSteps();
  const int yNumSteps = _matrixTab->yReadToEnd() ? -1 : _matrixTab->yNumSteps();

//   qDebug() << "Creating new data matrix ===>"
//            << "\n\tfileName:" << dataSource->fileName()
//            << "\n\tfileType:" << dataSource->fileType()
//            << "\n\tfield:" << field
//            << "\n\ttag:" << tag.tag()
//            << "\n\txStart:" << xStart
//            << "\n\tyStart:" << yStart
//            << "\n\txNumSteps:" << xNumSteps
//            << "\n\tyNumSteps:" << yNumSteps
//            << "\n\tskip:" << skip
//            << "\n\tdoSkip:" << doSkip
//            << "\n\tdoAve:" << doAve
//            << endl;

  Q_ASSERT(_document && _document->objectStore());
  DataMatrixPtr matrix = _document->objectStore()->createObject<DataMatrix>(tag);
  matrix->change(dataSource, field,
      xStart, yStart,
      xNumSteps, yNumSteps,
      doAve,
      doSkip, skip);

#if 0
  DataMatrixPtr matrix = new DataMatrix(
      dataSource, field, tag,
      xStart, yStart,
      xNumSteps, yNumSteps,
      doAve,
      doSkip, skip);
#endif

  matrix->writeLock();
  matrix->update(0);
  matrix->unlock();

  setDataMatrixDefaults(matrix);

  return static_cast<ObjectPtr>(matrix);
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
  const ObjectTag tag = ObjectTag(tagString(), ObjectTag::globalTagContext);

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

  Q_ASSERT(_document && _document->objectStore());
  GeneratedMatrixPtr matrix = _document->objectStore()->createObject<GeneratedMatrix>(tag);
  matrix->change(nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
#if 0
  GeneratedMatrixPtr matrix = new GeneratedMatrix(tag, nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
#endif

  matrix->writeLock();
  matrix->update(0);
  matrix->unlock();

  return static_cast<ObjectPtr>(matrix);
}


ObjectPtr MatrixDialog::editExistingDataObject() const {
  if (DataMatrixPtr dataMatrix = kst_cast<DataMatrix>(dataObject())) {
    const DataSourcePtr dataSource = _matrixTab->dataSource();

    //FIXME better validation than this please...
    if (!dataSource)
      return 0;

    const QString field = _matrixTab->field();
    const int skip = _matrixTab->skip();
    const bool doAve = _matrixTab->doAve();
    const bool doSkip = _matrixTab->doSkip();
    const int xStart = _matrixTab->xStartCountFromEnd() ? -1 : _matrixTab->xStart();
    const int yStart = _matrixTab->yStartCountFromEnd() ? -1 : _matrixTab->yStart();
    const int xNumSteps = _matrixTab->xReadToEnd() ? -1 : _matrixTab->xNumSteps();
    const int yNumSteps = _matrixTab->yReadToEnd() ? -1 : _matrixTab->yNumSteps();

    dataMatrix->writeLock();
    dataMatrix->change(dataSource, field,
        xStart, yStart,
        xNumSteps, yNumSteps,
        doAve,
        doSkip, skip);
    dataMatrix->update(0);
    dataMatrix->unlock();
    setDataMatrixDefaults(dataMatrix);

  } else if (GeneratedMatrixPtr generatedMatrix = kst_cast<GeneratedMatrix>(dataObject())) {
    const uint nX = _matrixTab->nX();
    const uint nY = _matrixTab->nY();
    const double minX = _matrixTab->minX();
    const double minY = _matrixTab->minY();
    const double stepX = _matrixTab->stepX();
    const double stepY = _matrixTab->stepY();
    const double gradZMin = _matrixTab->gradientZAtMin();
    const double gradZMax = _matrixTab->gradientZAtMax();
    const bool xDirection =  _matrixTab->xDirection();

    generatedMatrix->writeLock();
    generatedMatrix->change(nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
    generatedMatrix->update(0);
    generatedMatrix->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
