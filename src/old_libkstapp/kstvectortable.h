/***************************************************************************
                          kstvectortable.h  -  description
                             -------------------
    begin                : Thu Mar 4 2004
    copyright            : (C) 2004 The University of British Columbia 
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTVECTORTABLE_H
#define KSTVECTORTABLE_H

#include <q3table.h>

class KstVectorTable : public Q3Table
{
  public:
    KstVectorTable( QWidget * parent = 0, const char * name = 0 );

    void setVector(QString strVector);
    void paintCell( QPainter * p, int row, int col, const QRect & cr, bool selected, const QColorGroup & cg );

    void resizeData(int) {}
    QWidget* createEditor(int, int, bool) const { return 0; }
    Q3TableItem* item(int, int) { return 0; }
    void setItem(int, int, Q3TableItem *) {}
    void clearCell(int, int) {}
    void insertWidget(int, int, QWidget *) {}
    QWidget* cellWidget(int, int) const { return 0; }
    void clearCellWidget(int, int) {}

  private:
    QString _strVector;
};

#endif
// vim: ts=2 sw=2 et
