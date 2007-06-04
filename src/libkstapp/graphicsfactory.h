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

#ifndef GRAPHICSFACTORY_H
#define GRAPHICSFACTORY_H

#include <QXmlStreamReader>
#include <QStringList>

#include "kstdataobject.h"
#include "kst_export.h"
#include "view.h"

namespace Kst {
class ViewItem;

class GraphicsFactory {
  public:
    GraphicsFactory();
    virtual ~GraphicsFactory();

    // This takes ownership
    static void registerFactory(const QString& node, GraphicsFactory *factory);
    static void registerFactory(const QStringList& nodes, GraphicsFactory *factory);
    KST_EXPORT static ViewItem *parse(QXmlStreamReader& stream, View *view, ViewItem *parent = 0);
    virtual ViewItem *generateGraphics(QXmlStreamReader& stream, View *view, ViewItem *parent = 0) = 0;
};

}

#endif

// vim: ts=2 sw=2 et
