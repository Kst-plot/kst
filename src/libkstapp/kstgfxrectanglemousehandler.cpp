/***************************************************************************
                 kstgfxrectanglemousehandler.cpp  -  description
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 by University of British Columbia
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

#include <stdlib.h>
 
#include <qpainter.h>

#include "kst.h"
#include "kstdoc.h"
#include "kstgfxrectanglemousehandler.h"
#include "kstgfxmousehandlerutils.h"
#include "kstviewbox.h"
#include "kstviewwidget.h"

KstGfxRectangleMouseHandler::KstGfxRectangleMouseHandler()
: KstGfxMouseHandler() {
  // initial default settings before any sticky settings
  KstViewBoxPtr defaultBox = new KstViewBox;
  defaultBox->setBorderWidth(2);
  defaultBox->setBorderColor(Qt::black);
  defaultBox->setForegroundColor(Qt::white);
  defaultBox->setXRound(0);
  defaultBox->setYRound(0);
  _defaultObject = KstViewObjectPtr(defaultBox);
}  


KstGfxRectangleMouseHandler::~KstGfxRectangleMouseHandler() {
}


void KstGfxRectangleMouseHandler::pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom) {
  if (_cancelled || !_mouseDown) {
    return;  
  }
  
  QRect old = _prevBand;
  
  _prevBand = KstGfxMouseHandlerUtils::newRect(pos, _mouseOrigin, geom, shift);

  if (old != _prevBand) {
    QPainter p;
    p.begin(view->widget());
    p.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    p.setRasterOp(Qt::NotROP);
    if (old.topLeft() != QPoint(-1, -1)) {
      p.drawRect(old);
    } 
    p.drawRect(_prevBand);
    p.end();
  }
}


void KstGfxRectangleMouseHandler::releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(shift)
      
  if (!_mouseDown) {
    // if mouse was never down, pretend it wasn't released
    return;
  }
  _mouseDown = false;
      
  if (!_cancelled && _mouseOrigin != pos) {
    // make a new rectangle
    KstViewBoxPtr box = new KstViewBox;
    copyDefaults(KstViewObjectPtr(box));
    box->move(_prevBand.topLeft());
    box->resize(_prevBand.size());
    KstViewObjectPtr container = view->findDeepestChild(_prevBand);
    if (!container) {
      container = view;
    }
    container->appendChild(KstViewObjectPtr(box));
    KstApp::inst()->document()->setModified();
    KstApp::inst()->updateViewManager(true);
    view->paint(KstPainter::P_PAINT);
  }
  _prevBand = QRect(-1, -1, 0, 0);
}


// vim: ts=2 sw=2 et
