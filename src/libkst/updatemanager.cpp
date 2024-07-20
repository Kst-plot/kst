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

#include "updatemanager.h"

//#include "primitive.h"
#include "datasource.h"
#include "objectstore.h"
//#include "measuretime.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#define DEFAULT_MIN_UPDATE_PERIOD 2000

namespace Kst {

static UpdateManager *_self = 0;
void UpdateManager::cleanup() {
  delete _self;
  _self = 0;
}


UpdateManager *UpdateManager::self() {
  if (!_self) {
    _self = new UpdateManager;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


UpdateManager::UpdateManager() {
  _serial = 0;
  _minUpdatePeriod = DEFAULT_MIN_UPDATE_PERIOD;
  _paused = false;
  _store = 0;
  _delayedUpdateScheduled = false;
  _updateInProgress = false;
  _time.start();
}


UpdateManager::~UpdateManager() {
}

void UpdateManager::delayedUpdates() {
  _delayedUpdateScheduled = false;
  doUpdates();
}

void UpdateManager::doUpdates(bool forceImmediate) {
  if (_delayedUpdateScheduled && !forceImmediate) {
    return;
  }

  if (!_store) {
    return;
  }

  //FIXME: should we just skip updating data sources in this case?
  if (_paused && !forceImmediate) {
    return;
  }

  int dT = _time.elapsed();
  if (((dT<_minUpdatePeriod) || (_updateInProgress)) && (!forceImmediate)) {
    if (!_delayedUpdateScheduled) {
      _delayedUpdateScheduled = true;
      int deferTime = _minUpdatePeriod-dT;
      if (deferTime <= 0) {
        deferTime = 20; // if an update is already in progress, wait this long to check again.
      }
      QTimer::singleShot(deferTime, this, SLOT(delayedUpdates()));
    }
    return;
  }
  _updateInProgress = true;
  _time.restart();

  _serial++;

  int n_updated=0, n_deferred=0, n_unchanged = 0;
  qint64 retval = 0;

  // update the datasources
  foreach (DataSourcePtr ds, _store->dataSourceList()) {
    ds->writeLock();
    ds->objectUpdate(_serial);
    ds->unlock();
  }

  // We are going to check all data objects regardless of whether
  // files have been changed, because the necessity of an update
  // may have been triggered by something else.

  //MeasureTime t(" UpdateManager::doUpdates loop");

  int i_loop = 0;
  int maxloop = _store->objectList().size();
  do {
    n_updated = n_unchanged = n_deferred = 0;
    // update data objects
    foreach (ObjectPtr p, _store->objectList()) {
      p->writeLock();
      retval = p->objectUpdate(_serial);
      p->unlock();

      if (retval == Object::Updated) {
        n_updated++;
      } else if (retval == Object::Deferred) {
        n_deferred++;
      } else if (retval == Object::NoChange) {
        n_unchanged++;
      }
    }
    maxloop = qMin(maxloop,n_deferred);
    i_loop++;
  } while ((n_deferred + n_updated > 0) && (i_loop<=maxloop));

  if (forceImmediate) {
    foreach(DataSourcePtr ds, _store->dataSourceList()) {
        ds->vector().readingDone();
    }
  }

  emit objectsUpdated(_serial);
}
}

// vim: ts=2 sw=2 et
