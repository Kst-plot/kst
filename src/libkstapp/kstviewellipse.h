/***************************************************************************
                              kstviewellipse.h
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

#ifndef KSTVIEWELLIPSE_H
#define KSTVIEWELLIPSE_H

#include "kstviewobject.h"

class KstViewEllipse;
typedef KstSharedPtr<KstViewEllipse> KstViewEllipsePtr;

class KstViewEllipse : public KstViewObject {
  Q_OBJECT
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(QColor fillColor READ foregroundColor WRITE setForegroundColor)
  Q_PROPERTY(bool transparentFill READ transparentFill WRITE setTransparentFill)
  public:
    KstViewEllipse();
    KstViewEllipse(const QDomElement& e);
    KstViewEllipse(const KstViewEllipse& ellipse);
    virtual ~KstViewEllipse();

    virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    virtual void setBorderColor(const QColor& to);
    virtual QColor borderColor() const;
    virtual void setBorderWidth(int width);
    virtual int borderWidth() const;
    
    // can't have Q_PROPERTY in KstViewObject?
    virtual void setForegroundColor(const QColor& color);
    virtual QColor foregroundColor() const;
    
    bool transparentFill() const;
    void setTransparentFill(bool yes);

    
    virtual void drawShadow(KstPainter& p, const QPoint& pos);

    virtual void paintSelf(KstPainter& p, const QRegion& bounds);

  public:
    virtual void save(Q3TextStream& ts, const QString& indent = QString::null);
    
    virtual QMap<QString, QVariant> widgetHints(const QString& propertyName) const; 
    
    virtual signed int directionFor(const QPoint& pos);
    
  private:
    int _borderWidth;
    QColor _borderColor;
    bool _transparentFill;
};

typedef KstObjectList<KstViewEllipsePtr> KstViewEllipseList;


#endif
// vim: ts=2 sw=2 et
