/***************************************************************************
                          kstscalar.h  -  the base scalar type
                             -------------------
    begin                : March 24, 2003
    copyright            : (C) 2003 by cbn
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

#ifndef SCALAR_H
#define SCALAR_H

#include "primitive.h"

class QXmlStreamWriter;

namespace Kst {

/** The base class for all scalars. */
class KSTCORE_EXPORT Scalar : public Primitive
{
  Q_OBJECT
  Q_PROPERTY(bool orphan READ orphan WRITE setOrphan)
  Q_PROPERTY(double value READ value WRITE setValue)
  Q_PROPERTY(bool displayable READ displayable WRITE setDisplayable)

  protected:
    Scalar(ObjectStore *store);

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual void _initializeShortName();

  public:
    virtual ~Scalar();

    /** Update the scalar.*/
    virtual void internalUpdate();

    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /* return a string representation of the scalar */
    QString label() const;

    /** Save scalar information */
    virtual void save(QXmlStreamWriter &s);

    Scalar& operator=(double v);

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;
    virtual QString sizeString() const;

    virtual ObjectList<Primitive> outputPrimitives() const { return ObjectList<Primitive>(); }
    virtual PrimitiveMap metas() const { return PrimitiveMap(); }

  public slots:
    double value() const;

    /** Set the value of the scalar - ignored for some types */
    void setValue(double inV);

    bool orphan() const;
    void setOrphan(bool orphan);

    bool displayable() const;
    void setDisplayable(bool displayable);

    bool editable() const;
    void setEditable(bool editable);

  protected:
    double _value;

  private:
    bool _orphan;
    bool _displayable;
    bool _editable;
};

typedef SharedPtr<Scalar> ScalarPtr;
typedef ObjectList<Scalar> ScalarList;
typedef ObjectMap<Scalar> ScalarMap;

}

Q_DECLARE_METATYPE(Kst::Scalar*)

#endif
// vim: ts=2 sw=2 et
