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

Object::Object() :
  Shared(), KstRWLock(),
  _store(0L), _manualDescriptiveName(QString()), _shortName(QString("FIXME - set _shortName"))
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
  _initial_mnum = _mnum; // text string

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
QString Object::Name() const {
  return descriptiveName()+":"+shortName();
}


QString Object::descriptiveName() const {
  if (_manualDescriptiveName.isEmpty()) {
      return _automaticDescriptiveName();
  } else {
    return _manualDescriptiveName;
  }
}


QString Object::shortName() const {
  return _shortName;
}


void Object::setDescriptiveName(QString new_name) {
  _manualDescriptiveName = new_name;
}

bool Object::descriptiveNameIsManual() const {
  return !(_manualDescriptiveName.isEmpty());
}

void Object::beginUpdate(ObjectPtr object) {
  processUpdate(object);
}

void Object::processUpdate(ObjectPtr object) {
  // Do nothing by default.
}

// to keep the size of the saved xml files smaller, 'I' 
// should be set to only the types who are effected by the 
// creation of the object.  (So, the type itself and any slave
// objects which are created)  eg: Vectors: VNUM|XNUM
// It doesn't hurt to add more (the default is all bits set)
// except that it increases the size of the .kst file, and
// slows loading (not much, but if you have hundreds of
// objects....)
void Object::saveNameInfo(QXmlStreamWriter &s, unsigned I) {
  if (descriptiveNameIsManual()) {
    s.writeAttribute("descriptiveNameIsManual", "true");
    s.writeAttribute("descriptiveName", descriptiveName());
  }
  if (I & VNUM)
    s.writeAttribute("initialVNum", QString::number(_initial_vnum));
  if (I & XNUM)
    s.writeAttribute("initialXNum", QString::number(_initial_xnum));
  if (I & PNUM)
    s.writeAttribute("initialPNum", QString::number(_initial_pnum));
  if (I & CSDNUM)
    s.writeAttribute("initialCSDNum", QString::number(_initial_csdnum));
  if (I & CNUM)
    s.writeAttribute("initialCNum", QString::number(_initial_cnum));
  if (I & ENUM)
    s.writeAttribute("initialENum", QString::number(_initial_enum));
  if (I & HNUM)
    s.writeAttribute("initialHNum", QString::number(_initial_hnum));
  if (I & INUM)
    s.writeAttribute("initialINum", QString::number(_initial_inum));
  if (I & PSDNUM)
    s.writeAttribute("initialPSDNum", QString::number(_initial_psdnum));
  if (I & TNUM)
    s.writeAttribute("initialTNum", QString::number(_initial_tnum));
  if (I & MNUM)
    s.writeAttribute("initialMNum", QString::number(_initial_mnum));

}

void Object::processShortNameIndexAttributes(QXmlStreamAttributes &attrs) {
  QStringRef R;

  R = attrs.value("initialVNum");
  if (!R.isEmpty()) 
    _vnum = R.toString().toInt();

  R = attrs.value("initialXNum");
  if (!R.isEmpty()) 
    _xnum = R.toString().toInt();

  R = attrs.value("initialPNum");
  if (!R.isEmpty()) 
    _pnum = R.toString().toInt();

  R = attrs.value("initialCSDNum");
  if (!R.isEmpty()) 
    _csdnum = R.toString().toInt();

  R = attrs.value("initialCNum");
  if (!R.isEmpty()) 
    _cnum = R.toString().toInt();

  R = attrs.value("initialENum");
  if (!R.isEmpty()) 
    _enum = R.toString().toInt();

  R = attrs.value("initialHNum");
  if (!R.isEmpty()) 
    _hnum = R.toString().toInt();

  R = attrs.value("initialINum");
  if (!R.isEmpty()) 
    _inum = R.toString().toInt();

  R = attrs.value("initialPSDNum");
  if (!R.isEmpty()) 
    _psdnum = R.toString().toInt();

  R = attrs.value("initialTNum");
  if (!R.isEmpty()) 
    _tnum = R.toString().toInt();

  R = attrs.value("initialMNum");
  if (!R.isEmpty()) 
    _mnum = R.toString().toInt();
}


}

// vim: ts=2 sw=2 et
