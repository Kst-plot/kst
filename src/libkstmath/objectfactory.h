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

#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <QXmlStreamReader>
#include <QStringList>

#include "kstdataobject.h"
#include "kst_export.h"

namespace Kst {

class ObjectFactory {
  public:
    ObjectFactory();
    virtual ~ObjectFactory();

    // This takes ownership
    static void registerFactory(const QString& node, ObjectFactory *factory);
    static void registerFactory(const QStringList& nodes, ObjectFactory *factory);
    KST_EXPORT static KstDataObjectPtr parse(QXmlStreamReader& stream);
    virtual KstDataObjectPtr generateObject(QXmlStreamReader& stream) = 0;
};

}

#endif

// vim: ts=2 sw=2 et
