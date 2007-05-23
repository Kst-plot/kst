/***************************************************************************
                          kstscalartable.cpp -  description
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

#include <qpainter.h>
#include "kstscalartable.h"
#include "kstdatacollection.h"

KstScalarTable::KstScalarTable( QWidget * parent, const char * name ) : Q3Table( parent, name ) {
}

void KstScalarTable::paintCell( QPainter* painter, int row, int col, const QRect& cr, bool selected, const QColorGroup& cg ) {
  KstScalarList::iterator it = KST::scalarList.begin();
  QString str;

  if (selected) {
    painter->eraseRect( 0, 0, cr.width(), cr.height() );
    painter->fillRect( 0, 0, cr.width(), cr.height(), cg.highlight() );
    painter->setPen(cg.highlightedText());
  } else {
    painter->eraseRect( 0, 0, cr.width(), cr.height() );
    painter->fillRect( 0, 0, cr.width(), cr.height(), cg.base() );
    painter->setPen(cg.text());
  }

  it += row;
  if( col == 0 ) {
    str = (*it)->tagName();
  } else if( col == 1 ) {
    str = (*it)->label();
  }

  painter->drawText(0, 0, cr.width(), cr.height(), Qt::AlignLeft, str);
}

// vim: ts=2 sw=2 et
