/***************************************************************************
                                kstviewbox.h
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

#ifndef KSTVIEWBOX_H
#define KSTVIEWBOX_H

#include "kstviewobject.h"

#include <kglobal.h>

class KstViewBox;
typedef KstSharedPtr<KstViewBox> KstViewBoxPtr;

class KstViewBox : public KstViewObject {
  Q_OBJECT
  Q_PROPERTY(int xRound READ xRound WRITE setXRound)
  Q_PROPERTY(int yRound READ yRound WRITE setYRound)
  Q_PROPERTY(bool transparentFill READ transparentFill WRITE setTransparentFill)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor) 
  public:
    KstViewBox();
    KstViewBox(const QDomElement& e);
    KstViewBox(const KstViewBox& box);
    ~KstViewBox();

    virtual KstViewObject* copyObjectQuietly(KstViewObject& parent, const QString& name = QString::null) const;
    void setXRound(int rnd);
    int xRound() const;
    void setYRound(int rnd);
    int yRound() const;
    void setCornerStyle(Qt::PenJoinStyle style);
    Qt::PenJoinStyle cornerStyle() const;
    
    bool transparentFill() const;
    void setTransparentFill(bool yes);

    void paintSelf(KstPainter& p, const QRegion& bounds);

    void setBorderColor(const QColor& c);
    const QColor& borderColor() const;

    void setBorderWidth(int w);
    int borderWidth() const;

    // just calls KstViewObject functions - Q_PROPERTY doesn't work in KstViewObject?
    virtual void setForegroundColor(const QColor& color);
    virtual QColor foregroundColor() const;
    virtual void setBackgroundColor(const QColor& color);
    virtual QColor backgroundColor() const;

  public:
    void save(QTextStream& ts, const QString& indent = QString::null);
    
    QMap<QString, QVariant > widgetHints(const QString& propertyName) const;

  private:
    QColor _borderColor;
    int _borderWidth;
    int _xRound, _yRound;
    Qt::PenJoinStyle _cornerStyle;
    bool _transparentFill;
};

typedef KstObjectList<KstViewBoxPtr> KstViewBoxList;


#endif
// vim: ts=2 sw=2 et
