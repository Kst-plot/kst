/***************************************************************************
                               primitive.h
                             -------------------
    begin                : Tue Jun 20 2006
    copyright            : Copyright (C) 2006, The University of Toronto
    email                : netterfield@astro.utoronto.ca
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
#include "dataprimitive.h"

namespace Kst {

class KSTCORE_EXPORT Primitive : public Object 
{
    Q_OBJECT
    Q_PROPERTY(Object* provider READ provider WRITE setProvider)

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;

    // Must not be a ObjectPtr!
    virtual void setProvider(Object* obj);

#ifdef KST_USE_QSHAREDPOINTER
    inline Object* provider() const { return _provider; }
#else
    inline ObjectPtr provider() const { return ObjectPtr(_provider); }
#endif

    void setSlaveName(QString slaveName);
    QString slaveName() { return _slaveName; }
    virtual QString propertyString() const;
    virtual QString  sizeString() const;

    virtual bool used() const;

    virtual ObjectList<Primitive> outputPrimitives() const {return ObjectList<Primitive>();}

  protected:
    Primitive(ObjectStore *store, Object* provider = 0L);

#ifdef KST_USE_QSHAREDPOINTER
    public:
#endif
    virtual ~Primitive();

    protected:

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    QString _slaveName;

    virtual qint64 minInputSerial() const;
    virtual qint64 minInputSerialOfLastChange() const;

    DataPrimitive *_dp;

  protected:
    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * FIXME: pretty sure this is wrong: it shouldn't be a qpointer... not sure
     * what should be going on here! */
#ifdef KST_USE_QSHAREDPOINTER
    Object* _provider;
#else
    QPointer<Object> _provider;
#endif
  private:
    friend class DataPrimitive;
    // Some stuff only needed by data primitives...
    // put it here so we can later use data primitives generically.
    // The problem is that the "Diamond problem" [see wikipedia] is even
    // tougher with QObjects.  So we decide that DataVectors and other
    // Data Primitives "have a" DataPrimitive.  Rather than using dynamic
    // cast to see if a primitive is a data primitive, check to see if _dp
    // has been allocated.  Only access the following through dp()->
    virtual SharedPtr<Primitive> _makeDuplicate() const {return 0;}
    virtual bool _checkValidity(const DataSourcePtr ds) const;
  public:
    DataPrimitive *dp() const {return _dp;}
};

typedef SharedPtr<Primitive> PrimitivePtr;
typedef ObjectList<Primitive> PrimitiveList;
typedef ObjectMap<PrimitivePtr> PrimitiveMap;

}

#endif
// vim: ts=2 sw=2 et
