/***************************************************************************
              namedobject.cpp: adds naming to kst object
                             -------------------
    begin                : May 29, 2008
    copyright            : (C) 2008 C. Barth Netterfield
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
#include "namedobject.h"

namespace Kst {
  
NamedObject::NamedObject() : _manualDescriptiveName(QString()), _shortName(QString("FIXME - set _shortName")) {

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

NamedObject::~NamedObject() {
}


// new Name system
QString NamedObject::Name() const {
  return descriptiveName()+" ("+shortName()+")";
}


QString NamedObject::descriptiveName() const {
  if (_manualDescriptiveName.isEmpty()) {
      return _automaticDescriptiveName();
  } else {
    return _manualDescriptiveName;
  }
}


QString NamedObject::shortName() const {
  return _shortName;
}


void NamedObject::setDescriptiveName(QString new_name) {
  _manualDescriptiveName = new_name;
}

bool NamedObject::descriptiveNameIsManual() const {
  return !(_manualDescriptiveName.isEmpty());
}

// to keep the size of the saved xml files smaller, 'I' 
// should be set to only the types who are effected by the 
// creation of the Object.  (So, the type itself and any slave
// objects which are created)  eg: Vectors: VNUM|XNUM
// It doesn't hurt to add more (the default is all bits set)
// except that it increases the size of the .kst file, and
// slows loading (not much, but if you have hundreds of
// objects....)
void NamedObject::saveNameInfo(QXmlStreamWriter &s, unsigned I) {
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

void NamedObject::processShortNameIndexAttributes(QXmlStreamAttributes &attrs) {
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

QString NamedObject::descriptionTip() const {
  return Name();
}

}