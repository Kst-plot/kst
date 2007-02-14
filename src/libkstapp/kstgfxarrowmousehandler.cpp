/***************************************************************************
                 kstgfxarrowmousehandler.cpp  -  description
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

#include "kstgfxarrowmousehandler.h"
#include "kstgfxmousehandlerutils.h"
#include "kst.h"
#include "kstdoc.h"
#include "kstviewarrow.h"
#include "kstviewwidget.h"

KstGfxArrowMouseHandler::KstGfxArrowMouseHandler()
: KstGfxMouseHandler() {
  // initial default settings before any sticky settings
  KstViewArrowPtr defaultArrow = new KstViewArrow;
  defaultArrow->setHasToArrow(true);
  defaultArrow->setHasFromArrow(false);
  defaultArrow->setToArrowScaling(1);
  defaultArrow->setWidth(2);
  defaultArrow->setPenStyle(Qt::SolidLine);
  defaultArrow->setForegroundColor(Qt::black);
  _defaultObject = KstViewObjectPtr(defaultArrow);
}


KstGfxArrowMouseHandler::~KstGfxArrowMouseHandler() {

}


void KstGfxArrowMouseHandler::pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom) {
  Q_UNUSED(geom)
  if (_cancelled || !_mouseDown) {
    return;
  }

  QRect old = _prevBand;

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


void KstGfxArrowMouseHandler::releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(shift)

  if (!_mouseDown) {
    // if mouse was never down, pretend it wasn't released
    return;
  }
  _mouseDown = false;

  // make a new _drawingArrow using defaults
  if (!_cancelled && _mouseOrigin != pos) {
    KstViewArrowPtr arrow = new KstViewArrow;
    copyDefaults(KstViewObjectPtr(arrow));
    arrow->setFrom(_prevBand.topLeft());
    arrow->setTo(_prevBand.bottomRight());
    KstViewObjectPtr container = view->findDeepestChild(_prevBand.normalize());
    if (!container) {
      container = view;
    }
    container->appendChild(KstViewObjectPtr(arrow));
    KstApp::inst()->document()->setModified();
    KstApp::inst()->updateViewManager(true);
    view->paint(KstPainter::P_PAINT);
  }
  _prevBand = QRect(-1, -1, 0, 0);
}


// vim: ts=2 sw=2 et
