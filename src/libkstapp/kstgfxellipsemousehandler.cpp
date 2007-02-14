/***************************************************************************
                 kstgfxellipsemousehandler.cpp  -  description
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

#include "kstgfxellipsemousehandler.h"
#include "kstgfxmousehandlerutils.h"
#include "kst.h"
#include "kstdoc.h"
#include "kstviewellipse.h"
#include "kstviewwidget.h"

KstGfxEllipseMouseHandler::KstGfxEllipseMouseHandler()
: KstGfxMouseHandler() {
  // initial default settings before any sticky settings
  KstViewEllipsePtr defaultEllipse = new KstViewEllipse;
  defaultEllipse->setBorderWidth(2);
  defaultEllipse->setBorderColor(Qt::black);
  defaultEllipse->setForegroundColor(Qt::white);
  _defaultObject = KstViewObjectPtr(defaultEllipse);
}


KstGfxEllipseMouseHandler::~KstGfxEllipseMouseHandler() {

}


void KstGfxEllipseMouseHandler::pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom) {
  if (_cancelled || !_mouseDown) {
    return;  
  }
  
  QRect old = _prevBand;
  
  _prevBand = KstGfxMouseHandlerUtils::newRectCentered(pos, _mouseOrigin, geom, shift);

  if (old != _prevBand) {
    QPainter p;
    p.begin(view->widget());
    p.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    p.setRasterOp(Qt::NotROP);
    if (old.topLeft() != QPoint(-1, -1)) {
      p.drawEllipse(old);
    }
    p.drawEllipse(_prevBand);
    p.end();
  }
}


void KstGfxEllipseMouseHandler::releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(shift)

  if (!_mouseDown) {
    // if mouse was never down, pretend it wasn't released
    return;
  }
  _mouseDown = false;

  if (!_cancelled && _mouseOrigin != pos) {
    KstViewEllipsePtr ellipse = new KstViewEllipse;
    copyDefaults(KstViewObjectPtr(ellipse));
    ellipse->move(_prevBand.topLeft());
    ellipse->resize(_prevBand.size());
    KstViewObjectPtr container = view->findDeepestChild(_prevBand);
    if (!container) {
      container = view;
    }
    container->appendChild(KstViewObjectPtr(ellipse));
    KstApp::inst()->document()->setModified();
    KstApp::inst()->updateViewManager(true);
    view->paint(KstPainter::P_PAINT);
  }
  _prevBand = QRect(-1, -1, 0, 0);
}



// vim: ts=2 sw=2 et
