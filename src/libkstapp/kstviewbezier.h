/***************************************************************************
                               kstviewbezier.h
                             -------------------
    begin                : Jun 14, 2005
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

#ifndef KSTVIEWBEZIER_H
#define KSTVIEWBEZIER_H

#include "kstviewobject.h"
#include <qglobal.h>
#include <q3pointarray.h>

class KstViewBezier;
typedef KstSharedPtr<KstViewBezier> KstViewBezierPtr;

class KstViewBezier : public KstViewObject {
  Q_OBJECT
  public:
    KstViewBezier();
    KstViewBezier(const QDomElement& e);
    ~KstViewBezier();

    void setPointA(const QPoint& pt);
    QPoint pointA() const;
    void setPointB(const QPoint& pt);
    QPoint pointB() const;
    void setPointC(const QPoint& pt);
    QPoint pointC() const;
    void setPointD(const QPoint& pt);
    QPoint pointD() const;
    void setWidth(int width);
    int width() const;
    void setCapStyle(Qt::PenCapStyle style);
    Qt::PenCapStyle capStyle() const;
    void setPenStyle(Qt::PenStyle style);
    Qt::PenStyle penStyle() const;

    void resize(const QSize&);
    void move(const QPoint&);

    void paint(KstPainter& p, const QRegion& bounds);

  public:
    void save(QTextStream& ts, const QString& indent = QString::null);

  private:
    int _width;
    Qt::PenCapStyle _capStyle;
    Qt::PenStyle _penStyle;
    Q3PointArray _points;
};

typedef KstObjectList<KstViewBezierPtr> KstViewBezierList;


#endif
// vim: ts=2 sw=2 et
