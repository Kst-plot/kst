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

#ifndef VECTORCURVERENDERITEM_H
#define VECTORCURVERENDERITEM_H

#include "plotrenderitem.h"

namespace Kst {

class VectorCurveRenderItem : public PlotRenderItem {
  public:
    VectorCurveRenderItem(const QString &name);
    virtual ~VectorCurveRenderItem();

    virtual void paint(QPainter *painter);

  protected:
    virtual QPointF mapToProjection(const QPointF &point);
    virtual QPointF mapFromProjection(const QPointF &point);
};

}

#endif

// vim: ts=2 sw=2 et
