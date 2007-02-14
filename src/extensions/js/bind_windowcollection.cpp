/***************************************************************************
                          bind_windowcollection.cpp
                             -------------------
    begin                : Mar 31 2005
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

#include "bind_windowcollection.h"
#include "bind_window.h"

#include <kst.h>

#include <kdebug.h>

KstBindWindowCollection::KstBindWindowCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "WindowCollection", true) {
}


KstBindWindowCollection::~KstBindWindowCollection() {
}


KJS::Value KstBindWindowCollection::length(KJS::ExecState *exec) const {
  return KJS::Number(collection(exec).count());
}


QStringList KstBindWindowCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  QStringList rc;
  KstApp *app = KstApp::inst();
  KMdiIterator<KMdiChildView*> *it = app->createIterator();
  if (it) {
    while (it->currentItem()) {
      rc += it->currentItem()->caption();
      it->next();
    }
    app->deleteIterator(it);
  }

  return rc;
}


KJS::Value KstBindWindowCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(item.qstring()));
  if (w) {
    return KJS::Object(new KstBindWindow(exec, w));
  }
  return KJS::Undefined();
}


KJS::Value KstBindWindowCollection::extract(KJS::ExecState *exec, unsigned item) const {
  QStringList wl = collection(exec);
  if (item >= wl.count()) {
    return KJS::Undefined();
  }

  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(wl[item]));
  if (w) {
    return KJS::Object(new KstBindWindow(exec, w));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
