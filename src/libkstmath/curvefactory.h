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

#ifndef CURVEFACTORY_H
#define CURVEFACTORY_H

#include "relationfactory.h"

namespace Kst {

class CurveFactory : public RelationFactory {
  public:
    CurveFactory();
    ~CurveFactory();
    RelationPtr generateRelation(ObjectStore *store, QXmlStreamReader& stream);
};

}

#endif

// vim: ts=2 sw=2 et
