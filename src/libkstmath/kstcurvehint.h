/***************************************************************************
                        kstcurvehint.h  -  Part of KST
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

#ifndef _KST_CURVEHINT_H
#define _KST_CURVEHINT_H

#include <qstring.h>
#include <qcolor.h>
#include <q3valuelist.h>

#include "vector.h"

class KstRelation;

class KstCurveHint : public KstShared {
  friend class KstDataObject;
  public:
    KstCurveHint(const QString& name = QString::null, const QString& x = QString::null, const QString& y = QString::null);
    
    virtual ~KstCurveHint();

    virtual const QString& curveName() const { return _curveName; }
    virtual const QString& xVectorName() const;
    virtual const QString& yVectorName() const;

    virtual Kst::VectorPtr xVector() const;
    virtual Kst::VectorPtr yVector() const;

    virtual KstSharedPtr<KstRelation> makeCurve(const QString& tag, const QColor& color) const;

  protected:
    QString _curveName, _xVectorName, _yVectorName;
};

typedef KstSharedPtr<KstCurveHint> KstCurveHintPtr;
typedef QList<KstCurveHintPtr> KstCurveHintList;

#endif

// vim: ts=2 sw=2 et
