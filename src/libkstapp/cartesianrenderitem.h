/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CARTESIANRENDERITEM_H
#define CARTESIANRENDERITEM_H

#include "plotrenderitem.h"

namespace Kst {

class CartesianRenderItem : public PlotRenderItem
{
  Q_OBJECT
  public:
    CartesianRenderItem(PlotItem *parentItem);
    virtual ~CartesianRenderItem();

    virtual void saveInPlot(QXmlStreamWriter &xml);
    virtual void paintRelations(QPainter *painter);

    bool configureFromXml(QXmlStreamReader &xml);
};

}

#endif

// vim: ts=2 sw=2 et
