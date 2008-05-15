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
extern int _mnum; // matrix

extern int max_vnum; // vectors
extern int max_pnum; // plugins
extern int max_csdnum; // csd
extern int max_cnum; // curves
extern int max_enum; // equations
extern int max_hnum; // histograms
extern int max_inum; // images
extern int max_psdnum; // psd
extern int max_xnum; // scalars
extern int max_tnum; // string
extern int max_mnum; // matrix

class Object : public QObject, public Shared, public KstRWLock {
  Q_OBJECT

  public:
    static QString type();

    enum UpdateType { NO_CHANGE = 0, UPDATE };

    enum ShortNameIndex {
      VNUM = 0x0001,
      PNUM = 0x0002,
      CSDNUM = 0x0004,
      CNUM = 0x0008,
      ENUM = 0x0010,
      HNUM = 0x0020,
      INUM = 0x0040,
      PSDNUM = 0x0080,
      XNUM = 0x0100,
      TNUM = 0x0200,
      MNUM = 0x0400
    };

    virtual UpdateType update() = 0;

    virtual const QString& typeString() const;
    static const QString staticTypeString;

    ObjectStore *store() const;

    // old tag name system to be deleted.
    virtual ObjectTag& tag();
    virtual const ObjectTag& tag() const;

    //new tag name system: see object names devel doc
    QString Name() const; // eg GYRO1:V1
    QString descriptiveName() const; // eg GYRO1: automatic or manual
    QString shortName() const; // eg V1: always automatically generated
    void setDescriptiveName(QString new_name); // auto if new_name.isEmpty()
    bool descriptiveNameIsManual() const;

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
    virtual QString _automaticDescriptiveName() const= 0;
    QString _shortName;
    QString _manualDescriptiveName;
    virtual void saveNameInfo(QXmlStreamWriter &s, unsigned I = 0xffff);

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
    int _initial_mnum; // matrix

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
