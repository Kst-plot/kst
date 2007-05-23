/***************************************************************************
                 kstgfx2dplotmousehandler.cpp  -  description
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
#include "kstgfx2dplotmousehandler.h"
#include "kstgfxmousehandlerutils.h"
#include "kst2dplot.h"
#include "kstviewwidget.h"
#include "kstdefaultnames.h"
#include "kstdatacollection.h"

KstGfx2DPlotMouseHandler::KstGfx2DPlotMouseHandler()
: KstGfxMouseHandler() {
  // initial default settings before any sticky settings
  Kst2DPlotPtr default2DPlot = new Kst2DPlot;
/*  defaultBox->setBorderWidth(2);
  defaultBox->setBorderColor(Qt::black);
  defaultBox->setForegroundColor(Qt::white);
  defaultBox->setXRound(0);
  defaultBox->setYRound(0);*/
  _defaultObject = KstViewObjectPtr(default2DPlot);
}  


KstGfx2DPlotMouseHandler::~KstGfx2DPlotMouseHandler() {
}


void KstGfx2DPlotMouseHandler::pressMove(KstTopLevelViewPtr view, const QPoint& pos, bool shift, const QRect& geom) {
  if (_cancelled || !_mouseDown) {
    return;  
  }
  
  QRect old = _prevBand;
  
  _prevBand = KstGfxMouseHandlerUtils::newRect(pos, _mouseOrigin, geom, shift);

  if (old != _prevBand) {
    QPainter p;
    p.begin(view->widget());
    p.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    p.setCompositionMode(QPainter::CompositionMode_Destination);
    if (old.topLeft() != QPoint(-1, -1)) {
      p.drawRect(old);
    } 
    p.drawRect(_prevBand);
    p.end();
  }
}


void KstGfx2DPlotMouseHandler::releasePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(shift)
      
  if (!_mouseDown) {
    // if mouse was never down, pretend it wasn't released
    return;
  }
  _mouseDown = false;
      
  if (!_cancelled && _mouseOrigin != pos) {
    // make a new 2dplot
    Kst2DPlotPtr plot = new Kst2DPlot;
    copyDefaults(KstViewObjectPtr(plot));
    plot->setTagName(KstObjectTag(KST::suggestPlotName(), KstObjectTag::globalTagContext));  // FIXME: tag context
    plot->move(_prevBand.topLeft());
    plot->resize(_prevBand.size());
    KstViewObjectPtr container = view->findDeepestChild(_prevBand);
    if (!container) {
      container = view;
    }
    container->appendChild(KstViewObjectPtr(plot));
    KstApp::inst()->document()->setModified();
    KstApp::inst()->updateViewManager(true);
    view->paint(KstPainter::P_PAINT);
  }
  _prevBand = QRect(-1, -1, 0, 0);
}


// vim: ts=2 sw=2 et
