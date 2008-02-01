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

#ifndef PLOTITEM_H
#define PLOTITEM_H

#include "viewitem.h"
#include "graphicsfactory.h"

#include <QHash>

#include "relation.h"
#include "plotrenderitem.h"
#include "plotiteminterface.h"

namespace Kst {

class PlotItem : public ViewItem, public PlotItemInterface
{
  Q_OBJECT
  public:
    enum MajorTickMode {
      Coarse = 2,
      Normal = 5,
      Fine = 10,
      VeryFine = 15
    };

    PlotItem(View *parent);
    virtual ~PlotItem();

    virtual QString plotName() const; //from PlotItemInterface

    QList<PlotRenderItem*> renderItems() const;
    PlotRenderItem *renderItem(PlotRenderItem::RenderType type);

    virtual void save(QXmlStreamWriter &xml);

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

    /* This is the rectangle of the PlotAxis and includes the axis labels. */
    QRectF plotAxisRect() const;

    /* This is the rectangle of the PlotRenderItem's and includes the actual curves. */
    QRectF plotRect() const;

    /* This is the rectangle containing the union of all the PlotRenderItem's projections. */
    QRectF projectionRect() const;

    bool isTiedZoom() const;
    void setTiedZoom(bool tiedZoom);

    qreal marginWidth() const;
    qreal marginHeight() const;

    qreal labelMarginWidth() const;
    qreal labelMarginHeight() const;

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

    bool isLeftLabelVisible() const;
    void setLeftLabelVisible(bool visible);

    bool isBottomLabelVisible() const;
    void setBottomLabelVisible(bool visible);

    bool isRightLabelVisible() const;
    void setRightLabelVisible(bool visible);

    bool isTopLabelVisible() const;
    void setTopLabelVisible(bool visible);

    void setLabelsVisible(bool visible);

    qreal axisMarginWidth() const;
    qreal axisMarginHeight() const;

    MajorTickMode xAxisMajorTickMode() const;
    void setXAxisMajorTickMode(MajorTickMode mode);

    MajorTickMode yAxisMajorTickMode() const;
    void setYAxisMajorTickMode(MajorTickMode mode);

    QPointF mapFromAxisToProjection(const QPointF &point) const;
    QPointF mapToAxisFromProjection(const QPointF &point) const;
    QRectF mapFromAxisToProjection(const QRectF &rect) const;
    QRectF mapToAxisFromProjection(const QRectF &rect) const;

    QPointF mapFromPlotToProjection(const QPointF &point) const;
    QPointF mapToPlotFromProjection(const QPointF &point) const;
    QRectF mapFromPlotToProjection(const QRectF &rect) const;
    QRectF mapToPlotFromProjection(const QRectF &rect) const;

    void triggerUpdate() { emit geometryChanged(); update(rect()); }

  Q_SIGNALS:
    void projectionRectChanged();
    void marginsChanged();
    void updatePlotRect();

  protected:
    virtual QTransform projectionAxisTransform() const;
    virtual QTransform projectionPlotTransform() const;

  private:
    qreal calculatedLabelMarginWidth() const;
    void setCalculatedLabelMarginWidth(qreal marginWidth);

    qreal calculatedLabelMarginHeight() const;
    void setCalculatedLabelMarginHeight(qreal marginHeight);

    QRectF horizontalLabelRect(bool calc) const;
    QRectF verticalLabelRect(bool calc) const;

    void paintLeftLabel(QPainter *painter);
    QSizeF calculateLeftLabelBound(QPainter *painter);
    void paintBottomLabel(QPainter *painter);
    QSizeF calculateBottomLabelBound(QPainter *painter);
    void paintRightLabel(QPainter *painter);
    QSizeF calculateRightLabelBound(QPainter *painter);
    void paintTopLabel(QPainter *painter);
    QSizeF calculateTopLabelBound(QPainter *painter);

    qreal calculatedAxisMarginWidth() const;
    void setCalculatedAxisMarginWidth(qreal marginWidth);

    qreal calculatedAxisMarginHeight() const;
    void setCalculatedAxisMarginHeight(qreal marginHeight);

    void computeMajorTicks(QList<qreal> *xMajorTicks, QList<qreal> *yMajorTicks) const;
    qreal computedMajorTickSpacing(Qt::Orientation orientation) const;

    QSizeF calculateXTickLabelBound(QPainter *painter, const QList<qreal> &xMajorTicks);
    QSizeF calculateYTickLabelBound(QPainter *painter, const QList<qreal> &yMajorTicks);

  private Q_SLOTS:
    void calculateProjectionRect();

  private:
    QHash<PlotRenderItem::RenderType, PlotRenderItem*> _renderers;
    bool _isTiedZoom;
    bool _isLeftLabelVisible;
    bool _isBottomLabelVisible;
    bool _isRightLabelVisible;
    bool _isTopLabelVisible;
    qreal _calculatedLabelMarginWidth;
    qreal _calculatedLabelMarginHeight;
    qreal _calculatedAxisMarginWidth;
    qreal _calculatedAxisMarginHeight;

    QRectF _projectionRect;

    QRectF _yLabelRect;
    QRectF _xLabelRect;

    MajorTickMode _xAxisMajorTickMode;
    MajorTickMode _yAxisMajorTickMode;

    friend class ViewGridLayout;
};

class KST_EXPORT CreatePlotCommand : public CreateCommand
{
  public:
    CreatePlotCommand() : CreateCommand(QObject::tr("Create Plot")) {}
    CreatePlotCommand(View *view) : CreateCommand(view, QObject::tr("Create Plot")) {}
    virtual ~CreatePlotCommand() {}
    virtual void createItem();
};

class KST_EXPORT CreatePlotForCurve : public CreateCommand
{
  public:
    CreatePlotForCurve(bool createLayout, bool appendToLayout)
      : CreateCommand(QObject::tr("Create Plot For Curve")),
        _createLayout(createLayout),
        _appendToLayout(appendToLayout) {}
    CreatePlotForCurve(bool createLayout, bool appendToLayout, View *view)
      : CreateCommand(view, QObject::tr("Create Plot For Curve")),
        _createLayout(createLayout),
        _appendToLayout(appendToLayout) {}
    virtual ~CreatePlotForCurve() {}
    virtual void createItem();

  private:
    bool _createLayout;
    bool _appendToLayout;
};

class PlotItemFactory : public GraphicsFactory {
  public:
    PlotItemFactory();
    ~PlotItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
