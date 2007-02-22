/***************************************************************************
                                  rwlock.h 
                             -------------------
    begin                : Feb 21, 2004
    copyright            : (C) 2004 The University of toronto
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

#ifndef RWLOCK_H
#define RWLOCK_H

#include <qmutex.h>
#include <qmap.h>
#include <qthread.h>
#include <qwaitcondition.h>

#include <config.h>
#include "kst_export.h"

// NOTE: In order to preserve binary compatibility with plugins, you must
//       update the plugin keys whenever you add, remove, or change member
//       variables or virtual functions, or when you remove or change
//       non-virtual functions.

class KST_EXPORT KstRWLock {
  public:
    KstRWLock();
    virtual ~KstRWLock();

    virtual void readLock() const;
    virtual void writeLock() const;

    virtual void unlock() const;

    enum LockStatus {UNLOCKED, READLOCKED, WRITELOCKED};

    virtual LockStatus lockStatus() const;
    virtual LockStatus myLockStatus() const;

  protected:
#ifdef ONE_LOCK_TO_RULE_THEM_ALL
    static
#else
    mutable
#endif
    QMutex _mutex;
    mutable QWaitCondition _readerWait, _writerWait;

    mutable int _readCount, _writeCount;
    mutable int _waitingReaders, _waitingWriters;

    mutable Qt::HANDLE _writeLocker;
    mutable QMap<Qt::HANDLE, int> _readLockers;
};


class KST_EXPORT KstReadLocker {
  public:
    KstReadLocker(KstRWLock *l) : _l(l) { _l->readLock(); }
    ~KstReadLocker() { _l->unlock(); }
  private:
    KstRWLock *_l;
};


class KST_EXPORT KstWriteLocker {
  public:
    KstWriteLocker(KstRWLock *l) : _l(l) { _l->writeLock(); }
    ~KstWriteLocker() { _l->unlock(); }
  private:
    KstRWLock *_l;
};

#endif
// vim: ts=2 sw=2 et
