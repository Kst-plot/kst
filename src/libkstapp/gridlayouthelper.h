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
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* These classes were taken from Qt designer in Qt 4.3 and adapted to work
with our layouts and items.*/

#ifndef GRIDLAYOUTHELPER_H
#define GRIDLAYOUTHELPER_H

#include "viewitem.h"
#include <QDebug>

namespace Kst {

class Grid
{
public:
    static Grid *buildGrid(const QList<ViewItem*> &itemList);
    static Grid *buildGrid(const QList<ViewItem*> &itemList, int columns);

    Grid(int rows, int cols);
    ~Grid();

    ViewItem* cell(int row, int col) const { return m_cells[ row * m_ncols + col]; }
    bool setCell(int row, int col, ViewItem* w);
    void setCells(QRect c, ViewItem* w);
    int numRows() const { return m_nrows; }
    int numCols() const { return m_ncols; }

    void simplify();
    bool locateWidget(ViewItem* w, int& row, int& col, int& rowspan, int& colspan);
    void appendItem(ViewItem *w);

private:
    void merge();
    int countRow(int r, int c) const;
    int countCol(int r, int c) const;
    void setRow(int r, int c, ViewItem* w, int count);
    void setCol(int r, int c, ViewItem* w, int count);
    bool isWidgetStartCol(int c) const;
    bool isWidgetEndCol(int c) const;
    bool isWidgetStartRow(int r) const;
    bool isWidgetEndRow(int r) const;
    bool isWidgetTopLeft(int r, int c) const;
    void extendLeft();
    void extendRight();
    void extendUp();
    void extendDown();

    const int m_nrows;
    const int m_ncols;

    ViewItem** m_cells;
    bool* m_cols;
    bool* m_rows;
};

}

#endif

// vim: ts=2 sw=2 et
