/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATASOURCEFACTORY_H
#define DATASOURCEFACTORY_H

#include <QXmlStreamReader>
#include <QStringList>

#include "datasource.h"
#include "kst_export.h"

namespace Kst {

class DataSourceFactory {
  public:
    DataSourceFactory();
    virtual ~DataSourceFactory();

    // This takes ownership
    static void registerFactory(const QString& node, DataSourceFactory *factory);
    static void registerFactory(const QStringList& nodes, DataSourceFactory *factory);
    KST_EXPORT static DataSourcePtr parse(QXmlStreamReader& stream);
    virtual DataSourcePtr generateDataSource(QXmlStreamReader& stream) = 0;
};

}

#endif

// vim: ts=2 sw=2 et
