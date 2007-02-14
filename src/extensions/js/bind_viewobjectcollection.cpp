/***************************************************************************
                        bind_viewobjectcollection.cpp
                             -------------------
    begin                : May 28 2005
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

#include "bind_viewobjectcollection.h"
#include "bind_viewobject.h"

#include <qdeepcopy.h>

#include <kst.h>
#include <kstviewobject.h>

#include <kdebug.h>

KstBindViewObjectCollection::KstBindViewObjectCollection(KJS::ExecState *exec, const KstViewObjectList& objects)
: KstBindCollection(exec, "ViewObjectCollection", true) {
  _objects = QDeepCopy<KstViewObjectList>(objects);
}


KstBindViewObjectCollection::KstBindViewObjectCollection(KJS::ExecState *exec, KstViewObjectPtr parent)
: KstBindCollection(exec, "ViewObjectCollection", false), _parent(parent) {
}


KstBindViewObjectCollection::KstBindViewObjectCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "ViewObjectCollection", true) {
}


KstBindViewObjectCollection::~KstBindViewObjectCollection() {
}


KJS::Value KstBindViewObjectCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_parent) {
    KstReadLocker rl(_parent);
    return KJS::Number(_parent->children().count());
  }

  return KJS::Number(_objects.count());
}


QStringList KstBindViewObjectCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_parent) {
    KstReadLocker rl(_parent);
    return _parent->children().tagNames();
  }

  return _objects.tagNames();
}


KJS::Value KstBindViewObjectCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstViewObjectPtr vp;
  if (_parent) {
    KstReadLocker rl(_parent);
    vp = *_parent->children().findTag(item.qstring());
  } else {
    vp = *_objects.findTag(item.qstring());
  }

  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(KstBindViewObject::bind(exec, vp));
}


KJS::Value KstBindViewObjectCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstViewObjectPtr vp;
  if (_parent) {
    KstReadLocker rl(_parent);
    if (item < _parent->children().count()) {
      vp = _parent->children()[item];
    }
  } else if (item < _objects.count()) {
    vp = _objects[item];
  }

  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(KstBindViewObject::bind(exec, vp));
}


KJS::Value KstBindViewObjectCollection::append(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_parent) {
    KstViewObjectPtr c = extractViewObject(exec, args[0]);
    if (!c) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    KstWriteLocker rl(_parent);
    c->detach();
    _parent->appendChild(c);
    _parent->setDirty();
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
    return KJS::Undefined();
  }

  return KstBindCollection::append(exec, args);
}


KJS::Value KstBindViewObjectCollection::prepend(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_parent) {
    KstViewObjectPtr c = extractViewObject(exec, args[0]);
    if (!c) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    KstWriteLocker rl(_parent);
    c->detach();
    _parent->prependChild(c);
    _parent->setDirty();
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
    return KJS::Undefined();
  }

  return KstBindCollection::prepend(exec, args);
}


KJS::Value KstBindViewObjectCollection::clear(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_parent) {
    KstWriteLocker rl(_parent);
    _parent->clearChildren();
    _parent->setDirty();
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
    return KJS::Undefined();
  }

  return KstBindCollection::clear(exec, args);
}


KJS::Value KstBindViewObjectCollection::remove(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_parent) {
    KstViewObjectPtr c = extractViewObject(exec, args[0]);
    if (c) {
      KstWriteLocker rl(_parent);
      _parent->removeChild(c);
      KstApp::inst()->paintAll(KstPainter::P_PAINT);
    } else {
      unsigned i = 0;
      if (args[0].type() == KJS::NumberType && args[0].toUInt32(i)) {
        if (i >= _parent->children().count()) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::RangeError);
          exec->setException(eobj);
        } else {
          KstWriteLocker rl(_parent);
          _parent->removeChild(_parent->children()[i]);
          KstApp::inst()->paintAll(KstPainter::P_PAINT);
        }
      } else {
        KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
        exec->setException(eobj);
        return KJS::Undefined();
      }
    }
    return KJS::Undefined();
  }

  return KstBindCollection::clear(exec, args);
}


// vim: ts=2 sw=2 et
