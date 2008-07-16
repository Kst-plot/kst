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
    _enabled(false),
    _rowCount(0),
    _columnCount(0),
    _spacing(QSizeF(DEFAULT_STRUT,DEFAULT_STRUT)),
    _margin(QSizeF(DEFAULT_STRUT,DEFAULT_STRUT)) {
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

  //Watch for margin changes
  if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem))
    connect(plotItem, SIGNAL(marginsChanged()), this, SLOT(update()));

  //FIXME these could be consolidated
  _items.append(item);
  _itemInfos.insert(viewItem, item);
  _itemLayouts.insert(qMakePair(item.row, item.column), item);
}


int ViewGridLayout::rowCount() const {
  return _rowCount;
}


int ViewGridLayout::columnCount() const {
  return _columnCount;
}


qreal ViewGridLayout::plotLabelMarginWidth(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginWidth.contains(item.columnSpan))
      return _plotMarginWidth.value(item.columnSpan).labelMargin;
  }

  return 0.0;
}


qreal ViewGridLayout::plotLabelMarginHeight(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginHeight.contains(item.rowSpan))
      return _plotMarginHeight.value(item.rowSpan).labelMargin;
  }

  return 0.0;
}


qreal ViewGridLayout::plotAxisMarginWidth(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginWidth.contains(item.columnSpan))
      return _plotMarginWidth.value(item.columnSpan).axisMargin;
  }

  return 0.0;
}


qreal ViewGridLayout::plotAxisMarginHeight(const PlotItem *plotItem) const {
  if (_itemInfos.contains(plotItem)) {
    LayoutItem item = _itemInfos.value(plotItem);
    if (_plotMarginHeight.contains(item.rowSpan))
      return _plotMarginHeight.value(item.rowSpan).axisMargin;
  }

  return 0.0;
}


bool ViewGridLayout::isEnabled() const {
  return _enabled;
}


void ViewGridLayout::setEnabled(bool enabled) {
  _enabled = enabled;
  emit enabledChanged(_enabled);
}


void ViewGridLayout::reset() {
  foreach (LayoutItem item, _items) {
    item.viewItem->setTransform(item.transform);
    item.viewItem->setPos(item.position);
    item.viewItem->setViewRect(item.rect);
    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem))
      plotItem->setLabelsVisible(true);
  }
}


void ViewGridLayout::resetSharedAxis() {
  foreach (LayoutItem item, _items) {
    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem))
      plotItem->setLabelsVisible(true);
  }
}


void ViewGridLayout::update() {

  updatePlotMargins();
  updateSharedAxis();

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

    if (item.viewItem->fixedSize()) {
      itemRect.setBottom(itemRect.top() + item.viewItem->rect().height());
      itemRect.setRight(itemRect.left() + item.viewItem->rect().width());
    } else if (item.viewItem->lockAspectRatio()) {
      qreal newHeight = itemRect.height();
      qreal newWidth = itemRect.width();

      qreal aspectRatio = item.viewItem->rect().width() / item.viewItem->rect().height();
      if ((newWidth / newHeight) > aspectRatio) {
        // newWidth is too large.  Use newHeight as key.
        newWidth = newHeight * aspectRatio;
      } else {
        // newHeight is either too large, or perfect.  use newWidth as key.
        newHeight = newWidth / aspectRatio;
      }
      itemRect.setBottom(itemRect.top() + newHeight);
      itemRect.setRight(itemRect.left() + newWidth);
    }
    item.viewItem->setViewRect(QRectF(QPoint(0,0), itemRect.size()));

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem))
      emit plotItem->updatePlotRect();

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
    PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

    if (!plotItem)
      continue;

    {
      qreal labelMargin = plotItem->calculatedLabelMarginWidth();
      qreal axisMargin = plotItem->calculatedAxisMarginWidth();

      if (_plotMarginWidth.contains(item.columnSpan)) {
        labelMargin = qMax(labelMargin, _plotMarginWidth.value(item.columnSpan).labelMargin);
        axisMargin = qMax(axisMargin, _plotMarginWidth.value(item.columnSpan).axisMargin);
      }

      PlotMargins marginsForColumnSpan;
      marginsForColumnSpan.labelMargin = labelMargin;
      marginsForColumnSpan.axisMargin = axisMargin;
      _plotMarginWidth.insert(item.columnSpan, marginsForColumnSpan);
    }

    {
      qreal labelMargin = plotItem->calculatedLabelMarginHeight();
      qreal axisMargin = plotItem->calculatedAxisMarginHeight();

      if (_plotMarginHeight.contains(item.rowSpan)) {
        labelMargin = qMax(labelMargin, _plotMarginHeight.value(item.rowSpan).labelMargin);
        axisMargin = qMax(axisMargin, _plotMarginHeight.value(item.rowSpan).axisMargin);
      }

      PlotMargins marginsForRowSpan;
      marginsForRowSpan.labelMargin = labelMargin;
      marginsForRowSpan.axisMargin = axisMargin;
      _plotMarginHeight.insert(item.rowSpan, marginsForRowSpan);
    }
  }
}


void ViewGridLayout::updateSharedAxis() {

  foreach (LayoutItem item, _items) {
    PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

    if (!plotItem)
      continue;

    //same horizontal range and same row/rowspan
    //same vertical range and same col/colspan
    shareAxisWithPlotToLeft(item);
    shareAxisWithPlotToRight(item);
    shareAxisWithPlotAbove(item);
    shareAxisWithPlotBelow(item);
  }
}


void ViewGridLayout::shareAxisWithPlotToLeft(LayoutItem item) {
  QPair<int, int> key = qMakePair(item.row, item.column - 1);
  if (!_itemLayouts.contains(key))
    return;

  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  LayoutItem left = _itemLayouts.value(key);
  PlotItem *leftItem = qobject_cast<PlotItem*>(left.viewItem);
  if (!leftItem) {
    plotItem->setLeftSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  //vertical range check...
  if (plotItem->projectionRect().top() != leftItem->projectionRect().top() ||
      plotItem->projectionRect().bottom() != leftItem->projectionRect().bottom()) {
    plotItem->setLeftSuppressed(false);
    leftItem->setRightSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  if (item.rowSpan == left.rowSpan && item.columnSpan == left.columnSpan) {
    plotItem->setLeftSuppressed(true);
    leftItem->setRightSuppressed(true);
    setSpacing(QSizeF(0.0, spacing().height()));
  }
}


void ViewGridLayout::shareAxisWithPlotToRight(LayoutItem item) {
  QPair<int, int> key = qMakePair(item.row, item.column + 1);
  if (!_itemLayouts.contains(key))
    return;

  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  LayoutItem right = _itemLayouts.value(key);
  PlotItem *rightItem = qobject_cast<PlotItem*>(right.viewItem);
  if (!rightItem) {
    plotItem->setRightSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  //vertical range check...
  if (plotItem->projectionRect().top() != rightItem->projectionRect().top() ||
      plotItem->projectionRect().bottom() != rightItem->projectionRect().bottom()) {
    plotItem->setRightSuppressed(false);
    rightItem->setLeftSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  if (item.rowSpan == right.rowSpan && item.columnSpan == right.columnSpan) {
    plotItem->setRightSuppressed(true);
    rightItem->setLeftSuppressed(true);
    setSpacing(QSizeF(0.0, spacing().height()));
  }
}


void ViewGridLayout::shareAxisWithPlotAbove(LayoutItem item) {
  QPair<int, int> key = qMakePair(item.row - 1, item.column);
  if (!_itemLayouts.contains(key))
    return;

  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  LayoutItem top = _itemLayouts.value(key);
  PlotItem *topItem = qobject_cast<PlotItem*>(top.viewItem);
  if (!topItem) {
    plotItem->setTopSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  //horizontal range check...
  if (plotItem->projectionRect().left() != topItem->projectionRect().left() ||
      plotItem->projectionRect().right() != topItem->projectionRect().right()) {
    plotItem->setTopSuppressed(false);
    topItem->setBottomSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  if (item.rowSpan == top.rowSpan && item.columnSpan == top.columnSpan) {
    plotItem->setTopSuppressed(true);
    topItem->setBottomSuppressed(true);
    setSpacing(QSizeF(spacing().width(), 0.0));
  }
}


void ViewGridLayout::shareAxisWithPlotBelow(LayoutItem item) {
  QPair<int, int> key = qMakePair(item.row + 1, item.column);
  if (!_itemLayouts.contains(key))
    return;

  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  LayoutItem bottom = _itemLayouts.value(key);
  PlotItem *bottomItem = qobject_cast<PlotItem*>(bottom.viewItem);
  if (!bottomItem) {
    plotItem->setBottomSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  //horizontal range check...
  if (plotItem->projectionRect().left() != bottomItem->projectionRect().left() ||
      plotItem->projectionRect().right() != bottomItem->projectionRect().right()) {
    plotItem->setBottomSuppressed(false);
    bottomItem->setTopSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  if (item.rowSpan == bottom.rowSpan && item.columnSpan == bottom.columnSpan) {
    plotItem->setBottomSuppressed(true);
    bottomItem->setTopSuppressed(true);
    setSpacing(QSizeF(spacing().width(), 0.0));
  }
}


#if 0
void LayoutMarginCommand::undo() {
  Q_ASSERT(_layout);
  _layout->setMargin(_originalMargin);
}


void LayoutMarginCommand::redo() {
  Q_ASSERT(_layout);
  _layout->setMargin(_newMargin);
}


void LayoutSpacingCommand::undo() {
  Q_ASSERT(_layout);
  _layout->setSpacing(_originalSpacing);
}


void LayoutSpacingCommand::redo() {
  Q_ASSERT(_layout);
  _layout->setSpacing(_newSpacing);
}
#endif

}

// vim: ts=2 sw=2 et
