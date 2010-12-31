/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CURVEHINT_H
#define CURVEHINT_H

#include <qstring.h>
#include <qcolor.h>

#include "vector.h"

namespace Kst {

class Relation;

#ifdef KST_USE_QSHAREDPOINTER
class CurveHint {
#else
class CurveHint : public Shared {
#endif
  friend class DataObject;
  public:
    explicit CurveHint(const QString& name = QString(), const QString& x = QString(), const QString& y = QString());

    virtual ~CurveHint();

    virtual const QString& curveName() const { return _curveName; }
    virtual const QString& xVectorName() const;
    virtual const QString& yVectorName() const;

#if 0
    virtual VectorPtr xVector() const;
    virtual VectorPtr yVector() const;

    virtual SharedPtr<Relation> makeCurve(const QString& tag, const QColor& color) const;
#endif

  protected:
    QString _curveName, _xVectorName, _yVectorName;
};

typedef SharedPtr<CurveHint> CurveHintPtr;
typedef QList<CurveHintPtr> CurveHintList;

}

#endif

// vim: ts=2 sw=2 et
