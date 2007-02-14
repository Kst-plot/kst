/***************************************************************************
                           bind_curvecollection.cpp
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

#include "bind_curvecollection.h"
#include "bind_curve.h"

#include <kst.h>
#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindCurveCollection::KstBindCurveCollection(KJS::ExecState *exec, KstVCurveList curves)
: KstBindCollection(exec, "CurveCollection", true) {
  _isPlot = false;
  _curves = curves.tagNames();
}


KstBindCurveCollection::KstBindCurveCollection(KJS::ExecState *exec, KstViewLegendPtr p)
: KstBindCollection(exec, "CurveCollection", false) {
  _isPlot = false;
  p->readLock();
  _legend = p.data();
  p->unlock();
}


KstBindCurveCollection::KstBindCurveCollection(KJS::ExecState *exec, Kst2DPlotPtr p)
: KstBindCollection(exec, "CurveCollection", false) {
  _isPlot = true;
  p->readLock();
  _plot = p->tagName();
  p->unlock();
}


KstBindCurveCollection::~KstBindCurveCollection() {
}


KJS::Value KstBindCurveCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      return KJS::Number(0);
    }
    KstReadLocker rl(p);
    return KJS::Number(p->Curves.count());
  } else if (_legend) {
    return KJS::Number(_legend->curves().count());
  }
  return KJS::Number(_curves.count());
}


QStringList KstBindCurveCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      return QStringList();
    }
    KstReadLocker rl(p);
    return p->Curves.tagNames();
  } else if (_legend) {
    return _legend->curves().tagNames();
  }

  return _curves;
}


KJS::Value KstBindCurveCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstVCurveList cl;
  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      return KJS::Undefined();
    }
    KstReadLocker rl(p);
    cl = kstObjectSubList<KstBaseCurve,KstVCurve>(p->Curves);
  } else if (_legend) {
    cl = kstObjectSubList<KstBaseCurve,KstVCurve>(_legend->curves());
  } else {
    cl = kstObjectSubList<KstDataObject,KstVCurve>(KST::dataObjectList);
  }

  KstVCurvePtr c = *cl.findTag(item.qstring());
  if (c) {
    return KJS::Object(new KstBindCurve(exec, c));
  }
  return KJS::Undefined();
}


KJS::Value KstBindCurveCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstVCurveList cl;
  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      return KJS::Null();
    }
    KstReadLocker rl(p);
    cl = kstObjectSubList<KstBaseCurve,KstVCurve>(p->Curves);
  } else if (_legend) {
    cl = kstObjectSubList<KstBaseCurve,KstVCurve>(_legend->curves());
  } else {
    cl = kstObjectSubList<KstDataObject,KstVCurve>(KST::dataObjectList);
  }

  KstVCurvePtr c;
  if (item < cl.count()) {
    c = cl[item];
  }
  if (c) {
    return KJS::Object(new KstBindCurve(exec, c));
  }
  return KJS::Undefined();
}


KJS::Value KstBindCurveCollection::append(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_isPlot) {
    KstVCurvePtr c = extractVCurve(exec, args[0]);
    if (!c) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    KstWriteLocker rl(p);
    if (!p->Curves.contains(c.data())) {
      p->addCurve(c.data());
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    }
    return KJS::Undefined();
  } else if (_legend) {
    KstVCurvePtr c = extractVCurve(exec, args[0]);
    if (!c) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    KstWriteLocker rl(_legend);
    _legend->addCurve(c.data());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
    return KJS::Undefined();
  }

  return KstBindCollection::append(exec, args);
}


KJS::Value KstBindCurveCollection::remove(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    KstVCurvePtr c = extractVCurve(exec, args[0], false);
    if (c) {
      KstWriteLocker rl(p);
      if (p->Curves.contains(c.data())) {
        p->removeCurve(c.data());
        KstApp::inst()->paintAll(KstPainter::P_PAINT);
      }
    } else {
      unsigned i = 0;
      if (args[0].type() == KJS::NumberType && args[0].toUInt32(i)) {
        if (i >= p->Curves.count()) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::RangeError);
          exec->setException(eobj);
        } else {
          KstWriteLocker rl(p);
          p->removeCurve(p->Curves[i].data());
          KstApp::inst()->paintAll(KstPainter::P_PAINT);
        }
      } else {
        KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
        exec->setException(eobj);
      }
    }
    return KJS::Undefined();
  } else if (_legend) {
    KstVCurvePtr c = extractVCurve(exec, args[0], false);
    if (!c) {
      unsigned i = 0;
      if (args[0].type() == KJS::NumberType && args[0].toUInt32(i)) {
        if (i >= _legend->curves().count()) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::RangeError);
          exec->setException(eobj);
        } else {
          KstWriteLocker rl(_legend);
          _legend->removeCurve(_legend->curves()[i]);
          KstApp::inst()->paintAll(KstPainter::P_PAINT);
        }
      } else {
        KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
        exec->setException(eobj);
      }
    } else {
      KstWriteLocker rl(_legend);
      _legend->removeCurve(c.data());
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    }
    return KJS::Undefined();
  }

  return KstBindCollection::remove(exec, args);
}


KJS::Value KstBindCurveCollection::clear(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_isPlot) {
    Kst2DPlotPtr p = *Kst2DPlot::globalPlotList().findTag(_plot);
    if (!p) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    if (!p->Curves.isEmpty()) {
      KstWriteLocker rl(p);
      p->clearCurves();
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    }
    return KJS::Undefined();
  } else if (_legend) {
    KstWriteLocker rl(_legend);
    _legend->clear();
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
    return KJS::Undefined();
  }

  return KstBindCollection::clear(exec, args);
}


// vim: ts=2 sw=2 et
