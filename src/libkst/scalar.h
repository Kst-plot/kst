/***************************************************************************
                          kstscalar.h  -  the base scalar type
                             -------------------
    begin                : March 24, 2003
    copyright            : (C) 2003 by cbn
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

#ifndef SCALAR_H
#define SCALAR_H

#include <qdom.h>
#include "kstprimitive.h"
#include "objectcollection.h"

class QXmlStreamWriter;

namespace Kst {

/** The base class for all scalars. */
class KST_EXPORT Scalar : public KstPrimitive {
  Q_OBJECT
  Q_PROPERTY(bool orphan READ orphan WRITE setOrphan)
  Q_PROPERTY(double value READ value WRITE setValue)
  Q_PROPERTY(bool displayable READ displayable WRITE setDisplayable)
  public:
    Scalar(ObjectTag tag = ObjectTag::invalidTag,
              Object *provider = 0L, double val = 0.0, bool orphan = false,
              bool displayable = true, bool editable = false);
    Scalar(const QDomElement& e);

    virtual ~Scalar();

  public:
    void setTagName(const ObjectTag& tag);

    /* return true if any scalars are dirty at the moment */
    static bool scalarsDirty();
    /* For use by the update thread */
    static void clearScalarsDirty();

    /* return a string representation of the scalar */
    QString label() const;

    /** Save scalar information */
    virtual void save(QXmlStreamWriter &s);

    /** Update the scalar.  Return true if there was new data. */
    UpdateType update(int updateCounter = -1);

    Scalar& operator=(double v);

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
  
  signals:
    void trigger();

  private:
    double _value;
    bool _orphan;
    bool _displayable;
    bool _editable;
};


typedef KstSharedPtr<Scalar> ScalarPtr;
typedef ObjectList<ScalarPtr> ScalarList;
typedef ObjectMap<ScalarPtr> ScalarMap;
typedef ObjectCollection<Scalar> ScalarCollection;

}

Q_DECLARE_METATYPE(Kst::Scalar*)

#endif
// vim: ts=2 sw=2 et
