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

#ifndef SCALARMODEL_H
#define SCALARMODEL_H

#include "primitivemodel.h"


namespace Kst {


class ScalarModel : public PrimitiveModel
{

  enum ColumnID { Name, Value };

public:
  ScalarModel(ObjectStore *store) : PrimitiveModel(store) {
    createTree<Scalar>();
  }

  void addDataSource(DataSourcePtr dataSource, ScalarTreeItem* parent = 0);
};




}

#endif

// vim: ts=2 sw=2 et
