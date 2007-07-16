/***************************************************************************
              kstobject.h: abstract base class for all Kst objects
                             -------------------
    begin                : May 22, 2003
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

#ifndef KSTOBJECT_H
#define KSTOBJECT_H

#include <qpointer.h>
#include <qmutex.h>
#include <qobject.h>
#include <qstring.h>
#include <qdebug.h>
#include <qstringlist.h>

#include "kst_export.h"
#include "kstsharedptr.h"
#include "rwlock.h"
#include "kstobjecttag.h"

class KstObject : public KstShared, public QObject, public KstRWLock {
  public:
    KstObject();

    enum UpdateType { NO_CHANGE = 0, UPDATE };

    virtual UpdateType update(int updateCounter = -1) = 0;

    virtual QString tagName() const;
    virtual KstObjectTag& tag();
    virtual const KstObjectTag& tag() const;
    virtual void setTagName(const KstObjectTag& tag);

    virtual QString tagLabel() const;
    // Returns count - 2 to account for "this" and the list pointer, therefore
    // you MUST have a reference-counted pointer to call this function
    virtual int getUsage() const;

    // Returns true if update has already been done
    virtual bool checkUpdateCounter(int update_counter);

    int operator==(const QString&) const;

    virtual bool deleteDependents();

    // @since 1.1.0
    virtual void setDirty(bool dirty = true);
    // @since 1.1.0
    bool dirty() const;

  protected:
    virtual ~KstObject();

    friend class UpdateThread;
    int _lastUpdateCounter;

    // @since 1.1.0
    UpdateType setLastUpdateResult(UpdateType result);
    // @since 1.1.0
    UpdateType lastUpdateResult() const;

  private:
    KstObjectTag _tag;
    bool _dirty;
    KstObject::UpdateType _lastUpdate;
} KST_EXPORT;

typedef KstSharedPtr<KstObject> KstObjectPtr;

template <typename T, typename U>
inline KstSharedPtr<T> kst_cast(KstSharedPtr<U> object) {
  return dynamic_cast<T*>(object.data());
}

#endif

// vim: ts=2 sw=2 et
