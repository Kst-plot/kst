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

#include "changedatasampledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "datamatrix.h"
#include "vscalar.h"
#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "dialogdefaults.h"
#include "updatemanager.h"
#include "updateserver.h"
#include "datarangeconversion.h"

#include <QMessageBox>

#include <limits>

namespace Kst {

ChangeDataSampleDialog::ChangeDataSampleDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  MainWindow::setWidgetFlags(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
    // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChangeDataSampleDialog without the object store");
  }

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(addAll()));

  connect(_vectorList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(availableDoubleClicked(QListWidgetItem*)));
  connect(_selectedVectorList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(selectedDoubleClicked(QListWidgetItem*)));

  connect(_vectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_selectedVectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_dataRange, SIGNAL(modified()), this, SLOT(modified()));

  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(UpdateServer::self(), SIGNAL(objectListsChanged()), this, SLOT(updateCurveListDialog()));

  initializeEntries();
  updateButtons();
  updateIndexEntries();

}


ChangeDataSampleDialog::~ChangeDataSampleDialog() {
}


void ChangeDataSampleDialog::show() {
  _rangeSourceName.clear();
  updateCurveListDialog();
  initializeEntries();
  _rangeSourceName.clear();
  updateIndexEntries();
  updateButtons();
  QDialog::show();
}


void ChangeDataSampleDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, _selectedVectorList->selectedItems()) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
  }

  _vectorList->clearSelection();
  updateButtons();
  updateIndexEntries();

}


void ChangeDataSampleDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
    _vectorList->clearSelection();
    updateButtons();
    updateIndexEntries();
  }
}


void ChangeDataSampleDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, _vectorList->selectedItems()) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
  }
  _selectedVectorList->clearSelection();
  updateButtons();
  updateIndexEntries();
}


void ChangeDataSampleDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
    _selectedVectorList->clearSelection();
    updateButtons();
    updateIndexEntries();
  }
}


void ChangeDataSampleDialog::updateIndexEntries() {

  const QString preferredStartUnits = _dataRange->startUnits();
  const QString preferredRangeUnits = _dataRange->rangeUnits();
  QListWidgetItem *firstTarget = _selectedVectorList->item(0);
  const QString newRangeSourceName = firstTarget ? firstTarget->text() : QString();
  DataSourcePtr newRangeSourceDataSource;
  QString newRangeSourceField;
  if (firstTarget) {
    if (DataVectorPtr vector =
            kst_cast<DataVector>(_store->retrieveObject(firstTarget->text()))) {
      newRangeSourceDataSource = vector->dataSource();
      newRangeSourceField = vector->field();
    }
  }
  const bool rangeSourceChanged =
      _rangeSourceName != newRangeSourceName ||
      _rangeSourceDataSource != newRangeSourceDataSource ||
      _rangeSourceField != newRangeSourceField;

  // Make a list of all the data sources for all of the vectors which have been selected.
  QList<DataSourcePtr> data_sources;

  _selectedVectorList->blockSignals(true);
  _selectedVectorList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedVectorList->selectedItems();
  _selectedVectorList->clearSelection();
  _selectedVectorList->blockSignals(false);

  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      if (!data_sources.contains(vector->dataSource())) {
        data_sources.append(vector->dataSource());
      }
    }
  }

  QList<IndexFieldProperties> index_fields;

  // make a list of fields which are provided by all data sources...
  if (data_sources.size()>0) {
    foreach (const IndexFieldProperties &ifp, data_sources[0]->indexFieldProperties()) {
      bool in_all = true;
      for (int i=1; i<data_sources.size(); i++) {
        bool found = false;
        for (const IndexFieldProperties &other : data_sources[i]->indexFieldProperties()) {
          if (other.name == ifp.name) { found = true; break; }
        }
        in_all = in_all && found;
      }
      if (in_all) {
        index_fields.append(ifp);
      }
    }
  }

  DataSourcePtr displayDataSource;
  if (firstTarget) {
    if (DataVectorPtr vector =
            kst_cast<DataVector>(_store->retrieveObject(firstTarget->text()))) {
      displayDataSource = vector->dataSource();
    }
  }
  _dataRange->setDataSource(displayDataSource);
  _dataRange->setReferenceField(newRangeSourceField);
  _dataRange->setStartUnits(preferredStartUnits);
  _dataRange->setRangeUnits(preferredRangeUnits);
  _dataRange->updateIndexList(index_fields);

  if (rangeSourceChanged) {
    _rangeSourceName = newRangeSourceName;
    _rangeSourceDataSource = newRangeSourceDataSource;
    _rangeSourceField = newRangeSourceField;
    if (firstTarget) {
      updateDefaults(firstTarget);
    } else {
      _dataRange->clearValues();
    }
  }
  updateButtons();
}


void ChangeDataSampleDialog::updateButtons() {
  bool valid = (_selectedVectorList->count() > 0) && (_dataRange->rangeIsValid());
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
  _add->setEnabled(_vectorList->selectedItems().count() > 0);
  _addAll->setEnabled(_vectorList->count() > 0);
  _remove->setEnabled(_selectedVectorList->selectedItems().count() > 0);
  _removeAll->setEnabled(_selectedVectorList->count() > 0);
}


void ChangeDataSampleDialog::modified() {
  updateButtons();
}

void ChangeDataSampleDialog::updateCurveListDialog() {

  PrimitiveList dataVectors = _store->getFramePrimitives();
  _vectorList->blockSignals(true);

  _vectorList->clearSelection();
  _selectedVectorList->clearSelection();

  // make sure all items in _vectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _vectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _vectorList->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // make sure all items in _selectedVectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _selectedVectorList->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // insert into _vectorList all items in store not in one of the lists.
  for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
    bool listed = false;
    for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        _selectedVectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<_vectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        _vectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(dataVectors.at(i_vector)->Name());
      _vectorList->addItem(wi);
      wi->setToolTip(dataVectors.at(i_vector)->descriptionTip());
    }
  }
  _vectorList->blockSignals(false);
  updateIndexEntries();
}


void ChangeDataSampleDialog::addAll() {
  _vectorList->selectAll();
  addButtonClicked();
}


void ChangeDataSampleDialog::removeAll() {
  _selectedVectorList->selectAll();
  removeButtonClicked();
}


void ChangeDataSampleDialog::initializeEntries() {
  _dataRange->clearValues();
  _dataRange->setCountFromEnd(dialogDefaults().value("vector/countFromEnd",false).toBool());
  _dataRange->setReadToEnd(dialogDefaults().value("vector/readToEnd",true).toBool());
  _dataRange->setSkip(dialogDefaults().value("vector/skip", 0).toInt());
  _dataRange->setDoSkip(dialogDefaults().value("vector/doSkip", false).toBool());
  _dataRange->setDoFilter(dialogDefaults().value("vector/doAve",false).toBool());
  _dataRange->setStartUnits(dialogDefaults().value("vector/startUnits",tr("frames")).toString());
  _dataRange->setRangeUnits(dialogDefaults().value("vector/rangeUnits",tr("frames")).toString());
}


void ChangeDataSampleDialog::updateDefaults(QListWidgetItem* item) {
  if (!item) {
    return;
  }

  if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(item->text()))) {
    vector->readLock();

    const bool countFromEnd = vector->reqCountFromEnd();
    const bool readToEnd = vector->reqReadToEnd();
    const qint64 startFrame = DataRangeConversion::floorFrame(vector->reqStartFrame());
    const qint64 frameCount = DataRangeConversion::floorFrame(vector->reqNumFrames());
    _dataRange->setSkip(vector->skip());
    _dataRange->setDoSkip(vector->doSkip());
    _dataRange->setDoFilter(vector->doAve());

    vector->unlock();

    _dataRange->setCountFromEnd(countFromEnd);
    _dataRange->setReadToEnd(readToEnd);
    if (countFromEnd) {
      _dataRange->setStart(0);
      if (!_dataRange->setCountFromEndFrameCount(frameCount)) {
        QString frameUnits = QStringLiteral("frames");
        for (const IndexFieldProperties &properties :
             _dataRange->indexFieldProperties()) {
          if (properties.is_frame) {
            frameUnits = properties.name;
            break;
          }
        }
        _dataRange->setRangeUnits(frameUnits);
        if (!_dataRange->setCountFromEndFrameCount(frameCount)) {
          _dataRange->setRange(frameCount);
        }
      }
    } else if (readToEnd) {
      _dataRange->setStart(startFrame);
      _dataRange->setRange(0);
    } else if (frameCount > 0 &&
               startFrame <= std::numeric_limits<qint64>::max() - frameCount) {
      if (!_dataRange->setFrameRange(startFrame, startFrame + frameCount)) {
        QString frameUnits = QStringLiteral("frames");
        for (const IndexFieldProperties &properties :
             _dataRange->indexFieldProperties()) {
          if (properties.is_frame) {
            frameUnits = properties.name;
            break;
          }
        }
        _dataRange->setStartUnits(frameUnits);
        _dataRange->setRangeUnits(frameUnits);
        if (!_dataRange->setFrameRange(startFrame, startFrame + frameCount)) {
          _dataRange->setStart(startFrame);
          _dataRange->setRange(frameCount);
        }
      }
    } else {
      _dataRange->setStart(startFrame);
      _dataRange->setRange(frameCount);
    }
  }
}


void ChangeDataSampleDialog::OKClicked() {
  apply();
  accept();
}


void ChangeDataSampleDialog::apply() {
  _selectedVectorList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedVectorList->selectedItems();

  QString start_units;
  QString range_units;
  QHash<QString, double> f0_map;
  QHash<QString, double> r_map;
  QHash<QString, bool> countFromEnd_map;
  QHash<QString, bool> readToEnd_map;
  bool custom_start_index = (!_dataRange->startIsFrame()) && (!_dataRange->countFromEnd());

  bool custom_range_index = (!_dataRange->rangeIsFrame()) && (!_dataRange->readToEnd());
  if (!custom_start_index && !custom_range_index) { // FIXME: also for custom index.
    start_units.clear();

    // see if we have enough memory
    //FIXME: doesn't consider data objects that depend on this, and it should
    //FIXME: doesn't work under windows or mac
    double current_memory_used = 0.0;
    double memory_needed = 0.0;
    for (int i = 0; i < selectedItems.size(); ++i) {
      if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
        current_memory_used += double(vector->length())*sizeof(double);
        long ns=0;
        if (_dataRange->readToEnd()) {
          ns = vector->fileLength() - (int)_dataRange->start();
        } else {
          ns = (int)_dataRange->range();
        }
        if (_dataRange->doSkip()) {
          ns/=_dataRange->skip();
        } else {
          ns *= vector->samplesPerFrame();
        }
        memory_needed += double(ns)*sizeof(double);
      }
    }

    double memory_available = Data::AvailableMemory();
    if (memory_needed-current_memory_used > memory_available) {
      //QApplication::restoreOverrideCursor();
      QMessageBox::warning(this, tr("Insufficient Memory"), tr("You requested to read in %1 MB of data but it seems that you only have approximately %2 MB of usable memory available.  You cannot load this much data."
                                                                   ).arg((memory_needed-current_memory_used)/(1024*1024)).arg(memory_available/(1024*1024)));
      return;
    }
  }

  start_units = _dataRange->startUnits();
  range_units = _dataRange->rangeUnits();

  for (int i = 0; i < selectedItems.size(); ++i) {
    QString filename;
    DataSourcePtr datasource;
    QString field;
    bool valid = false;
    bool isVector = false;
    bool use_custom_start_index = false;
    bool use_custom_range_index = false;
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      filename = vector->filename();
      datasource = vector->dataSource();
      field = vector->field();
      valid = true;
      isVector = true;
      use_custom_range_index = custom_range_index;
      use_custom_start_index = custom_start_index;
    } else if (DataMatrixPtr matrix = kst_cast<DataMatrix>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      filename = matrix->filename();
      datasource = matrix->dataSource();
      valid = true;
    } else if (VScalarPtr vscalar = kst_cast<VScalar>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      filename = vscalar->filename();
      datasource = vscalar->dataSource();
      valid = true;
    }

    if (valid) {
      if (isVector) {
        continue;
      }
      if (!f0_map.contains(filename)) {
        double f0;
        double r;
        bool cfe = false; // countFromEnd
        bool rte = false; // readToEnd

        bool converted = false;

        if (isVector && (use_custom_start_index || use_custom_range_index)) {
          double startOffset = _dataRange->start();
          double rangeCount = _dataRange->range();
          converted = DataRangeConversion::resolveToFrameRange(_dataRange,
                                                               datasource,
                                                               field,
                                                               &startOffset,
                                                               &rangeCount,
                                                               0,
                                                               0);
          if (converted) {
            f0 = _dataRange->countFromEnd() ? -1 : startOffset;
            r = _dataRange->readToEnd() ? -1 : rangeCount;
            cfe = _dataRange->countFromEnd();
            rte = _dataRange->readToEnd();
          }
        }


        if (!converted) {
          if (use_custom_start_index) {

            f0 = datasource->indexToFrame(_dataRange->start(), start_units);
          } else if (_dataRange->countFromEnd()) {
            f0 = -1; // keep -1 for matrix/vscalar backward compat
            cfe = true;
          } else {
            f0 = _dataRange->start();
          }
          if (use_custom_range_index) {
            r = _dataRange->range()*datasource->framePerIndex(range_units);
          } else if (_dataRange->readToEnd()) {
            r = -1; // keep -1 for matrix/vscalar backward compat
            rte = true;
          } else {
            r = _dataRange->range();
          }
        }

        f0_map.insert(filename, f0);
        r_map.insert(filename, r);
        countFromEnd_map.insert(filename, cfe);
        readToEnd_map.insert(filename, rte);
      }
    }
  }

  bool anyChanges = false;
  QStringList conversionFailures;
  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
          vector->readLock();
          const QString field = vector->field();
          const double currentStartValue = vector->reqStartFrame();
          const double currentCountValue = vector->reqNumFrames();
          const bool currentCountFromEnd = vector->reqCountFromEnd();
          const bool currentReadToEnd = vector->reqReadToEnd();
          const bool currentDoSkip = vector->doSkip();
          const int currentSkip = vector->skip();
          const bool currentDoAve = vector->doAve();
          vector->unlock();

      double startOffset = _dataRange->start();
      double rangeCount = _dataRange->range();
      if (!DataRangeConversion::resolveToFrameRange(
            _dataRange, vector->dataSource(), field,
              &startOffset, &rangeCount, nullptr, nullptr)) {
        conversionFailures.append(vector->Name());
        continue;
      }

      const bool countFromEnd = _dataRange->countFromEnd();
      const bool readToEnd = _dataRange->readToEnd();
      qint64 desiredStart = DataRangeConversion::floorFrame(startOffset);
      qint64 desiredCount = DataRangeConversion::floorFrame(rangeCount);

      const qint64 availableFrames =
          DataRangeConversion::floorFrame(vector->fileLength());
      if (availableFrames > 0 && !readToEnd) {
        if (countFromEnd) {
          desiredCount = qMin(desiredCount, availableFrames);
        } else {
          desiredStart = qMin(desiredStart, availableFrames - 1);
          desiredCount = qMin(desiredCount, availableFrames - desiredStart);
        }
        if (availableFrames >= 2 && desiredCount < 2) {
          desiredCount = qMin(qint64(2), countFromEnd
                                                ? availableFrames
                                                : availableFrames - desiredStart);
        }
      } else if (!readToEnd) {
        desiredCount = 0;
      }

      const qint64 currentStart =
          DataRangeConversion::floorFrame(currentStartValue);
      const qint64 currentCount =
          DataRangeConversion::floorFrame(currentCountValue);
      const bool desiredSkip = _dataRange->doSkip();
      const int desiredSkipCount = desiredSkip ? qMax(1, _dataRange->skip()) : 0;
      const bool unchanged =
          (countFromEnd || currentStart == desiredStart) &&
          (readToEnd || currentCount == desiredCount) &&
          currentCountFromEnd == countFromEnd &&
          currentReadToEnd == readToEnd &&
          currentDoSkip == desiredSkip &&
          currentSkip == desiredSkipCount &&
          currentDoAve == _dataRange->doFilter();
      if (unchanged) {
        continue;
      }

      vector->writeLock();
      vector->changeFrames(countFromEnd ? 0 : desiredStart,
                            countFromEnd,
                            readToEnd ? 0 : desiredCount,
                            readToEnd,
                            _dataRange->skip(),
                            _dataRange->doSkip(),
                            _dataRange->doFilter());
      vector->setStartUnits(start_units);
      vector->setRangeUnits(range_units);
      vector->unlock();
      anyChanges = true;
    } else if (DataMatrixPtr matrix = kst_cast<DataMatrix>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      matrix->writeLock();
      double from = f0_map.value(matrix->filename());
      double range = r_map.value(matrix->filename());
      if (from < 0) {
        matrix->setFrame(-1);
      } else {
        matrix->setFrame((int)(from+range-1));
      }
      matrix->setStartUnits(start_units);
      matrix->setRangeUnits(range_units);
      matrix->registerChange();
      matrix->unlock();
      anyChanges = true;
    } else if (VScalarPtr vscalar = kst_cast<VScalar>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      vscalar->writeLock();
      double from = f0_map.value(vscalar->filename());
      double range = r_map.value(vscalar->filename());
      vscalar->changeFrame((int)(from+range-1));
      //vscalar->setStartUnits(start_units);
      //vscalar->setRangeUnits(range_units);
      vscalar->registerChange();
      vscalar->unlock();
      anyChanges = true;
    }
  }

  if (anyChanges) {
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();
  }

  if (!conversionFailures.isEmpty()) {
    QMessageBox::warning(
        this, tr("Unable to Convert Range"),
        tr("The selected index range could not be converted for these vectors; they were left unchanged:\n%1")
            .arg(conversionFailures.join(QLatin1Char('\n'))));
  }

  dialogDefaults().setValue("vector/range", _dataRange->range());
  dialogDefaults().setValue("vector/start", _dataRange->start());
  dialogDefaults().setValue("vector/countFromEnd", _dataRange->countFromEnd());
  dialogDefaults().setValue("vector/readToEnd", _dataRange->readToEnd());
  dialogDefaults().setValue("vector/skip", _dataRange->skip());
  dialogDefaults().setValue("vector/doSkip", _dataRange->doSkip());
  dialogDefaults().setValue("vector/doAve", _dataRange->doFilter());
  dialogDefaults().setValue("vector/startUnits", _dataRange->startUnits());
  dialogDefaults().setValue("vector/rangeUnits", _dataRange->rangeUnits());

  updateCurveListDialog();
  if (anyChanges) {
    kstApp->mainWindow()->document()->setChanged(true);
  }
}


}

// vim: ts=2 sw=2 et
