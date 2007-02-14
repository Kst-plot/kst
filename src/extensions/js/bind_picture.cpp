/***************************************************************************
                               bind_picture.cpp
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

#include "bind_picture.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindPicture::KstBindPicture(KJS::ExecState *exec, KstViewPicturePtr d, const char *name)
: KstBindBorderedViewObject(exec, d.data(), name ? name : "Picture") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPicture::KstBindPicture(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindBorderedViewObject(exec, globalObject, name ? name : "Picture") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindBorderedViewObject::addFactory("Picture", KstBindPicture::bindFactory);
  }
}


KstBindViewObject *KstBindPicture::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewPicturePtr v = kst_cast<KstViewPicture>(obj);
  if (v) {
    return new KstBindPicture(exec, v);
  }
  return 0L;
}


KstBindPicture::KstBindPicture(int id, const char *name)
: KstBindBorderedViewObject(id, name ? name : "Picture Method") {
}


KstBindPicture::~KstBindPicture() {
}


KJS::Object KstBindPicture::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
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

  KstViewPicturePtr b = new KstViewPicture;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindPicture(exec, b));
}


struct PictureBindings {
  const char *name;
  KJS::Value (KstBindPicture::*method)(KJS::ExecState*, const KJS::List&);
};


struct PictureProperties {
  const char *name;
  void (KstBindPicture::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPicture::*get)(KJS::ExecState*) const;
};


static PictureBindings pictureBindings[] = {
  { "load", &KstBindPicture::load },
  { 0L, 0L }
};


static PictureProperties pictureProperties[] = {
  { "image", &KstBindPicture::setImage, &KstBindPicture::image },
  { "url", 0L, &KstBindPicture::url },
  { "refreshTimer", &KstBindPicture::setRefreshTimer, &KstBindPicture::refreshTimer },
  { 0L, 0L, 0L }
};


int KstBindPicture::methodCount() const {
  return sizeof pictureBindings + KstBindBorderedViewObject::methodCount();
}


int KstBindPicture::propertyCount() const {
  return sizeof pictureProperties + KstBindBorderedViewObject::propertyCount();
}


KJS::ReferenceList KstBindPicture::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindBorderedViewObject::propList(exec, recursive);

  for (int i = 0; pictureProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(pictureProperties[i].name)));
  }

  return rc;
}


bool KstBindPicture::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pictureProperties[i].name; ++i) {
    if (prop == pictureProperties[i].name) {
      return true;
    }
  }

  return KstBindBorderedViewObject::hasProperty(exec, propertyName);
}


void KstBindPicture::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindBorderedViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; pictureProperties[i].name; ++i) {
    if (prop == pictureProperties[i].name) {
      if (!pictureProperties[i].set) {
        break;
      }
      (this->*pictureProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindBorderedViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPicture::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindBorderedViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; pictureProperties[i].name; ++i) {
    if (prop == pictureProperties[i].name) {
      if (!pictureProperties[i].get) {
        break;
      }
      return (this->*pictureProperties[i].get)(exec);
    }
  }
  
  return KstBindBorderedViewObject::get(exec, propertyName);
}


KJS::Value KstBindPicture::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindBorderedViewObject::methodCount();
  if (id > start) {
    KstBindPicture *imp = dynamic_cast<KstBindPicture*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*pictureBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindBorderedViewObject::call(exec, self, args);
}


void KstBindPicture::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindBorderedViewObject::methodCount();
  for (int i = 0; pictureBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPicture(i + start + 1));
    obj.put(exec, pictureBindings[i].name, o, KJS::Function);
  }
}


#define makePicture(X) dynamic_cast<KstViewPicture*>(const_cast<KstObject*>(X.data()))

void KstBindPicture::setRefreshTimer(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewPicturePtr d = makePicture(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setRefreshTimer(w);
  }
}


KJS::Value KstBindPicture::refreshTimer(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewPicturePtr d = makePicture(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->refreshTimer());
  }
  return KJS::Number(0);
}


KJS::Value KstBindPicture::url(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewPicturePtr d = makePicture(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->url());
  }
  return KJS::String();
}


KJS::Value KstBindPicture::load(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Boolean(false);
  }
  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Boolean(false);
  }
  KstViewPicturePtr d = makePicture(_d);
  bool rc = false;
  if (d) {
    KstWriteLocker wl(d);
    rc = d->setImage(args[0].toString(exec).qstring());
    if (rc) {
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    }
  }
  return KJS::Boolean(rc);
}


void KstBindPicture::setImage(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Image)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewPicturePtr d = makePicture(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setImage(cv.toImage());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindPicture::image(KJS::ExecState *exec) const {
  KstViewPicturePtr d = makePicture(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->image());
  }
  return KJSEmbed::convertToValue(exec, QImage());
}


#undef makePicture

// vim: ts=2 sw=2 et
