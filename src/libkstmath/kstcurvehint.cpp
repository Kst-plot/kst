/***************************************************************************
                        kstcurvehint.cpp  -  Part of KST
                             -------------------
    begin                : Sun Sep 26 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "kstcurvehint.h"
#include "kstdatacollection.h"
#include "kstrelation.h"
#include "kstvcurve.h"

#include <qdebug.h>

KstCurveHint::KstCurveHint(const QString& name, const QString& x, const QString& y)
: KstShared(),  _curveName(name), _xVectorName(x), _yVectorName(y) {
}


KstCurveHint::~KstCurveHint() {
}


const QString& KstCurveHint::xVectorName() const {
  return _xVectorName;
}


const QString& KstCurveHint::yVectorName() const {
  return _yVectorName;
}


Kst::VectorPtr KstCurveHint::xVector() const {
  return *KST::vectorList.findTag(_xVectorName);
}


Kst::VectorPtr KstCurveHint::yVector() const {
  return *KST::vectorList.findTag(_yVectorName);
}


KstRelationPtr KstCurveHint::makeCurve(const QString& tag, const QColor& color) const {
  Kst::VectorPtr x = xVector();
  Kst::VectorPtr y = yVector();
  if (!x || !y) {
    qDebug() << "Couldn't find either " << _xVectorName << " or " << _yVectorName << endl;
    return 0L;
  }

  return new KstVCurve(tag, x, y, 0L, 0L, 0L, 0L, color);
}


// vim: ts=2 sw=2 et
