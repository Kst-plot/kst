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

#include "viewgridlayout.h"

#include "viewitem.h"
#include "plotitem.h"

#include <QDebug>

// #define DEBUG_LAYOUT

static qreal DEFAULT_STRUT = 20.0;

namespace Kst {

ViewGridLayout::ViewGridLayout(ViewItem *parent)
  : QObject(parent),
    _rowCount(0),
    _columnCount(0),
    _spacing(QSizeF(DEFAULT_STRUT,DEFAULT_STRUT)),
    _margin(QSizeF(DEFAULT_STRUT,DEFAULT_STRUT)) {

  parent->setLayout(this);
}


ViewGridLayout::~ViewGridLayout() {
}


ViewItem *ViewGridLayout::parentItem() const {
  return qobject_cast<ViewItem*>(parent());
}


void ViewGridLayout::addViewItem(ViewItem *viewItem, int row, int column) {
  addViewItem(viewItem, row, column, 1, 1);
}


void ViewGridLayout::addViewItem(ViewItem *viewItem, int row, int column, int rowSpan, int columnSpan) {
  LayoutItem item;
  item.viewItem = viewItem;
  item.row = row;
  item.column = column;
  item.rowSpan = rowSpan;
  item.columnSpan = columnSpan;
  item.transform = viewItem->transform();
  item.position = viewItem->pos();
  item.rect = viewItem->rect();

  //Update the row/column counts...
  int maxRow = row + rowSpan;
  int maxColumn = column + columnSpan;

  _rowCount = maxRow > _rowCount ? maxRow : _rowCount;
  _columnCount = maxColumn > _columnCount ? maxColumn : _columnCount;

  _items.append(item);
  _itemInfos.insert(viewItem, item);
}


int ViewGridLayout::rowCount() const {
  return _rowCount;
}


int ViewGridLayout::columnCount() const {
  return _columnCount;
}


qreal ViewGridLayout::plotMarginWidth(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginWidth.contains(item.columnSpan))
      return _plotMarginWidth.value(item.columnSpan);
  }

  return 0.0;
}


qreal ViewGridLayout::plotMarginHeight(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginHeight.contains(item.rowSpan))
      return _plotMarginHeight.value(item.rowSpan);
  }

  return 0.0;
}


void ViewGridLayout::reset() {
  foreach (LayoutItem item, _items) {
    item.viewItem->setTransform(item.transform);
    item.viewItem->setPos(item.position);
    item.viewItem->setViewRect(item.rect);
  }
}


void ViewGridLayout::update() {

  updatePlotMargins();

  //For now we divide up equally... can do stretch factors and such later...

  QSizeF layoutSize(parentItem()->width() - _margin.width() * 2,
                    parentItem()->height() - _margin.height() * 2);

  QPointF layoutTopLeft = parentItem()->rect().topLeft();
  layoutTopLeft += QPointF(_margin.width(), _margin.height());

  QRectF layoutRect(layoutTopLeft, layoutSize);

  qreal itemWidth = layoutSize.width() / columnCount();
  qreal itemHeight = layoutSize.height() / rowCount();

#ifdef DEBUG_LAYOUT
  qDebug() << "layouting" << _items.count()
           << "itemWidth:" << itemWidth
           << "itemHeight:" << itemHeight
           << endl;
#endif

  foreach (LayoutItem item, _items) {
    QPointF topLeft(itemWidth * item.column, itemHeight * item.row);
    QSizeF size(itemWidth * item.columnSpan, itemHeight * item.rowSpan);
    topLeft += layoutTopLeft;

    QRectF itemRect(topLeft, size);

    if (itemRect.top() != layoutRect.top())
      itemRect.setTop(itemRect.top() + _spacing.height() / 2);
    if (itemRect.left() != layoutRect.left())
      itemRect.setLeft(itemRect.left() + _spacing.width() / 2);
    if (itemRect.bottom() != layoutRect.bottom())
      itemRect.setBottom(itemRect.bottom() - _spacing.height() / 2);
    if (itemRect.right() != layoutRect.right())
      itemRect.setRight(itemRect.right() - _spacing.width() / 2);

    item.viewItem->resetTransform();
    item.viewItem->setPos(itemRect.topLeft());
    item.viewItem->setViewRect(QRectF(QPoint(0,0), itemRect.size()));

#ifdef DEBUG_LAYOUT
    qDebug() << "layout"
             << "row:" << item.row
             << "column:" << item.column
             << "rowSpan:" << item.rowSpan
             << "columnSpan:" << item.columnSpan
             << "itemRect:" << itemRect
             << endl;
#endif
  }
}


void ViewGridLayout::updatePlotMargins() {
  _plotMarginWidth.clear();
  _plotMarginHeight.clear();
  foreach (LayoutItem item, _items) {
    PlotItem *plotItem = dynamic_cast<PlotItem*>(item.viewItem);

    if (!plotItem)
      continue;

    qreal marginForColumnSpan = plotItem->calculatedMarginWidth();
    if (_plotMarginWidth.contains(item.columnSpan))
      marginForColumnSpan = qMax(marginForColumnSpan, _plotMarginWidth.value(item.columnSpan));
    _plotMarginWidth.insert(item.columnSpan, marginForColumnSpan);

    qreal marginForRowSpan = plotItem->calculatedMarginHeight();
    if (_plotMarginHeight.contains(item.rowSpan))
      marginForRowSpan = qMax(marginForRowSpan, _plotMarginHeight.value(item.rowSpan));
    _plotMarginHeight.insert(item.rowSpan, marginForRowSpan);
  }
}

}

// vim: ts=2 sw=2 et
