/***************************************************************************
                            bind_colorsequence.cpp
                             -------------------
    begin                : Apr 11 2005
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

#include "bind_colorsequence.h"

#include <kstcolorsequence.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindColorSequence::KstBindColorSequence(KJS::ExecState *exec)
: KstBinding("ColorSequence") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindColorSequence::KstBindColorSequence(int id)
: KstBinding("ColorSequence Method", id) {
}


KstBindColorSequence::~KstBindColorSequence() {
}


KJS::Object KstBindColorSequence::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindColorSequence(exec));
}


struct ColorSequenceBindings {
  const char *name;
  KJS::Value (KstBindColorSequence::*method)(KJS::ExecState*, const KJS::List&);
};


struct ColorSequenceProperties {
  const char *name;
  void (KstBindColorSequence::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindColorSequence::*get)(KJS::ExecState*) const;
};


static ColorSequenceBindings colorSequenceBindings[] = {
  { "next", &KstBindColorSequence::next },
  { "tooClose", &KstBindColorSequence::tooClose },
  { 0L, 0L }
};


static ColorSequenceProperties colorSequenceProperties[] = {
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindColorSequence::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; colorSequenceProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(colorSequenceProperties[i].name)));
  }

  return rc;
}


bool KstBindColorSequence::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; colorSequenceProperties[i].name; ++i) {
    if (prop == colorSequenceProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindColorSequence::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; colorSequenceProperties[i].name; ++i) {
    if (prop == colorSequenceProperties[i].name) {
      if (!colorSequenceProperties[i].set) {
        break;
      }
      (this->*colorSequenceProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindColorSequence::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; colorSequenceProperties[i].name; ++i) {
    if (prop == colorSequenceProperties[i].name) {
      if (!colorSequenceProperties[i].get) {
        break;
      }
      return (this->*colorSequenceProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindColorSequence::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindColorSequence *imp = dynamic_cast<KstBindColorSequence*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*colorSequenceBindings[id - 1].method)(exec, args);
}


void KstBindColorSequence::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; colorSequenceBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindColorSequence(i + 1));
    obj.put(exec, colorSequenceBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindColorSequence::next(KJS::ExecState *exec, const KJS::List& args) {
  QColor rc;
  switch (args.size()) {
    case 0:
      rc = KstColorSequence::next();
      break;
    case 1:
      {
        QVariant prev = KJSEmbed::convertToVariant(exec, args[0]);
        if (!prev.canCast(QVariant::Color)) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
          exec->setException(eobj);
          return KJS::Undefined();
        }
        rc = KstColorSequence::next(prev.toColor());
      }
      break;
    case 2:
      {
        KstBaseCurveList bcl = KstBinding::extractCurveList(exec, args[1]);
        // check for an error code?
        QVariant prev = KJSEmbed::convertToVariant(exec, args[0]);
        if (!prev.canCast(QVariant::Color)) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
          exec->setException(eobj);
          return KJS::Undefined();
        }
        rc = KstColorSequence::next(kstObjectSubList<KstBaseCurve, KstVCurve>(bcl), prev.toColor());
      }
      break;
    default:
      KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
      exec->setException(eobj);
      return KJS::Undefined();
  }
  return KJSEmbed::convertToValue(exec, rc);
}


KJS::Value KstBindColorSequence::tooClose(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly two arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  QVariant left = KJSEmbed::convertToVariant(exec, args[0]);
  QVariant right = KJSEmbed::convertToVariant(exec, args[1]);
  if (!left.canCast(QVariant::Color) || !right.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return KJS::Boolean(KstColorSequence::colorsTooClose(left.toColor(), right.toColor()));
}

// vim: ts=2 sw=2 et
