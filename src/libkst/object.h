/***************************************************************************
                 object.h: abstract base class for all Kst objects
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

#ifndef OBJECT_H
#define OBJECT_H

#include <QPointer>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QMetaType>
#include <QXmlStreamWriter>

#include "kst_export.h"
#include "sharedptr.h"
#include "rwlock.h"
#include "objecttag.h"

namespace Kst {

class ObjectStore;
class Object;

typedef SharedPtr<Object> ObjectPtr;

// short name index variables
extern int _vnum; // vectors
extern int _pnum; // plugins
extern int _csdnum; // csd
extern int _cnum; // curves
extern int _enum; // equations
extern int _hnum; // histograms
extern int _inum; // images
extern int _psdnum; // psd
extern int _xnum; // scalars
extern int _tnum; // text string

class Object : public QObject, public Shared, public KstRWLock {
  Q_OBJECT

  public:
    static QString type();

    enum UpdateType { NO_CHANGE = 0, UPDATE };

    virtual UpdateType update() = 0;

    virtual const QString& typeString() const;
    static const QString staticTypeString;

    ObjectStore *store() const;

    // old tag name system to be deleted.
    virtual ObjectTag& tag();
    virtual const ObjectTag& tag() const;

    //new tag name system: see object names devel doc
    QString Name(); // eg GYRO1:V1
    QString descriptiveName(); // eg GYRO1: automatic or manual
    QString shortName(); // eg V1: always automatically generated
    void setDescriptiveName(QString new_name); // auto if new_name.isEmpty()
    bool descriptiveNameIsManual();

    // Returns count - 2 to account for "this" and the list pointer, therefore
    // you MUST have a reference-counted pointer to call this function
    virtual int getUsage() const;

    // TODO: do we need this?
//    int operator==(const QString&) const;

    virtual void deleteDependents();

    // @since 1.1.0
    virtual void setDirty(bool dirty = true);
    // @since 1.1.0
    bool dirty() const;

    virtual void beginUpdate(ObjectPtr object);
    virtual void processUpdate(ObjectPtr object);
    static void processShortNameIndexAttributes(QXmlStreamAttributes &attrs);

  protected:
    Object(const ObjectTag& tag = ObjectTag::invalidTag);

    virtual ~Object();

    void setTagName(const ObjectTag& tag);

    friend class ObjectStore;
    ObjectStore *_store;  // set by ObjectStore

    //new tag name system: see object names devel doc
    virtual QString _automaticDescriptiveName() = 0;
    QString _shortName;
    QString _manualDescriptiveName;
    virtual void saveNameInfo(QXmlStreamWriter &s);

    // object indices used for saving/resorting shortnames
    int _initial_vnum; // vectors
    int _initial_pnum; // plugins
    int _initial_csdnum; // csd
    int _initial_cnum; // curves
    int _initial_enum; // equations
    int _initial_hnum; // histograms
    int _initial_inum; // images
    int _initial_psdnum; // psd
    int _initial_xnum; // scalars
    int _initial_tnum; // text string

  private:
    ObjectTag _tag;
    bool _dirty;

} KST_EXPORT;



template <typename T, typename U>
inline SharedPtr<T> kst_cast(SharedPtr<U> object) {
  return qobject_cast<T*>(object.data());
}
// FIXME: make this safe
template <typename T>
inline SharedPtr<T> kst_cast(QObject *object) {
  return qobject_cast<T*>(object);
}

}

Q_DECLARE_METATYPE(Kst::Object*)

#endif

// vim: ts=2 sw=2 et
