/***************************************************************************
                                bind_axis.cpp
                             -------------------
    begin                : Jan 13 2006
    copyright            : (C) 2006 The University of Toronto
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

#include "bind_axis.h"
#include "bind_timeinterpretation.h"

#include <kst.h>
#include <kstdatacollection.h>
#include <kstplotlabel.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindAxis::KstBindAxis(KJS::ExecState *exec, Kst2DPlotPtr d, bool isX)
: QObject(), KstBinding("Axis", false), _d(d.data()), _xAxis(isX) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindAxis::KstBindAxis(int id)
: QObject(), KstBinding("Axis Method", id) {
}


KstBindAxis::~KstBindAxis() {
}


struct AxisBindings {
  const char *name;
  KJS::Value (KstBindAxis::*method)(KJS::ExecState*, const KJS::List&);
};


struct AxisProperties {
  const char *name;
  void (KstBindAxis::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindAxis::*get)(KJS::ExecState*) const;
};


static AxisBindings axisBindings[] = {
  { "scaleAuto", &KstBindAxis::scaleAuto },
  { "scaleAutoSpikeInsensitive", &KstBindAxis::scaleAutoSpikeInsensitive },
  { "scaleExpression", &KstBindAxis::scaleExpression },
  { "scaleRange", &KstBindAxis::scaleRange },
  { 0L, 0L }
};


static AxisProperties axisProperties[] = {
  { "log", &KstBindAxis::setLog, &KstBindAxis::log },
  { "suppressed", &KstBindAxis::setSuppressed, &KstBindAxis::suppressed },
  { "oppositeSuppressed", &KstBindAxis::setOppositeSuppressed, &KstBindAxis::oppositeSuppressed },
  { "offsetMode", &KstBindAxis::setOffsetMode, &KstBindAxis::offsetMode },
  { "reversed", &KstBindAxis::setReversed, &KstBindAxis::reversed },
  { "majorGridLines", &KstBindAxis::setMajorGridLines, &KstBindAxis::majorGridLines },
  { "minorGridLines", &KstBindAxis::setMinorGridLines, &KstBindAxis::minorGridLines },
  { "transformation", &KstBindAxis::setTransformation, &KstBindAxis::transformation },
  { "innerTicks", &KstBindAxis::setInnerTicks, &KstBindAxis::innerTicks },
  { "outerTicks", &KstBindAxis::setOuterTicks, &KstBindAxis::outerTicks },
  { "majorGridColor", &KstBindAxis::setMajorGridColor, &KstBindAxis::majorGridColor },
  { "minorGridColor", &KstBindAxis::setMinorGridColor, &KstBindAxis::minorGridColor },
  { "minorTickCount", &KstBindAxis::setMinorTickCount, &KstBindAxis::minorTickCount },
  { "majorTickDensity", &KstBindAxis::setMajorTickDensity, &KstBindAxis::majorTickDensity },
  { "scaleMode", 0L, &KstBindAxis::scaleMode },
  { "label", &KstBindAxis::setLabel, &KstBindAxis::label },
  { "type", 0L, &KstBindAxis::type },
  { "interpretation", 0L, &KstBindAxis::interpretation },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindAxis::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; axisProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(axisProperties[i].name)));
  }

  return rc;
}


bool KstBindAxis::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; axisProperties[i].name; ++i) {
    if (prop == axisProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindAxis::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; axisProperties[i].name; ++i) {
    if (prop == axisProperties[i].name) {
      if (!axisProperties[i].set) {
        break;
      }
      (this->*axisProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindAxis::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; axisProperties[i].name; ++i) {
    if (prop == axisProperties[i].name) {
      if (!axisProperties[i].get) {
        break;
      }
      return (this->*axisProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindAxis::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindAxis *imp = dynamic_cast<KstBindAxis*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*axisBindings[id - 1].method)(exec, args);
}


void KstBindAxis::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; axisBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindAxis(i + 1));
    obj.put(exec, axisBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindAxis::type(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_xAxis ? "X" : "Y");
}


KJS::Value KstBindAxis::label(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::String(_d->xLabel()->text());
  } else {
    return KJS::String(_d->yLabel()->text());
  }
}


void KstBindAxis::setLabel(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->xLabel()->setText(value.toString(exec).qstring());
  } else {
    _d->yLabel()->setText(value.toString(exec).qstring());
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::transformation(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::String(_d->xTransformedExp());
  } else {
    return KJS::String(_d->yTransformedExp());
  }
}


void KstBindAxis::setTransformation(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXTransformedExp(value.toString(exec).qstring());
  } else {
    _d->setYTransformedExp(value.toString(exec).qstring());
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::log(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->isXLog());
  } else {
    return KJS::Boolean(_d->isYLog());
  }
}


void KstBindAxis::setLog(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setLog(value.toBoolean(exec), _d->isYLog());
  } else {
    _d->setLog(_d->isXLog(), value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::majorGridLines(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->hasXMajorGrid());
  } else {
    return KJS::Boolean(_d->hasYMajorGrid());
  }
}


void KstBindAxis::setMajorGridLines(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXGridLines(value.toBoolean(exec), _d->hasXMinorGrid());
  } else {
    _d->setYGridLines(value.toBoolean(exec), _d->hasYMinorGrid());
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::minorGridLines(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->hasXMinorGrid());
  } else {
    return KJS::Boolean(_d->hasYMinorGrid());
  }
}


void KstBindAxis::setMinorGridLines(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXGridLines(_d->hasXMajorGrid(), value.toBoolean(exec));
  } else {
    _d->setYGridLines(_d->hasYMajorGrid(), value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::reversed(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->xReversed());
  } else {
    return KJS::Boolean(_d->yReversed());
  }
}


void KstBindAxis::setReversed(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXReversed(value.toBoolean(exec));
  } else {
    _d->setYReversed(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::offsetMode(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->xOffsetMode());
  } else {
    return KJS::Boolean(_d->yOffsetMode());
  }
}


void KstBindAxis::setOffsetMode(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXOffsetMode(value.toBoolean(exec));
  } else {
    _d->setYOffsetMode(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::suppressed(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->suppressBottom());
  } else {
    return KJS::Boolean(_d->suppressLeft());
  }
}


void KstBindAxis::setSuppressed(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setSuppressBottom(value.toBoolean(exec));
  } else {
    _d->setSuppressLeft(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::oppositeSuppressed(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->suppressTop());
  } else {
    return KJS::Boolean(_d->suppressRight());
  }
}


void KstBindAxis::setOppositeSuppressed(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setSuppressTop(value.toBoolean(exec));
  } else {
    _d->setSuppressRight(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::innerTicks(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->xTicksInPlot());
  } else {
    return KJS::Boolean(_d->yTicksInPlot());
  }
}


void KstBindAxis::setInnerTicks(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXTicksInPlot(value.toBoolean(exec));
  } else {
    _d->setYTicksInPlot(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::outerTicks(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Boolean(_d->xTicksOutPlot());
  } else {
    return KJS::Boolean(_d->yTicksOutPlot());
  }
}


void KstBindAxis::setOuterTicks(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  if (_xAxis) {
    _d->setXTicksOutPlot(value.toBoolean(exec));
  } else {
    _d->setYTicksOutPlot(value.toBoolean(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::majorGridColor(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  return KJSEmbed::convertToValue(exec, _d->majorGridColor());
}


void KstBindAxis::setMajorGridColor(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker rl(_d);
  _d->setGridLinesColor(cv.toColor(), _d->minorGridColor(), false, _d->defaultMinorGridColor());
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::minorGridColor(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  return KJSEmbed::convertToValue(exec, _d->minorGridColor());
}


void KstBindAxis::setMinorGridColor(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker rl(_d);
  _d->setGridLinesColor(_d->majorGridColor(), cv.toColor(), _d->defaultMajorGridColor(), false);
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::interpretation(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindTimeInterpretation(exec, const_cast<KstBindAxis*>(this))); // yuck
}


KJS::Value KstBindAxis::minorTickCount(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  if (_xAxis) {
    return KJS::Number(_d->xMinorTicks());
  } else {
    return KJS::Number(_d->yMinorTicks());
  }
}


void KstBindAxis::setMinorTickCount(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXMinorTicks(value.toInt32(exec));
  } else {
    _d->setYMinorTicks(value.toInt32(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::majorTickDensity(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  int i = 0;
  if (_xAxis) {
    i = _d->xMajorTicks();
  } else {
    i = _d->yMajorTicks();
  }
  switch (i) {
    case 2:
      i = 0;
      break;
    case 10:
      i = 2;
      break;
    case 15:
      i = 3;
      break;
    case 5:
    default:
      i = 1;
      break;
  }

  return KJS::Number(i);
}


void KstBindAxis::setMajorTickDensity(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }

  int i = value.toInt32(exec);
  switch (i) {
    case 0:
        i = 2;
      break;
    case 1:
        i = 5;
      break;
    case 2:
        i = 10;
      break;
    case 3:
        i = 15;
      break;
    default:
      KJS::Object eobj = KJS::Error::create(exec, KJS::RangeError);
      exec->setException(eobj);
      return;
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXMajorTicks(value.toInt32(exec));
  } else {
    _d->setYMajorTicks(value.toInt32(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindAxis::scaleMode(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d);
  KstScaleModeType i;
  if (_xAxis) {
    i = _d->xScaleMode();
  } else {
    i = _d->yScaleMode();
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Number(i);
}


KJS::Value KstBindAxis::scaleAuto(KJS::ExecState *exec, const KJS::List& args) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly zero arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXScaleMode(AUTO);
  } else {
    _d->setYScaleMode(AUTO);
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Undefined();
}


KJS::Value KstBindAxis::scaleAutoSpikeInsensitive(KJS::ExecState *exec, const KJS::List& args) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly zero arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXScaleMode(NOSPIKE);
  } else {
    _d->setYScaleMode(NOSPIKE);
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Undefined();
}


KJS::Value KstBindAxis::scaleExpression(KJS::ExecState *exec, const KJS::List& args) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly two arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType || args[1].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXScaleMode(EXPRESSION);
    _d->setXExpressions(args[0].toString(exec).qstring(), args[1].toString(exec).qstring());
  } else {
    _d->setYScaleMode(EXPRESSION);
    _d->setYExpressions(args[0].toString(exec).qstring(), args[1].toString(exec).qstring());
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Undefined();
}


KJS::Value KstBindAxis::scaleRange(KJS::ExecState *exec, const KJS::List& args) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly two arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::NumberType || args[1].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstWriteLocker rl(_d);
  if (_xAxis) {
    _d->setXScaleMode(FIXED);
    _d->setXScale(args[0].toNumber(exec), args[1].toNumber(exec));
  } else {
    _d->setYScaleMode(FIXED);
    _d->setYScale(args[0].toNumber(exec), args[1].toNumber(exec));
  }
  _d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
