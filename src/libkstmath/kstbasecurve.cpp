/***************************************************************************
                   kstbasecurve.cpp: base class for a curve
                             -------------------
    begin                : June 2003
    copyright            : (C) 2003 University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include "kstbasecurve.h"
#include "kstdatacollection.h"

KstBaseCurve::KstBaseCurve(const QDomElement& e) : KstDataObject(e) {
  commonConstructor();
}


KstBaseCurve::KstBaseCurve() : KstDataObject() {
  commonConstructor();
}


void KstBaseCurve::commonConstructor() {
  _ns_maxx = _ns_minx = _ns_maxy = _ns_miny = MaxX = MinX = MinPosX = MeanX = MaxY = MinY = MinPosY = 0.0;
  NS = 0;

  _ignoreAutoScale = false;
  _parsedLegendTag = 0L;
}


KstBaseCurve::~KstBaseCurve() {
  delete _parsedLegendTag;
  _parsedLegendTag = 0L;
}


bool KstBaseCurve::deleteDependents() {
  bool rc = KstDataObject::deleteDependents();
  KstData::self()->removeCurveFromPlots(this);
  return rc;
}


void KstBaseCurve::setIgnoreAutoScale(bool ignoreAutoScale) {
  setDirty();
  _ignoreAutoScale = ignoreAutoScale;
}

void KstBaseCurve::updateParsedLegendTag() {  
  delete _parsedLegendTag; 
  if (_legendText.isEmpty()) {
    _parsedLegendTag = Label::parse(tagName(), false, false);
  } else {
    _parsedLegendTag = Label::parse(legendText(), true, false);
  }
}

Label::Parsed *KstBaseCurve::parsedLegendTag() {
  if (!_parsedLegendTag) {
    updateParsedLegendTag();
  }
  return _parsedLegendTag;
}

void KstBaseCurve::setLegendText(const QString& theValue) { 
  _legendText = theValue; 
  updateParsedLegendTag(); 
  emit modifiedLegendEntry();
}

#include "kstbasecurve.moc"
// vim: ts=2 sw=2 et
