/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
**
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

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

/* These classes were taken from Qt designer in Qt 4.3 and adapted to work
with our layouts and items.*/

#include "gridlayouthelper.h"

#include <math.h>
#include <QDebug>

#define DEBUG_GRID_BUILDING 0

namespace Kst {

Grid *Grid::buildGrid(const QList<ViewItem*> &itemList)
{
    if (!itemList.count())
        return 0;

//     qDebug() << "building Automatic grid" << itemList.count();
    // Pixel to cell conversion:
    // By keeping a list of start'n'stop values (x & y) for each viewitem,
    // it is possible to create a very small grid of cells to represent
    // the item layout.
    // -----------------------------------------------------------------

    // modified by cbn to have a tolerance of 1/8 the size of the smallest object

    // We need a list of both start and stop values for x- & y-axis
    QVector<int> x( itemList.count()*2 );
    QVector<int> y( itemList.count()*2 );

    // Using push_back would look nicer, but operator[] is much faster
    int index  = 0;
    ViewItem* v = 0;
    int minWidth = itemList.at(0)->viewRect().width();
    int minHeight = itemList.at(0)->viewRect().height();
    for (int i = 0; i < itemList.size(); ++i) {
        v = itemList.at(i);
        QRect itemPos = QRect(v->pos().toPoint(), v->viewRect().size().toSize());
        x[index]   = itemPos.left();
        x[index+1] = itemPos.right();
        y[index]   = itemPos.top();
        y[index+1] = itemPos.bottom();
        if (itemPos.width() < minWidth) {
          minWidth = itemPos.width();
        }
        if (itemPos.height() < minHeight) {
          minHeight = itemPos.height();
        }
        index += 2;
    }

    qSort(x);
    qSort(y);

    // Remove duplicate x enteries (Remove next, if equal to current)
    if ( !x.empty() ) {
        for (QVector<int>::iterator current = x.begin() ;
             (current != x.end()) && ((current+1) != x.end()) ; )
            if ( abs(*current - *(current+1)) < minWidth/8 )
                x.erase(current+1);
            else
                current++;
    }

    // Remove duplicate y enteries (Remove next, if equal to current)
    if ( !y.empty() ) {
        for (QVector<int>::iterator current = y.begin() ;
             (current != y.end()) && ((current+1) != y.end()) ; )
            if ( abs(*current - *(current+1)) < minHeight/8 )
                y.erase(current+1);
            else
                current++;
    }

    int rows = y.size() - 1;
    int cols = x.size() - 1;
    rows++;

#if DEBUG_GRID_BUILDING
    qDebug() << "Building automatic grid with" << rows << "rows and" << cols << "columns";
#endif

    Grid *grid = new Grid(rows, cols);

    // Mark the cells in the grid that contains a widget
    foreach (ViewItem *v, itemList) {
        QRect itemPos = QRect(v->pos().toPoint(), v->viewRect().size().toSize());

        QRect c(0, 0, 0, 0);

        // From left til right (not including)
        for (int cw=0; cw<x.size(); cw++) {
            if (abs(x[cw] - itemPos.left()) <= minWidth/8)
                c.setLeft(cw);
            if (x[cw] <  itemPos.right()+ minWidth/8)
                c.setRight(cw);
        }

        // From top til bottom (not including)
        for (int ch=0; ch<y.size(); ch++) {
            if (abs(y[ch] - itemPos.top()) < minHeight/8  )
                c.setTop(ch);
            if (y[ch] <  itemPos.bottom() + minHeight/8)
                c.setBottom(ch);
        }

        // Make sure the sizes are all positive.
        if (c.width() < 0)
          c.setRight(c.left());

        if (c.height() < 0)
          c.setBottom(c.top());

        grid->setCells(c, v); // Mark cellblock
    }

    grid->simplify();

    return grid;
}

void Grid::setCells(QRect c, ViewItem* w) {
#if DEBUG_GRID_BUILDING
    qDebug() << "setCells requested for rect" << c;
#endif
    QVector<int> skippedRows;
    QVector<int> skippedColumns;
    for (int rows = c.bottom()-c.top()-1; rows >= 0; rows--)
        for (int cols = c.right()-c.left()-1; cols >= 0; cols--) {
            if (!(skippedRows.contains(c.top()+rows) || skippedColumns.contains(c.left()+cols))) {
              if (!setCell(c.top()+rows, c.left()+cols, w)) {
                skippedRows.append(c.top()+rows);
                skippedColumns.append(c.left()+cols);
#if DEBUG_GRID_BUILDING
                qDebug() << "skipping rows" << skippedRows << "skipping columns" << skippedColumns;
#endif
              }
        }
    }
}

bool Grid::setCell(int row, int col, ViewItem* w) {
#if DEBUG_GRID_BUILDING
    qDebug() << "Setting cell" << row << col; 
#endif
    if (cell(row, col)) {
#if DEBUG_GRID_BUILDING
      qDebug() << "Cell has already been added";
#endif
      return false;
    } else {
      m_cells[ row * m_ncols + col] = w;
      return true;
    }
}

Grid *Grid::buildGrid(const QList<ViewItem*> &itemList, int columns)
{
    if (!itemList.count())
        return 0;

    if (columns == 0) {
      return buildGrid(itemList);
    }

    int rows = ceil((qreal)itemList.count() / columns);
    if (rows * columns == itemList.count()) {
      rows++;
#if DEBUG_GRID_BUILDING
      qDebug() << "Padding required, row count is now" << rows;
#endif
    }

#if DEBUG_GRID_BUILDING
    qDebug() << "Building custom grid with" << rows << "rows and" << columns << "columns";
#endif

    QMap<int, ViewItem*> sortedItems;
    foreach(ViewItem* item, itemList) {
      int sortId = item->pos().x() + item->pos().y() * 10;
      sortedItems.insert(sortId, item);
    }

    Grid *grid = new Grid(rows, columns);

    // Mark the cells in the grid that contains a widget
    int row = 0, col = 0;
    foreach (ViewItem *v, sortedItems) {
        grid->setCell(row, col, v);
        col++;
        if (col == columns) {
          row++;
          col = 0;
        }
    }

    grid->simplify();
    return grid;
}

Grid::Grid(int r, int c) :
    m_nrows(r),
    m_ncols(c),
    m_cells(new ViewItem*[ r * c ]),
    m_cols(new bool[ c ]),
    m_rows(new bool[ r ])
{
    qFill(m_cells, m_cells + r * c,  static_cast<ViewItem *>(0));
}

Grid::~Grid()
{
    delete [] m_cells;
    delete [] m_cols;
    delete [] m_rows;
}

int Grid::countRow(int r, int c) const
{
    ViewItem* w = cell(r, c);
    int i = c + 1;
    while (i < m_ncols && cell(r, i) == w)
        i++;
    return i - c;
}

int Grid::countCol(int r, int c) const
{
    ViewItem* w = cell(r, c);
    int i = r + 1;
    while (i < m_nrows && cell(i, c) == w)
        i++;
    return i - r;
}

void Grid::setCol(int r, int c, ViewItem* w, int count)
{
    for (int i = 0; i < count; i++)
        setCell(r + i, c, w);
}

void Grid::setRow(int r, int c, ViewItem* w, int count)
{
    for (int i = 0; i < count; i++)
        setCell(r, c + i, w);
}

bool Grid::isWidgetStartCol(int c) const
{
    int r;
    for (r = 0; r < m_nrows; r++) {
        if (cell(r, c) && ((c==0) || (cell(r, c)  != cell(r, c-1)))) {
            return true;
        }
    }
    return false;
}

bool Grid::isWidgetEndCol(int c) const
{
    int r;
    for (r = 0; r < m_nrows; r++) {
        if (cell(r, c) && ((c == m_ncols-1) || (cell(r, c) != cell(r, c+1))))
            return true;
    }
    return false;
}

bool Grid::isWidgetStartRow(int r) const
{
    int c;
    for (c = 0; c < m_ncols; c++) {
        if (cell(r, c) && ((r==0) || (cell(r, c) != cell(r-1, c))))
            return true;
    }
    return false;
}

bool Grid::isWidgetEndRow(int r) const
{
    int c;
    for (c = 0; c < m_ncols; c++) {
        if (cell(r, c) && ((r == m_nrows-1) || (cell(r, c) != cell(r+1, c))))
            return true;
    }
    return false;
}


bool Grid::isWidgetTopLeft(int r, int c) const
{
    ViewItem* w = cell(r, c);
    if (!w)
        return false;
    return (!r || cell(r-1, c) != w) && (!c || cell(r, c-1) != w);
}

void Grid::extendLeft()
{
    int r,c,i;
    for (c = 1; c < m_ncols; c++) {
        for (r = 0; r < m_nrows; r++) {
            ViewItem* w = cell(r, c);
            if (!w)
                continue;

            int cc = countCol(r, c);
            int stretch = 0;
            for (i = c-1; i >= 0; i--) {
                if (cell(r, i))
                    break;
                if (countCol(r, i) < cc)
                    break;
                if (isWidgetEndCol(i))
                    break;
                if (isWidgetStartCol(i)) {
                    stretch = c - i;
                    break;
                }
            }
            if (stretch) {
                for (i = 0; i < stretch; i++)
                    setCol(r, c-i-1, w, cc);
            }
        }
    }
}


void Grid::extendRight()
{
    int r,c,i;
    for (c = m_ncols - 2; c >= 0; c--) {
        for (r = 0; r < m_nrows; r++) {
            ViewItem* w = cell(r, c);
            if (!w)
                continue;
            int cc = countCol(r, c);
            int stretch = 0;
            for (i = c+1; i < m_ncols; i++) {
                if (cell(r, i))
                    break;
                if (countCol(r, i) < cc)
                    break;
                if (isWidgetStartCol(i))
                    break;
                if (isWidgetEndCol(i)) {
                    stretch = i - c;
                    break;
                }
            }
            if (stretch) {
                for (i = 0; i < stretch; i++)
                    setCol(r, c+i+1, w, cc);
            }
        }
    }

}

void Grid::extendUp()
{
    int r,c,i;
    for (r = 1; r < m_nrows; r++) {
        for (c = 0; c < m_ncols; c++) {
            ViewItem* w = cell(r, c);
            if (!w)
                continue;
            int cr = countRow(r, c);
            int stretch = 0;
            for (i = r-1; i >= 0; i--) {
                if (cell(i, c))
                    break;
                if (countRow(i, c) < cr)
                    break;
                if (isWidgetEndRow(i))
                    break;
                if (isWidgetStartRow(i)) {
                    stretch = r - i;
                    break;
                }
            }
            if (stretch) {
                for (i = 0; i < stretch; i++)
                    setRow(r-i-1, c, w, cr);
            }
        }
    }
}

void Grid::extendDown()
{
    int r,c,i;
    for (r = m_nrows - 2; r >= 0; r--) {
        for (c = 0; c < m_ncols; c++) {
            ViewItem* w = cell(r, c);
            if (!w)
                continue;
            int cr = countRow(r, c);
            int stretch = 0;
            for (i = r+1; i < m_nrows; i++) {
                if (cell(i, c))
                    break;
                if (countRow(i, c) < cr)
                    break;
                if (isWidgetStartRow(i))
                    break;
                if (isWidgetEndRow(i)) {
                    stretch = i - r;
                    break;
                }
            }
            if (stretch) {
                for (i = 0; i < stretch; i++)
                    setRow(r+i+1, c, w, cr);
            }
        }
    }

}

void Grid::simplify()
{
    extendLeft();
    extendRight();
    extendUp();
    extendDown();
    merge();
}

void Grid::merge()
{
    int r,c;
    for (c = 0; c < m_ncols; c++)
        m_cols[c] = false;

    for (r = 0; r < m_nrows; r++)
        m_rows[r] = false;

    for (c = 0; c < m_ncols; c++) {
        for (r = 0; r < m_nrows; r++) {
            if (isWidgetTopLeft(r, c)) {
                m_rows[r] = true;
                m_cols[c] = true;
            }
        }
    }
}

bool Grid::locateWidget(ViewItem *w, int &row, int &col, int &rowspan, int &colspan)
{
    int r, c, r2, c2;

    for (c = 0; c < m_ncols; c++) {
        for (r = 0; r < m_nrows; r++) {
            if (cell(r, c) == w) {
                row = 0;
                for (r2 = 1; r2 <= r; r2++) {
                    if (m_rows[r2-1])
                        row++;
                }
                col = 0;
                for (c2 = 1; c2 <= c; c2++) {
                    if (m_cols[c2-1])
                        col++;
                }
                rowspan = 0;
                for (r2 = r ; r2 < m_nrows && cell(r2, c) == w; r2++) {
                    if (m_rows[r2])
                        rowspan++;
                }
                colspan = 0;
                for (c2 = c; c2 < m_ncols && cell(r, c2) == w; c2++) {
                    if (m_cols[c2])
                        colspan++;
                }
                return true;
            }
        }
    }
    return false;
}

void Grid::appendItem(ViewItem *w) {
//   qDebug() << "Appending Item to grid";

  for (int c = 0; c < m_ncols; c++) {
      for (int r = 0; r < m_nrows; r++) {
          if ((cell(r, c) == 0) && m_rows[r] && m_cols[c]) {
#if DEBUG_GRID_BUILDING
            qDebug() << "Found empty cell" << r << c;
#endif
            setCell(r, c, w);

            simplify();
            return;
          }
      }
  }

#if DEBUG_GRID_BUILDING
  qDebug() << "No empty cell located.  Appending to end.";
#endif
  if (cell(m_nrows - 1, 0) == 0) {
      setCell(m_nrows - 1, 0, w);

      simplify();
  }
}

}

// vim: ts=2 sw=2 et
