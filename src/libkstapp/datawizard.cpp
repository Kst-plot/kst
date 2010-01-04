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

#include "datawizard.h"

#include <QFileInfo>
#include <QMessageBox>
#include <psversion.h>
#include <sysinfo.h>
#include <QThreadPool>

#include "colorsequence.h"
#include "curve.h"
#include "datacollection.h"
#include "datasourcedialog.h"
#include "datavector.h"
#include "dialogdefaults.h"
#include "document.h"
#include "mainwindow.h"
#include "objectstore.h"
#include "plotitem.h"
#include "plotiteminterface.h"
#include "settings.h"
#include "applicationsettings.h"
#include "updatemanager.h"

namespace Kst {

DataWizardPageDataSource::DataWizardPageDataSource(ObjectStore *store, QWidget *parent)
  : QWizardPage(parent), _pageValid(false), _store(store), _requestID(0) {
   setupUi(this);

   connect(_url, SIGNAL(changed(const QString&)), this, SLOT(sourceChanged(const QString&)));
   connect(_configureSource, SIGNAL(clicked()), this, SLOT(configureSource()));

   QString default_source = _dialogDefaults->value("vector/datasource",".").toString();
  _url->setFile(default_source);
  _url->setFocus();
}


DataWizardPageDataSource::~DataWizardPageDataSource() {
}


bool DataWizardPageDataSource::isComplete() const {
  return _pageValid;
}


DataSourcePtr DataWizardPageDataSource::dataSource() const {

  return _dataSource;
}


QStringList DataWizardPageDataSource::dataSourceFieldList() const {
  return _dataSource->fieldList();
}


void DataWizardPageDataSource::configureSource() {
  DataSourceDialog dialog(DataDialog::New, _dataSource, this);
  dialog.exec();
  sourceChanged(_dataSource->fileName());
}


void DataWizardPageDataSource::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _pageValid = true;

  _dataSource = DataSource::findOrLoadSource(_store, filename);
  _fileType->setText(_dataSource->fileType());

  _dataSource->readLock();
  _configureSource->setEnabled(_dataSource->hasConfigWidget());
  _dataSource->unlock();

  {
    DataSourcePtr tmpds = _dataSource; // increase usage count
    _store->cleanUpDataSourceList();
  }

  emit completeChanged();
  emit dataSourceChanged();
}


void DataWizardPageDataSource::sourceChanged(const QString& file) {
  _pageValid = false;
  _fileType->setText(QString());
  _configureSource->setEnabled(false);
  emit completeChanged();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


DataWizardPageVectors::DataWizardPageVectors(QWidget *parent)
  : QWizardPage(parent) {
   setupUi(this);

  _up->setIcon(QPixmap(":kst_uparrow.png"));
  _down->setIcon(QPixmap(":kst_downarrow.png"));
  _add->setIcon(QPixmap(":kst_rightarrow.png"));
  _remove->setIcon(QPixmap(":kst_leftarrow.png"));
  _up->setToolTip(i18n("Raise in plot order: Alt+Up"));
  _down->setToolTip(i18n("Lower in plot order: Alt+Down"));
  _add->setToolTip(i18n("Select: Alt+s"));
  _remove->setToolTip(i18n("Remove: Alt+r"));

  connect(_add, SIGNAL(clicked()), this, SLOT(add()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(remove()));
  connect(_up, SIGNAL(clicked()), this, SLOT(up()));
  connect(_down, SIGNAL(clicked()), this, SLOT(down()));
  connect(_vectors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(add()));
  connect(_vectorsToPlot, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(remove()));
  connect(_vectorReduction, SIGNAL(textChanged(const QString&)), this, SLOT(filterVectors(const QString&)));
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
}


bool DataWizardPageVectors::vectorsSelected() const {
  return _vectorsToPlot->count() > 0;
}


bool DataWizardPageVectors::isComplete() const {
  return vectorsSelected();
}


void DataWizardPageVectors::remove() {
  int j=0;
  for (int i = 0; i < _vectorsToPlot->count(); i++) {
    if (_vectorsToPlot->item(i) && _vectorsToPlot->item(i)->isSelected()) {
      _vectors->addItem(_vectorsToPlot->takeItem(i));
      j = i;
    }
  }
  if (j>=_vectorsToPlot->count()) {
    j = _vectorsToPlot->count()-1;
  }
  _vectorsToPlot->setFocus();
  _vectorsToPlot->setCurrentRow(j);
  _vectors->clearSelection();

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
  for (int i=0; i<selected.count(); i++) {
    _vectors->item(i)->setSelected(true);
  }
}


void DataWizardPageVectors::searchVectors() {
  QString s = _vectorReduction->text();
  if (!s.isEmpty()) {
    if (s[0] != '*') {
      s = "*" + s;
    }
    if (s[s.length()-1] != '*') {
      s += "*";
    }
    _vectorReduction->setText(s);
  }
}


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


bool DataWizardPagePlot::xAxisLabels() const {
  return _xAxisLabels->isChecked();
}


bool DataWizardPagePlot::yAxisLabels() const {
  return _yAxisLabels->isChecked();
}


bool DataWizardPagePlot::legendsOn() const {
  return _legendsOn->isChecked();
}


bool DataWizardPagePlot::legendsAuto() const {
  return _legendsAuto->isChecked();
}


int DataWizardPagePlot::plotCount() const {
  return _plotNumber->value();
}

PlotItemInterface *DataWizardPagePlot::existingPlot() const {
  return qVariantValue<PlotItemInterface*>(_existingPlotName->itemData(_existingPlotName->currentIndex()));
}


void DataWizardPagePlot::updatePlotBox() {
  foreach (PlotItemInterface *plot, Data::self()->plotList()) {
    _existingPlotName->addItem(plot->plotName(), qVariantFromValue(plot));
  }

  bool havePlots = _existingPlotName->count() > 0;
  _cycleExisting->setEnabled(havePlots);
  _existingPlot->setEnabled(havePlots);
  _existingPlotName->setEnabled(havePlots && _existingPlot->isChecked());
}


DataWizardPageDataPresentation::DataWizardPageDataPresentation(ObjectStore *store, QWidget *parent)
  : QWizardPage(parent), _pageValid(false) {
   setupUi(this);

  _xVectorExisting->setObjectStore(store);
  _xVectorExisting->setToLastX();
  _xAxisUseExisting->setChecked(_xVectorExisting->count()>0);
  dataRange()->loadWidgetDefaults();
  getFFTOptions()->loadWidgetDefaults();

  connect(_xAxisCreateFromField, SIGNAL(toggled(bool)), this, SLOT(optionsUpdated()));
  connect(_xVector, SIGNAL(currentIndexChanged(int)), this, SLOT(optionsUpdated()));
  connect(_xVectorExisting, SIGNAL(selectionChanged(QString)), this, SLOT(optionsUpdated()));

  _FFTOptions->GroupBoxFFTOptions->setCheckable(true);
  _FFTOptions->GroupBoxFFTOptions->setTitle(i18n("Create S&pectra Plots.  FFT Options:"));
  _FFTOptions->GroupBoxFFTOptions->setChecked(false); // fixme: use persistant defaults
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


void DataWizardPageDataPresentation::updateVectors() {
  _xVector->clear();
  _xVector->addItems(((DataWizard*)wizard())->dataSourceFieldList());
  _pageValid = validOptions();

  int x_index = _xVector->findText(_dialogDefaults->value("curve/xvectorfield","INDEX").toString());
  if (x_index<0) {
    x_index = _xVector->findText("INDEX");
  }
  if (x_index<0) {
    x_index = 0;
  }
  _xVector->setCurrentIndex(x_index);

  emit completeChanged();
}


void DataWizardPageDataPresentation::applyFilter(bool filter) {
  emit filterApplied(filter);
}


bool DataWizardPageDataPresentation::isComplete() const {
  return _pageValid;
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


DataWizard::DataWizard(QWidget *parent)
  : QWizard(parent), _document(0) {

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _document = mw->document();
  } else {
    // FIXME: we need a document
    qFatal("ERROR: can't construct a DataWizard without a document");
  }

  Q_ASSERT(_document);
  _pageDataSource = new DataWizardPageDataSource(_document->objectStore(), this);
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
  disconnect(button(QWizard::FinishButton), SIGNAL(clicked()), (QDialog*)this, SLOT(accept()));
  connect(button(QWizard::FinishButton), SIGNAL(clicked()), this, SLOT(finished()));

  // the dialog needs to know that the default has been set....
  _pageDataSource->sourceChanged(_dialogDefaults->value("vector/datasource",".").toString());

}


DataWizard::~DataWizard() {
}


QStringList DataWizard::dataSourceFieldList() const {
  return _pageDataSource->dataSourceFieldList();
}


void DataWizard::finished() {
  DataVectorList vectors;
  uint n_curves = 0;
  uint n_steps = 0;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  DataSourcePtr ds = _pageDataSource->dataSource();

  // check for sufficient memory
  unsigned long memoryRequested = 0, memoryAvailable = 1024*1024*1024; // 1GB
  double frames;
#ifdef __linux__
  meminfo();
  memoryAvailable = S(kb_main_free + kb_main_buffers + kb_main_cached);
#endif

  ds->writeLock();

  double startOffset = _pageDataPresentation->dataRange()->start();
  double rangeCount = _pageDataPresentation->dataRange()->range();
  // only add to memory requirement if xVector is to be created
  if (_pageDataPresentation->createXAxisFromField()) {
    if (_pageDataPresentation->dataRange()->readToEnd()) {
      frames = ds->frameCount(_pageDataPresentation->vectorField()) - startOffset;
    } else {
      frames = qMin(rangeCount,double(ds->frameCount(_pageDataPresentation->vectorField())));
    }

    if (_pageDataPresentation->dataRange()->doSkip() && _pageDataPresentation->dataRange()->skip() > 0) {
      memoryRequested += frames / _pageDataPresentation->dataRange()->skip() * sizeof(double);
    } else {
      memoryRequested += frames * ds->samplesPerFrame(_pageDataPresentation->vectorField())*sizeof(double);
    }
  }

  // memory estimate for the y vectors
  {
    int fftLen = int(pow(2.0, double(_pageDataPresentation->getFFTOptions()->FFTLength() - 1)));
    for (int i = 0; i < _pageVectors->plotVectors()->count(); i++) {
      QString field = _pageVectors->plotVectors()->item(i)->text();

      if (_pageDataPresentation->dataRange()->readToEnd()) {
        frames = ds->frameCount(field) - startOffset;
      } else {
        frames = rangeCount;
        if (frames > (unsigned long)ds->frameCount(field)) {
          frames = ds->frameCount();
        }
      }

      if (_pageDataPresentation->dataRange()->doSkip() && _pageDataPresentation->dataRange()->skip() > 0) {
        memoryRequested += frames / _pageDataPresentation->dataRange()->skip()*sizeof(double);
      } else {
        memoryRequested += frames * ds->samplesPerFrame(field)*sizeof(double);
      }
      if (_pageDataPresentation->plotPSD()) {
        memoryRequested += fftLen * 6;
      }
    }
  }

  ds->unlock();
  if (memoryRequested > memoryAvailable) {
    QApplication::restoreOverrideCursor();
    QMessageBox::warning(this, i18n("Insufficient Memory"), i18n("You requested to read in %1 MB of data but it seems that you only have approximately %2 MB of usable memory available.  You cannot load this much data.").arg(memoryRequested/(1024*1024)).arg(memoryAvailable/(1024*1024)));
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

    _dialogDefaults->setValue("curve/xvectorfield",field);
    Q_ASSERT(_document && _document->objectStore());

    DataVectorPtr dxv = _document->objectStore()->createObject<DataVector>();

    dxv->writeLock();
    dxv->change(ds, field,
        _pageDataPresentation->dataRange()->countFromEnd() ? -1 : startOffset,
        _pageDataPresentation->dataRange()->readToEnd() ? -1 : rangeCount,
        _pageDataPresentation->dataRange()->skip(),
        _pageDataPresentation->dataRange()->doSkip(),
        _pageDataPresentation->dataRange()->doFilter());

    dxv->registerChange();
    dxv->unlock();
    xv = dxv;

  } else {
    xv = kst_cast<Vector>(_pageDataPresentation->selectedVector());
  }

  // only create create the y-vectors
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
  if ((_pagePlot->plotTabPlacement() == DataWizardPagePlot::NewTab) ||
      (_pagePlot->plotTabPlacement() == DataWizardPagePlot::SeparateTabs)) {
    if (_document->currentView()->scene()->items().count()>0) {
     _document->createView();
   }
  }


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
        CreatePlotForCurve *cmd = new CreatePlotForCurve();
        cmd->createItem();

        plotItem = static_cast<PlotItem*>(cmd->item());
        plotList.append(plotItem);
      }
      break;
    }
    case DataWizardPagePlot::MultiplePlots:
    {
      bool separate_plots =
          ((_pagePlot->plotTabPlacement() == DataWizardPagePlot::SeparateTabs) && _pageDataPresentation->plotPSD()
           && _pageDataPresentation->plotData());

      int nplots = vectors.count() * (_pageDataPresentation->plotPSD() + _pageDataPresentation->plotData());

      if (separate_plots)
        nplots/=2;

      for (int i = 0; i < nplots; ++i) {
        CreatePlotForCurve *cmd = new CreatePlotForCurve();
        cmd->createItem();

        plotItem = static_cast<PlotItem*>(cmd->item());
        plotList.append(plotItem);
      }
      if (separate_plots) {
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
      color = ColorSequence::next();
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
      curve->setLineWidth(Settings::globalSettings()->defaultLineWeight);
      curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

      curve->writeLock();
      curve->registerChange();
      curve->unlock();

      Q_ASSERT(plotList[i_plot]);

      PlotRenderItem *renderItem = plotList[i_plot]->renderItem(PlotRenderItem::Cartesian);
      renderItem->addRelation(kst_cast<Relation>(curve));

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
                              _pageDataPresentation->getFFTOptions()->output(),
                              _pageDataPresentation->getFFTOptions()->interpolateOverHoles());

        powerspectrum->registerChange();
        powerspectrum->unlock();

        CurvePtr curve = _document->objectStore()->createObject<Curve>();
        Q_ASSERT(curve);

        curve->setXVector(powerspectrum->vX());
        curve->setYVector(powerspectrum->vY());
        curve->setHasPoints(_pagePlot->drawLinesAndPoints() || _pagePlot->drawPoints());
        curve->setHasLines(_pagePlot->drawLinesAndPoints() || _pagePlot->drawLines());
        curve->setLineWidth(Settings::globalSettings()->defaultLineWeight);
        curve->setPointType(ptype++ % KSTPOINT_MAXTYPE);

        if (!_pageDataPresentation->plotDataPSD() || colors.count() <= indexColor) {
          color = ColorSequence::next();
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

  // legends and labels
  bool xLabels = _pagePlot->xAxisLabels();
  bool yLabels = _pagePlot->yAxisLabels();

  //double fontScale = ApplicationSettings::self()->referenceFontSize()/sqrt((double) plotList.count()) -
  //                   ApplicationSettings::self()->referenceFontSize()+
  //                   ApplicationSettings::self()->referenceFontSize()/3;

  double fontScale;
  if (plotsInPage==0) {
    plotsInPage = plotList.count();
    if (plotsInPage==0) plotsInPage = 1;
    fontScale = ApplicationSettings::self()->referenceFontSize()/sqrt((double)plotsInPage)-
                       ApplicationSettings::self()->referenceFontSize() +
                       _dialogDefaults->value("plot/globalFontScale",0.0).toDouble();
    foreach (PlotItem* plot, plotList) {
      plot->setGlobalFontScale(fontScale);
      plot->leftLabelDetails()->setFontScale(fontScale);
      plot->rightLabelDetails()->setFontScale(fontScale);
      plot->topLabelDetails()->setFontScale(fontScale);
      plot->bottomLabelDetails()->setFontScale(fontScale);
      plot->numberLabelDetails()->setFontScale(fontScale);
    }
  } else {
    foreach (PlotItem* plot, plotList) {
      _document->currentView()->configurePlotFontDefaults(plot); // copy plots already in window
    }
  }

  foreach (PlotItem* plot, plotList) {
    if (!xLabels) {
      plot->leftLabelDetails()->setText(QString(" "));
      plot->rightLabelDetails()->setText(QString(" "));
    }
    if (!yLabels) {
      plot->topLabelDetails()->setText(QString(" "));
      plot->bottomLabelDetails()->setText(QString(" "));
    }

    plot->update();
    plot->parentView()->appendToLayout(_pagePlot->layout(), plot, _pagePlot->gridColumns());

  }

  fontScale = ApplicationSettings::self()->referenceFontSize()/sqrt((double) plotsInPage)-
                     ApplicationSettings::self()->referenceFontSize() +
                     _dialogDefaults->value("legend/fontScale",0.0).toDouble();

  foreach (PlotItem* plot, plotList) {
    if (_pagePlot->legendsOn()) {
      plot->setShowLegend(true);
      plot->legend()->setFontScale(fontScale);
    } else if (_pagePlot->legendsAuto()) {
      if (plot->renderItem(PlotRenderItem::Cartesian)->relationList().count() > 1) {
        plot->setShowLegend(true);
        plot->legend()->setFontScale(fontScale);
      }
    }
  }

  UpdateManager::self()->doUpdates(true);
  kstApp->mainWindow()->document()->setChanged(true);
  QApplication::restoreOverrideCursor();
  accept();

}

}

// vim: ts=2 sw=2 et
