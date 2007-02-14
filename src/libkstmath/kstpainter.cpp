/***************************************************************************
                               kstpainter.cpp
                             -------------------
    begin                : Nov 25, 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "kstpainter.h"

KstPainter::KstPainter(PaintType t)
: QPainter(), _type(t), _drawInlineUI(false), _makingMask(false) {
}


KstPainter::~KstPainter() {
}


void KstPainter::setType(PaintType t) {
  _type = t;
}


KstPainter::PaintType KstPainter::type() const {
  return _type;
}


bool KstPainter::drawInlineUI() const {
  return _drawInlineUI;
}


void KstPainter::setDrawInlineUI(bool draw) {
  _drawInlineUI = draw;
}


bool KstPainter::makingMask() const {
  return _makingMask;
}


void KstPainter::setMakingMask(bool making) {
  _makingMask = making;
}


int KstPainter::lineWidthAdjustmentFactor() const {
  int factor = 1;
  
  if (type() == P_PRINT || type() == P_EXPORT) {
    const QRect& w(window());
    
    //
    // we want a line width of 1 to give a width of approximately 1 pt (where 1 pt = 1/72 
    //  inch) when printing on letter sized paper. Assuming a margin of 0.25 inch in one 
    //  direction (on both sides) and 0.5 inch in the other direction (again on both sides)
    //  and a printer resolution of 600 dpi:
    //
    //  w.width() + w.height() = ( 8 + 11 - ( 2 * 0.5 ) - ( 2 * 0.25 ) ) * 600 = 10500
    //
    //  we want a scale factor of 600/72 = 8.333 in the line width, so our magic number simply 
    //  becomes:
    //
    //  w.width() + w.height() / scale factor = 10200 / 8.333 ~ 1300
    //
    
    factor = (w.width() + w.height()) / 1300;
  }
  
  return factor > 0 ? factor : 1;
}

// vim: ts=2 sw=2 et
