/***************************************************************************
                          dataststring.h  -  a string from a data source
                             -------------------
    begin                : September, 2008
    copyright            : (C) 2008 by cbn
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

#ifndef DATASTRING_H
#define DATASTRING_H

#include "kstcore_export.h"
#include "dataprimitive.h"
#include "string_kst.h"

class QXmlStreamWriter;

namespace Kst {

/**A class for handling data strings for kst.
 *@author cbn
 */

/** A string which gets its value from a data file. */
class KSTCORE_EXPORT DataString : public String, public DataPrimitive 
{
  Q_OBJECT

  protected:
    DataString(ObjectStore *store);
    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    /** Update the string */
    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

  public:
    virtual ~DataString();
    virtual void internalUpdate();

    struct KSTCORE_EXPORT ReadInfo 
    {
      ReadInfo(QString* s, int f=0) : value(s), frame(f) {}
      QString* value;
      int frame; // used for fields that have <string>s as a function of index
    };

    struct KSTCORE_EXPORT DataInfo 
    {
      DataInfo();
      int frameCount; // used for fields that have <string>s as a function of index
    };


    void reload();

    virtual void reset();

    virtual QString typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /** change the properties of a DataString */
    void change(DataSourcePtr file, const QString &field, int frame);
    void changeFile(DataSourcePtr file);

    // used for fields that have <string>s as a function of index
    int frame() const {return _frame;}
    void setFrame(int f) {_frame = f;}
    bool isStream();

    /** Save data string to kst session file */
    virtual void save(QXmlStreamWriter &s);

    virtual QString descriptionTip() const;

    virtual QString propertyString() const;

    bool isValid() const;

    virtual ScriptInterface* createScriptInterface();

    int fileLength() const;

  private:
    /** make a copy of the DataString */
    virtual PrimitivePtr makeDuplicate() const;
    virtual bool checkValidity(const DataSourcePtr& ds) const;

    int _frame;

};

typedef SharedPtr<DataString> DataStringPtr;
typedef ObjectList<DataString> DataStringList;

}

#endif
// vim: ts=2 sw=2 et
