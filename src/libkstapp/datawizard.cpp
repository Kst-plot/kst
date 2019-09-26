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

#include "datawizard.h"

#include <QFileInfo>
#include <QMessageBox>
#include <psversion.h>
#include <sysinfo.h>
#include <QThreadPool>

#include "colorsequence.h"
#include "curve.h"
#include "datacollection.h"
#include "datasourceconfiguredialog.h"
#include "datavector.h"
#include "dialogdefaults.h"
#include "document.h"
#include "mainwindow.h"
#include "objectstore.h"
#include "plotitem.h"
#include "plotiteminterface.h"
#include "applicationsettings.h"
#include "updatemanager.h"
#include "datasourcepluginmanager.h"
#include "sharedaxisboxitem.h"
#include "boxitem.h"
#include "updateserver.h"
#include "geticon.h"


namespace Kst {


//
// DataWizardPageDataSource
//

DataWizardPageDataSource::DataWizardPageDataSource(ObjectStore *store, QWidget *parent, const QString& default_source)
  : QWizardPage(parent), _pageValid(false), _store(store), _requestID(0)
{
  setupUi(this);

  MainWindow::setWidgetFlags(this);

  connect(_url, SIGNAL(changed(QString)), this, SLOT(sourceChanged(QString)));
  connect(_configureSource, SIGNAL(clicked()), this, SLOT(configureSource()));
  //connect(_recentFiles, SIGNAL(currentTextChanged(QString)), _url, SLOT(setFile(QString)));
  connect(_recentFiles, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(recentFileClicked(QListWidgetItem*)));
  connect(_url, SIGNAL(changed(QString)), _recentFiles, SLOT(clearSelection()));
  connect(_cleanupRecentFiles, SIGNAL(clicked(bool)), this, SLOT(cleanupRecentDataFilesClicked()));

  if (default_source.isEmpty()) {
    _url->setFile(dialogDefaults().value("vector/datasource", ".").toString());
  } else {
    _url->setFile(default_source);
  }
  _url->setFocus();

  _updateBox->addItem(tr("Time Interval", "update periodically"));
  _updateBox->addItem(tr("Change Detection", "update when a change is detected"));
  _updateBox->addItem(tr("No Update", "do not update the file"));
  updateUpdateBox();

  QStringList recentFiles = kstApp->mainWindow()->recentDataFiles();

  _recentFiles->addItems(recentFiles);

  int h = fontMetrics().lineSpacing();
  _url->setFixedHeight(h*4/3);

}


DataWizardPageDataSource::~DataWizardPageDataSource() {
}


void DataWizardPageDataSource::updateUpdateBox()
{
  if (_dataSource) {
    _updateBox->setEnabled(true);
    switch (_dataSource->updateType()) {
      case DataSource::Timer: _updateBox->setCurrentIndex(0); break;
      case DataSource::File:  _updateBox->setCurrentIndex(1); break;
      case DataSource::None:  _updateBox->setCurrentIndex(2); break;
      default:                _updateBox->setCurrentIndex(dialogDefaults().value("wizard/updateType", 0).toInt()); break;
    };
  } else {
    _updateBox->setEnabled(false);
  }
}

void DataWizardPageDataSource::setTypeActivated() {
  updateTypeActivated(_updateBox->currentIndex());
}

int DataWizardPageDataSource::updateType()
{
  return _updateBox->currentIndex();
}

void DataWizardPageDataSource::updateTypeActivated(int idx)
{
  if (!_dataSource) {
    _updateBox->setEnabled(false);
    return;
  }
  _updateBox->setEnabled(true);
  switch (idx) {
    case 0: _dataSource->startUpdating(DataSource::Timer); break;
    case 1: _dataSource->startUpdating(DataSource::File);  break;
    case 2: _dataSource->startUpdating(DataSource::None);  break;
    default: break;
  };
}

void DataWizardPageDataSource::recentFileClicked(QListWidgetItem *item)
{
  _url->setFile(item->text());
}

void DataWizardPageDataSource::cleanupRecentDataFilesClicked()
{
  kstApp->mainWindow()->cleanupRecentDataFilesList();
  QStringList recentFiles = kstApp->mainWindow()->recentDataFiles();

  _recentFiles->clear();
  _recentFiles->addItems(recentFiles);
}


bool DataWizardPageDataSource::isComplete() const {
  return _pageValid;
}


DataSourcePtr DataWizardPageDataSource::dataSource() const {

  return _dataSource;
}


QStringList DataWizardPageDataSource::dataSourceFieldList() const {
  return _dataSource->vector().list();
}


void DataWizardPageDataSource::configureSource() {
  QPointer<DataSourceConfigureDialog> dialog = new DataSourceConfigureDialog(DataDialog::New, _dataSource, this);
  if ( dialog->exec() == QDialog::Accepted ) {
    sourceChanged(_dataSource->fileName());
  }
  delete dialog;
}


void DataWizardPageDataSource::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _pageValid = true;
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename, true);
  connect(_dataSource, SIGNAL(progress(int,QString)), kstApp->mainWindow(), SLOT(updateProgress(int,QString)));
  _fileType->setText(_dataSource->fileType());  

  _dataSource->readLock();
  _configureSource->setEnabled(_dataSource->hasConfigWidget());
  _dataSource->unlock();

  {
    DataSourcePtr tmpds = _dataSource; // increase usage count
    _store->cleanUpDataSourceList();
  }

  updateUpdateBox();
  emit completeChanged();
  emit dataSourceChanged();
}


void DataWizardPageDataSource::sourceChanged(const QString& file) {
  _pageValid = false;
  _fileType->setText(QString());
  _configureSource->setEnabled(false);
  _updateBox->setEnabled(false);
  emit completeChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString,int)), this, SLOT(sourceValid(QString,int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}



//
// DataWizardPageVectors
//

DataWizardPageVectors::DataWizardPageVectors(QWidget *parent)
  : QWizardPage(parent) {
   setupUi(this);

  _up->setIcon(KstGetIcon("kst_uparrow"));
  _down->setIcon(KstGetIcon("kst_downarrow"));
  _add->setIcon(KstGetIcon("kst_rightarrow"));
  _remove->setIcon(KstGetIcon("kst_leftarrow"));
  _up->setToolTip(tr("Raise in plot order: Alt+Up"));
  _down->setToolTip(tr("Lower in plot order: Alt+Down"));
  _add->setToolTip(tr("Select: Alt+s"));
  _remove->setToolTip(tr("Remove: Alt+r"));

  connect(_add, SIGNAL(clicked()), this, SLOT(add()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(remove()));
  connect(_up, SIGNAL(clicked()), this, SLOT(up()));
  connect(_down, SIGNAL(clicked()), this, SLOT(down()));
  connect(_vectors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add()));
  connect(_vectorsToPlot, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(remove()));
  connect(_vectorReduction, SIGNAL(textChanged(QString)), this, SLOT(filterVectors(QString)));
  connect(_vectorSearch, SIGNAL(clicked()), this, SLOT(searchVectors()));

  _vectors->setSortingEnabled(false);
  _vectorsToPlot->setSortingEnabled(false);
}


DataWizardPageVectors::~DataWizardPageVectors() {
}


QListWidget* DataWizardPageVectors::plotVectors() const {
  return _vectorsToPlot;
}

void DataWizardPageVectors::updateVectors() {

  _vectors->clear();
  _vectorsToPlot->clear();

  _vectors->addItems(((DataWizard*)wizard())->dataSourceFieldList());
  _availableLabel->setText(tr("%1 vector(s) available").arg(_vectors->count()));
}


bool DataWizardPageVectors::vectorsSelected() const {
  return _vectorsToPlot->count() > 0;
}


bool DataWizardPageVectors::isComplete() const {
  return vectorsSelected();
}


void DataWizardPageVectors::remove() {
  int j=0, i=0;
  int count = _vectorsToPlot->count();
  for (i = 0; i < count; ++i) {
    if (_vectorsToPlot->item(i-j) && _vectorsToPlot->item(i-j)->isSelected()) {
      _vectors->addItem(_vectorsToPlot->takeItem(i-j));
      j += 1;
    }
  }
  _vectors->clearSelection();

  _selectedLabel->setText(tr("%1 vector(s) selected").arg(_vectorsToPlot->count()));

  emit completeChanged();
}


void DataWizardPageVectors::add() {

  for (int i = 0; i < _vectors->count(); i++) {
    if (_vectors->item(i) && _vectors->item(i)->isSelected()) {
      _vectorsToPlot->addItem(_vectors->takeItem(i));
      i--;
    }
  }

  _vectorsToPlot->setCurrentRow(_vectorsToPlot->count() - 1);
  _selectedLabel->setText(tr("%1 vector(s) selected").arg(_vectorsToPlot->count()));
  emit completeChanged();
}


void DataWizardPageVectors::up() {
  _vectorsToPlot->setFocus();

  int i = _vectorsToPlot->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _vectorsToPlot->takeItem(i);
    _vectorsToPlot->insertItem(i-1, item);
    _vectorsToPlot->clearSelection();
    _vectorsToPlot->setCurrentItem(item);
    emit completeChanged();
  }
}


void DataWizardPageVectors::down() {
  // move item down
  int i = _vectorsToPlot->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _vectorsToPlot->takeItem(i);
    _vectorsToPlot->insertItem(i+1, item);
    _vectorsToPlot->clearSelection();
    _vectorsToPlot->setCurrentItem(item);
    emit completeChanged();
  }
}


void DataWizardPageVectors::filterVectors(const QString& filter) {
  _vectors->clearSelection();

  if (filter=="*") { // optimization
    _vectors->selectAll();
    return;
  }

  QRegExp re(filter, Qt::CaseSensitive, QRegExp::Wildcard);
  QStringList selected;

  for (int i = 0; i < _vectors->count(); i++) {
    QListWidgetItem *item = _vectors->item(i);
    if (re.exactMatch(item->text())) {
      item = _vectors->takeItem(i);
      selected.append(item->text());
      i--;
    }
  }

  _vectors->insertItems(0, selected);

  // special case optimization:
  // selecting and unselecting individual items is expensive,
  // but selecting all of them is fast,
  // so either select or select all, then unselect, which ever is fewer.
  if (selected.count() > _vectors->count()/2) {
    _vectors->selectAll();
    for (int i=selected.count(); i<_vectors->count(); i++) {
      _vectors->item(i)->setSelected(false);
    }
  } else {
    for (int i=0; i<selected.count(); i++) {
      _vectors->item(i)->setSelected(true);
    }
  }

  if (selected.count()>0) {
    _vectors->scrollToTop();
  }
}


void DataWizardPageVectors::searchVectors() {
  QString s = _vectorReduction->text();
  if (!s.isEmpty()) {
    if (s[0] != '*') {
      s = '*' + s;
    }
    if (s[s.length()-1] != '*') {
      s += '*';
    }
    _vectorReduction->setText(s);
  }
}


//
// DataWizardPageFilters
//

DataWizardPageFilters::DataWizardPageFilters(QWidget *parent)
  : QWizardPage(parent) {
   setupUi(this);
}


DataWizardPageFilters::~DataWizardPageFilters() {
}


DataWizardPagePlot::DataWizardPagePlot(QWidget *parent)
  : QWizardPage(parent) {
   setupUi(this);

  connect(_customGrid, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));

  updatePlotBox();
  updateButtons();
}



//
// DataWizardPagePlot
//

DataWizardPagePlot::~DataWizardPagePlot() {
}


void DataWizardPagePlot::updateButtons() {
  _gridColumns->setEnabled(_customGrid->isChecked());
}


DataWizardPagePlot::CurvePlotPlacement DataWizardPagePlot::curvePlacement() const {
  CurvePlotPlacement placement = OnePlot;
  if (_multiplePlots->isChecked()) {
    placement = MultiplePlots;
  } else if (_cycleThrough->isChecked()) {
    placement = CyclePlotCount;
  } else if (_cycleExisting->isChecked()) {
    placement = CycleExisting;
  } else if (_existingPlot->isChecked()) {
    placement = ExistingPlot;
  }
  return placement;
}

DataWizardPagePlot::PlotTabPlacement DataWizardPagePlot::plotTabPlacement() const {
  PlotTabPlacement placement = CurrentTab;
  if (_newTab->isChecked()) {
    placement = NewTab;
  } else if (_separateTabs->isChecked()) {
    placement = SeparateTabs;
  }

  return placement;
}


CurvePlacement::Layout DataWizardPagePlot::layout() const {
  if (_autoLayout->isChecked())
    return CurvePlacement::Auto;
  else if (_customGrid->isChecked())
    return CurvePlacement::Custom;
  else
    return CurvePlacement::Protect;
}


int DataWizardPagePlot::gridColumns() const {
  return _gridColumns->value();
}


bool DataWizardPagePlot::drawLines() const {
  return _drawLines->isChecked();
}


bool DataWizardPagePlot::drawPoints() const {
  return _drawPoints->isChecked();
}


bool DataWizardPagePlot::drawLinesAndPoints() const {
  return _drawBoth->isChecked();
}


bool DataWizardPagePlot::PSDLogX() const {
  return _psdLogX->isChecked();
}


bool DataWizardPagePlot::PSDLogY() const {
  return _psdLogY->isChecked();
}


bool DataWizardPagePlot::legendsOn() const {
  return _legendsOn->isChecked();
}


bool DataWizardPagePlot::legendsAuto() const {
  return _legendsAuto->isChecked();
}

bool DataWizardPagePlot::legendsVertical() const {
  return _legendsVertical->isChecked();
}

bool DataWizardPagePlot::rescaleFonts() const {
  return _rescaleFonts->isChecked();
}

bool DataWizardPagePlot::shareAxis() const {
  return _shareAxis->isChecked();
}

int DataWizardPagePlot::plotCount() const {
  return _plotNumber->value();
}

PlotItemInterface *DataWizardPagePlot::existingPlot() const {
  return _existingPlotName->itemData(_existingPlotName->currentIndex()).value<PlotItemInterface*>();
}


void DataWizardPagePlot::updatePlotBox() {
  _plotNumber->setValue(dialogDefaults().value("wizard/plotCount",2).toInt());

  if (dialogDefaults().value("wizard/legendsAuto",true).toBool()) {
    _legendsOn->setChecked(true);
  } else if (dialogDefaults().value("wizard/legendsOn",false).toBool()) {
    _legendsAuto->setChecked(true);
  } else {
    _legendsOff->setChecked(true);
  }
  _psdLogX->setChecked(dialogDefaults().value("wizard/logX",false).toBool());
  _psdLogY->setChecked(dialogDefaults().value("wizard/logY",false).toBool());

  _legendsOn->setChecked(dialogDefaults().value("wizard/legendsOn",false).toBool());
  _legendsAuto->setChecked(dialogDefaults().value("wizard/legendsAuto",false).toBool());
  _legendsVertical->setChecked(dialogDefaults().value("legend/verticalDisplay",false).toBool());

  _rescaleFonts->setChecked(dialogDefaults().value("wizard/rescaleFonts", true).toBool());
  _shareAxis->setChecked(dialogDefaults().value("wizard/shareAxis", false).toBool());
  _shareAxis->hide(); //FIXME - not done yet.

  if (dialogDefaults().value("wizard/linesOnly", true).toBool()) {
    _drawLines->setChecked(true);
  } else if (dialogDefaults().value("wizard/pointsOnly", true).toBool()) {
    _drawPoints->setChecked(true);
  } else {
    _drawBoth->setChecked(true);
  }

  foreach (PlotItemInterface *plot, Data::self()->plotList()) {
    _existingPlotName->addItem(plot->plotName(), qVariantFromValue(plot));
  }
  bool havePlots = _existingPlotName->count() > 0;
  _cycleExisting->setEnabled(havePlots);
  _existingPlot->setEnabled(havePlots);
  _existingPlotName->setEnabled(havePlots && _existingPlot->isChecked());
  _plotLayoutBox->setEnabled(!_existingPlot->isChecked());

  CurvePlotPlacement placement = static_cast<CurvePlotPlacement>(dialogDefaults().value("wizard/curvePlacement",MultiplePlots).toInt());
  switch (placement) {
  case OnePlot:
    _onePlot->setChecked(true);
    break;
  case MultiplePlots:
    _multiplePlots->setChecked(true);
    break;
  case CyclePlotCount:
    _cycleThrough->setChecked(true);
    break;
  case CycleExisting:
    if (havePlots) {
      _cycleExisting->setChecked(true);
    } else {
      _multiplePlots->setChecked(true);
    }
    break;
  case ExistingPlot:
    if (havePlots) {
      _existingPlot->setChecked(true);
    } else {
      _onePlot->setChecked(true);
    }
    break;
  default:
    _multiplePlots->setChecked(true);
    break;
  }
  CurvePlacement::Layout layout = static_cast<CurvePlacement::Layout>(dialogDefaults().value("wizard/plotLayout", CurvePlacement::Auto).toInt());
  switch (layout) {
  case CurvePlacement::Auto:
    _autoLayout->setChecked(true);
    break;
  case CurvePlacement::Custom:
    _customGrid->setChecked(true);
    break;
  case CurvePlacement::Protect:
    _protectLayout->setChecked(true);
    break;
  }

  PlotTabPlacement tabPlacement = static_cast<DataWizardPagePlot::PlotTabPlacement>(dialogDefaults().value("wizard/plotPlacement", SeparateTabs).toInt());
  switch (tabPlacement) {
  case CurrentTab:
    _currentTab->setChecked(true);
    break;
  case NewTab:
    _newTab->setChecked(true);
    break;
  case SeparateTabs:
    _separateTabs->setChecked(true);
    break;
  }

  _gridColumns->setValue(dialogDefaults().value("wizard/gridColumns", CurvePlacement::Auto).toInt());
}



//
// DataWizardPageDataPresentation
//

DataWizardPageDataPresentation::DataWizardPageDataPresentation(ObjectStore *store, DataWizard *parent)
  : QWizardPage(parent), _pageValid(false), _dw(parent) {
   setupUi(this);

  _xVectorExisting->setObjectStore(store);
  _xVectorExisting->setToLastX(dialogDefaults().value("curve/xvectorfield","INDEX").toString());
  _xAxisUseExisting->setChecked(_xVectorExisting->count()>0);
  dataRange()->loadWidgetDefaults();
  getFFTOptions()->loadWidgetDefaults();

  connect(_xAxisCreateFromField, SIGNAL(toggled(bool)), this, SLOT(optionsUpdated()));
  connect(_xVector, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsUpdated()));
  connect(_xVectorExisting, SIGNAL(selectionChanged(QString)), this, SLOT(optionsUpdated()));

  connect(_xVectorExisting, SIGNAL(selectionChanged(QString)), this, SLOT(checkWarningLabel()));
  connect(_xAxisCreateFromField, SIGNAL(toggled(bool)), this, SLOT(checkWarningLabel()));
  connect(_DataRange, SIGNAL(modified()), this, SLOT(checkWarningLabel()));
  connect(_DataRange, SIGNAL(modified()), this, SIGNAL(completeChanged()));
  connect(_xAxisGroup, SIGNAL(toggled(bool)), this, SLOT(checkWarningLabel()));

  _FFTOptions->GroupBoxFFTOptions->setCheckable(true);
  _FFTOptions->GroupBoxFFTOptions->setTitle(tr("Create S&pectra Plots. Set FFT options below:"));

  _FFTOptions->GroupBoxFFTOptions->setChecked(dialogDefaults().value("wizard/doPSD",false).toBool());
  _xAxisGroup->setChecked(dialogDefaults().value("wizard/doXY",true).toBool());

  checkWarningLabel();
}


DataWizardPageDataPresentation::~DataWizardPageDataPresentation() {
}


FFTOptions* DataWizardPageDataPresentation::getFFTOptions() const {
  return _FFTOptions;
}


DataRange* DataWizardPageDataPresentation::dataRange() const {
  return _DataRange;
}



bool DataWizardPageDataPresentation::createXAxisFromField() const {
  return _xAxisCreateFromField->isChecked();
}


QString DataWizardPageDataPresentation::vectorField() const {
  return _xVector->currentText();
}


VectorPtr DataWizardPageDataPresentation::selectedVector() const {
  return _xVectorExisting->selectedVector();
}


bool DataWizardPageDataPresentation::plotPSD() const {
  return _FFTOptions->GroupBoxFFTOptions->isChecked();
}


bool DataWizardPageDataPresentation::plotData() const {
  return _xAxisGroup->isChecked();
}


bool DataWizardPageDataPresentation::plotDataPSD() const {
  return (plotPSD() && plotData());
}


void DataWizardPageDataPresentation::optionsUpdated() {
  _pageValid = validOptions();
  emit completeChanged();
}

void DataWizardPageDataPresentation::checkWarningLabel()
{
  bool warn = false;
  if (_xAxisGroup->isChecked() &&
      _xAxisUseExisting->isChecked()) {
    DataVectorPtr xv = kst_cast<DataVector>(_xVectorExisting->selectedVector());
    if (xv) {
      // do the number of requested frames match?
      if (xv->readToEOF()) {
        if (!_DataRange->readToEnd()) {
          warn = true;
        }
      } else {
        if (xv->reqNumFrames() != _DataRange->range() ||
            _DataRange->readToEnd()) {
          warn = true;
        }
      }
      // does the starting frame match?
      if (xv->countFromEOF()) {
        if (!_DataRange->countFromEnd()) {
          warn = true;
        }
      } else {
        if (xv->reqStartFrame() != _DataRange->start() ||
            (_DataRange->countFromEnd())) {
          warn = true;
        }
      }
      if (warn) {
        _xAxisWarningLabel->setText(tr("Warning: the data range of the existing X vector does not match the Y vectors."));
      } else {
        if (_dw->_pageDataSource->dataSource()) {
          if (_dw->_pageDataSource->dataSource()->fileName() != xv->filename()) {
            warn = true;
            _xAxisWarningLabel->setText(tr("Warning: the file name of the existing X vector does not match the Y vectors."));
          }
        }
      }
    } else if (_xVectorExisting->selectedVector()) {
      _xAxisWarningLabel->setText(tr("Warning: the selected X vector may not match the data range of the Y vectors."));
      warn = true;
    }
  }
  _xAxisWarningLabel->setVisible(warn);
  _XAxisWarningLabel2->setVisible(warn);
}


void DataWizardPageDataPresentation::updateVectors() {
  _xVector->clear();
  _xVector->addItems(((DataWizard*)wizard())->dataSourceFieldList());
  _pageValid = validOptions();

  int x_index = _xVector->findText(dialogDefaults().value("curve/xvectorfield","INDEX").toString());
  if (x_index<0) {
    x_index = _xVector->findText("INDEX");
  }
  if (x_index<0) {
    x_index = 0;
  }
  _xVector->setCurrentIndex(x_index);

  dataRange()->updateIndexList(((DataWizard*)wizard())->dataSourceIndexList());

  emit completeChanged();
}


void DataWizardPageDataPresentation::applyFilter(bool filter) {
  emit filterApplied(filter);
}


bool DataWizardPageDataPresentation::isComplete() const {
  return _DataRange->rangeIsValid() && _pageValid;
}


bool DataWizardPageDataPresentation::validOptions() {
  if (!_xAxisGroup->isEnabled()) {
    return true;
  }

  if (_xAxisCreateFromField->isChecked()) {
    QString txt = _xVector->currentText();
    for (int i = 0; i < _xVector->count(); ++i) {
      if (_xVector->itemText(i) == txt) {
        return true;
      }
    }
    return false;
  } else {
    return (_xVectorExisting->selectedVector());
  }
}



int DataWizardPageDataPresentation::nextId() const {
//  if (_applyFilters->isChecked()) {
//    return DataWizard::PageFilters;
//  } else {
    return DataWizard::PagePlot;
//  }
}



//
// DataWizard
//

DataWizard::DataWizard(QWidget *parent, const QString& fileToOpen)
  : QWizard(parent), _document(0) {

  MainWindow *mw = qobject_cast<MainWindow*>(parent);
  if (!mw) {
    // we need a document
    // not sure that this can ever happen.
    qFatal("ERROR: can't construct a DataWizard without a document");
    return;
  }

  _document = mw->document();
  Q_ASSERT(_document);

  _pageDataSource = new DataWizardPageDataSource(_document->objectStore(), this, fileToOpen);
  connect(_pageDataSource, SIGNAL(progress(int,QString)), mw, SLOT(updateProgress(int,QString)));
  _pageVectors = new DataWizardPageVectors(this);
  _pageDataPresentation = new DataWizardPageDataPresentation(_document->objectStore(), this);
  _pageFilters = new DataWizardPageFilters(this);
  _pagePlot = new DataWizardPagePlot(this);

  setPage(PageDataSource, _pageDataSource);
  setPage(PageVectors, _pageVectors);
  setPage(PageDataPresentation, _pageDataPresentation);
  setPage(PageFilters, _pageFilters);
  setPage(PagePlot, _pagePlot);

  setWindowTitle("Data Wizard");
  setAttribute(Qt::WA_DeleteOnClose);

  show();

  connect(_pageDataSource, SIGNAL(dataSourceChanged()), _pageVectors, SLOT(updateVectors()));
  connect(_pageDataSource, SIGNAL(dataSourceChanged()), _pageDataPresentation, SLOT(updateVectors()));
  connect(_pageDataSource, SIGNAL(dataSourceChanged()), _pageDataPresentation, SLOT(checkWarningLabel()));
  connect(_pageDataSource, SIGNAL(destroyed()), kstApp->mainWindow(), SLOT(cleanUpDataSourceList()));
  disconnect(button(QWizard::FinishButton), SIGNAL(clicked()), (QDialog*)this, SLOT(accept()));
  connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SLOT(finished()));


  // the dialog needs to know that the default has been set....
  _pageDataSource->sourceChanged(dialogDefaults().value("vector/datasource",".").toString());

  if (!fileToOpen.isEmpty()) {
    _pageDataSource->sourceChanged(fileToOpen);
  }

}


DataWizard::~DataWizard() {
}


QStringList DataWizard::dataSourceFieldList() const {
  return _pageDataSource->dataSourceFieldList();
}

QStringList DataWizard::dataSourceIndexList() const {
  return _pageDataSource->dataSource()->indexFields();
}


void DataWizard::finished() {
  DataVectorList vectors;
  uint n_curves = 0;
  uint n_steps = 0;

  dialogDefaults().setValue("wizard/updateType", _pageDataSource->updateType());
  dialogDefaults().setValue("wizard/doPSD", _pageDataPresentation->plotPSD());
  dialogDefaults().setValue("wizard/doXY", _pageDataPresentation->plotData());
  dialogDefaults().setValue("wizard/curvePlacement", _pagePlot->curvePlacement());
  dialogDefaults().setValue("wizard/plotPlacement", _pagePlot->plotTabPlacement());
  dialogDefaults().setValue("wizard/plotCount", _pagePlot->plotCount());

  dialogDefaults().setValue("wizard/legendsOn", _pagePlot->legendsOn());
  dialogDefaults().setValue("wizard/legendsAuto", _pagePlot->legendsAuto());
  dialogDefaults().setValue("legend/verticalDisplay", _pagePlot->legendsVertical());
  dialogDefaults().setValue("wizard/logX", _pagePlot->PSDLogX());
  dialogDefaults().setValue("wizard/logY", _pagePlot->PSDLogY());

  dialogDefaults().setValue("wizard/rescaleFonts", _pagePlot->rescaleFonts());
  dialogDefaults().setValue("wizard/shareAxis", _pagePlot->shareAxis());

  dialogDefaults().setValue("wizard/linesOnly", _pagePlot->drawLines());
  dialogDefaults().setValue("wizard/pointsOnly", _pagePlot->drawPoints());
  dialogDefaults().setValue("wizard/plotLayout", _pagePlot->layout());
  dialogDefaults().setValue("wizard/gridColumns", _pagePlot->gridColumns());
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  DataSourcePtr ds = _pageDataSource->dataSource();

  if (!ds.isPtrValid()) {
    return;
  }

  ds->enableUpdates();

  emit dataSourceLoaded(ds->fileName());

  // check for sufficient memory
  double memoryRequested = 0;
  double memoryAvailable = Data::AvailableMemory();
  double frames;

  ds->writeLock();

  double startOffset = _pageDataPresentation->dataRange()->start();
  double rangeCount = _pageDataPresentation->dataRange()->range();

  bool customStartIndex = (_pageDataPresentation->dataRange()->_startUnits->currentIndex() != 0) &&
                          (!_pageDataPresentation->dataRange()->countFromEnd());
  bool customRangeCount = (_pageDataPresentation->dataRange()->_rangeUnits->currentIndex() != 0) &&
                          (!_pageDataPresentation->dataRange()->readToEnd());

  if (customStartIndex) {
    startOffset = ds->indexToFrame(_pageDataPresentation->dataRange()->start(), _pageDataPresentation->dataRange()->startUnits());
  }

  if (customRangeCount) {
    rangeCount = _pageDataPresentation->dataRange()->range()*ds->framePerIndex(_pageDataPresentation->dataRange()->rangeUnits());
  }

  bool separate_tabs =
      ((_pagePlot->plotTabPlacement() == DataWizardPagePlot::SeparateTabs) && _pageDataPresentation->plotPSD()
       && _pageDataPresentation->plotData());

  // only add to memory requirement if xVector is to be created
  if (_pageDataPresentation->createXAxisFromField()) {
    if (_pageDataPresentation->dataRange()->readToEnd()) {
      frames = ds->vector().dataInfo(_pageDataPresentation->vectorField()).frameCount - startOffset;
    } else {
      frames = qMin(rangeCount,double(ds->vector().dataInfo(_pageDataPresentation->vectorField()).frameCount));
    }

    if (_pageDataPresentation->dataRange()->doSkip() && _pageDataPresentation->dataRange()->skip() > 0) {
      memoryRequested += double(frames) / _pageDataPresentation->dataRange()->skip() * sizeof(double);
    } else {
      memoryRequested += double(frames) * ds->vector().dataInfo(_pageDataPresentation->vectorField()).samplesPerFrame * sizeof(double);
    }
  }

  // memory estimate for the y vectors
  {
    int fftLen = int(pow(2.0, double(_pageDataPresentation->getFFTOptions()->FFTLength() - 1)));
    ds->vector().prepareRead(_pageVectors->plotVectors()->count());
    for (int i = 0; i < _pageVectors->plotVectors()->count(); i++) {
      QString field = _pageVectors->plotVectors()->item(i)->text();

      if (_pageDataPresentation->dataRange()->readToEnd()) {
        frames = ds->vector().dataInfo(field).frameCount - startOffset;
      } else {
        frames = rangeCount;
        int fc = ds->vector().dataInfo(field).frameCount;
        if (frames > (unsigned long) fc) {
          frames = fc;
        }
      }

      if (_pageDataPresentation->dataRange()->doSkip() && _pageDataPresentation->dataRange()->skip() > 0) {
        memoryRequested += double(frames) / _pageDataPresentation->dataRange()->skip()*sizeof(double);
      } else {
        memoryRequested += double(frames) * ds->vector().dataInfo(field).samplesPerFrame * sizeof(double);
      }
      if (_pageDataPresentation->plotPSD()) {
        memoryRequested += fftLen * 6.0;
      }
    }
  }


  ds->unlock();
  if (memoryRequested > memoryAvailable) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, tr("Insufficient Memory"), tr("You requested to read in %1 MB of data but it seems that you only have approximately %2 MB of usable memory available.  You cannot load this much data.").arg(memoryRequested/(1024*1024)).arg(memoryAvailable/(1024*1024)));
    return;
  }

  n_steps += _pageVectors->plotVectors()->count();
  if (_pageDataPresentation->plotPSD()) {
    n_steps += _pageVectors->plotVectors()->count();
  }

  VectorPtr xv;
  // only create x vector if needed
  if (_pageDataPresentation->createXAxisFromField()) {
    n_steps += 1; // for the creation of the x-vector

    const QString field = _pageDataPresentation->vectorField();

    dialogDefaults().setValue("curve/xvectorfield",field);
    Q_ASSERT(_document && _document->objectStore());

    DataVectorPtr dxv = _document->objectStore()->createObject<DataVector>();

    dxv->writeLock();
    dxv->change(ds, field,
        _pageDataPresentation->dataRange()->countFromEnd() ? -1 : startOffset,
        _pageDataPresentation->dataRange()->readToEnd() ? -1 : rangeCount,
        _pageDataPresentation->dataRange()->skip(),
        _pageDataPresentation->dataRange()->doSkip(),
        _pageDataPresentation->dataRange()->doFilter());

    if (customStartIndex) {
      dxv->setStartUnits(_pageDataPresentation->dataRange()->_startUnits->currentText());
    }

    if (customRangeCount) {
      dxv->setRangeUnits(_pageDataPresentation->dataRange()->_rangeUnits->currentText());
    }

    dxv->registerChange();
    dxv->unlock();
    xv = dxv;
  } else {
    xv = kst_cast<Vector>(_pageDataPresentation->selectedVector());
  }

  bool xAxisIsTime = xv->isTime();

  {
    DataVectorPtr vector;
    for (int i = 0; i < _pageVectors->plotVectors()->count(); i++) {
      QString field = _pageVectors->plotVectors()->item(i)->text();

      Q_ASSERT(_document && _document->objectStore());

      vector = _document->objectStore()->createObject<DataVector>();

      vector->writeLock();
      vector->change(ds, field,
          _pageDataPresentation->dataRange()->countFromEnd() ? -1 : startOffset,
          _pageDataPresentation->dataRange()->readToEnd() ? -1 : rangeCount,
          _pageDataPresentation->dataRange()->skip(),
          _pageDataPresentation->dataRange()->doSkip(),
          _pageDataPresentation->dataRange()->doFilter());

      if (customStartIndex) {
        vector->setStartUnits(_pageDataPresentation->dataRange()->_startUnits->currentText());
      }

      if (customRangeCount) {
        vector->setRangeUnits(_pageDataPresentation->dataRange()->_rangeUnits->currentText());
      }

      vector->registerChange();
      vector->unlock();

      vectors.append(vector);
      ++n_curves;
    }
    if (n_curves>0) {
      _pageDataPresentation->dataRange()->setWidgetDefaults();
      setDataVectorDefaults(vector);
    }
  }

  // Create a new tab, if we asked for it and the current tab isn't empty.
  if ((_pagePlot->plotTabPlacement() == DataWizardPagePlot::NewTab) || separate_tabs) {
//      (_pagePlot->plotTabPlacement() == DataWizardPagePlot::SeparateTabs)) {
    if (_document->currentView()->scene()->items().count()>0) {
     _document->createView();
   }
  }

  if (_pageDataPresentation->plotPSD() || _pageDataPresentation->plotData()) {
    // create the necessary plots
    QList<PlotItem*> plotList;
    PlotItem *plotItem = 0;
    bool relayout = true;
    int plotsInPage = _document->currentView()->scene()->items().count();

    switch (_pagePlot->curvePlacement()) {
    case DataWizardPagePlot::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_pagePlot->existingPlot());
      plotList.append(plotItem);
      relayout = false;
      break;
    }
    case DataWizardPagePlot::OnePlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      plotList.append(plotItem);
      if (_pageDataPresentation->plotDataPSD()) {
        if (separate_tabs) {
          _document->createView();
        }
        CreatePlotForCurve *cmd = new CreatePlotForCurve();
        cmd->createItem();

        plotItem = static_cast<PlotItem*>(cmd->item());
        plotList.append(plotItem);
      }
      break;
    }
    case DataWizardPagePlot::MultiplePlots:
    {
      int nplots = vectors.count() * (_pageDataPresentation->plotPSD() + _pageDataPresentation->plotData());

      if (separate_tabs)
        nplots/=2;

      for (int i = 0; i < nplots; ++i) {
        CreatePlotForCurve *cmd = new CreatePlotForCurve();
        cmd->createItem();

        plotItem = static_cast<PlotItem*>(cmd->item());
        plotList.append(plotItem);
      }
      if (separate_tabs) {
        _document->createView();
        for (int i = 0; i < nplots; ++i) {
          CreatePlotForCurve *cmd = new CreatePlotForCurve();
          cmd->createItem();

          plotItem = static_cast<PlotItem*>(cmd->item());
          plotList.append(plotItem);
        }
      }

      break;
    }
    case DataWizardPagePlot::CycleExisting:
    {
      foreach (PlotItemInterface *plot, Data::self()->plotList()) {
        plotItem = static_cast<PlotItem*>(plot);
        plotList.append(plotItem);
      }
      relayout = false;
      break;
    }
    case DataWizardPagePlot::CyclePlotCount:
    {
      int nplots = _pagePlot->plotCount() * (_pageDataPresentation->plotPSD() + _pageDataPresentation->plotData());
      for (int i = 0; i < nplots; ++i) {
        CreatePlotForCurve *cmd = new CreatePlotForCurve();
        cmd->createItem();

        plotItem = static_cast<PlotItem*>(cmd->item());
        plotList.append(plotItem);
      }
    }
    default:
      break;
    }

    // create the data curves
    QList<QColor> colors;
    QColor color;
    int ptype = 0;
    int i_plot = 0;
    for (DataVectorList::Iterator it = vectors.begin(); it != vectors.end(); ++it) {
      if (_pageDataPresentation->plotData()) {
        color = ColorSequence::self().next();
        colors.append(color);

        DataVectorPtr vector = kst_cast<DataVector>(*it);
        Q_ASSERT(vector);

        Q_ASSERT(_document && _document->objectStore());
        CurvePtr curve = _document->objectStore()->createObject<Curve>();

        curve->setXVector(xv);
        curve->setYVector(vector);
        curve->setXError(0);
        curve->setYError(0);
        curve->setXMinusError(0);
        curve->setYMinusError(0);
        curve->setColor(color);
        curve->setHasPoints(_pagePlot->drawLinesAndPoints() || _pagePlot->drawPoints());
        curve->setHasLines(_pagePlot->drawLinesAndPoints() || _pagePlot->drawLines());
        curve->setLineWidth(dialogDefaults().value("curves/lineWidth",0).toInt());
        curve->setPointSize(dialogDefaults().value("curves/pointSize",CURVE_DEFAULT_POINT_SIZE).toDouble());
        curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

        curve->writeLock();
        curve->registerChange();
        curve->unlock();

        Q_ASSERT(plotList[i_plot]);

        PlotRenderItem *renderItem = plotList[i_plot]->renderItem(PlotRenderItem::Cartesian);
        renderItem->addRelation(kst_cast<Relation>(curve));

        plotList[i_plot]->xAxis()->setAxisInterpret(xAxisIsTime);

        // increment i_plot, as appropriate;
        if (_pagePlot->curvePlacement() != DataWizardPagePlot::OnePlot) {
          ++i_plot;
          if (_pagePlot->curvePlacement()==DataWizardPagePlot::CyclePlotCount) {
            if (i_plot == _pagePlot->plotCount()) {
              i_plot = 0;
            }
          } else if (i_plot == plotList.count()) {
            i_plot = 0;
          }
        }
      }
    }

    if (_pagePlot->curvePlacement() == DataWizardPagePlot::OnePlot) {
      // if we are one plot, now we can move to the psd plot
      if (++i_plot == plotList.count()) {
        i_plot = 0;
      }
    } else if (_pageDataPresentation->plotDataPSD()) {
      i_plot = plotList.count()/2;
    }

    // create the PSDs
    if (_pageDataPresentation->plotPSD()) {
      int indexColor = 0;
      ptype = 0;

      PSDPtr powerspectrum;
      int n_psd=0;

      for (DataVectorList::Iterator it = vectors.begin(); it != vectors.end(); ++it) {
        if ((*it)->length() > 0) {

          Q_ASSERT(_document && _document->objectStore());
          powerspectrum = _document->objectStore()->createObject<PSD>();
          n_psd++;
          Q_ASSERT(powerspectrum);

          powerspectrum->writeLock();

          powerspectrum->change(*it,
                                _pageDataPresentation->getFFTOptions()->sampleRate(),
                                _pageDataPresentation->getFFTOptions()->interleavedAverage(),
                                _pageDataPresentation->getFFTOptions()->FFTLength(),
                                _pageDataPresentation->getFFTOptions()->apodize(),
                                _pageDataPresentation->getFFTOptions()->removeMean(),
                                _pageDataPresentation->getFFTOptions()->vectorUnits(),
                                _pageDataPresentation->getFFTOptions()->rateUnits(),
                                _pageDataPresentation->getFFTOptions()->apodizeFunction(),
                                _pageDataPresentation->getFFTOptions()->sigma(),
                                _pageDataPresentation->getFFTOptions()->output());

          powerspectrum->registerChange();
          powerspectrum->unlock();

          CurvePtr curve = _document->objectStore()->createObject<Curve>();
          Q_ASSERT(curve);

          curve->setXVector(powerspectrum->vX());
          curve->setYVector(powerspectrum->vY());
          curve->setHasPoints(_pagePlot->drawLinesAndPoints() || _pagePlot->drawPoints());
          curve->setHasLines(_pagePlot->drawLinesAndPoints() || _pagePlot->drawLines());
          curve->setLineWidth(dialogDefaults().value("curves/lineWidth",0).toInt());
          curve->setPointSize(dialogDefaults().value("curves/pointSize",CURVE_DEFAULT_POINT_SIZE).toDouble());
          curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

          if (!_pageDataPresentation->plotDataPSD() || colors.count() <= indexColor) {
            color = ColorSequence::self().next();
          } else {
            color = colors[indexColor];
            indexColor++;
          }
          curve->setColor(color);

          curve->writeLock();
          curve->registerChange();
          curve->unlock();

          Q_ASSERT(plotList[i_plot]);

          PlotRenderItem *renderItem = plotList[i_plot]->renderItem(PlotRenderItem::Cartesian);
          plotList[i_plot]->xAxis()->setAxisLog(_pagePlot->PSDLogX());
          plotList[i_plot]->yAxis()->setAxisLog(_pagePlot->PSDLogY());
          renderItem->addRelation(kst_cast<Relation>(curve));

          if (_pagePlot->curvePlacement() != DataWizardPagePlot::OnePlot) {
            // change plots if we are not onePlot
            if (++i_plot == plotList.count()) {
              if (_pageDataPresentation->plotDataPSD()) { // if xy and psd
                i_plot = plotList.count()/2;
              } else {
                i_plot = 0;;
              }
            }
          }
        }
      }

      if (n_psd>0) {
        _pageDataPresentation->getFFTOptions()->setWidgetDefaults();
      }
    }

    if (relayout && !plotList.isEmpty()) {
      if (plotsInPage==0 || _pagePlot->rescaleFonts()) {
        int np = plotList.count();
        int n_add = np;
        bool two_pages = (plotList.at(np-1)->view() != plotList.at(0)->view());
        if (two_pages) {
          n_add/=2;
        }
        if (np > 0) { // don't crash if there are no plots
          plotList.at(0)->view()->resetPlotFontSizes(plotList.mid(0, n_add)); // set font sizes on first page.
          if (two_pages) { // and second, if there is one.
            plotList.at(np-1)->view()->resetPlotFontSizes(plotList.mid(n_add, n_add));
          }
        }
      }
      foreach (PlotItem* plot, plotList) {
        plot->view()->configurePlotFontDefaults(plot); // copy plots already in window
      }

      CurvePlacement::Layout layout_type = _pagePlot->layout();
      int num_columns = _pagePlot->gridColumns();
      if (plotsInPage == 0) { // no format to protext
        if (layout_type != CurvePlacement::Custom) {
          layout_type = CurvePlacement::Custom;
          if (separate_tabs) {
            //        if (_pagePlot->plotTabPlacement() == DataWizardPagePlot::SeparateTabs) {
            num_columns = sqrt((double)plotList.size()/2);
          } else {
            num_columns = sqrt((double)plotList.size());
          }
        }
      }
      foreach (PlotItem* plot, plotList) {
        if (xAxisIsTime) {
          // For ASCII, we can get the time/date format string from the datasource config
          DataSourcePtr ds = _pageDataSource->dataSource();
          if (ds->typeString() == "ASCII file") {
            if (!ds->timeFormat().isEmpty()) { // Only set it when we use a specific ASCII format
              plot->xAxis()->setAxisDisplayFormatString(ds->timeFormat());
              plot->xAxis()->setAxisDisplay(AXIS_DISPLAY_QTDATETIME_FORMAT);
            }
          }
        }
        plot->update();
        plot->view()->appendToLayout(layout_type, plot, num_columns);
      }
      if (!plotList.isEmpty() && layout_type == CurvePlacement::Custom) {
        plotList.at(0)->createCustomLayout(num_columns);
        if (_pageDataPresentation->plotDataPSD()) {
          plotList.at(plotList.count()/2)->createCustomLayout(num_columns);
        }
      }

    }
    foreach (PlotItem* plot, plotList) {
      if (_pagePlot->legendsOn()) {
        plot->setShowLegend(true, true);
        plot->legend()->setVerticalDisplay(_pagePlot->legendsVertical());
      } else if (_pagePlot->legendsAuto()) {
        if (plot->renderItem(PlotRenderItem::Cartesian)->relationList().count() > 1) {
          plot->setShowLegend(true, true);
          plot->legend()->setVerticalDisplay(_pagePlot->legendsVertical());
        }
      } else {
        plot->setShowLegend(false,false);
      }
    }

    if (_pagePlot->shareAxis()) {
      //FIXME: apply shared axis
      // also delete the line _shareAxis->hide();
    }
  }

  UpdateManager::self()->doUpdates(true);

  kstApp->mainWindow()->document()->setChanged(true);
  QApplication::restoreOverrideCursor();
  accept();

  UpdateServer::self()->requestUpdateSignal();

  _pageDataSource->setTypeActivated();
  ds->vector().readingDone();
}

}

// vim: ts=2 sw=2 et
