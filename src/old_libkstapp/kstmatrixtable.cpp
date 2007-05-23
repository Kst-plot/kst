/***************************************************************************
                          kstmatrixtable.cpp -  description
                             -------------------
    begin                : Thu Mar 24 2005
    copyright            : (C) 2005 The University of British Columbia 
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

// includes files for Qt
#include <qpainter.h>
#include <q3table.h>

// include files for KDE
#include "kstmatrix.h"
#include "kstmatrixtable.h"
#include "kstdatacollection.h"

KstMatrixTable::KstMatrixTable( QWidget * parent, const char * name ) : Q3Table( parent, name ) {
}

void KstMatrixTable::setMatrix(QString strMatrix) {
  _strMatrix = strMatrix;
}

void KstMatrixTable::paintCell( QPainter* painter, int row, int col, const QRect& cr, bool selected, const QColorGroup& cg ) {
  KstMatrixPtr matrix = *KST::matrixList.findTag(_strMatrix);
  QString str;
  double value;
  
  painter->eraseRect( 0, 0, cr.width(), cr.height() );
  if (selected) {
    painter->fillRect( 0, 0, cr.width(), cr.height(), cg.highlight() );
    painter->setPen(cg.highlightedText());
  } else {
    painter->fillRect( 0, 0, cr.width(), cr.height(), cg.base() );
    painter->setPen(cg.text());
  }

  if (matrix) {
    bool ok;
    value = matrix->valueRaw(col, row, &ok);
    if (ok) {
      str.setNum(value, 'g', 16);
    }
  }

  painter->drawText(0, 0, cr.width(), cr.height(), Qt::AlignLeft, str);
}

// vim: ts=2 sw=2 et
