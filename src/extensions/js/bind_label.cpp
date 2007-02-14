/***************************************************************************
                               bind_label.cpp
                               ---------------
    begin                : Jun 14 2005
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

#include "bind_label.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindLabel::KstBindLabel(KJS::ExecState *exec, KstViewLabelPtr d, const char *name)
: KstBindBorderedViewObject(exec, d.data(), name ? name : "Label") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindLabel::KstBindLabel(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindBorderedViewObject(exec, globalObject, name ? name : "Label") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindBorderedViewObject::addFactory("Label", KstBindLabel::bindFactory);
  }
}


KstBindViewObject *KstBindLabel::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewLabelPtr v = kst_cast<KstViewLabel>(obj);
  if (v) {
    return new KstBindLabel(exec, v);
  }
  return 0L;
}


KstBindLabel::KstBindLabel(int id, const char *name)
: KstBindBorderedViewObject(id, name ? name : "Label Method") {
}


KstBindLabel::~KstBindLabel() {
}


KJS::Object KstBindLabel::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() == 0 || args.size() > 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  KstViewObjectPtr view = extractViewObject(exec, args[0]);
  if (!view) {
    KstViewWindow *w = extractWindow(exec, args[0]);
    if (w) {
      view = w->view();
    } else {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  QString txt;

  if (args.size() == 2) {
    if (args[1].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
    txt = args[1].toString(exec).qstring();
  }

  KstViewLabelPtr b = new KstViewLabel(txt);
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindLabel(exec, b));
}


struct LabelBindings {
  const char *name;
  KJS::Value (KstBindLabel::*method)(KJS::ExecState*, const KJS::List&);
};


struct LabelProperties {
  const char *name;
  void (KstBindLabel::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindLabel::*get)(KJS::ExecState*) const;
};


static LabelBindings labelBindings[] = {
  { "adjustSizeForText", &KstBindLabel::adjustSizeForText },
  { 0L, 0L }
};


static LabelProperties labelProperties[] = {
  { "text", &KstBindLabel::setText, &KstBindLabel::text },
  { "font", &KstBindLabel::setFont, &KstBindLabel::font },
  { "fontSize", &KstBindLabel::setFontSize, &KstBindLabel::fontSize },
  { "justification", &KstBindLabel::setJustification, &KstBindLabel::justification },
  { "rotation", &KstBindLabel::setRotation, &KstBindLabel::rotation },
  { "dataPrecision", &KstBindLabel::setDataPrecision, &KstBindLabel::dataPrecision },
  { "interpreted", &KstBindLabel::setInterpreted, &KstBindLabel::interpreted },
  { "scalarReplacement", &KstBindLabel::setScalarReplacement, &KstBindLabel::scalarReplacement },
  { "autoResize", &KstBindLabel::setAutoResize, &KstBindLabel::autoResize },
  { 0L, 0L, 0L }
};


int KstBindLabel::methodCount() const {
  return sizeof labelBindings + KstBindBorderedViewObject::methodCount();
}


int KstBindLabel::propertyCount() const {
  return sizeof labelProperties + KstBindBorderedViewObject::propertyCount();
}


KJS::ReferenceList KstBindLabel::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindBorderedViewObject::propList(exec, recursive);

  for (int i = 0; labelProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(labelProperties[i].name)));
  }

  return rc;
}


bool KstBindLabel::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; labelProperties[i].name; ++i) {
    if (prop == labelProperties[i].name) {
      return true;
    }
  }

  return KstBindBorderedViewObject::hasProperty(exec, propertyName);
}


void KstBindLabel::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindBorderedViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; labelProperties[i].name; ++i) {
    if (prop == labelProperties[i].name) {
      if (!labelProperties[i].set) {
        break;
      }
      (this->*labelProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindBorderedViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindLabel::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindBorderedViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; labelProperties[i].name; ++i) {
    if (prop == labelProperties[i].name) {
      if (!labelProperties[i].get) {
        break;
      }
      return (this->*labelProperties[i].get)(exec);
    }
  }
  
  return KstBindBorderedViewObject::get(exec, propertyName);
}


KJS::Value KstBindLabel::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindBorderedViewObject::methodCount();
  if (id > start) {
    KstBindLabel *imp = dynamic_cast<KstBindLabel*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*labelBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindBorderedViewObject::call(exec, self, args);
}


void KstBindLabel::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindBorderedViewObject::methodCount();
  for (int i = 0; labelBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindLabel(i + start + 1));
    obj.put(exec, labelBindings[i].name, o, KJS::Function);
  }
}


#define makeLabel(X) dynamic_cast<KstViewLabel*>(const_cast<KstObject*>(X.data()))

void KstBindLabel::setText(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setText(value.toString(exec).qstring());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::text(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->text());
  }
  return KJS::String();
}


void KstBindLabel::setFont(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setFontName(value.toString(exec).qstring());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::font(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->fontName());
  }
  return KJS::String();
}


void KstBindLabel::setFontSize(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned int i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setFontSize(i);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::fontSize(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->fontSize());
  }
  return KJS::Number(0);
}


void KstBindLabel::setJustification(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned int i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setJustification(i);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::justification(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->justification());
  }
  return KJS::Number(0);
}


void KstBindLabel::setRotation(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setRotation(value.toNumber(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::rotation(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->rotation());
  }
  return KJS::Number(0);
}


void KstBindLabel::setDataPrecision(KJS::ExecState *exec, const KJS::Value& value) {
  uint i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setDataPrecision(i);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::dataPrecision(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->dataPrecision());
  }
  return KJS::Number(0);
}


void KstBindLabel::setInterpreted(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setInterpreted(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::interpreted(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->interpreted());
  }
  return KJS::Boolean(false);
}


void KstBindLabel::setScalarReplacement(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setDoScalarReplacement(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLabel::scalarReplacement(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLabelPtr d = makeLabel(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->doScalarReplacement());
  }
  return KJS::Boolean(false);
}


void KstBindLabel::setAutoResize(KJS::ExecState *exec, const KJS::Value& value) {
  Q_UNUSED(exec)
  Q_UNUSED(value)
}


KJS::Value KstBindLabel::autoResize(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(true);
}


KJS::Value KstBindLabel::adjustSizeForText(KJS::ExecState *exec, const KJS::List& args) {
  KstViewLabelPtr d = makeLabel(_d);
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
  //d->adjustSizeForText();
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Value();
}


#undef makeLabel

// vim: ts=2 sw=2 et
