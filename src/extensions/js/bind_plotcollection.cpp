/***************************************************************************
                           bind_plotcollection.cpp
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

#include "bind_plotcollection.h"
#include "bind_plot.h"

#include <kstviewwindow.h>

#include <kdebug.h>

KstBindPlotCollection::KstBindPlotCollection(KJS::ExecState *exec, Kst2DPlotList plots)
: KstBindCollection(exec, "PlotCollection") {
  _isWindow = false;
  _plots = plots.tagNames();
}


KstBindPlotCollection::KstBindPlotCollection(KJS::ExecState *exec, KstViewWindow *w)
: KstBindCollection(exec, "PlotCollection") {
  _isWindow = true;
  _window = w->caption();
}


KstBindPlotCollection::~KstBindPlotCollection() {
}


KJS::Value KstBindPlotCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isWindow) {
    KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_window));
    if (!w) {
      return KJS::Number(0);
    }
    return KJS::Number(w->view()->findChildrenType<Kst2DPlot>(true).count());
  }
  return KJS::Number(_plots.count());
}


QStringList KstBindPlotCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isWindow) {
    KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_window));
    if (!w) {
      return QStringList();
    }
    Kst2DPlotList l = w->view()->findChildrenType<Kst2DPlot>(true);
    return l.tagNames();
  }

  return _plots;
}


KJS::Value KstBindPlotCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  Q_UNUSED(exec)
  Kst2DPlotList pl;
  if (_isWindow) {
    KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_window));
    if (!w) {
      return KJS::Undefined();
    }
    pl = w->view()->findChildrenType<Kst2DPlot>(true);
  } else {
    pl = Kst2DPlot::globalPlotList();
  }

  Kst2DPlotPtr p = *pl.findTag(item.qstring());
  if (p) {
    return KJS::Object(new KstBindPlot(exec, p));
  }
  return KJS::Undefined();
}


KJS::Value KstBindPlotCollection::extract(KJS::ExecState *exec, unsigned item) const {
  Q_UNUSED(exec)
  Kst2DPlotList pl;
  if (_isWindow) {
    KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(_window));
    if (!w) {
      return KJS::Undefined();
    }
    pl = w->view()->findChildrenType<Kst2DPlot>(true);
  } else {
    pl = Kst2DPlot::globalPlotList();
  }

  if (item >= pl.count()) {
    return KJS::Undefined();
  }
  Kst2DPlotPtr p = pl[item];
  return KJS::Object(new KstBindPlot(exec, p));
}


// vim: ts=2 sw=2 et
