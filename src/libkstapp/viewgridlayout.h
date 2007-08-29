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

  public Q_SLOTS:
    void update();

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

    QList<LayoutItem> _items;
};

}

#endif

// vim: ts=2 sw=2 et
