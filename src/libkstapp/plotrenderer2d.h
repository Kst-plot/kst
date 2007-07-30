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

#ifndef PLOTRENDERER2D_H
#define PLOTRENDERER2D_H

#include <QList>
#include <QPainterPath>

#include "kstrelation.h"

namespace Kst {

enum RenderType2D { Cartesian, Polar, Sinusoidal };

class PlotRenderer2D {
  public:
    PlotRenderer2D(const QString &name);
    virtual ~PlotRenderer2D();

    void setType(RenderType2D type);
    RenderType2D type();

    // FIXME better name?
    void setRange(const QRectF &range);
    QRectF range();

    // FIXME better name?
    void setProjectedRange(const QRectF &range);
    QRectF projectedRange();

    void setRelationList(const KstRelationList &relationList);
    KstRelationList relationList() const;

    virtual QList<QPainterPath> projectedPaths() = 0;

  protected:
    virtual QPointF mapToProjection(const QPointF &point) = 0;
    virtual QPointF mapFromProjection(const QPointF &point) = 0;

    QRectF mapToProjection(const QRectF &rect);
    QRectF mapFromProjection(const QRectF &rect);

    // FIXME still not clear...
    // Recompute auxilliary range information if the
    // properties of the full plot have changed.
    void refreshRange();

  private:
    QString _name;
    RenderType2D _type;

    QRectF _range;
    QRectF _projectedRange;

    KstRelationList _relationList;
};

}

#endif

// vim: ts=2 sw=2 et
