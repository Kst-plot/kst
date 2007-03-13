/***************************************************************************
                               updatethread.h
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

#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <qmutex.h>
#include <qthread.h>
#include <q3valuelist.h>

#include <kstobject.h>
#include <qwaitcondition.h>

class KstBaseCurve;
class KstDoc;

class UpdateThread : public QThread {
  public:
    UpdateThread(KstDoc*);
    virtual ~UpdateThread();

    void setPaused(bool paused);
    bool paused() const;
    void setFinished(bool finished);
    void setUpdateTime(int updateTime);
    void forceUpdate();
    bool finished() const;

  protected:
    friend class UpdateJob;
    virtual void run();

    bool doUpdates(bool force = false, bool *gotData = 0L);

    void startNewUpdate();
    void enqueueUpdate(KstObject *obj);
    KstObjectPtr dequeueUpdate();
    KstObject::UpdateType updateResult() const;
    void waitForJobs();

    QWaitCondition _queueCondition, _emptyQueueCondition;
    int _updateCounter;
    KstObject::UpdateType _updateResult;

    void jobIsRunning();
    void jobIsDone();
    bool jobsAreRunning() const;

  private:
    bool _paused, _done;
    bool _force;
    QWaitCondition _waitCondition;
    mutable QMutex _statusMutex;
    KstDoc *_doc;
    int _updateTime;
    Q3ValueList<KstBaseCurve*> _updatedCurves; // HACK: temporary use in update reworking
    KstObjectList<KstObjectPtr> _updateQueue;
    QMutex _updateQueueMutex;
    mutable QMutex _jobCountMutex;
    int _jobCount;
};


#endif

// vim: ts=2 sw=2 et
