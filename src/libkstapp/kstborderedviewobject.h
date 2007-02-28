/***************************************************************************
                           kstborderedviewobject.h
                             -------------------
    begin                : Mar 11, 2004
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

#ifndef KSTBORDEREDVIEWOBJECT_H
#define KSTBORDEREDVIEWOBJECT_H

#include "kstviewobject.h"
#include "kst_export.h"


/***************************************************************************

 (position().x(), position().y())
      +-------------------------------------------------------+  ---
      |                   Margin                              |   ^
      |   +---------------------------- Border -----------+   |   |
      |   |            Padding                            |   |   |
      |   |                                               |   |   |
      |   |     +-----------------------------------+     |   |   |
      |   |     |        Object contents            |     |   |   |
      |   |     |                                   |     |   |  Height
      |   |     |                                   |     |   |   |
      |   |     |                                   |     |   |   |
      |   |     |                                   |     |   |   |
      |   |     +-----------------------------------+     |   |   |
      |   |                                               |   |   |
      |   |                                               |   |   |
      |   +-----------------------------------------------+   |   |
      |                                                       |   V
      +-------------------------------------------------------+  ---

      |<----------------------- Width ----------------------->|


Contents Rect
-------------
object.width = width() - 2*(margin() + padding() + borderWidth());
object.height = height() - 2*(margin() + padding() + borderWidth());
object.x = position().x() + margin() + padding() + borderWidth();
object.y = position().y() + margin() + padding() + borderWidth();

***************************************************************************/

class KstBorderedViewObject;
typedef KstSharedPtr<KstBorderedViewObject> KstBorderedViewObjectPtr;

class KST_EXPORT KstBorderedViewObject : public KstViewObject {
  Q_OBJECT
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
  Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor) 
  Q_PROPERTY(int margin READ margin WRITE setMargin) 
  Q_PROPERTY(int padding READ padding WRITE setPadding) 

  protected:
    KstBorderedViewObject(const QString& type);
  public:
    KstBorderedViewObject(const QDomElement& e);
    KstBorderedViewObject(const KstBorderedViewObject& borderedViewObject);
    virtual ~KstBorderedViewObject();

    virtual void save(Q3TextStream& ts, const QString& indent = QString::null);
    virtual void saveAttributes(Q3TextStream& ts, const QString& indent = QString::null);

    void setBorderColor(const QColor& c);
    const QColor& borderColor() const;

    void setBorderWidth(int w);
    int borderWidth() const;

    void setMargin(int w);
    int margin() const;

    void setPadding(int p);
    int padding() const;

    // See above for gross details
    virtual QRect contentsRect() const;
    virtual void setContentsRect(const QRect& rect);
    
    virtual QMap<QString, QVariant> widgetHints(const QString& propertyName) const;
    
    // just calls KstViewObject functions - Q_PROPERTY doesn't work in KstViewObject?
    virtual void setForegroundColor(const QColor& color);
    virtual QColor foregroundColor() const;
    virtual void setBackgroundColor(const QColor& color);
    virtual QColor backgroundColor() const;

    virtual void paintSelf(KstPainter& p, const QRegion& bounds);

  protected:
    virtual void readBinary(QDataStream& str);
    virtual void writeBinary(QDataStream& str);
    void saveAttributesOnly(Q3TextStream& ts, const QString& indent = QString::null);
    // Internal, indication of design problem.  Adjusts the contents rect based
    // on a given line width adjustment factor.
    QRect contentsRectForPainter(const KstPainter& painter) const;
    void setContentsRectForPainter(const KstPainter& painter, const QRect& rect);
  

  private:
    QColor _borderColor;
    int _borderWidth;
    int _padding, _margin;
};

typedef KstObjectList<KstBorderedViewObjectPtr> KstBorderedViewObjectList;


#endif
// vim: ts=2 sw=2 et
