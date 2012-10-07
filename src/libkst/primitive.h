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


class Primitive;
typedef ObjectMap<Primitive> PrimitiveMap;

class KSTCORE_EXPORT Primitive : public Object 
{
    Q_OBJECT
    Q_PROPERTY(Object* provider READ provider WRITE setProvider)

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;

    // Must not be a ObjectPtr!
    virtual void setProvider(Object* obj);

    inline ObjectPtr provider() const { return ObjectPtr(_provider); }

    void setSlaveName(QString slaveName);
    QString slaveName() const { return _slaveName; }
    virtual QString propertyString() const;
    virtual QString  sizeString() const;

    virtual bool used() const;

    virtual ObjectList<Primitive> outputPrimitives() const = 0;

    virtual PrimitiveMap metas() const = 0;

    // used for sorting dataobjects by Document::sortedDataObjectList()
    virtual bool flagSet() const { return _flag; }
    virtual void setFlag(bool f) { _flag = f;}

  protected:
    Primitive(ObjectStore *store, Object* provider = 0L);

    virtual ~Primitive();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    QString _slaveName;

    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

    virtual void fatalError(const QString& msg);

  protected:
    /** Possibly null.  Be careful, this is non-standard usage of a KstShared.
     * FIXME: pretty sure this is wrong: it shouldn't be a qpointer... not sure
     * what should be going on here! */
    QPointer<Object> _provider;

  private:
    bool _flag; // used for sorting dataobjects by Document::sortedDataObjectList()
};

typedef SharedPtr<Primitive> PrimitivePtr;
typedef ObjectList<Primitive> PrimitiveList;
typedef ObjectMap<Primitive> PrimitiveMap;

}

#endif
// vim: ts=2 sw=2 et
