/***************************************************************************
              object.cpp: abstract base class for all Kst objects
                             -------------------
    begin                : May 25, 2003
    copyright            : (C) 2003 The University of Toronto
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

#include "kst_i18n.h"

#include "object.h"
#include "objectstore.h"

namespace Kst {

#if 0
static int i = 0;
#endif

const QString Object::staticTypeString = I18N_NOOP("Object");

Object::Object(const ObjectTag& tag) :
  QObject(), Shared(), KstRWLock(),
  _store(0L), _manualDescriptiveName(QString()), _shortName(QString("FIXME - set _shortName")), _tag(tag)
{
  _dirty = false;

  _initial_vnum = _vnum; // vectors
  _initial_pnum = _pnum; // plugins
  _initial_csdnum = _csdnum; // csd
  _initial_cnum = _cnum; // curves
  _initial_enum = _enum; // equations
  _initial_hnum = _hnum; // histograms
  _initial_inum = _inum; // images
  _initial_psdnum = _psdnum; // psd
  _initial_xnum = _xnum; // scalars
  _initial_tnum = _tnum; // text string

}


Object::~Object() {
  if (_store) {
    _store->removeObject(this);
  }
}


QString Object::type() {
  return staticMetaObject.className();
}


const QString& Object::typeString() const {
  return staticTypeString;
}


#if 0
int Object::operator==(const QString& tag) const {
  return (tag == _tag.tagString() || tag == _tag.displayString()) ? 1 : 0;
}
#endif


#if 0
inline QString Object::tagName() const {
  return _tag.tag();
}
#endif


ObjectTag& Object::tag() {
  return _tag;
}


const ObjectTag& Object::tag() const {
  return _tag;
}


void Object::setTagName(const ObjectTag& tag) {
  if (tag == _tag) {
    return;
  }

  _tag = tag;
  setObjectName(_tag.tagString().toLocal8Bit().data());

  if (_store) {
    // handle rename through object store
    _store->renameObject(this, tag);
  }
}


#if 0
QString Object::tagLabel() const {
  return QString("[%1]").arg(_tag.tagString());
}
#endif


// Returns count - 2 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int Object::getUsage() const {
  return _KShared_count() - 1;
}


void Object::deleteDependents() {
}


void Object::setDirty(bool dirty) {
  _dirty = dirty;
}


bool Object::dirty() const {
  return _dirty;
}


ObjectStore* Object::store() const {
  return _store;
}


// new Name system
QString Object::Name() {
  return descriptiveName()+":"+shortName();
}


QString Object::descriptiveName() {
  if (_manualDescriptiveName.isEmpty()) {
    return _automaticDescriptiveName();
  } else {
    return _manualDescriptiveName;
  }
}


QString Object::shortName() {
  return _shortName;
}


void Object::setDescriptiveName(QString new_name) {
  _manualDescriptiveName = new_name;
}

bool Object::descriptiveNameIsManual() {
  return !(_manualDescriptiveName.isEmpty());
}

void Object::beginUpdate(ObjectPtr object) {
  processUpdate(object);
}

void Object::processUpdate(ObjectPtr object) {
  // Do nothing by default.
}

void Object::saveNameInfo(QXmlStreamWriter &s) {
  if (descriptiveNameIsManual()) {
    s.writeAttribute("descriptiveNameIsManual", "true");
    s.writeAttribute("descriptiveName", descriptiveName());
  }
  s.writeAttribute("initialVNum", QString::number(_initial_vnum));
  s.writeAttribute("initialXNum", QString::number(_initial_xnum));
  s.writeAttribute("initialPNum", QString::number(_initial_pnum));
  s.writeAttribute("initialCSDNum", QString::number(_initial_csdnum));
  s.writeAttribute("initialCNum", QString::number(_initial_cnum));
  s.writeAttribute("initialENum", QString::number(_initial_enum));
  s.writeAttribute("initialHNum", QString::number(_initial_hnum));
  s.writeAttribute("initialINum", QString::number(_initial_inum));
  s.writeAttribute("initialPSDNum", QString::number(_initial_psdnum));
  s.writeAttribute("initialTNum", QString::number(_initial_tnum));

}

void Object::processShortNameIndexAttributes(QXmlStreamAttributes &attrs) {

  _vnum = attrs.value("initialVNum").toString().toInt();
  _xnum = attrs.value("initialXNum").toString().toInt();
  _pnum = attrs.value("initialPNum").toString().toInt();
  _csdnum = attrs.value("initialCSDNum").toString().toInt();
  _cnum = attrs.value("initialCNum").toString().toInt();
  _enum = attrs.value("initialENum").toString().toInt();
  _hnum = attrs.value("initialHNum").toString().toInt();
  _inum = attrs.value("initialINum").toString().toInt();
  _psdnum = attrs.value("initialPSDNum").toString().toInt();
  _tnum = attrs.value("initialTNum").toString().toInt();
}

}

// vim: ts=2 sw=2 et
