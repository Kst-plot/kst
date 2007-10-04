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
    PrimitivePtr generatePrimitive(QXmlStreamReader& stream);
};

class SVectorFactory : public PrimitiveFactory {
  public:
    SVectorFactory();
    ~SVectorFactory();
    PrimitivePtr generatePrimitive(QXmlStreamReader& stream);
};

class AVectorFactory : public PrimitiveFactory {
  public:
    AVectorFactory();
    ~AVectorFactory();
    PrimitivePtr generatePrimitive(QXmlStreamReader& stream);
};

class RVectorFactory : public PrimitiveFactory {
  public:
    RVectorFactory();
    ~RVectorFactory();
    PrimitivePtr generatePrimitive(QXmlStreamReader& stream);
};

}

#endif

// vim: ts=2 sw=2 et
