/***************************************************************************
                               primitive.h
                             -------------------
    begin                : Tue Jun 20 2006
    copyright            : Copyright (C) 2006, The University of Toronto
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

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <QPointer>

#include "kst_export.h"
#include "object.h"
#include "objectlist.h"
#include "objectmap.h"

namespace Kst {

class KST_EXPORT Primitive : public Object {
  Q_OBJECT
  Q_PROPERTY(Object* provider READ provider WRITE setProvider)

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;

    // Must not be a ObjectPtr!
    virtual void setProvider(Object* obj);

    inline ObjectPtr provider() const { return ObjectPtr(_provider); }

    /** Update the primitive via the provider and/or internalUpdate().
        Return true if there was new data. */
    UpdateType update(int update_counter = -1);

    void setSlaveName(QString slaveName);
  protected:
    Primitive(ObjectStore *store, const ObjectTag &tag = ObjectTag::invalidTag, Object* provider = 0L);

    virtual ~Primitive();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName();

    QString _slaveName;

  protected:
    virtual Object::UpdateType internalUpdate(Object::UpdateType providerRC);

    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * The purpose of this is to trigger hierarchical updates properly.
     */
    QPointer<Object> _provider;
};

typedef SharedPtr<Primitive> PrimitivePtr;
typedef ObjectList<PrimitivePtr> PrimitiveList;
typedef ObjectMap<PrimitivePtr> PrimitiveMap;

}

#endif
// vim: ts=2 sw=2 et
