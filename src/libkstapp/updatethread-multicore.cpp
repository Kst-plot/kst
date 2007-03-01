/***************************************************************************
                              updatethread.cpp
                              ----------------
    begin                : Dec 29 2003
    copyright            : (C) 2003 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "updatethread.h"

#include <assert.h>

#include <q3deepcopy.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <qdebug.h>

#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"
#include "kstdoc.h"
#include "kstrvector.h"
#include "kstvcurve.h"
#include "threadevents.h"

#define THREADJOBS 3

// 0 - none, 1 - some, 2 - lots, 3 - too much
#define UPDATEDEBUG 1

UpdateThread::UpdateThread(KstDoc *doc)
: QThread(), _paused(false), _done(false), _statusMutex(false), _doc(doc) {

  // Update variables
  _updateCounter = 0;
  _force = false;
  _jobCount = 0;
}


UpdateThread::~UpdateThread() {
}


class UpdateJob : public QThread {
  public:
    UpdateJob(UpdateThread *t) : QThread(), _t(t) {
    }

    virtual ~UpdateJob() {
    }

  protected:
    virtual void run() {
      for (KstObjectPtr o;;) {
        UpdateThread *t = _t;
        if (!t || t->finished()) {
          break;
        }

        if (!o) {
          o = t->dequeueUpdate();
        }

        if (o) {
          t->jobIsRunning();
          o->writeLock();
          KstObject::UpdateType rc = o->update(t->_updateCounter);
          o->unlock();
          if (rc == KstObject::UPDATE) {
            t->_updateResult = rc;
          }
          t->jobIsDone();
        }

        o = 0L;

        if (t->finished()) {
          break;
        }

        o = t->dequeueUpdate();

        if (o) {
          continue;
        }

        t->_queueCondition.wait();
      }
      // FIXME need to signify to the update thread that we're dead
    }

    friend class UpdateThread;
    UpdateThread *_t;
};


void UpdateThread::run() {
  bool force;
  int updateTime;

  _done = false;

  Q3ValueList<UpdateJob *> jobs;

  for (int i = 0; i < THREADJOBS; ++i) {
    UpdateJob *j = new UpdateJob(this);
    jobs.append(j);
    j->start();
  }

  while (!_done) {
    _statusMutex.lock();
    updateTime = _updateTime;
    _statusMutex.unlock();

    if (_waitCondition.wait(_updateTime)) {
#if UPDATEDEBUG > 0
      qDebug() << "Update timer " << _updateTime << endl;
#endif
      if (!_force) {
        break;
      }
    }

    _statusMutex.lock();
    if (_done) {
      _statusMutex.unlock();
      break;
    }
    force = _force;
    _force = false;
    _statusMutex.unlock();

    if (_paused && !force) {
#if UPDATEDEBUG > 0
      qDebug() << "Update thread paused..." << endl;
#endif
      continue;
    }

    bool gotData = false;
    if (doUpdates(force, &gotData) && !_done) {
#if UPDATEDEBUG > 1
      qDebug() << "Update resulted in: TRUE!" << endl;
#endif
      if (gotData) {
        qDebug() << "Posting UpdateDataDialogs" << endl;
        ThreadEvent *e = new ThreadEvent(ThreadEvent::UpdateDataDialogs);
        e->_curves = _updatedCurves;
        e->_counter = _updateCounter;
        QApplication::postEvent(_doc, e);
        // this event also triggers an implicit repaint
      } else {
        QApplication::postEvent(_doc, new ThreadEvent(ThreadEvent::Repaint));
      }
      // Wait for UI thread to finish events.  If we don't wait
      // 1: the UI thread could get flooded with events
      // 2: the update thread could change vectors during a paint, causing
      //    inconsistent curves to be plotted ie, the X vector updated
      //    and the Y vector not yet updated...

      // Race warning: Syncronization of updating() is not assured,
      // but updating() will always return a valid answer which was
      // true 'close' to when we asked.  This will safely keep the
      // update thread from over filling the UI thread.  The usleeps
      // will hopefully give the UI thread a chance to set itself...

      usleep(1000); // 1 ms on 2.6 kernel. 10ms on 2.4 kernel

      while (!_done && _doc->updating()) {  // wait for the UI to finish old events
        usleep(1000);
      }
      usleep(1000);
      // check again... not strictly needed given implicit repaint below,
      // but it should just return false, so no harm done.
      while (!_done && _doc->updating()) {
        usleep(1000);
      }
    } else {
      QApplication::postEvent(_doc, new ThreadEvent(ThreadEvent::NoUpdate));
    }
  }

  _done = true; // should be redundant
  for (Q3ValueList<UpdateJob*>::Iterator i = jobs.begin(); i != jobs.end(); ++i) {
    (*i)->_t = 0L;
  }
  _queueCondition.wakeAll();
  usleep(1000);
  QApplication::postEvent(_doc, new ThreadEvent(ThreadEvent::Done));
}


bool UpdateThread::doUpdates(bool force, bool *gotData) {
  _updatedCurves.clear(); // HACK

  if (gotData) {
    *gotData = false;
  }

#if UPDATEDEBUG > 0
  if (force) {
    qDebug() << "Forced update!" << endl;
  }
#endif

  if (++_updateCounter < 1) {
    _updateCounter = 1; // check for wrap around
  }

#if UPDATEDEBUG > 2
  qDebug() << "UPDATE: counter=" << _updateCounter << endl;
#endif

  startNewUpdate();

  {
    // Must make a copy to avoid deadlock
    KstBaseCurveList cl;
    KstDataObjectList dol;
    kstObjectSplitList<KstDataObject, KstBaseCurve>(KST::dataObjectList, cl, dol);
    qSort(cl); 
    qSort(dol); 

    // Update all curves
    for (uint i = 0; i < cl.count(); ++i) {
      KstBaseCurvePtr bcp = cl[i];
      assert(bcp.data());
#if UPDATEDEBUG > 1
      qDebug() << "updating curve: " << (void*)bcp << " - " << bcp->tagName() << endl;
#endif
      enqueueUpdate(bcp);

      if (_done || (_paused && !force)) {
#if UPDATEDEBUG > 1
        qDebug() << "5 Returning from scan with U=" << (int)U << endl;
#endif
        waitForJobs();
        for (uint j = 0; j <= i; ++j) {
          // Note: this should probably be read locked
          if (cl[i]->lastUpdateResult() == KstObject::UPDATE) {
            _updatedCurves.append(cl[i]);
          }
        }
        return updateResult();
      }
    }

    waitForJobs();

    for (KstBaseCurveList::Iterator i = cl.begin(); i != cl.end(); ++i) {
      // Note: this should probably be read locked
      if ((*i)->lastUpdateResult() == KstObject::UPDATE) {
        _updatedCurves.append(*i);
      }
    }

    // Update all data objects
    for (uint i = 0; i < dol.count(); ++i) {
      KstDataObjectPtr dp = dol[i];
      assert(dp.data());
#if UPDATEDEBUG > 1
      qDebug() << "updating data object: " << (void*)dp << " - " << dp->tagName() << endl;
#endif
      enqueueUpdate(dp);

      if (_done || (_paused && !force)) {
#if UPDATEDEBUG > 1
        qDebug() << "5 Returning from scan with U=" << (int)U << endl;
#endif
        waitForJobs();
        return updateResult();
      }
    }
    waitForJobs();
  }

  // Update the files
  if (!_paused) { // don't update even if paused && force
    KST::dataSourceList.lock().readLock();
    unsigned cnt = KST::dataSourceList.count();
    for (uint i = 0; i < cnt; ++i) {
      KstDataSourcePtr dsp = KST::dataSourceList[i];

      enqueueUpdate(dsp);

      if (_done) {
        waitForJobs();
        KST::dataSourceList.lock().unlock();
        return false;
      }
    }
    KST::dataSourceList.lock().unlock();
    waitForJobs();
  }

  if (KstScalar::scalarsDirty()) {
    KstScalar::clearScalarsDirty(); // Must do this first and take a risk of
                                    // falling slightly behind
    KST::scalarList.lock().readLock();
    KstScalarList sl = Q3DeepCopy<KstScalarList>(KST::scalarList.list()); // avoid deadlock on exit
    KST::scalarList.lock().unlock();
    for (KstScalarList::ConstIterator i = sl.begin(); i != sl.end(); ++i) {
      KstScalarPtr sp = *i;

      enqueueUpdate(sp);

      if (_done) {
        waitForJobs();
        return false;
      }
    }
    waitForJobs();
  }

  if (updateResult() == KstObject::UPDATE) {
    qDebug() << "Update plots" << endl;
    if (gotData) { // FIXME: do we need to consider all the other exit points?
      *gotData = true;
    }
  }

#if UPDATEDEBUG > 1
  qDebug() << "6 Returning from scan with U=" << (int)U << endl;
#endif
  return updateResult() == KstObject::UPDATE;
}


void UpdateThread::setUpdateTime(int updateTime) {
  QMutexLocker ml(&_statusMutex);
  _updateTime = updateTime;
}


void UpdateThread::setPaused(bool paused) {
  QMutexLocker ml(&_statusMutex);
  _paused = paused;
}


void UpdateThread::setFinished(bool finished) {
  _statusMutex.lock();
  _done = finished;
  _force = false;
  _statusMutex.unlock();
  _waitCondition.wakeOne();
}


bool UpdateThread::finished() const {
  QMutexLocker ml(&_statusMutex);
  return _done;
}


void UpdateThread::forceUpdate() {
  if (_done) {
    return;
  }
  _statusMutex.lock();
  _force = true;
  _statusMutex.unlock();
  _waitCondition.wakeOne();
}


bool UpdateThread::paused() const {
  QMutexLocker ml(&_statusMutex);
  return _paused;
}



void UpdateThread::startNewUpdate() {
  //_resultMutex.lock();
  _updateResult = KstObject::NO_CHANGE;
  //_resultMutex.unlock();
}


KstObjectPtr UpdateThread::dequeueUpdate() {
  KstObjectPtr rc;
  _updateQueueMutex.lock();
  if (_updateQueue.isEmpty()) {
    if (!jobsAreRunning()) {
      _emptyQueueCondition.wakeAll();
    }
  } else {
    // We could make this smarter if we had isLocked()
    rc = _updateQueue.first();
    _updateQueue.pop_front();
  }
  _updateQueueMutex.unlock();
  return rc;
}


bool UpdateThread::jobsAreRunning() const {
  bool running;
  _jobCountMutex.lock();
  running = _jobCount > 0;
  _jobCountMutex.unlock();
  return running;
}
  

void UpdateThread::jobIsRunning() {
  _jobCountMutex.lock();
  ++_jobCount;
  _jobCountMutex.unlock();
}


void UpdateThread::jobIsDone() {
  _jobCountMutex.lock();
  --_jobCount;
  _jobCountMutex.unlock();
}


void UpdateThread::enqueueUpdate(KstObject *obj) {
  _updateQueueMutex.lock();
  _updateQueue.append(obj);
  _updateQueueMutex.unlock();
  _queueCondition.wakeOne();
}


KstObject::UpdateType UpdateThread::updateResult() const {
  KstObject::UpdateType rc;
  //_resultMutex.lock();
  rc = _updateResult;
  //_resultMutex.unlock();
  return rc;
}


void UpdateThread::waitForJobs() {
  _updateQueueMutex.lock();
  bool empty = _updateQueue.isEmpty();
  _updateQueueMutex.unlock();
  if (empty) {
    return;
  }
  _emptyQueueCondition.wait();
}


// vim: ts=2 sw=2 et
