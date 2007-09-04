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

#include <QDebug>

namespace Kst {

ViewGridLayout::ViewGridLayout(ViewItem *parent)
  : QObject(parent), _rowCount(0), _columnCount(0) {

  parent->setLayout(this);
  connect(parent, SIGNAL(geometryChanged()), this, SLOT(update()));
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

  //Update the row/column counts...
  int maxRow = row + rowSpan;
  int maxColumn = column + columnSpan;

  _rowCount = maxRow > _rowCount ? maxRow : _rowCount;
  _columnCount = maxColumn > _columnCount ? maxColumn : _columnCount;

  _items.append(item);
}


int ViewGridLayout::rowCount() const {
  return _rowCount;
}


int ViewGridLayout::columnCount() const {
  return _columnCount;
}


void ViewGridLayout::update() {

  //For now we divide up equally... can do stretch factors and such later...
  qreal itemWidth = parentItem()->width() / columnCount();
  qreal itemHeight = parentItem()->height() / rowCount();

//   qDebug() << "layouting" << _items.count()
//            << "itemWidth:" << itemWidth
//            << "itemHeight:" << itemHeight
//            << endl;

  foreach (LayoutItem item, _items) {
    QPointF topLeft(itemWidth * item.column, itemHeight * item.row);
    QSizeF size(itemWidth * item.columnSpan, itemHeight * item.rowSpan);

    item.viewItem->resetTransform();
    item.viewItem->setPos(topLeft);
    item.viewItem->setViewRect(QRectF(QPoint(0,0), size));

//     qDebug() << "layout"
//              << "row:" << item.row
//              << "column:" << item.column
//              << "rowSpan:" << item.rowSpan
//              << "columnSpan:" << item.columnSpan
//              << "topLeft:" << topLeft
//              << "size:" << size
//              << endl;
  }

}

}

// vim: ts=2 sw=2 et
