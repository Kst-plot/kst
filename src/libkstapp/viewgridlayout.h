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

#ifndef VIEWGRIDLAYOUT_H
#define VIEWGRIDLAYOUT_H

#include <QObject>
#include <QSizeF>

namespace Kst {

class ViewItem;

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

    QSizeF spacing() const { return _spacing; }
    void setSpacing(const QSizeF &spacing) { _spacing = spacing; }

    QSizeF margin() const { return _margin; }
    void setMargin(const QSizeF &margin) { _margin = margin; }

    qreal plotMarginWidth() const;
    qreal plotMarginHeight() const;

  public Q_SLOTS:
    void update();

  private Q_SLOTS:
    void updatePlotMargins();

  private:
    struct LayoutItem {
      ViewItem *viewItem;
      int row;
      int column;
      int rowSpan;
      int columnSpan;
    };

    int _rowCount;
    int _columnCount;

    QSizeF _spacing;
    QSizeF _margin;

    QList<LayoutItem> _items;
    qreal _plotMarginWidth;
    qreal _plotMarginHeight;
};

}

#endif

// vim: ts=2 sw=2 et
