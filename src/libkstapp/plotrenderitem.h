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

#ifndef PLOTRENDERITEM_H
#define PLOTRENDERITEM_H

#include <QList>
#include <QPainterPath>

#include "kstrelation.h"

namespace Kst {

enum RenderType { Cartesian, Polar, Sinusoidal };

class PlotRenderItem {
  public:
    PlotRenderItem(const QString &name);
    virtual ~PlotRenderItem();

    void setType(RenderType type);
    RenderType type();

    void setPlotRect(const QRectF &plotRect);
    QRectF plotRect();

    void setRelationList(const KstRelationList &relationList);
    KstRelationList relationList() const;

    virtual void paint(QPainter *painter) = 0;

  protected:
    virtual QPointF mapToProjection(const QPointF &point) = 0;
    virtual QPointF mapFromProjection(const QPointF &point) = 0;

    QRectF mapToProjection(const QRectF &rect);
    QRectF mapFromProjection(const QRectF &rect);

  private:
    QString _name;
    RenderType _type;

    QRectF _plotRect;

    KstRelationList _relationList;
};

}

#endif

// vim: ts=2 sw=2 et
