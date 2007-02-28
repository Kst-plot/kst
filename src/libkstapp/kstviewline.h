/***************************************************************************
                               kstviewline.h
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

#ifndef KSTVIEWLINE_H
#define KSTVIEWLINE_H

#include "kstviewobject.h"
#include <qglobal.h>

class KstViewLine;
typedef KstSharedPtr<KstViewLine> KstViewLinePtr;

class KstViewLine : public KstViewObject {
  Q_OBJECT
  Q_PROPERTY(int width READ width WRITE setWidth)
  Q_PROPERTY(int lineStyle READ penStyleWrap WRITE setPenStyleWrap)
  Q_PROPERTY(QColor lineColor READ foregroundColor WRITE setForegroundColor)

  public:
    KstViewLine(const QString& type = "Line");
    KstViewLine(const QDomElement& e);
    KstViewLine(const KstViewLine& line);
    virtual ~KstViewLine();

    virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    // sets the "from" point - geometry not updated until setTo() is called
    virtual void setFrom(const QPoint& from);
    virtual QPoint from() const;
    // sets the "to" point and moves and resizes the line
    virtual void setTo(const QPoint& to);
    virtual QPoint to() const;
    virtual void setWidth(int width);
    virtual int width() const;

    // needed for Q_PROPERTY to work
    // (can't have enums declared outside of class in Q_PROPERTY macro)
    int penStyleWrap() const;
    void setPenStyleWrap(int style);
    // just calls KstViewObject functions - Q_PROPERTY doesn't work in KstViewObject?
    void setForegroundColor(const QColor& color);
    QColor foregroundColor() const;

    void move(const QPoint& pos);

    virtual void setCapStyle(Qt::PenCapStyle style);
    virtual Qt::PenCapStyle capStyle() const;
    virtual void setPenStyle(Qt::PenStyle style);
    virtual Qt::PenStyle penStyle() const;

    virtual void drawSelectRect(KstPainter& p);
    virtual void drawFocusRect(KstPainter& p);

    virtual signed int directionFor(const QPoint& pos);

    virtual QMap<QString, QVariant> widgetHints(const QString& propertyName) const;

    virtual void drawShadow(KstPainter& p, const QPoint& pos);

    virtual QRect surroundingGeometry() const;

  public slots:
    virtual void paintSelf(KstPainter& p, const QRegion& bounds);

  public:
    virtual void save(Q3TextStream& ts, const QString& indent = QString::null);

  protected:
    enum Direction { UpLeft = 0, UpRight = 1, DownLeft = 2, DownRight = 3};
    Direction _orientation;
    void updateOrientation();
    int _width;
    Qt::PenCapStyle _capStyle;
    Qt::PenStyle _penStyle;

    QPoint _from, _to; //requested from and to points
};

typedef KstObjectList<KstViewLinePtr> KstViewLineList;


#endif
// vim: ts=2 sw=2 et
