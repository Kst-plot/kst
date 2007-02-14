/***************************************************************************
                                bind_plot.cpp
                             -------------------
    begin                : Mar 30 2005
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

#include "bind_plot.h"
#include "bind_axis.h"
#include "bind_curvecollection.h"
#include "bind_legend.h"

#include <kst2dplot.h>
#include <kstplotlabel.h>
#include <ksttoplevelview.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindPlot::KstBindPlot(KJS::ExecState *exec, Kst2DPlotPtr d)
: KstBindBorderedViewObject(exec, d.data(), "Plot") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPlot::KstBindPlot(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindBorderedViewObject(exec, globalObject, "Plot") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    //globalObject->put(exec, "Plot", o);
    KstBindBorderedViewObject::addFactory("Plot", KstBindPlot::bindFactory);
  }
}


KstBindViewObject *KstBindPlot::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  Kst2DPlotPtr v = kst_cast<Kst2DPlot>(obj);
  if (v) {
    return new KstBindPlot(exec, v);
  }
  return 0L;
}


KstBindPlot::KstBindPlot(int id)
: KstBindBorderedViewObject(id, "Plot Method") {
}


KstBindPlot::~KstBindPlot() {
}


KJS::Object KstBindPlot::construct(KJS::ExecState *exec, const KJS::List& args) {
  KstViewWindow *w = 0L;
  if (args.size() == 1) {
    w = extractWindow(exec, args[0]);
    if (!w) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  } else {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  QString n = w->createObject<Kst2DPlot>(KST::suggestPlotName(), false);
  Kst2DPlotPtr p = *w->view()->findChildrenType<Kst2DPlot>(true).findTag(n);
  if (!p) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Object();
  }

  w->view()->paint(KstPainter::P_PAINT);

  return KJS::Object(new KstBindPlot(exec, p));
}


struct PlotBindings {
  const char *name;
  KJS::Value (KstBindPlot::*method)(KJS::ExecState*, const KJS::List&);
};


struct PlotProperties {
  const char *name;
  void (KstBindPlot::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPlot::*get)(KJS::ExecState*) const;
};


static PlotBindings plotBindings[] = {
  { "createLegend", &KstBindPlot::createLegend },
  { 0L, 0L }
};


static PlotProperties plotProperties[] = {
  { "curves", 0L, &KstBindPlot::curves },
  { "legend", 0L, &KstBindPlot::legend },
  { "topLabel", &KstBindPlot::setTopLabel, &KstBindPlot::topLabel },
  { "xAxis", 0L, &KstBindPlot::xAxis },
  { "yAxis", 0L, &KstBindPlot::yAxis },
  { "tied", &KstBindPlot::setTied, &KstBindPlot::tied },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindPlot::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindBorderedViewObject::propList(exec, recursive);
  
  for (int i = 0; plotProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(plotProperties[i].name)));
  }

  return rc;
}


bool KstBindPlot::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; plotProperties[i].name; ++i) {
    if (prop == plotProperties[i].name) {
      return true;
    }
  }

  return KstBindBorderedViewObject::hasProperty(exec, propertyName);
}


void KstBindPlot::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindBorderedViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; plotProperties[i].name; ++i) {
    if (prop == plotProperties[i].name) {
      if (!plotProperties[i].set) {
        break;
      }
      (this->*plotProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindBorderedViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPlot::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindBorderedViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; plotProperties[i].name; ++i) {
    if (prop == plotProperties[i].name) {
      if (!plotProperties[i].get) {
        break;
      }
      return (this->*plotProperties[i].get)(exec);
    }
  }
  
  return KstBindBorderedViewObject::get(exec, propertyName);
}


KJS::Value KstBindPlot::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindBorderedViewObject::methodCount();
  if (id > start) {
    KstBindPlot *imp = dynamic_cast<KstBindPlot*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    return (imp->*plotBindings[id - start - 1].method)(exec, args);
  }

  return KstBindBorderedViewObject::call(exec, self, args);
}


void KstBindPlot::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindBorderedViewObject::methodCount();
  for (int i = 0; plotBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPlot(i + start + 1));
    obj.put(exec, plotBindings[i].name, o, KJS::Function);
  }
}

int KstBindPlot::methodCount() const {
  return sizeof plotBindings + KstBindBorderedViewObject::methodCount();
}


int KstBindPlot::propertyCount() const {
  return sizeof plotProperties + KstBindBorderedViewObject::propertyCount();
}


#define makePlot(X) dynamic_cast<Kst2DPlot*>(const_cast<KstObject*>(X.data()))

KJS::Value KstBindPlot::curves(KJS::ExecState *exec) const {
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindCurveCollection(exec, d));
  }
  return KJS::Null();
}


KJS::Value KstBindPlot::legend(KJS::ExecState *exec) const {
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    KstViewLegendPtr vl = d->legend();
    if (vl) {
      return KJS::Object(new KstBindLegend(exec, vl));
    }
  }
  return KJS::Null();
}


void KstBindPlot::setTopLabel(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->topLabel()->setText(value.toString(exec).qstring());
  }
}


KJS::Value KstBindPlot::topLabel(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->topLabel()->text());
  }
  return KJS::String();
}


KJS::Value KstBindPlot::xAxis(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindAxis(exec, d, true));
  }
  return KJS::Object();
}


KJS::Value KstBindPlot::yAxis(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindAxis(exec, d, false));
  }
  return KJS::Object();
}


KJS::Value KstBindPlot::createLegend(KJS::ExecState *exec, const KJS::List& args) {
  Kst2DPlotPtr d = makePlot(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstWriteLocker rl(d);
  KstViewLegendPtr vl = d->getOrCreateLegend();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindLegend(exec, vl));
}


void KstBindPlot::setTied(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setTied(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindPlot::tied(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  Kst2DPlotPtr d = makePlot(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->isTied());
  }
  return KJS::Boolean(false);
}


#undef makePlot

// vim: ts=2 sw=2 et
