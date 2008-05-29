/***************************************************************************
                     string.h  -  the base string type
                             -------------------
    begin                : Sept 29, 2004
    copyright            : (C) 2004 by The University of Toronto
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

#ifndef STRING_H
#define STRING_H

#include <QDomElement>

#include "primitive.h"

class QXmlStreamWriter;

namespace Kst {

class KST_EXPORT String : public Primitive {
  Q_OBJECT
  Q_PROPERTY(bool orphan READ orphan WRITE setOrphan)

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

  protected:
    String(ObjectStore *store, Object *provider = 0L, const QString& val = QString::null, bool orphan = false);
//     String(ObjectStore *store, QDomElement& e);

    virtual ~String();

    friend class ObjectStore;

  public:
    /** Save information */
    void save(QXmlStreamWriter &s);

    /** Update the vector.  Return true if there was new data. */
    UpdateType update();

    String& operator=(const QString& v);
    String& operator=(const char *v);

  public slots:
    /* return the value of the string */
    const QString& value() const { return _value; }

    /** Set the value of the string - ignored for some types */
    void setValue(const QString& inV);

    bool orphan() const { return _orphan; }
    void setOrphan(bool orphan) { _orphan = orphan; }

    bool editable() const { return _editable; }
    void setEditable(bool editable) { _editable = editable; }

  signals:
    void trigger();

  private:
    QString _value;
    bool _orphan : 1;
    bool _editable;
};

typedef SharedPtr<String> StringPtr;
typedef ObjectList<String> StringList;
typedef ObjectMap<String> StringMap;

}

Q_DECLARE_METATYPE(Kst::String*)

#endif
// vim: ts=2 sw=2 et
