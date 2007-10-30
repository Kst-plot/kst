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

#ifndef PRIMITIVEFACTORY_H
#define PRIMITIVEFACTORY_H

#include <QXmlStreamReader>
#include <QStringList>

#include "primitive.h"
#include "kst_export.h"

namespace Kst {

class PrimitiveFactory {
  public:
    PrimitiveFactory();
    virtual ~PrimitiveFactory();

    // This takes ownership
    static void registerFactory(const QString& node, PrimitiveFactory *factory);
    static void registerFactory(const QStringList& nodes, PrimitiveFactory *factory);
    KST_EXPORT static PrimitivePtr parse(ObjectStore *store, QXmlStreamReader& stream);
    virtual PrimitivePtr generatePrimitive(ObjectStore *store, QXmlStreamReader& stream) = 0;
};

}

#endif

// vim: ts=2 sw=2 et
