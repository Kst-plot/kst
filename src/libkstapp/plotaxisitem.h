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

#ifndef PLOTAXISITEM_H
#define PLOTAXISITEM_H

#include "viewitem.h"

namespace Kst {

class PlotItem;
class ObjectStore;

class PlotAxisItem : public ViewItem
{
  Q_OBJECT
  public:
    enum MajorTickMode {
      Coarse = 2,
      Normal = 5,
      Fine = 10,
      VeryFine = 15
    };

    PlotAxisItem(PlotItem *parentItem);
    virtual ~PlotAxisItem();

    PlotItem *plotItem() const;

    MajorTickMode xAxisMajorTickMode() const;
    void setXAxisMajorTickMode(MajorTickMode mode);

    MajorTickMode yAxisMajorTickMode() const;
    void setYAxisMajorTickMode(MajorTickMode mode);

    QRectF plotRect() const;
    QRectF projectionRect() const;

    qreal marginWidth() const;
    qreal marginHeight() const;

    virtual void save(QXmlStreamWriter &xml);
    void saveInPlot(QXmlStreamWriter &xml);
    bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);
    virtual void paint(QPainter *painter);

    virtual void paintMajorGridLines(QPainter *painter,
                                     const QList<qreal> &xMajorTicks,
                                     const QList<qreal> &yMajorTicks);

    virtual void paintMajorTicks(QPainter *painter,
                                 const QList<qreal> &xMajorTicks,
                                 const QList<qreal> &yMajorTicks);

    virtual void paintMajorTickLabels(QPainter *painter,
                                      const QList<qreal> &xMajorTicks,
                                      const QList<qreal> &yMajorTicks);

    QPointF mapFromAxisToProjection(const QPointF &point) const;
    QPointF mapToAxisFromProjection(const QPointF &point) const;
    QRectF mapFromAxisToProjection(const QRectF &rect) const;
    QRectF mapToAxisFromProjection(const QRectF &rect) const;

    QPointF mapFromPlotToProjection(const QPointF &point) const;
    QPointF mapToPlotFromProjection(const QPointF &point) const;
    QRectF mapFromPlotToProjection(const QRectF &rect) const;
    QRectF mapToPlotFromProjection(const QRectF &rect) const;

  Q_SIGNALS:
    void marginChanged();

  protected:
    virtual QTransform projectionAxisTransform() const;
    virtual QTransform projectionPlotTransform() const;

    virtual bool maybeReparent();

  private Q_SLOTS:
    void updateGeometry();

  private:
    void computeMajorTicks(QList<qreal> *xMajorTicks, QList<qreal> *yMajorTicks) const;
    qreal computedMajorTickSpacing(Qt::Orientation orientation) const;

    QSizeF calculateXTickLabelBound(QPainter *painter, const QList<qreal> &xMajorTicks);
    QSizeF calculateYTickLabelBound(QPainter *painter, const QList<qreal> &yMajorTicks);

  private:
    qreal _marginWidth;
    qreal _marginHeight;

    QRectF _yLabelRect;
    QRectF _xLabelRect;

    MajorTickMode _xAxisMajorTickMode;
    MajorTickMode _yAxisMajorTickMode;
};

}

#endif

// vim: ts=2 sw=2 et
