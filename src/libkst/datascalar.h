/***************************************************************************
                          datastscalar.h  -  a scalar from a data source
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

#ifndef DATASCALAR_H
#define DATASCALAR_H

#include "scalar.h"
#include "datasource.h"
#include "kst_export.h"

class QXmlStreamWriter;

namespace Kst {

/**A class for handling data scalars for kst.
 *@author cbn
 */

/** A scalar which gets its value from a data file. */
class KST_EXPORT DataScalar : public Scalar {
  Q_OBJECT

  protected:
    DataScalar(ObjectStore *store);
    virtual ~DataScalar();

    friend class ObjectStore;

    virtual QString _automaticDescriptiveName() const;

  public:
    virtual const QString& typeString() const;
    static const QString staticTypeString;
    static const QString staticTypeTag;

    /** change the properties of a DataScalar */
    void change(DataSourcePtr file, const QString &field);
    void changeFile(DataSourcePtr file);

    /** return the name of the file */
    QString filename() const;

    /** return the field name */
    const QString& field() const;

    /** Save scalar information */
    virtual void save(QXmlStreamWriter &s);

    /** Update the scalar.  Return true if there was new data. */
    virtual UpdateType update();

    /** make a copy of the DataScalar */
    SharedPtr<DataScalar> makeDuplicate() const;

    /** the data source */
    DataSourcePtr dataSource() const;

    virtual QString descriptionTip() const;

    /** return true if it has a valid file and field, or false otherwise */
    bool isValid() const;

  public Q_SLOTS:
    void sourceUpdated(ObjectPtr object);

  private:
    /** file to read for rvectors */
    DataSourcePtr _file;

    /** For the scalar field in the data source */
    QString _field;

};

typedef SharedPtr<DataScalar> DataScalarPtr;
typedef ObjectList<DataScalar> DataScalarList;

}

#endif
// vim: ts=2 sw=2 et
