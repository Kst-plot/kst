/***************************************************************************
                          datastprimitive.h  -  add datasource handling
                          capability to a primitive
                             -------------------
    begin                : October, 2009
    copyright            : (C) 2009 by cbn
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


#ifndef DATAPRIMITIVE_H
#define DATAPRIMITIVE_H

#include <QString>


#include "datasource.h"

namespace Kst {

class DataPrimitive
{
public:
    DataPrimitive();
    ~DataPrimitive();

    /** return the name of the file */
    QString filename() const;

    /** return the field name */
    const QString& field() const;

    /** the data source */
    DataSourcePtr dataSource() const;

    /** change the datasource */
    virtual void changeFile(DataSourcePtr file) = 0;

    /** return true if it has a valid file and field, or false otherwise */
    virtual bool isValid() const = 0;

  protected:
    /** file to read for rvectors */
    DataSourcePtr _file;

    /** For the scalar field in the data source */
    QString _field;

};

typedef SharedPtr<DataPrimitive> DataPrimitivePtr;
}
#endif // DATAPRIMITIVE_H
