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

#ifndef VECTORFACTORY_H
#define VECTORFACTORY_H

#include "primitivefactory.h"

namespace Kst {

class VectorFactory : public PrimitiveFactory {
  public:
    VectorFactory();
    ~VectorFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

class GeneratedVectorFactory : public PrimitiveFactory {
  public:
    GeneratedVectorFactory();
    ~GeneratedVectorFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

class EditableVectorFactory : public PrimitiveFactory {
  public:
    EditableVectorFactory();
    ~EditableVectorFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

class DataVectorFactory : public PrimitiveFactory {
  public:
    DataVectorFactory();
    ~DataVectorFactory();
    PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream);
};

}

#endif

// vim: ts=2 sw=2 et
