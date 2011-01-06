/***************************************************************************
                          datastprimitive.h  -  add datasource handling
                          capability to a primitive
                             -------------------
    begin                : October, 2009
    copyright            : (C) 2009 by cbn
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


#ifndef DATAPRIMITIVE_H
#define DATAPRIMITIVE_H

#include "sharedptr.h"
#include "kst_export.h"

#include <QString>

class QXmlStreamWriter;
class QXmlStreamAttributes;

namespace Kst {

class DataSource;
typedef SharedPtr<DataSource> DataSourcePtr;

class Primitive;
typedef SharedPtr<Primitive> PrimitivePtr;

class KSTCORE_EXPORT DataPrimitive
{
public:    
    virtual ~DataPrimitive();

    /** return the name of the file */
    QString filename() const;

    /** save filename to xml stream */
    void saveFilename(QXmlStreamWriter&);
    static void saveFilename(const QString&, QXmlStreamWriter&);

    static QString readFilename(const QXmlStreamAttributes&);


    /** return the field name */
    const QString& field() const;

    /** the data source */
    DataSourcePtr dataSource() const;
    void setDataSource(const DataSourcePtr&);

    /** change the datasource */
    virtual void changeFile(DataSourcePtr file);

    /** return true if it has a valid file and field, or false otherwise */
    //virtual bool isValid() const = 0;

    /** file to read */
    QString _field;    

    virtual PrimitivePtr makeDuplicate() const = 0;

    virtual bool checkValidity(const DataSourcePtr& ds) const = 0;

  protected:
    DataPrimitive(Primitive* primitive);

  private:
    DataPrimitive();
    struct Private;
    Private& d;
};

}

Q_DECLARE_INTERFACE(Kst::DataPrimitive, "DataPrimitive")

#endif // DATAPRIMITIVE_H
