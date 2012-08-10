/***************************************************************************
              namedobject.cpp: adds naming to kst object
                             -------------------
    begin                : May 29, 2008
    copyright            : (C) 2008 C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
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

#include <QFontMetrics>
#include <QWidget>

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
  _initial_plotnum = _plotnum; // plots
  _initial_lnum = _lnum; // legend
  _initial_dnum = _dnum; // view image
  _initial_dsnum = _dsnum; // datasource

}

NamedObject::~NamedObject() {
}


QString NamedObject::Name() const {
  return descriptiveName()+" ("+shortName()+')';
}

/** limit the length of the string to length.  However, do not shorten
  the name so much that the shortname is truncated. */

QString NamedObject::lengthLimitedName(int length) const {
  QString name = Name();
  if (name.length()<=length) {
    return name;
  }

  length -= shortName().length() + 3;

  if (length <= 5) { // not enough room for "a...z (V2)"
    return '(' + shortName() + ')';
  }

  int dnl = descriptiveName().length();

  int d =  dnl - length + 3;
  int s = (length-3)/2;

  return descriptiveName().replace(s, d, QString("...")) + " (" + shortName()+')';

}

QString NamedObject::sizeLimitedName(const QFont& font, const int& width) const {
    QFontMetrics fontMetrics=QFontMetrics(font);
    // initial guess
    int combo_chars = width / fontMetrics.averageCharWidth() - 2;
    int nameLength = Name().length();

    QString name = lengthLimitedName(combo_chars);
    while ((combo_chars <= nameLength+1) &&
           (fontMetrics.width(name) < width - fontMetrics.maxWidth())) {
      combo_chars++;
      name = lengthLimitedName(combo_chars);
    }
    while ((combo_chars>0) &&
           (fontMetrics.width(name) > width  - fontMetrics.maxWidth())) {
      combo_chars--;
      name = lengthLimitedName(combo_chars);
    }
    return name;
}

QString NamedObject::sizeLimitedName(const QWidget *widget ) const {
  return sizeLimitedName(widget->font(),widget->width());
}

QString NamedObject::CleanedName() const {
  QString clean_name = Name();
  clean_name.replace("\\_","_");
  clean_name.replace("\\[","[");
  clean_name.replace("\\]","]");

  return clean_name;
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
  if (I & PLOTNUM)
    s.writeAttribute("initialPlotNum", QString::number(_initial_plotnum));
  if (I & LNUM)
    s.writeAttribute("initialLNum", QString::number(_initial_lnum));
  if (I & DNUM)
    s.writeAttribute("initialDNum", QString::number(_initial_dnum));
  if (I & DSNUM)
    s.writeAttribute("initialDSNum", QString::number(_initial_dsnum));
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

  R = attrs.value("initialPlotNum");
  if (!R.isEmpty())
    _plotnum = R.toString().toInt();

  R = attrs.value("initialLNum");
  if (!R.isEmpty())
    _lnum = R.toString().toInt();

  R = attrs.value("initialDNum");
  if (!R.isEmpty())
    _dnum = R.toString().toInt();

  R = attrs.value("initialDSNum");
  if (!R.isEmpty())
    _dsnum = R.toString().toInt();
}


// Reset all name indexes.  Should only be used by ObjectStore when clearing the store entirely.
void NamedObject::resetNameIndex() {
  _vnum = 1; // vectors
  _pnum = 1; // plugins
  _csdnum = 1; // csd
  _cnum = 1; // curves
  _enum = 1; // equations
  _hnum = 1; // histograms
  _inum = 1; // images
  _psdnum = 1; // psd
  _xnum = 1; // scalars
  _tnum = 1; // text string
  _mnum = 1; // matrix
  _plotnum = 1; // plots
  _lnum = 1; // legends
  _dnum = 1; // other view objects
  _dsnum = 1; // datasource

  max_vnum = 0; // vectors
  max_pnum = 0; // plugins
  max_csdnum = 0; // csd
  max_cnum = 0; // curves
  max_enum = 0; // equations
  max_hnum = 0; // histograms
  max_inum = 0; // images
  max_psdnum = 0; // psd
  max_xnum = 0; // scalars
  max_tnum = 0; // text string
  max_mnum = 0; // matrix
  max_plotnum = 0;
  max_lnum = 0;
  max_dnum = 0;
  max_dsnum = 0;
}

// for sorting a list in order of creation (ie, short name number)
bool shortNameLessThan(NamedObject *o1, NamedObject *o2) {
  QString n1 = o1->shortName();
  QString n2 = o2->shortName();

  n1.remove(0,1);
  n2.remove(0,1);
  return (n1.toInt() < n2.toInt());
}

//void NamedObject::_initializeShortName() {
//}

// QString NamedObject::descriptionTip() const {
//   return Name();
// }

}
