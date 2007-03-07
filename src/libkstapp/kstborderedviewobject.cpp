/***************************************************************************
                          kstborderedviewobject.cpp
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

#include "kstborderedviewobject.h"

#include <kglobal.h>
#include <klocale.h>

#include <qpainter.h>
#include <q3stylesheet.h>
#include <qvariant.h>

KstBorderedViewObject::KstBorderedViewObject(const QString& type)
: KstViewObject(type), _borderColor(QColor(0, 0, 0)), _borderWidth(0), _padding(0), _margin(0) {
}


KstBorderedViewObject::KstBorderedViewObject(const QDomElement& e)
: KstViewObject(e), _borderColor(QColor(0, 0, 0)), _borderWidth(0), _padding(0), _margin(0) {
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement(); // try to convert the node to an element.
    if (!el.isNull()) { // the node was really an element.
      if (el.tagName() == "border") {
        _borderColor.setNamedColor(el.attribute( "color", "#7f0000" ));
        _borderWidth = el.attribute( "width", "0" ).toInt();
        _padding = el.attribute( "padding", "0" ).toInt();
        _margin = el.attribute( "margin", "0" ).toInt();
      }
    }
    n = n.nextSibling();
  }
}


KstBorderedViewObject::KstBorderedViewObject(const KstBorderedViewObject& borderedViewObject)
: KstViewObject(borderedViewObject) {
  setBorderColor(borderedViewObject.borderColor());
  setBorderWidth(borderedViewObject.borderWidth());
  setMargin(borderedViewObject.margin());
  setPadding(borderedViewObject.padding());
}


KstBorderedViewObject::~KstBorderedViewObject() {
}


void KstBorderedViewObject::saveAttributesOnly(QTextStream& ts, const QString& indent) {
  ts << indent << "<border color=\""
    << Q3StyleSheet::escape(_borderColor.name())
    << "\" width=\"" << _borderWidth
    << "\" padding=\"" << _padding
    << "\" margin=\"" << _margin << "\" />" << endl;
}


void KstBorderedViewObject::save(QTextStream& ts, const QString& indent) {
  saveAttributesOnly(ts, indent);
  KstViewObject::save(ts, indent);
}


void KstBorderedViewObject::saveAttributes(QTextStream& ts, const QString& indent) {
  saveAttributesOnly(ts, indent);
  KstViewObject::saveAttributes(ts, indent);
}


void KstBorderedViewObject::paintSelf(KstPainter& p, const QRegion& bounds) {
  p.save();
  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    if (p.makingMask()) {
      p.setCompositionMode(QPainter::CompositionMode_Source);
      KstViewObject::paintSelf(p, bounds);
    } else {
      const QRegion clip(clipRegion());
      KstViewObject::paintSelf(p, bounds - clip);
      p.setClipRegion(bounds & clip);
    }
  }
  if (_borderWidth > 0) {
    QRect r;
    const int bw(_borderWidth * p.lineWidthAdjustmentFactor());
    QPen pen(_borderColor, bw);
    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    r.setX(_geom.left() + _margin + bw / 2);
    r.setY(_geom.top() + _margin + bw / 2);
    r.setWidth(_geom.width() - 2 * _margin - bw + 1);
    r.setHeight(_geom.height() - 2 * _margin - bw + 1);
    p.drawRect(r);
  }
  p.restore();
}


void KstBorderedViewObject::setBorderColor(const QColor& c) {
  if (_borderColor != c) {
    setDirty();
    _borderColor = c;
  }
}


const QColor& KstBorderedViewObject::borderColor() const {
  return _borderColor;
}


void KstBorderedViewObject::setBorderWidth(int w) {
  int mw = qMax(0, w);
  if (_borderWidth != mw) {
    _borderWidth = mw;
    setDirty();
  }
}


int KstBorderedViewObject::borderWidth() const {
  return _borderWidth;
}


void KstBorderedViewObject::setMargin(int w) {
  int mm = qMax(0, w);
  if (_margin != mm) {
    _margin = mm;
    setDirty();
  }
}


int KstBorderedViewObject::margin() const {
  return _margin;
}


void KstBorderedViewObject::setPadding(int p) {
  int mp = qMax(0, p);
  if (_padding != mp) {
    _padding = mp;
    setDirty();
  }
}


int KstBorderedViewObject::padding() const {
  return _padding;
}


QRect KstBorderedViewObject::contentsRectForPainter(const KstPainter& painter) const {
  QRect rc;
  const int mpb = (_margin + _padding + _borderWidth) * painter.lineWidthAdjustmentFactor();
  rc.setX(_geom.left() + mpb);
  rc.setY(_geom.top() + mpb);
  rc.setWidth(_geom.width() - 2 * mpb);
  rc.setHeight(_geom.height() - 2 * mpb);
  return rc;
}


QRect KstBorderedViewObject::contentsRect() const {
  QRect rc;
  const int mpb = _margin + _padding + _borderWidth;
  rc.setX(_geom.left() + mpb);
  rc.setY(_geom.top() + mpb);
  rc.setWidth(_geom.width() - 2 * mpb);
  rc.setHeight(_geom.height() - 2 * mpb);
  return rc;
}


void KstBorderedViewObject::setContentsRectForPainter(const KstPainter& painter, const QRect& rect) {
  const int mpb = (_margin + _padding + _borderWidth) * painter.lineWidthAdjustmentFactor();
  _geom.setX(rect.left() - mpb);
  _geom.setY(rect.top() - mpb);
  _geom.setWidth(rect.width() + 2 * mpb);
  _geom.setHeight(rect.height() + 2 * mpb);
}


void KstBorderedViewObject::setContentsRect(const QRect& rect) {
  const int mpb = _margin + _padding + _borderWidth;
  _geom.setX(rect.left() - mpb);
  _geom.setY(rect.top() - mpb);
  _geom.setWidth(rect.width() + 2 * mpb);
  _geom.setHeight(rect.height() + 2 * mpb);
  
  if(_parent) {
    _geom = _geom.intersect(_parent->geometry());
  }
}


void KstBorderedViewObject::writeBinary(QDataStream& str) {
  KstViewObject::writeBinary(str);
  str << _borderColor << _borderWidth << _padding << _margin;
}


void KstBorderedViewObject::readBinary(QDataStream& str) {
  KstViewObject::readBinary(str);
  str >> _borderColor >> _borderWidth >> _padding >> _margin;
}


QMap<QString, QVariant> KstBorderedViewObject::widgetHints(const QString& propertyName) const {
  QMap<QString, QVariant> map = KstViewObject::widgetHints(propertyName);
  if (!map.empty()) {
    return map;
  }

  if (propertyName == "borderColor") {
    map.insert(QString("_kst_widgetType"), QString("KColorButton"));
    map.insert(QString("_kst_label"), i18n("Border color"));
  } else if (propertyName == "borderWidth") {
    map.insert(QString("_kst_widgetType"), QString("QSpinBox"));
    map.insert(QString("_kst_label"), i18n("Border width"));
    map.insert(QString("minimum"), 0);
  }
  return map;
}


void KstBorderedViewObject::setBackgroundColor(const QColor& color) {
  KstViewObject::setBackgroundColor(color);
}


QColor KstBorderedViewObject::backgroundColor() const {
  return KstViewObject::backgroundColor();
}


void KstBorderedViewObject::setForegroundColor(const QColor& color) {
  KstViewObject::setForegroundColor(color);
}


QColor KstBorderedViewObject::foregroundColor() const {
  return KstViewObject::foregroundColor();
}


#include "kstborderedviewobject.moc"
// vim: ts=2 sw=2 et
