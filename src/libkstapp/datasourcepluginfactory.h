/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATASOURCEPLUGINFACTORY_H
#define DATASOURCEPLUGINFACTORY_H

#include "datasourcefactory.h"

namespace Kst {

class DataSourcePluginFactory : public DataSourceFactory {
  public:
    DataSourcePluginFactory();
    ~DataSourcePluginFactory();
    DataSourcePtr generateDataSource(ObjectStore *store, QXmlStreamReader& stream);
};

}

#endif

// vim: ts=2 sw=2 et
