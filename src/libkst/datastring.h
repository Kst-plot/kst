/***************************************************************************
                          dataststring.h  -  a string from a data source
                             -------------------
    begin                : September, 2008
    copyright            : (C) 2008 by cbn
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

#ifndef DATASTRING_H
#define DATASTRING_H

#include "kst_export.h"
#include "dataprimitive.h"
#include "string_kst.h"

class QXmlStreamWriter;

namespace Kst {

/**A class for handling data strings for kst.
 *@author cbn
 */

/** A string which gets its value from a data file. */
class KSTCORE_EXPORT DataString : public String, public DataPrimitive {
  Q_OBJECT

  protected:
    DataString(ObjectStore *store);
    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    /** Update the string */
    virtual qint64 minInputSerial() const;
    virtual qint64 minInputSerialOfLastChange() const;

  public:
    virtual ~DataString();
    virtual void internalUpdate();

    struct KSTCORE_EXPORT ReadInfo 
    {
      ReadInfo(QString* s) : value(s) {}
      QString* value;
    };

    struct KSTCORE_EXPORT Optional 
    {
    };


    void reload();

    virtual void reset();

    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /** change the properties of a DataString */
    void change(DataSourcePtr file, const QString &field);
    void changeFile(DataSourcePtr file);

    /** Save scalar information */
    virtual void save(QXmlStreamWriter &s);

    /** make a copy of the DataString */
    SharedPtr<DataString> makeDuplicate() const;

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;

    bool isValid() const;
};

typedef SharedPtr<DataString> DataStringPtr;
typedef ObjectList<DataString> DataStringList;

}

#endif
// vim: ts=2 sw=2 et
