/***************************************************************************
                          vscalar.h  -  a scalar from a sample of a vector field
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

#ifndef VSCALAR_H
#define VSCALAR_H

#include "scalar.h"
#include "datasource.h"
#include "kstcore_export.h"

class QXmlStreamWriter;

namespace Kst {

/**A class for handling vector scalars for kst.
 *@author cbn
 */

/** A scalar which gets its value from a vector field in a data file. */
class KSTCORE_EXPORT VScalar : public Scalar, public DataPrimitive {
  Q_OBJECT

  protected:
    VScalar(ObjectStore *store);
    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

    virtual qint64 minInputSerial() const;
    virtual qint64 maxInputSerialOfLastChange() const;

  public:
    virtual ~VScalar();

    /** Update the scalar. */
    virtual void internalUpdate();

    virtual QString typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /** change the properties of a VScalar */
    void change(DataSourcePtr file, const QString &field, int f0);
    void changeFile(DataSourcePtr file);
    void changeFrame(int in_f0) {_f0 = in_f0;}

    /** return the name of the file */
    QString filename() const;

    /** return the field name */
    const QString& field() const;

    /** return sample number */
    int F0() const;

    /** return file length */
    int fileLength();

    /** Save scalar information */
    virtual void save(QXmlStreamWriter &s);

    virtual QString descriptionTip() const;

    /** return true if it has a valid file and field, or false otherwise */
    bool isValid() const;

    virtual QString propertyString() const;

    virtual ScriptInterface* createScriptInterface();

    /** make a copy of the VScalar */
    virtual PrimitivePtr makeDuplicate() const;
    virtual bool checkValidity(const DataSourcePtr& ds) const;
  private:
    /** For the scalar field in the data source */
    //QString _field;

    int _f0;


};

typedef SharedPtr<VScalar> VScalarPtr;
typedef ObjectList<VScalar> VScalarList;

}

#endif
// vim: ts=2 sw=2 et
