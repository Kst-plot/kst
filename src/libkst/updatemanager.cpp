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

#include "../libkstapp/tabwidget.h"
#include "updatemanager.h"

#include "primitive.h"
#include "datasource.h"
#include "objectstore.h"
//#include "application.h"
//#include "document.h"

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#define MAX_UPDATES 2000

#define BENCHMARK 0

#if BENCHMARK
  QTime bench_time, benchtmp;
  int b1 = 0, b2 = 0;
#endif

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
  _maxUpdate = MAX_UPDATES;
  _paused = false;
  _store = 0;
  _delayedUpdateScheduled = false;
  _time.start();
}


UpdateManager::~UpdateManager() {
}

void UpdateManager::delayedUpdates() {
  _delayedUpdateScheduled = false;
  doUpdates();
}

void UpdateManager::doUpdates(bool forceImmediate) {
  Q_UNUSED(forceImmediate)

  if (!_store) {
    return;
  }

  if (_paused && !forceImmediate) {
    return;
  }

  int dT = _time.elapsed();
  if ((dT<_maxUpdate) && (!forceImmediate)) {
    if (!_delayedUpdateScheduled) {
      _delayedUpdateScheduled = true;
      QTimer::singleShot(_maxUpdate-dT, this, SLOT(delayedUpdates()));
    }
    return;
  }
  _time.restart();

  _serial++;

  int n_updated=0, n_deferred=0, n_unchanged = 0;
  qint64 retval;

  // update the datasources
  foreach (DataSourcePtr ds, _store->dataSourceList()) {
    ds->writeLock();
    retval = ds->objectUpdate(_serial);
    ds->unlock();
    if (retval == Object::Updated) n_updated++;
    else if (retval == Object::Deferred) n_deferred++;
    else if (retval == Object::NoChange) n_unchanged++;
  }

  //qDebug() << "ds up: " << n_updated << "  ds def: " << n_deferred << " n_no: " << n_unchanged;

  int i_loop = retval = 0;
  int maxloop = _store->objectList().size();
  //qDebug() << "starting update loop.  Maxloop: " << maxloop;
  do {
    n_updated = n_unchanged = n_deferred = 0;
    // update data objects
    foreach (ObjectPtr p, _store->objectList()) {
      p->writeLock();
      retval = p->objectUpdate(_serial);
      p->unlock();

      if (retval == Object::Updated) n_updated++;
      else if (retval == Object::Deferred) n_deferred++;
      else if (retval == Object::NoChange) n_unchanged++;
      maxloop = qMin(maxloop,n_deferred);
    }
    //qDebug() << "loop: " << i_loop << " obj up: " << n_updated << "  obj def: " << n_deferred << " obj_no: " << n_unchanged;
    i_loop++;
  } while ((retval>0) && (i_loop<=maxloop));

  emit objectsUpdated(_serial);
}
}

// vim: ts=2 sw=2 et
