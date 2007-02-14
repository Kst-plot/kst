/***************************************************************************
                          kstgfxmousehandler.cpp  -  description
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

#include <qmetaobject.h>
#include <qvariant.h>

#include "kstgfxmousehandler.h"
#include "ksttoplevelview.h"
#include "kstviewobject.h"

KstGfxMouseHandler::KstGfxMouseHandler() {
  _mouseOrigin = QPoint(-1,-1);
  _mouseMoved = false;
  _mouseDown = false;
}


KstGfxMouseHandler::~KstGfxMouseHandler() {
}


void KstGfxMouseHandler::handlePress(KstTopLevelViewPtr view, const QPoint& pos, bool shift) {
  Q_UNUSED(view)
  Q_UNUSED(shift)
  _mouseOrigin = pos;
  _mouseMoved = false;
  _cancelled = false;
  _mouseDown = true;
}


void KstGfxMouseHandler::saveDefaults(KstViewObjectPtr obj) {
  _defaultObject = obj;
}


void KstGfxMouseHandler::copyDefaults(KstViewObjectPtr newObj) {
  if (_defaultObject) {
    int numProperties = _defaultObject->metaObject()->numProperties(true);
    for (int i = 0; i < numProperties; i++) {
      const QMetaProperty* property = _defaultObject->metaObject()->property(i, true);
      newObj->setProperty(property->name(), _defaultObject->property(property->name()));
    }
  }
}


void KstGfxMouseHandler::cancelMouseOperations(KstTopLevelViewPtr view) {
  _cancelled = true;
  view->paint(KstPainter::P_PAINT);
}


void KstGfxMouseHandler::updateFocus(KstTopLevelViewPtr view, const QPoint& pos) {
  Q_UNUSED(view)
  Q_UNUSED(pos)
}

// vim: ts=2 sw=2 et
