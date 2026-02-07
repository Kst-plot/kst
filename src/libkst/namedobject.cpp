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
#include <QDebug>

namespace Kst {
  
NamedObject::NamedObject() : _manualDescriptiveName(QString()), _shortName(QString("FIXME - set _shortName"))
{

  _initial_vectornum = _vectornum; // vectors
  _initial_pluginnum = _pluginnum; // plugins
  _initial_csdnum = _csdnum; // csd
  _initial_curvenum = _curvecnum; // curves
  _initial_equationnum = _equationnum; // equations
  _initial_histogramnum = _histogramnum; // histograms
  _initial_imagenum = _imagenum; // images
  _initial_psdnum = _psdnum; // psd
  _initial_scalarnum = _scalarnum; // scalars
  _initial_stringnum = _stringnum; // text string
  _initial_matrixnum = _matrixnum; // text string
  _initial_plotnum = _plotnum; // plots
  _initial_legendnum = _legendnum; // legend
  _initial_viewitemnum = _viewitemnum; // view image
  _initial_datasourcenum = _datasourcenum; // datasource

  _sizeCache = new SizeCache;
  _sizeCache->fontSize = 0;
  _sizeCache->nameWidthPixels = 0;
  _sizeCache->name.clear();
}

NamedObject::~NamedObject() {
  delete _sizeCache;
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
  return (CleanedName());

    QFontMetrics fontMetrics=QFontMetrics(font);

    int mw = fontMetrics.maxWidth();
    if ((_sizeCache->fontSize == font.pointSize() ) &&
        (_sizeCache->name == Name()) &&
        (_sizeCache->nameWidthPixels < width - mw)) {
      return (_sizeCache->name);
    }

    _sizeCache->name = Name();
    _sizeCache->nameWidthPixels = fontMetrics.horizontalAdvance(_sizeCache->name);
    _sizeCache->fontSize = font.pointSize();

    if (_sizeCache->nameWidthPixels < width - mw) {
      return _sizeCache->name;
    }

    int combo_chars = width / fontMetrics.averageCharWidth() - 2;
    int nameLength = _sizeCache->name.length();

    QString name = lengthLimitedName(combo_chars);
    while ((combo_chars <= nameLength+1) &&
           (fontMetrics.horizontalAdvance(name) < width - mw)) {
      combo_chars++;
      name = lengthLimitedName(combo_chars);
    }
    while ((combo_chars>0) &&
           (fontMetrics.horizontalAdvance(name) > width  - mw)) {
      combo_chars--;
      name = lengthLimitedName(combo_chars);
    }
    return name;
}

QString NamedObject::sizeLimitedName(const QWidget *widget ) const {
  return (CleanedName());

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
  if (I & VECTORNUM)
    s.writeAttribute("initialVNum", QString::number(_initial_vectornum));
  if (I & SCALARNUM)
    s.writeAttribute("initialXNum", QString::number(_initial_scalarnum));
  if (I & PLUGINNUM)
    s.writeAttribute("initialPNum", QString::number(_initial_pluginnum));
  if (I & CSDNUM)
    s.writeAttribute("initialCSDNum", QString::number(_initial_csdnum));
  if (I & CURVENUM)
    s.writeAttribute("initialCNum", QString::number(_initial_curvenum));
  if (I & EQUATIONNUM)
    s.writeAttribute("initialENum", QString::number(_initial_equationnum));
  if (I & HISTOGRAMNUM)
    s.writeAttribute("initialHNum", QString::number(_initial_histogramnum));
  if (I & IMAGENUM)
    s.writeAttribute("initialINum", QString::number(_initial_imagenum));
  if (I & PSDNUM)
    s.writeAttribute("initialPSDNum", QString::number(_initial_psdnum));
  if (I & STRINGNUM)
    s.writeAttribute("initialTNum", QString::number(_initial_stringnum));
  if (I & MATRIXNUM)
    s.writeAttribute("initialMNum", QString::number(_initial_matrixnum));
  if (I & PLOTNUM)
    s.writeAttribute("initialPlotNum", QString::number(_initial_plotnum));
  if (I & LEGENDNUM)
    s.writeAttribute("initialLNum", QString::number(_initial_legendnum));
  if (I & VIEWITEMNUM)
    s.writeAttribute("initialDNum", QString::number(_initial_viewitemnum));
  if (I & DATASOURCENUM)
    s.writeAttribute("initialDSNum", QString::number(_initial_datasourcenum));
}

void NamedObject::processShortNameIndexAttributes(QXmlStreamAttributes &attrs) {
  QStringView R;

  R = attrs.value("initialVNum");
  if (!R.isEmpty()) 
    _vectornum = R.toInt();

  R = attrs.value("initialXNum");
  if (!R.isEmpty()) {
      _scalarnum = R.toInt();
  }

  R = attrs.value("initialPNum");
  if (!R.isEmpty()) 
    _pluginnum = R.toInt();

  R = attrs.value("initialCSDNum");
  if (!R.isEmpty()) 
    _csdnum = R.toInt();

  R = attrs.value("initialCNum");
  if (!R.isEmpty()) 
    _curvecnum = R.toInt();

  R = attrs.value("initialENum");
  if (!R.isEmpty()) 
    _equationnum = R.toInt();

  R = attrs.value("initialHNum");
  if (!R.isEmpty()) 
    _histogramnum = R.toInt();

  R = attrs.value("initialINum");
  if (!R.isEmpty()) 
    _imagenum = R.toInt();

  R = attrs.value("initialPSDNum");
  if (!R.isEmpty()) 
    _psdnum = R.toInt();

  R = attrs.value("initialTNum");
  if (!R.isEmpty()) 
    _stringnum = R.toInt();

  R = attrs.value("initialMNum");
  if (!R.isEmpty()) 
    _matrixnum = R.toInt();

  R = attrs.value("initialPlotNum");
  if (!R.isEmpty())
    _plotnum = R.toInt();

  R = attrs.value("initialLNum");
  if (!R.isEmpty())
    _legendnum = R.toInt();

  R = attrs.value("initialDNum");
  if (!R.isEmpty())
    _viewitemnum = R.toInt();

  R = attrs.value("initialDSNum");
  if (!R.isEmpty())
    _datasourcenum = R.toInt();
}


// Reset all name indexes.  Should only be used by ObjectStore when clearing the store entirely.
void NamedObject::resetNameIndex() {
  _vectornum = 1; // vectors
  _pluginnum = 1; // plugins
  _csdnum = 1; // csd
  _curvecnum = 1; // curves
  _equationnum = 1; // equations
  _histogramnum = 1; // histograms
  _imagenum = 1; // images
  _psdnum = 1; // psd
  _scalarnum = 1; // scalars
  _stringnum = 1; // text string
  _matrixnum = 1; // matrix
  _plotnum = 1; // plots
  _legendnum = 1; // legends
  _viewitemnum = 1; // other view objects
  _datasourcenum = 1; // datasource

  max_vectornum = 0; // vectors
  max_pluginnum = 0; // plugins
  max_csdnum = 0; // csd
  max_curvenum = 0; // curves
  max_equationnum = 0; // equations
  max_histogramnum = 0; // histograms
  max_imagenum = 0; // images
  max_psdnum = 0; // psd
  max_scalarnum = 0; // scalars
  max_stringnum = 0; // text string
  max_matrixnum = 0; // matrix
  max_plotnum = 0;
  max_legendnum = 0;
  max_viewitemnum = 0;
  max_datasourcenum = 0;
}

// for sorting a list in order of creation (ie, short name number)
bool shortNameLessThan(NamedObject *o1, NamedObject *o2) {
  QString n1 = o1->shortName();
  QString n2 = o2->shortName();

  n1.remove(0,1);
  n2.remove(0,1);
  return (n1.toInt() < n2.toInt());
}

void resetNameIndexes() {
   _vectornum = 1; // vectors
   _pluginnum = 1; // plugins
   _csdnum = 1; // csd
   _curvecnum = 1; // curves
   _equationnum = 1; // equations
   _histogramnum = 1; // histograms
   _imagenum = 1; // images
   _psdnum = 1; // psd
   _scalarnum = 1; // scalars
   _stringnum = 1; // text string
   _matrixnum = 1; // matrix
   _plotnum = 1; // plots
   _legendnum = 1; // legend
   _viewitemnum = 1; // view item
   _datasourcenum = 1; // datasource

   max_vectornum = 0; // vectors
   max_pluginnum = 0; // plugins
   max_csdnum = 0; // csd
   max_curvenum = 0; // curves
   max_equationnum = 0; // equations
   max_histogramnum = 0; // histograms
   max_imagenum = 0; // images
   max_psdnum = 0; // psd
   max_scalarnum = 0; // scalars
   max_stringnum = 0; // text string
   max_matrixnum = 0; // matrix
   max_plotnum = 0; // plots
   max_legendnum = 0; // legends
   max_viewitemnum = 0; // view item
   max_datasourcenum = 0; // datasource

}

//void NamedObject::_initializeShortName() {
//}

// QString NamedObject::descriptionTip() const {
//   return Name();
// }

}
