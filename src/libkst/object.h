/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include <qpointer.h>
#include <qmutex.h>
#include <qobject.h>
#include <qstring.h>
#include <qdebug.h>
#include <qstringlist.h>
#include <QMetaType>

#include "kst_export.h"
#include "sharedptr.h"
#include "rwlock.h"
#include "objecttag.h"

namespace Kst {

class Object : public Shared, public QObject, public KstRWLock {
  public:
    Object();

    enum UpdateType { NO_CHANGE = 0, UPDATE };

    virtual UpdateType update(int updateCounter = -1) = 0;

    virtual QString tagName() const;
    virtual ObjectTag& tag();
    virtual const ObjectTag& tag() const;
    virtual void setTagName(const ObjectTag& tag);

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
    virtual ~Object();

    friend class UpdateThread;
    int _lastUpdateCounter;

    // @since 1.1.0
    UpdateType setLastUpdateResult(UpdateType result);
    // @since 1.1.0
    UpdateType lastUpdateResult() const;

  private:
    ObjectTag _tag;
    bool _dirty;
    Object::UpdateType _lastUpdate;
} KST_EXPORT;

typedef SharedPtr<Object> ObjectPtr;

template <typename T, typename U>
inline SharedPtr<T> kst_cast(SharedPtr<U> object) {
  return dynamic_cast<T*>(object.data());
}


}

Q_DECLARE_METATYPE(Kst::Object*)

#endif

// vim: ts=2 sw=2 et
