/***************************************************************************
                 kstgfxlinemousehandler.cpp  -  description
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

#include "kstgfxlinemousehandler.h"
#include "kstgfxmousehandlerutils.h"
#include "kst.h"
#include "ksdebug.h"
#include "kstdoc.h"
#include "kstviewline.h"
#include "kstviewwidget.h"

KstGfxLineMouseHandler::KstGfxLineMouseHandler()
: KstGfxMouseHandler() {
  // initial default settings before any sticky settings
  KstViewLinePtr defaultLine = new KstViewLine;
  defaultLine->setWidth(2);
  defaultLine->setPenStyle(Qt::SolidLine);
  defaultLine->setForegroundColor(Qt::black);
  _defaultObject = KstViewObjectPtr(defaultLine);
}  


KstGfxLineMouseHandler::~KstGfxLineMouseHandler() {
}


void KstGfxLineMouseHandler::pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom) {
  Q_UNUSED(geom)
  if (_cancelled || !_mouseDown) {
    return;  
  }

  const QRect old(_prevBand);

  _prevBand = KstGfxMouseHandlerUtils::newLine(pos, _mouseOrigin, shift, view->geometry());
  
  if (old != _prevBand) {
    QPainter p;
    p.begin(view->widget());
    p.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    p.setRasterOp(Qt::NotROP);
    if (old.topLeft() != QPoint(-1, -1)) {
      p.drawLine(old.topLeft(), old.bottomRight());
    } 
    p.drawLine(_prevBand.topLeft(), _prevBand.bottomRight());
    p.end();
  }
}


void KstGfxLineMouseHandler::releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(shift)
  
  if (!_mouseDown) {
    // if mouse was never down, pretend it wasn't released
    return;
  }
  _mouseDown = false;
      
  if (!_cancelled && _mouseOrigin != pos) {
    // make a new line
    KstViewLinePtr line = new KstViewLine;
    copyDefaults(KstViewObjectPtr(line));
    line->setFrom(_prevBand.topLeft());
    line->setTo(_prevBand.bottomRight());
    KstViewObjectPtr container = view->findDeepestChild(_prevBand.normalize());
    if (!container) {
      container = view;
    }
    container->appendChild(KstViewObjectPtr(line));
    KstApp::inst()->document()->setModified();
    KstApp::inst()->updateViewManager(true);
    view->paint(KstPainter::P_PAINT);
  }
  _prevBand = QRect(-1, -1, 0, 0);
}

// vim: ts=2 sw=2 et
