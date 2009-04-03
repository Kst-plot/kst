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

#include "view.h"
#include "viewitem.h"
#include "plotitem.h"
#include "layoutboxitem.h"
#include "gridlayouthelper.h"

#include <QDebug>

// 0 off, 1 On
#define DEBUG_LAYOUT 0
#define DEBUG_PLOT_STANDARDIZATION 0
#define DEBUG_SHAREDAXIS 0

// What factor should be used to determine "close" plot sizes.
#define PLOT_STANDARDIZATION_FACTOR .2

namespace Kst {

ViewGridLayout::ViewGridLayout(ViewItem *parent)
  : QObject(parent),
    _enabled(false),
    _rowCount(0),
    _columnCount(0),
    _shareX(false),
    _shareY(false) {
  _spacing = parent->layoutSpacing();
  _margin = parent->layoutMargins();
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
  }
}


void ViewGridLayout::sharePlots(ViewItem *item, QPainter *painter) {
  Q_ASSERT(item);
  Q_ASSERT(item->parentView());

  // Gather all children of the SharedAxisBoxItem.
  QList<ViewItem*> viewItems;
  QList<QGraphicsItem*> list = item->QGraphicsItem::children();
  if (list.isEmpty())
    return; //not added to undostack

  foreach (QGraphicsItem *graphicsItem, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(graphicsItem);
    if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != item)
      continue;
    viewItems.append(viewItem);
  }

  if (viewItems.isEmpty())
    return; //not added to undostack


  // Build an automatic layout to try to maintain the existing layout.
  Grid *grid = Grid::buildGrid(viewItems, 0);
  Q_ASSERT(grid);

  ViewGridLayout *layout = new ViewGridLayout(item);

  foreach (ViewItem *v, viewItems) {
    int r = 0, c = 0, rs = 0, cs = 0;
    if (grid->locateWidget(v, r, c, rs, cs)) {
      layout->addViewItem(v, r, c, rs, cs);
    } else {
      grid->appendItem(v);
      if (grid->locateWidget(v, r, c, rs, cs)) {
        layout->addViewItem(v, r, c, rs, cs);
      } else {
        qDebug() << "ooops, viewItem does not fit in layout" << endl;
      }
    }
  }
  layout->apply();
  layout->apply();

  // Using the automatic layout as a basis, build a custom grid with the same column count,
  // this will remove all spans, making each plot the same size.  When this is built, the 
  // sharedAxis layout will be applied.
  int columnCount = layout->columnCount();
  grid = Grid::buildGrid(viewItems, columnCount);
  Q_ASSERT(grid);

  layout = new ViewGridLayout(item);

  foreach (ViewItem *v, viewItems) {
    int r = 0, c = 0, rs = 0, cs = 0;
    if (grid->locateWidget(v, r, c, rs, cs)) {
      layout->addViewItem(v, r, c, rs, cs);
    } else {
      grid->appendItem(v);
      if (grid->locateWidget(v, r, c, rs, cs)) {
        layout->addViewItem(v, r, c, rs, cs);
      } else {
        qDebug() << "ooops, viewItem does not fit in layout" << endl;
      }
    }
  }

  layout->shareAxis(painter);
}


void ViewGridLayout::standardizePlotMargins(ViewItem *item, QPainter *painter) {
  QList<PlotItem*> plotItems;
  if (item->parentView()) {
    QList<QGraphicsItem*> list = item->parentView()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem || !viewItem->isVisible())
        continue;
      if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
        if (!plotItem->isInSharedAxisBox()) {  // Let the box worry about itself.
          plotItems.append(plotItem);
        }
      }
    }
  }

#if DEBUG_PLOT_STANDARDIZATION
  qDebug() << "Ready to standarize" << plotItems.count() << "plots";
#endif

  QMap<int, qreal> leftMarginWidths;
  QMap<int, qreal> rightMarginWidths;
  QMap<int, qreal> topMarginWidths;
  QMap<int, qreal> bottomMarginHeights;
  foreach (PlotItem* plotItem, plotItems) {
    plotItem->calculateBorders(painter);
    if (leftMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] < plotItem->leftMarginSize()) {
      leftMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] = plotItem->leftMarginSize();
    }
    if (rightMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] < plotItem->rightMarginSize()) {
      rightMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] = plotItem->rightMarginSize();
    }
    if (topMarginWidths[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] < plotItem->topMarginSize()) {
      topMarginWidths[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] = plotItem->topMarginSize();
    }
    if (bottomMarginHeights[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] < plotItem->bottomMarginSize()) {
      bottomMarginHeights[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] = plotItem->bottomMarginSize();
    }
  }

#if DEBUG_PLOT_STANDARDIZATION
  qDebug() << "Maximum left margin widths" << leftMarginWidths;
  qDebug() << "Maximum right margin widths" << rightMarginWidths;
  qDebug() << "Maximum top margin widths" << topMarginWidths;
  qDebug() << "Maximum bottom margin heights" << bottomMarginHeights;
#endif


  foreach (PlotItem* plotItem, plotItems) {

#if DEBUG_PLOT_STANDARDIZATION
    qDebug() << "Margin left width is " << plotItem->leftMarginSize() << "setting to" << leftMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->leftMarginSize();
    qDebug() << "Margin right width is " << plotItem->rightMarginSize() << "setting to" << rightMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->rightMarginSize();
    qDebug() << "Margin top width is " << plotItem->topMarginSize() << "setting to" << topMarginWidths[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->topMarginSize();
    qDebug() << "Margin bottom height is " << plotItem->bottomMarginSize() << "setting to" << bottomMarginHeights[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->bottomMarginSize();
#endif

    qreal leftPadding = leftMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->leftMarginSize();
    qreal rightPadding = rightMarginWidths[floor(plotItem->width()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->rightMarginSize();
    qreal topPadding = topMarginWidths[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->topMarginSize();
    qreal bottomPadding = bottomMarginHeights[floor(plotItem->height()*PLOT_STANDARDIZATION_FACTOR)] - plotItem->bottomMarginSize();

    plotItem->setPadding(leftPadding, rightPadding, topPadding, bottomPadding);
  }
}


void ViewGridLayout::apply() {
  updatePlotMargins();

  //For now we divide up equally... can do stretch factors and such later...

  QSizeF layoutSize(parentItem()->width() - _margin.width() * 2,
                    parentItem()->height() - _margin.height() * 2);

  QPointF layoutTopLeft = parentItem()->rect().topLeft();
  layoutTopLeft += QPointF(_margin.width(), _margin.height());

  QRectF layoutRect(layoutTopLeft, layoutSize);

  qreal itemWidth = layoutSize.width() / columnCount();
  qreal itemHeight = layoutSize.height() / rowCount();

#if DEBUG_LAYOUT
  qDebug() << "layouting" << _items.count()
           << "itemWidth:" << itemWidth
           << "itemHeight:" << itemHeight
           << endl;
#endif

  PlotItem *plot = 0;
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

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem)) {
      plot = plotItem;
      emit plotItem->updatePlotRect();
    }

#if DEBUG_LAYOUT
    qDebug() << "layout"
             << "row:" << item.row
             << "column:" << item.column
             << "rowSpan:" << item.rowSpan
             << "columnSpan:" << item.columnSpan
             << "itemRect:" << itemRect
             << endl;
#endif
  }
  if (plot) {
    plot->setPlotBordersDirty(true);
  }
}


void ViewGridLayout::shareAxis(QPainter *painter) {
  calculateSharing();
  updateSharedAxis();

  // Determine area of layout.  Minimal spacing on SharedAxisBoxItems.
  QSizeF layoutSize(parentItem()->width() - 1, parentItem()->height() - 1);
  QPointF layoutTopLeft = parentItem()->rect().topLeft();
  layoutTopLeft += QPointF(0, 1);

  QRectF layoutRect(layoutTopLeft, layoutSize);

  QMap<int, int> leftLabelBounds;
  QMap<int, int> rightLabelBounds;
  QMap<int, int> topLabelBounds;
  QMap<int, int> bottomLabelBounds;

#if DEBUG_SHAREDAXIS
  qDebug() << "Creating Shared Axis Layout in rect " << layoutRect
           << "rowCount" << rowCount() << "columnCount" << columnCount();
#endif

  foreach (LayoutItem item, _items) {
    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem)) {
      plotItem->calculateBorders(painter);
      if (plotItem->leftMarginSize() > leftLabelBounds[item.column]) {
        leftLabelBounds[item.column] = plotItem->leftMarginSize();
      }
      if (plotItem->rightMarginSize() > rightLabelBounds[item.column]) {
        rightLabelBounds[item.column] = plotItem->rightMarginSize();
      }
      if (plotItem->topMarginSize() > topLabelBounds[item.row]) {
        topLabelBounds[item.row] = plotItem->topMarginSize();
      }
      if (plotItem->bottomMarginSize() > bottomLabelBounds[item.row]) {
        bottomLabelBounds[item.row] = plotItem->bottomMarginSize();
      }
    }
  }

  // Adjust to make sure there is sufficient room for the tiedzoom box.
  if (topLabelBounds[0] < parentItem()->tiedZoomSize().height()) {
    topLabelBounds[0] = parentItem()->tiedZoomSize().height();
  }
  if (rightLabelBounds[columnCount()] < parentItem()->tiedZoomSize().width()) {
    rightLabelBounds[columnCount()] = parentItem()->tiedZoomSize().width();
  }

#if DEBUG_SHAREDAXIS
  qDebug() << "Calculated maximum bounds for labels.";
  qDebug() << "Left bounds by column" << leftLabelBounds;
  qDebug() << "Right bounds by column" << rightLabelBounds;
  qDebug() << "Top bounds by row" << topLabelBounds;
  qDebug() << "Bottom bounds by row" << bottomLabelBounds;
#endif

  bool blockMode = false;
  bool rowMode = false;
  bool colMode = false;

#if DEBUG_SHAREDAXIS
  qDebug() << "Calculated sharing modes" << "sharing X = " << _shareX << "sharing Y = " << _shareY;
#endif

  if (_shareX && _shareY) {
#if DEBUG_SHAREDAXIS
    qDebug() << "Sharing X & Y, one block, all projectionRect's inside frame";
#endif
    blockMode = true;
  } else if (_shareX) {
    if (columnCount() == 1) {
#if DEBUG_SHAREDAXIS
      qDebug() << "Sharing only X, one column, use block logic";
#endif
      blockMode = true;
    } else {
#if DEBUG_SHAREDAXIS
      qDebug() << "Sharing only X, multiple columns, use block for each column, columns divide space equally - number of columns" << columnCount();
#endif
      colMode = true;
    }
  } else if (_shareY) {
    if (rowCount() == 1) {
#if DEBUG_SHAREDAXIS
      qDebug() << "Sharing only Y, one row, use block logic";
#endif
      blockMode = true;
    } else {
#if DEBUG_SHAREDAXIS
      qDebug() << "Sharing only Y, multiple rows, use block for each row, rows divide space equally - number of rows" << rowCount();
#endif
      rowMode = true;
    }
  }

  int totalProjWidth;
  int totalProjHeight;

  if (blockMode) {
    totalProjWidth = (layoutRect.width() - leftLabelBounds[0]) - rightLabelBounds[columnCount() - 1];
    totalProjHeight = (layoutRect.height() - topLabelBounds[0]) - bottomLabelBounds[rowCount() - 1];
  } else if (rowMode) {
    int totalHeight = 0;
    for (int i = 0; i < rowCount(); i++) {
      totalHeight = topLabelBounds[i] + bottomLabelBounds[i];
    }
    totalProjWidth = layoutRect.width() - leftLabelBounds[0] - rightLabelBounds[columnCount() - 1];
    totalProjHeight = layoutRect.height() - totalHeight;

  } else if (colMode) {
    int totalWidth = 0;
    for (int i = 0; i < columnCount(); i++) {
      totalWidth = leftLabelBounds[i] + rightLabelBounds[i];
    }
    totalProjWidth = layoutRect.width() - totalWidth;
    totalProjHeight = layoutRect.height() - topLabelBounds[0] - bottomLabelBounds[columnCount() - 1];
  } else {
    return;
  }

  int columnProjWidth = totalProjWidth / (columnCount());
  int rowProjHeight = totalProjHeight / (rowCount());

  QMap<int, QMap<int, int> > cellHeights;
  QMap<int, QMap<int, int> > cellWidths;

  foreach (LayoutItem item, _items) {
    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem)) {
      int width = columnProjWidth;
      if (plotItem->leftLabelDetails()->isVisible()) {
        width += leftLabelBounds[item.column];
      }
      if (plotItem->rightLabelDetails()->isVisible()) {
        width += rightLabelBounds[item.column];
      }
      cellWidths[item.row][item.column] = width;

      int height = rowProjHeight;
      if (plotItem->topLabelDetails()->isVisible()) {
        height += topLabelBounds[item.row];
      }
      if (plotItem->bottomLabelDetails()->isVisible()) {
        height += bottomLabelBounds[item.row];
      }
      cellHeights[item.row][item.column] = height;
    }
  }

#if DEBUG_SHAREDAXIS
  qDebug() << "Calculated Total projectRect values - width = " << totalProjWidth << " height = " << totalProjHeight;
  qDebug() << "Column Projection Width" << columnProjWidth << "Row Projection Height" << rowProjHeight;
  qDebug() << "Calculated cell widths (row x column)" << cellWidths;
  qDebug() << "Calculated cell heights (row x column)" << cellHeights;
#endif

  foreach (LayoutItem item, _items) {
    int columnStart = 0;
    for (int i = 0; i < item.column; i++) {
      columnStart += cellWidths[item.row][i];
    }

    int rowStart = 0;
    for (int i = 0; i < item.row; i++) {
      rowStart += cellHeights[i][item.column];
    }

    QPointF itemTopLeft(columnStart, rowStart);
    itemTopLeft += layoutTopLeft;
    QSizeF itemSize(cellWidths[item.row][item.column], cellHeights[item.row][item.column]);
    QRectF itemRect(itemTopLeft, itemSize);

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem)) {
      if (plotItem->leftLabelDetails()->isVisible()) {
        plotItem->setLeftPadding(leftLabelBounds[item.column] - plotItem->leftMarginSize());
      }
      if (plotItem->rightLabelDetails()->isVisible()) {
        plotItem->setRightPadding(rightLabelBounds[item.column] - plotItem->rightMarginSize());
      }
      if (plotItem->topLabelDetails()->isVisible()) {
        plotItem->setTopPadding(topLabelBounds[item.row] - plotItem->topMarginSize());
      }
      if (plotItem->bottomLabelDetails()->isVisible()) {
        plotItem->setBottomPadding(bottomLabelBounds[item.row] - plotItem->bottomMarginSize());
      }
    }

    item.viewItem->resetTransform();
    item.viewItem->setPos(itemRect.topLeft());
    item.viewItem->setViewRect(QRectF(QPoint(0,0), itemRect.size()));

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem))
      emit plotItem->updatePlotRect();

#if DEBUG_SHAREDAXIS
    qDebug() << "Shared Axis Plot item details:"
             << "row:" << item.row
             << "column:" << item.column
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
      qreal labelMargin = plotItem->labelMarginWidth();
      qreal axisMargin = plotItem->axisMarginWidth();

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
      qreal labelMargin = plotItem->labelMarginHeight();
      qreal axisMargin = plotItem->axisMarginHeight();

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
  if (!_shareX && !_shareY) {
    return;
  }
  foreach (LayoutItem item, _items) {
    PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

    if (!plotItem)
      continue;

    //same horizontal range and same row/rowspan
    //same vertical range and same col/colspan
    if (_shareX) {
      shareAxisWithPlotAbove(item);
      shareAxisWithPlotBelow(item);
    }
    if (_shareY) {
      shareAxisWithPlotToLeft(item);
      shareAxisWithPlotToRight(item);
    }
  }
}


void ViewGridLayout::calculateSharing() {
  bool xMatch = true;
  bool yMatch = true;

  bool first = true;

  qreal xStart = 0.0, xStop = 0.0;
  qreal yStart = 0.0, yStop = 0.0;

  foreach (LayoutItem item, _items) {
    PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

    if (!plotItem)
      continue;

    if (first) {
      xStart = plotItem->projectionRect().left();
      xStop = plotItem->projectionRect().right();
      yStart = plotItem->projectionRect().top();
      yStop = plotItem->projectionRect().bottom();
      first = false;
    } else {
      if (xMatch && (plotItem->projectionRect().left() != xStart || plotItem->projectionRect().right() != xStop)) {
        xMatch = false;
      }
      if (yMatch && (plotItem->projectionRect().top() != yStart || plotItem->projectionRect().bottom() != yStop)) {
        yMatch = false;
      }
    }
  }
  if (xMatch || yMatch) {
    _shareX = xMatch;
    _shareY = yMatch;
  } else {
    _shareX = true;
    _shareY = true;
  }
}


void ViewGridLayout::updateProjections(ViewItem *item) {
  bool xMatch = true;
  bool yMatch = true;

  bool first = true;

  qreal xStart = 0.0, xStop = 0.0;
  qreal yStart = 0.0, yStop = 0.0;
  qreal xMin, xMax, yMin, yMax;

  QList<ViewItem*> viewItems;
  QList<QGraphicsItem*> list = item->QGraphicsItem::children();
  if (list.isEmpty())
    return; //not added to undostack

  foreach (QGraphicsItem *graphicsItem, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(graphicsItem);
    if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != item)
      continue;

    PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem);

    if (!plotItem)
      continue;

    if (first) {
      xStart = plotItem->projectionRect().left();
      xStop = plotItem->projectionRect().right();
      yStart = plotItem->projectionRect().top();
      yStop = plotItem->projectionRect().bottom();
      xMin = xStart;
      xMax = xStop;
      yMin = yStart;
      yMax = yStop;
      first = false;
    } else {
      if (xMatch && (plotItem->projectionRect().left() != xStart || plotItem->projectionRect().right() != xStop)) {
        xMatch = false;
      }
      if (yMatch && (plotItem->projectionRect().top() != yStart || plotItem->projectionRect().bottom() != yStop)) {
        yMatch = false;
      }
      if (xMin > plotItem->projectionRect().left()) {
        xMin = plotItem->projectionRect().left();
      }
      if (xMax < plotItem->projectionRect().right()) {
        xMax = plotItem->projectionRect().right();
      }
      if (yMin > plotItem->projectionRect().top()) {
        yMin = plotItem->projectionRect().top();
      }
      if (yMax < plotItem->projectionRect().bottom()) {
        yMax = plotItem->projectionRect().bottom();
      }
    }
  }

  if (!xMatch && !yMatch) {
    xMatch = true;
    yMatch = true;
  }

  QRectF projectionRect(QPointF(xMin, yMin), QPointF(xMax, yMax));

  foreach (QGraphicsItem *graphicsItem, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(graphicsItem);
    if (!viewItem || viewItem->hasStaticGeometry() || !viewItem->allowsLayout() || viewItem->parentItem() != item)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      if (xMatch && yMatch) {
        plotItem->zoomFixedExpression(projectionRect);
      } else if (xMatch) {
        plotItem->zoomXRange(projectionRect);
      } else if (yMatch) {
        plotItem->zoomYRange(projectionRect);
      }
    }
  }
}


void ViewGridLayout::shareAxisWithPlotToLeft(LayoutItem item) {
  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  QPair<int, int> key = qMakePair(item.row, item.column - 1);
  if (!_itemLayouts.contains(key)) {
    plotItem->setLeftSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  LayoutItem left = _itemLayouts.value(key);
  PlotItem *leftItem = qobject_cast<PlotItem*>(left.viewItem);
  if (!leftItem) {
    plotItem->setLeftSuppressed(false);
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
  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  QPair<int, int> key = qMakePair(item.row, item.column + 1);
  if (!_itemLayouts.contains(key)) {
    plotItem->setRightSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }


  LayoutItem right = _itemLayouts.value(key);
  PlotItem *rightItem = qobject_cast<PlotItem*>(right.viewItem);
  if (!rightItem) {
    plotItem->setRightSuppressed(false);
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
  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  QPair<int, int> key = qMakePair(item.row - 1, item.column);
  if (!_itemLayouts.contains(key)) {
    plotItem->setTopSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  LayoutItem top = _itemLayouts.value(key);
  PlotItem *topItem = qobject_cast<PlotItem*>(top.viewItem);
  if (!topItem) {
    plotItem->setTopSuppressed(false);
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
  PlotItem *plotItem = qobject_cast<PlotItem*>(item.viewItem);

  QPair<int, int> key = qMakePair(item.row + 1, item.column);
  if (!_itemLayouts.contains(key)) {
    plotItem->setBottomSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  LayoutItem bottom = _itemLayouts.value(key);
  PlotItem *bottomItem = qobject_cast<PlotItem*>(bottom.viewItem);
  if (!bottomItem) {
    plotItem->setBottomSuppressed(false);
    setSpacing(QSizeF(spacing().width(), spacing().height()));
    return;
  }

  if (item.rowSpan == bottom.rowSpan && item.columnSpan == bottom.columnSpan) {
    plotItem->setBottomSuppressed(true);
    bottomItem->setTopSuppressed(true);
    setSpacing(QSizeF(spacing().width(), 0.0));
  }
}

}

// vim: ts=2 sw=2 et
