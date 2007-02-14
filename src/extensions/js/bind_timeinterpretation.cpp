/***************************************************************************
                          bind_timeinterpretation.cpp
                             -------------------
    begin                : Jan 16 2006
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

#include "bind_timeinterpretation.h"

#include <kstdatacollection.h>

#include <kst.h>
#include <kdebug.h>

KstBindTimeInterpretation::KstBindTimeInterpretation(KJS::ExecState *exec, KstBindAxis *d)
: KstBinding("TimeInterpretation", false), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindTimeInterpretation::KstBindTimeInterpretation(int id)
: KstBinding("TimeInterpretation Method", id) {
}


KstBindTimeInterpretation::~KstBindTimeInterpretation() {
}


struct TimeInterpretationBindings {
  const char *name;
  KJS::Value (KstBindTimeInterpretation::*method)(KJS::ExecState*, const KJS::List&);
};


struct TimeInterpretationProperties {
  const char *name;
  void (KstBindTimeInterpretation::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindTimeInterpretation::*get)(KJS::ExecState*) const;
};


static TimeInterpretationBindings timeInterpretationBindings[] = {
  { 0L, 0L }
};


static TimeInterpretationProperties timeInterpretationProperties[] = {
  { "active", &KstBindTimeInterpretation::setActive, &KstBindTimeInterpretation::active },
  { "axisType", 0L, &KstBindTimeInterpretation::axisType },
  { "input", &KstBindTimeInterpretation::setInput, &KstBindTimeInterpretation::input },
  { "output", &KstBindTimeInterpretation::setOutput, &KstBindTimeInterpretation::output },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindTimeInterpretation::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; timeInterpretationProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(timeInterpretationProperties[i].name)));
  }

  return rc;
}


bool KstBindTimeInterpretation::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; timeInterpretationProperties[i].name; ++i) {
    if (prop == timeInterpretationProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindTimeInterpretation::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; timeInterpretationProperties[i].name; ++i) {
    if (prop == timeInterpretationProperties[i].name) {
      if (!timeInterpretationProperties[i].set) {
        break;
      }
      (this->*timeInterpretationProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindTimeInterpretation::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; timeInterpretationProperties[i].name; ++i) {
    if (prop == timeInterpretationProperties[i].name) {
      if (!timeInterpretationProperties[i].get) {
        break;
      }
      return (this->*timeInterpretationProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindTimeInterpretation::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindTimeInterpretation *imp = dynamic_cast<KstBindTimeInterpretation*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*timeInterpretationBindings[id - 1].method)(exec, args);
}


void KstBindTimeInterpretation::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; timeInterpretationBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindTimeInterpretation(i + 1));
    obj.put(exec, timeInterpretationBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindTimeInterpretation::axisType(KJS::ExecState *exec) const {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  return _d->type(exec);
}


KJS::Value KstBindTimeInterpretation::active(KJS::ExecState *exec) const {
  if (!_d || !_d->_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp;
  KstAxisDisplay disp;
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
  }
  return KJS::Boolean(isIt);
}


void KstBindTimeInterpretation::setActive(KJS::ExecState *exec, const KJS::Value& value) {
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
  KstWriteLocker wl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp;
  KstAxisDisplay disp;
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
    _d->_d->setXAxisInterpretation(value.toBoolean(exec), interp, disp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
    _d->_d->setYAxisInterpretation(value.toBoolean(exec), interp, disp);
  }
  _d->_d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindTimeInterpretation::input(KJS::ExecState *exec) const {
  if (!_d || !_d->_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp;
  KstAxisDisplay disp;
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
  }
  int rc = 0;
  switch (interp) {
    case AXIS_INTERP_YEAR:
      rc = 5;
      break;
    case AXIS_INTERP_CTIME:
      rc = 0;
      break;
    case AXIS_INTERP_JD:
      rc = 2;
      break;
    case AXIS_INTERP_MJD:
      rc = 3;
      break;
    case AXIS_INTERP_RJD:
      rc = 4;
      break;
    case AXIS_INTERP_AIT:
      rc = 1;
      break;
    default:
      break;
  }
  return KJS::Number(rc);
}


void KstBindTimeInterpretation::setInput(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp, newInterp;
  KstAxisDisplay disp;
  switch (i) {
    default:
    case 0:
      newInterp = AXIS_INTERP_CTIME;
      break;
    case 1:
      newInterp = AXIS_INTERP_AIT;
      break;
    case 2:
      newInterp = AXIS_INTERP_JD;
      break;
    case 3:
      newInterp = AXIS_INTERP_MJD;
      break;
    case 4:
      newInterp = AXIS_INTERP_RJD;
      break;
    case 5:
      newInterp = AXIS_INTERP_YEAR;
      break;
  }
    
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
    _d->_d->setXAxisInterpretation(isIt, newInterp, disp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
    _d->_d->setYAxisInterpretation(isIt, newInterp, disp);
  }
  _d->_d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}


KJS::Value KstBindTimeInterpretation::output(KJS::ExecState *exec) const {
  if (!_d || !_d->_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstReadLocker rl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp;
  KstAxisDisplay disp;
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
  }
  int rc = 0;
  switch (disp) {
    case AXIS_DISPLAY_DDMMYYHHMMSS_SS:
      rc = 0;
      break;
    case AXIS_DISPLAY_YYMMDDHHMMSS_SS:
      rc = 1;
      break;
    case AXIS_DISPLAY_JD:
      rc = 2;
      break;
    case AXIS_DISPLAY_MJD:
      rc = 3;
      break;
    case AXIS_DISPLAY_RJD:
      rc = 4;
      break;
    case AXIS_DISPLAY_YEAR:
      rc = 5;
      break;
    case AXIS_DISPLAY_KDE_SHORTDATE:
      rc = 6;
      break;
    case AXIS_DISPLAY_KDE_LONGDATE:
      rc = 7;
      break;
    default:
      break;
  }
  return KJS::Number(rc);
}


void KstBindTimeInterpretation::setOutput(KJS::ExecState *exec, const KJS::Value& value) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return;
  }
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d->_d);
  bool isIt;
  KstAxisInterpretation interp;
  KstAxisDisplay disp, newDisp;
  switch (i) {
    default:
    case 0:
      newDisp = AXIS_DISPLAY_DDMMYYHHMMSS_SS;
      break;
    case 1:
      newDisp = AXIS_DISPLAY_YYMMDDHHMMSS_SS;
      break;
    case 2:
      newDisp = AXIS_DISPLAY_JD;
      break;
    case 3:
      newDisp = AXIS_DISPLAY_MJD;
      break;
    case 4:
      newDisp = AXIS_DISPLAY_RJD;
      break;
    case 5:
      newDisp = AXIS_DISPLAY_YEAR;
      break;
    case 6:
      newDisp = AXIS_DISPLAY_KDE_SHORTDATE;
      break;
    case 7:
      newDisp = AXIS_DISPLAY_KDE_LONGDATE;
      break;
  }
    
  if (_d->_xAxis) {
    _d->_d->getXAxisInterpretation(isIt, interp, disp);
    _d->_d->setXAxisInterpretation(isIt, interp, newDisp);
  } else {
    _d->_d->getYAxisInterpretation(isIt, interp, disp);
    _d->_d->setYAxisInterpretation(isIt, interp, newDisp);
  }
  _d->_d->setDirty();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
}

// vim: ts=2 sw=2 et
