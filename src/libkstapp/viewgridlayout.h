/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef VIEWGRIDLAYOUT_H
#define VIEWGRIDLAYOUT_H

#include <QObject>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QTransform>
#include <QHash>

namespace Kst {

class View;
class ViewItem;
class PlotItem;

class ViewGridLayout : public QObject
{
  Q_OBJECT
  public:
    ViewGridLayout(ViewItem *parent);
    virtual ~ViewGridLayout();

    ViewItem *parentItem() const;

    void addViewItem(ViewItem *viewItem, int row, int column);
    void addViewItem(ViewItem *viewItem, int row, int column, int rowSpan, int columnSpan);

    int rowCount() const;
    int columnCount() const;
    void setColumnCount(int count) {_columnCount = (count>_columnCount)? count : _columnCount ;}

    QSizeF spacing() const { return _spacing; }
    void setSpacing(const QSizeF &spacing) { _spacing = spacing; }

    QSizeF margin() const { return _margin; }
    void setMargin(const QSizeF &margin) { _margin = margin; }

    qreal plotLabelMarginWidth(const PlotItem *plotItem) const;
    qreal plotLabelMarginHeight(const PlotItem *plotItem) const;

    qreal plotAxisMarginWidth(const PlotItem *plotItem) const;
    qreal plotAxisMarginHeight(const PlotItem *plotItem) const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    void calculateSharing();
    static void updateProjections(ViewItem *item, bool forceXShare = false, bool forceYShare = false);

    static void standardizePlotMargins(ViewItem *item, QPainter *painter);
    static void sharePlots(ViewItem *item, QPainter *painter, bool creation);

    void reset();

  public Q_SLOTS:
    void apply();
    void shareAxis(QPainter *painter, bool creation);

  Q_SIGNALS:
    void enabledChanged(bool enabled);

  private Q_SLOTS:
    void updatePlotMargins();

  private:
    struct LayoutItem {
      ViewItem *viewItem;
      int row;
      int column;
      int rowSpan;
      int columnSpan;
      QTransform transform;
      QPointF position;
      QRectF rect;
    };

    struct PlotMargins {
      qreal labelMargin;
      qreal axisMargin;
    };

  private:
    void updateSharedAxis();
    void unshareAxis();
    void shareAxisWithPlotToLeft(LayoutItem item);
    void shareAxisWithPlotToRight(LayoutItem item);
    void shareAxisWithPlotAbove(LayoutItem item);
    void shareAxisWithPlotBelow(LayoutItem item);

  private:
    bool _enabled;
    int _rowCount;
    int _columnCount;

    bool _shareX;
    bool _shareY;

    QSizeF _spacing;
    QSizeF _margin;

    QList<LayoutItem> _items;
    QHash<int, PlotMargins> _plotMarginWidth;
    QHash<int, PlotMargins> _plotMarginHeight;
    QHash<const ViewItem*, LayoutItem> _itemInfos;
    QHash< QPair<int, int>, LayoutItem> _itemLayouts;
};

}

#endif

// vim: ts=2 sw=2 et
