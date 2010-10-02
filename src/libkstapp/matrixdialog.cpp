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

#define NO_GENERATED_OPTIONS

#include "matrixdialog.h"

#include "dialogpage.h"
#include "datasourcedialog.h"
#include "editmultiplewidget.h"

#include "datamatrix.h"
#include "generatedmatrix.h"

#include "datacollection.h"

#include "document.h"
#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "dialogdefaults.h"

#include <QDir>
#include <QThreadPool>

namespace Kst {

MatrixTab::MatrixTab(ObjectStore *store, QWidget *parent)
  : DataTab(parent), _mode(DataMatrix), _store(store), _initField(QString()), _requestID(0) {

  setupUi(this);
  setTabTitle(tr("Matrix"));

  connect(_readFromSource, SIGNAL(toggled(bool)), this, SLOT(readFromSourceChanged()));
  connect(_fileName, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));
  connect(_configure, SIGNAL(clicked()), this, SLOT(showConfigWidget()));

  connect(_xStartCountFromEnd, SIGNAL(clicked()), this, SLOT(xStartCountFromEndClicked()));
  connect(_yStartCountFromEnd, SIGNAL(clicked()), this, SLOT(yStartCountFromEndClicked()));
  connect(_xNumStepsReadToEnd, SIGNAL(clicked()), this, SLOT(xNumStepsReadToEndClicked()));
  connect(_yNumStepsReadToEnd, SIGNAL(clicked()), this, SLOT(yNumStepsReadToEndClicked()));

  connect(_readFromSource, SIGNAL(clicked()), this, SLOT(updateEnables()));
  connect(_generateGradient, SIGNAL(clicked()), this, SLOT(updateEnables()));
  connect(_doSkip, SIGNAL(clicked()), this, SLOT(updateEnables()));

  connect(_xStart, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_yStart, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_xNumSteps, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_yNumSteps, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_skip, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_nX, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_nY, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_gradientZAtMin, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_gradientZAtMax, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_minX, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_minX, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_xStep, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_yStep, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_xStartCountFromEnd, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_yStartCountFromEnd, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_xNumStepsReadToEnd, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_yNumStepsReadToEnd, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_doSkip, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_doAverage, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_gradientX, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_gradientY, SIGNAL(clicked()), this, SIGNAL(modified()));

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

  if (_dataRangeGroup->isEnabled()) {
    _skip->setEnabled(_doSkip->isChecked());
    _doAverage->setEnabled(_doSkip->isChecked());
    xStartCountFromEndClicked();
    xNumStepsReadToEndClicked();
    yStartCountFromEndClicked();
    yNumStepsReadToEndClicked();
  }
}


void MatrixTab::hideGeneratedOptions() {
  _sourceGroup->setVisible(false);
  _gradientGroup->setVisible(false);
}


void MatrixTab::hideDataOptions() {
  _sourceGroup->setVisible(false);
  _dataSourceGroup->setVisible(false);
  _dataRangeGroup->setVisible(false);
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
  _initField = field; // for delayed index setting
  _field->setCurrentIndex(_field->findText(field));
}


void MatrixTab::setFieldList(const QStringList &fieldList) {
  _field->clear();
  _field->addItems(fieldList);
}


uint MatrixTab::nX() const {
  return _nX->text().toInt();
}


bool MatrixTab::nXDirty() const {
  return (!_nX->text().isEmpty());
}


void MatrixTab::setNX(uint nX) {
  _nX->setValue(nX);
}


uint MatrixTab::nY() const {
  return _nY->text().toInt();
}


bool MatrixTab::nYDirty() const {
  return (!_nY->text().isEmpty());
}


void MatrixTab::setNY(uint nY) {
  _nY->setValue(nY);
}


double MatrixTab::minX() const {
  return _minX->text().toDouble();
}


bool MatrixTab::minXDirty() const {
  return (!_minX->text().isEmpty());
}


void MatrixTab::setMinX(double minX) {
  _minX->setText(QString::number(minX));
}


double MatrixTab::minY() const {
  return _minY->text().toDouble();
}


bool MatrixTab::minYDirty() const {
  return (!_minY->text().isEmpty());
}


void MatrixTab::setMinY(double minY) {
  _minY->setText(QString::number(minY));
}


double MatrixTab::stepX() const {
  return _xStep->text().toDouble();
}


bool MatrixTab::stepXDirty() const {
  return (!_xStep->text().isEmpty());
}


void MatrixTab::setStepX(double stepX) {
  _xStep->setText(QString::number(stepX));
}


double MatrixTab::stepY() const {
  return _yStep->text().toDouble();
}


bool MatrixTab::stepYDirty() const {
  return (!_yStep->text().isEmpty());
}


void MatrixTab::setStepY(double stepY) {
  _yStep->setText(QString::number(stepY));
}


int MatrixTab::xStart() const {
  return _xStart->text().toInt();
}


bool MatrixTab::xStartDirty() const {
  return (!_xStart->text().isEmpty());
}


void MatrixTab::setXStart(int xStart) {
  _xStart->setValue(xStart);
}


int MatrixTab::yStart() const {
  return _yStart->text().toInt();
}


bool MatrixTab::yStartDirty() const {
  return (!_yStart->text().isEmpty());
}


void MatrixTab::setYStart(int yStart) {
  _yStart->setValue(yStart);
}


int MatrixTab::xNumSteps() const {
  return _xNumSteps->text().toInt();
}


bool MatrixTab::xNumStepsDirty() const {
  return (!_xNumSteps->text().isEmpty());
}


void MatrixTab::setXNumSteps(int xNumSteps) {
  _xNumSteps->setValue(xNumSteps);
}


int MatrixTab::yNumSteps() const {
  return _yNumSteps->text().toInt();
}


bool MatrixTab::yNumStepsDirty() const {
  return (!_yNumSteps->text().isEmpty());
}


void MatrixTab::setYNumSteps(int yNumSteps) {
  _yNumSteps->setValue(yNumSteps);
}


int MatrixTab::skip() const {
  return _skip->text().toInt();
}


bool MatrixTab::skipDirty() const {
  return (!_skip->text().isEmpty());
}


void MatrixTab::setSkip(int skip) {
  _skip ->setValue(skip);
}


double MatrixTab::gradientZAtMin() const {
  return _gradientZAtMin->text().toDouble();
}


bool MatrixTab::gradientZAtMinDirty() const {
  return (!_gradientZAtMin->text().isEmpty());
}


void MatrixTab::setGradientZAtMin(double gradientZAtMin) {
  _gradientZAtMin->setText(QString::number(gradientZAtMin));
}


double MatrixTab::gradientZAtMax() const {
  return _gradientZAtMax->text().toDouble();
}


bool MatrixTab::gradientZAtMaxDirty() const {
  return (!_gradientZAtMax->text().isEmpty());
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


bool MatrixTab::doAverage() const {
  return _doAverage->isChecked();
}


bool MatrixTab::doAverageDirty() const {
  return _doAverage->checkState() == Qt::PartiallyChecked;
}


void MatrixTab::setDoAverage(bool doAverage) {
  _doAverage->setChecked(doAverage);
}


bool MatrixTab::doSkip() const {
  return _doSkip->isChecked();
}


bool MatrixTab::doSkipDirty() const {
  return _doSkip->checkState() == Qt::PartiallyChecked;
}


void MatrixTab::setDoSkip(bool doSkip) {
  _doSkip->setChecked(doSkip);
}


bool MatrixTab::xStartCountFromEnd() const {
  return _xStartCountFromEnd->isChecked();
}


bool MatrixTab::xStartCountFromEndDirty() const {
  return _xNumStepsReadToEnd->checkState() == Qt::PartiallyChecked;
}


void MatrixTab::setXStartCountFromEnd(bool xStartCountFromEnd) {
  _xStartCountFromEnd->setChecked(xStartCountFromEnd);
}


bool MatrixTab::yStartCountFromEnd() const {
  return _yStartCountFromEnd->isChecked();
}


bool MatrixTab::yStartCountFromEndDirty() const {
  return _yNumStepsReadToEnd->checkState() == Qt::PartiallyChecked;
}


void MatrixTab::setYStartCountFromEnd(bool yStartCountFromEnd) {
  _yStartCountFromEnd->setChecked(yStartCountFromEnd);
}


bool MatrixTab::xReadToEnd() const {
  return _xNumStepsReadToEnd->isChecked();
}


bool MatrixTab::xReadToEndDirty() const {
  return _xNumStepsReadToEnd->checkState() == Qt::PartiallyChecked;
}


void MatrixTab::setXReadToEnd(bool xReadToEnd) {
  _xNumStepsReadToEnd->setChecked(xReadToEnd);
}


bool MatrixTab::yReadToEnd() const {
  return _yNumStepsReadToEnd->isChecked();
}


bool MatrixTab::yReadToEndDirty() const {
  return _yNumStepsReadToEnd->checkState() == Qt::PartiallyChecked;
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
  emit sourceChanged();
}


void MatrixTab::setMatrixMode(MatrixMode mode) {
  _mode = mode;
  _readFromSource->setChecked(mode == DataMatrix);
  _generateGradient->setChecked(mode == GeneratedMatrix);
}


void MatrixTab::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);

  _field->setEnabled(true);

  _dataSource->readLock();

  _field->addItems(_dataSource->matrix().list());
  if (!_initField.isEmpty()) {
    setField(_initField);
  }
  _field->setEditable(!_dataSource->matrix().isListComplete() && !_dataSource->matrix().list().empty());
  _configure->setEnabled(_dataSource->hasConfigWidget());

  _dataSource->unlock();

  emit sourceChanged();
}


void MatrixTab::fileNameChanged(const QString &file) {
  _field->clear();
  _field->setEnabled(false);
  _configure->setEnabled(false);
  emit sourceChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


void MatrixTab::showConfigWidget() {
  DataSourceDialog dialog(dataDialog()->editMode(), _dataSource, this);
  dialog.exec();
  fileNameChanged(_dataSource->fileName());
}


void MatrixTab::clearTabValues() {
  _xStart->clear();
  _yStart->clear();
  _xNumSteps->clear();
  _yNumSteps->clear();
  _xNumStepsReadToEnd->setCheckState(Qt::PartiallyChecked);
  _yNumStepsReadToEnd->setCheckState(Qt::PartiallyChecked);
  _doSkip->setCheckState(Qt::PartiallyChecked);
  _doAverage->setCheckState(Qt::PartiallyChecked);
  _gradientZAtMin->clear();
  _gradientZAtMax->clear();
  _nX->clear();
  _nY->clear();
  _minX->clear();
  _minY->clear();
  _xStep->clear();
  _yStep->clear();
  setXDirection(true);
}


void MatrixTab::enableSingleEditOptions(bool enabled) {
  _dataSourceGroup->setEnabled(enabled);
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
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));
  connect(_matrixTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


MatrixDialog::~MatrixDialog() {
}


// QString MatrixDialog::tagString() const {
//   return DataDialog::tagString();
// }


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

#ifdef NO_GENERATED_OPTIONS
    _matrixTab->hideGeneratedOptions();
#endif

  } else if (DataMatrixPtr dataMatrix = kst_cast<DataMatrix>(matrix)) {
    _matrixTab->setMatrixMode(MatrixTab::DataMatrix);
    _matrixTab->setFile(dataMatrix->dataSource()->fileName());
    _matrixTab->setDataSource(dataMatrix->dataSource());
    _matrixTab->setField(dataMatrix->field());
    _matrixTab->setXStartCountFromEnd(dataMatrix->xCountFromEnd());
    _matrixTab->setYStartCountFromEnd(dataMatrix->yCountFromEnd());
    _matrixTab->setXNumSteps(dataMatrix->xNumSteps());
    _matrixTab->setYNumSteps(dataMatrix->yNumSteps());
    _matrixTab->setMinX(dataMatrix->minX());
    _matrixTab->setMinY(dataMatrix->minY());
    _matrixTab->setStepX(dataMatrix->xStepSize());
    _matrixTab->setStepY(dataMatrix->yStepSize());

    _matrixTab->setXStart(dataMatrix->reqXStart());
    _matrixTab->setYStart(dataMatrix->reqYStart());
    _matrixTab->setXReadToEnd(dataMatrix->xReadToEnd());
    _matrixTab->setYReadToEnd(dataMatrix->yReadToEnd());
    _matrixTab->setSkip(dataMatrix->skip());
    _matrixTab->setDoSkip(dataMatrix->doSkip());
    _matrixTab->setDoAverage(dataMatrix->doAverage());
    _matrixTab->hideGeneratedOptions();
    if (_editMultipleWidget) {
      DataMatrixList objects = _document->objectStore()->getObjects<DataMatrix>();
      _editMultipleWidget->clearObjects();
      foreach(DataMatrixPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
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
    if (_editMultipleWidget) {
      DataMatrixList objects = _document->objectStore()->getObjects<DataMatrix>();
      _editMultipleWidget->clearObjects();
      foreach(DataMatrixPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


void MatrixDialog::editMultipleMode() {
  _matrixTab->enableSingleEditOptions(false);
  _matrixTab->clearTabValues();
}


void MatrixDialog::editSingleMode() {
  _matrixTab->enableSingleEditOptions(true);
   configureTab(dataObject());
}


void MatrixDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_matrixTab->matrixMode() == MatrixTab::GeneratedMatrix || !_matrixTab->field().isEmpty());
}


ObjectPtr MatrixDialog::createNewDataObject() {
  switch(_matrixTab->matrixMode()) {
  case MatrixTab::DataMatrix:
    return createNewDataMatrix();
  case MatrixTab::GeneratedMatrix:
    return createNewGeneratedMatrix();
  default:
    return 0;
  }
}


ObjectPtr MatrixDialog::createNewDataMatrix() {
 const DataSourcePtr dataSource = _matrixTab->dataSource();

  //FIXME better validation than this please...
  if (!dataSource)
    return 0;

  const QString field = _matrixTab->field();
  const int skip = _matrixTab->skip();
  const bool doAverage = _matrixTab->doAverage();
  const bool doSkip = _matrixTab->doSkip();
  const int xStart = _matrixTab->xStartCountFromEnd() ? -1 : _matrixTab->xStart();
  const int yStart = _matrixTab->yStartCountFromEnd() ? -1 : _matrixTab->yStart();
  const int xNumSteps = _matrixTab->xReadToEnd() ? -1 : _matrixTab->xNumSteps();
  const int yNumSteps = _matrixTab->yReadToEnd() ? -1 : _matrixTab->yNumSteps();
  const double minX = _matrixTab->minX();
  const double minY = _matrixTab->minY();
  const double stepX = _matrixTab->stepX();
  const double stepY = _matrixTab->stepY();

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

  DataMatrixPtr matrix = _document->objectStore()->createObject<DataMatrix>();
  matrix->change(dataSource, field,
      xStart, yStart,
      xNumSteps, yNumSteps, doAverage,
      doSkip, skip, minX, minY, stepX, stepY);

  if (DataDialog::tagStringAuto()) {
     matrix->setDescriptiveName(QString());
  } else {
     matrix->setDescriptiveName(DataDialog::tagString());
  }

  matrix->writeLock();
  matrix->registerChange();
  matrix->unlock();

  setDataMatrixDefaults(matrix);

  _dataObjectName = matrix->Name();

  return static_cast<ObjectPtr>(matrix);
}


ObjectPtr MatrixDialog::createNewGeneratedMatrix() {
  const uint nX = _matrixTab->nX();
  const uint nY = _matrixTab->nY();
  const double minX = _matrixTab->minX();
  const double minY = _matrixTab->minY();
  const double stepX = _matrixTab->stepX();
  const double stepY = _matrixTab->stepY();
  const double gradZMin = _matrixTab->gradientZAtMin();
  const double gradZMax = _matrixTab->gradientZAtMax();
  const bool xDirection =  _matrixTab->xDirection();

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
  GeneratedMatrixPtr matrix = _document->objectStore()->createObject<GeneratedMatrix>();
  matrix->change(nX, nY, minX, minY, stepX, stepY, gradZMin, gradZMax, xDirection);
  if (DataDialog::tagStringAuto()) {
     matrix->setDescriptiveName(QString());
  } else {
     matrix->setDescriptiveName(DataDialog::tagString());
  }

  matrix->writeLock();
  matrix->registerChange();
  matrix->unlock();

  _dataObjectName = matrix->Name();

  return static_cast<ObjectPtr>(matrix);
}


ObjectPtr MatrixDialog::editExistingDataObject() const {
  if (DataMatrixPtr dataMatrix = kst_cast<DataMatrix>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        DataMatrixPtr matrix = kst_cast<DataMatrix>(_document->objectStore()->retrieveObject(objectName));
        if (matrix) {
          const int skip = _matrixTab->skipDirty() ? _matrixTab->skip() : matrix->skip();
          int xStart = _matrixTab->xStartDirty() ? _matrixTab->xStart() : matrix->reqXStart();
          int yStart = _matrixTab->yStartDirty() ? _matrixTab->yStart() : matrix->reqYStart();
          int xNumSteps = _matrixTab->xNumStepsDirty() ? _matrixTab->xNumSteps() : matrix->xNumSteps();
          int yNumSteps = _matrixTab->yNumStepsDirty() ? _matrixTab->yNumSteps() : matrix->yNumSteps();
          const double minX = _matrixTab->minXDirty() ? _matrixTab->minX() : matrix->minX();
          const double minY = _matrixTab->minYDirty() ? _matrixTab->minY() : matrix->minY();
          const double stepX = _matrixTab->stepXDirty() ? _matrixTab->stepX() : matrix->xStepSize();
          const double stepY = _matrixTab->stepYDirty() ? _matrixTab->stepY() : matrix->yStepSize();

          if (_matrixTab->xStartCountFromEndDirty()) {
              xStart = _matrixTab->xStartCountFromEnd() ? -1 : _matrixTab->xStart();
              xNumSteps = _matrixTab->xReadToEnd() ? -1 : _matrixTab->xNumSteps();
          }

          if (_matrixTab->yStartCountFromEndDirty()) {
              yStart = _matrixTab->yStartCountFromEnd() ? -1 : _matrixTab->yStart();
              yNumSteps = _matrixTab->yReadToEnd() ? -1 : _matrixTab->yNumSteps();
          }

          bool doSkip = _matrixTab->doSkipDirty() ?  _matrixTab->doSkip() : matrix->doSkip();
          bool doAve = _matrixTab->doAverageDirty() ?  _matrixTab->doAverage() : matrix->doAverage();

          matrix->writeLock();
          matrix->changeFrames(xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);
          matrix->registerChange();
          matrix->unlock();
        }
      }
    } else {
      const DataSourcePtr dataSource = _matrixTab->dataSource();

      //FIXME better validation than this please...
      if (!dataSource)
        return 0;

      const QString field = _matrixTab->field();
      const int skip = _matrixTab->skip();
      const bool doAverage = _matrixTab->doAverage();
      const bool doSkip = _matrixTab->doSkip();
      const int xStart = _matrixTab->xStartCountFromEnd() ? -1 : _matrixTab->xStart();
      const int yStart = _matrixTab->yStartCountFromEnd() ? -1 : _matrixTab->yStart();
      const int xNumSteps = _matrixTab->xReadToEnd() ? -1 : _matrixTab->xNumSteps();
      const int yNumSteps = _matrixTab->yReadToEnd() ? -1 : _matrixTab->yNumSteps();
      const double minX = _matrixTab->minX();
      const double minY = _matrixTab->minY();
      const double stepX = _matrixTab->stepX();
      const double stepY = _matrixTab->stepY();

      dataMatrix->writeLock();
      dataMatrix->change(dataSource, field, xStart, yStart, xNumSteps, yNumSteps, doAverage, doSkip, skip, minX, minY, stepX, stepY);
      dataMatrix->registerChange();
      dataMatrix->unlock();
      setDataMatrixDefaults(dataMatrix);
    }
  } else if (GeneratedMatrixPtr generatedMatrix = kst_cast<GeneratedMatrix>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        GeneratedMatrixPtr matrix = kst_cast<GeneratedMatrix>(_document->objectStore()->retrieveObject(objectName));
        if (matrix) {
          const uint nX = _matrixTab->nXDirty() ? _matrixTab->nX() : matrix->xNumSteps();
          const uint nY = _matrixTab->nYDirty() ? _matrixTab->nY() : matrix->yNumSteps();
          const double minX = _matrixTab->minXDirty() ? _matrixTab->minX() : matrix->minX();
          const double minY = _matrixTab->minYDirty() ? _matrixTab->minY() : matrix->minY();
          const double stepX = _matrixTab->stepXDirty() ? _matrixTab->stepX() : matrix->xStepSize();
          const double stepY = _matrixTab->stepYDirty() ? _matrixTab->stepY() : matrix->yStepSize();
          const double gradientZAtMin = _matrixTab->gradientZAtMinDirty() ? _matrixTab->gradientZAtMin() : matrix->gradZMin();
          const double gradientZAtMax = _matrixTab->gradientZAtMaxDirty() ? _matrixTab->gradientZAtMax() : matrix->gradZMax();
          const bool xDirection = _matrixTab->gradientZAtMaxDirty() || _matrixTab->gradientZAtMinDirty() ? _matrixTab->xDirection() : matrix->xDirection();

          matrix->writeLock();
          matrix->change(nX, nY, minX, minY, stepX, stepY, gradientZAtMin, gradientZAtMax, xDirection);
          matrix->registerChange();
          matrix->unlock();
        }
      }
    } else {
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
      generatedMatrix->registerChange();
      generatedMatrix->unlock();
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
