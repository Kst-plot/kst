/***************************************************************************
                          kstfittable.cpp -  description
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
#include <q3table.h>
#include "kstfittable.h"

KstFitTable::KstFitTable( QWidget * parent, const char * name ) : Q3Table( parent, name ) {
  _pdParams = 0L;
  _pdCovars = 0L;
  _iNumParams = 0;
  _iNumCovars = 0;
  _dChi2Nu    = 0.0;
}


KstFitTable::~KstFitTable() {
  delete _pdParams;
  delete _pdCovars;
}


void KstFitTable::setParameters(double* pParams, int iNumParams, double* pCovars, int iNumCovars, double dChi2Nu) {
  delete _pdParams;
  delete _pdCovars;
  _iNumParams = iNumParams;
  _pdParams   = pParams;
  _iNumCovars = iNumCovars;
  _pdCovars   = pCovars;
  _dChi2Nu    = dChi2Nu;

  setNumRows(iNumParams + 2);
  setNumCols(iNumParams + 2);
}


void KstFitTable::paintCell( QPainter* painter, int row, int col, const QRect& cr, bool selected, const QColorGroup& cg ) {
  QString str;

  if (selected) {
    painter->eraseRect(0, 0, cr.width(), cr.height());
    painter->fillRect(0, 0, cr.width(), cr.height(), cg.highlight());
    painter->setPen(cg.highlightedText());
  } else {
    painter->eraseRect(0, 0, cr.width(), cr.height());
    painter->fillRect(0, 0, cr.width(), cr.height(), cg.base());
    painter->setPen(cg.text());
  }

  if (row < _iNumParams) {
    if (col == 0) {
      if (_pdParams && row <= _iNumParams) {
        str.setNum(_pdParams[row], 'g', 6);
      }
    } else if (col >= 2) {
      if (_pdCovars && row <= _iNumCovars) {
        str.setNum(_pdCovars[(row*_iNumParams)+(col-2)], 'g', 6);
      }
    }
  } else if (row == _iNumParams+1 && col == 0) {
    str.setNum(_dChi2Nu, 'g', 6);
  }    

  painter->drawText(0, 0, cr.width(), cr.height(), Qt::AlignLeft, str);
}

// vim: ts=2 sw=2 et
