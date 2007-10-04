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

#ifndef STRING_KST_H
#define STRING_KST_H

#include <qdom.h>
#include "primitive.h"
#include "objectlist.h"
#include "objectmap.h"
#include "objectcollection.h"

class QXmlStreamWriter;

namespace Kst {

class KST_EXPORT String : public Primitive {
  Q_OBJECT
  Q_PROPERTY(bool orphan READ orphan WRITE setOrphan)
  public:
    String(ObjectTag in_tag = ObjectTag::invalidTag, Object *provider = 0L, const QString& val = QString::null, bool orphan = false);
    String(QDomElement& e);

    ~String();

  public:
    void setTagName(const ObjectTag& tag);

    /** Save information */
    void save(QXmlStreamWriter &s);

    /** Update the vector.  Return true if there was new data. */
    UpdateType update(int updateCounter = -1);

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
typedef ObjectList<StringPtr> StringList;
typedef ObjectMap<StringPtr> StringMap;
typedef ObjectCollection<String> StringCollection;

}

Q_DECLARE_METATYPE(Kst::String*)

#endif
// vim: ts=2 sw=2 et
