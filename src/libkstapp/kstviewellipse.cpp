/***************************************************************************
                               kstviewellipse.cpp
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

#include "kstaccessibility.h" 
#include "kstgfxellipsemousehandler.h"
#include "kstviewellipse.h"
#include "kstviewobjectfactory.h"

#include <klocale.h>

#include <qmetaobject.h>
#include <qpainter.h>
#include <qvariant.h>

KstViewEllipse::KstViewEllipse()
: KstViewObject("Ellipse"), _borderWidth(1) {
  setTransparent(true);
  _transparentFill = false;
  _standardActions |= Delete | Edit;
}


KstViewEllipse::KstViewEllipse(const QDomElement& e)
: KstViewObject(e) {
  _transparentFill = false;
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement(); 
    if (!el.isNull()) {
      if (metaObject()->findProperty(el.tagName().toLatin1(), true) > -1) {
        setProperty(el.tagName().toLatin1(), QVariant(el.text()));  
      }  
    }
    n = n.nextSibling();      
  }
  
  // always have these values
  _type = "Ellipse";
  setTransparent(true);
  _standardActions |= Delete | Edit;
  _layoutActions |= Delete | Raise | Lower | RaiseToTop | LowerToBottom | Rename | MoveTo | Copy | CopyTo;
}


KstViewEllipse::KstViewEllipse(const KstViewEllipse& ellipse)
: KstViewObject(ellipse) {
  _transparentFill = ellipse._transparentFill;
  _borderWidth = ellipse._borderWidth;
  _borderColor = ellipse._borderColor;
  
  // these always have these values
  _type = "Ellipse";
  _standardActions |= Delete | Edit;
}


KstViewEllipse::~KstViewEllipse() {
}


KstViewObject* KstViewEllipse::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  Q_UNUSED(name)

  KstViewEllipse* viewEllipse = new KstViewEllipse(*this);
  parent.appendChild(viewEllipse, true);
  
  return viewEllipse;
}


void KstViewEllipse::paintSelf(KstPainter& p, const QRegion& bounds) {
  p.save();
  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    if (p.makingMask()) {
      p.setRasterOp(Qt::SetROP);
      KstViewObject::paintSelf(p, geometry());
    } else {
      const QRegion clip(clipRegion());
      KstViewObject::paintSelf(p, bounds - clip);
      p.setClipRegion(bounds & clip);
    }
  }

  const int bw(_borderWidth * p.lineWidthAdjustmentFactor());
  QPen pen(bw > 0 ? _borderColor : _foregroundColor, bw);
  p.setPen(pen);
  if (_transparentFill) {
    p.setBrush(Qt::NoBrush);  
  } else {
    p.setBrush(_foregroundColor);
  }
  const QRect g(geometry());
  p.drawEllipse(g.x() + bw/2, g.y() + bw/2, g.width() - bw, g.height() - bw);
  p.restore();
}


void KstViewEllipse::save(QTextStream& ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  KstViewObject::save(ts, indent + "  ");
  ts << indent << "</" << type() << ">" << endl;
}


void KstViewEllipse::setBorderColor(const QColor& c) {
  if (_borderColor != c) {
    _borderColor = c;
    setDirty();
  }
}


QColor KstViewEllipse::borderColor() const {
  return _borderColor;
}


void KstViewEllipse::setBorderWidth(int width) {
  if (_borderWidth != width) {
    _borderWidth = qMax(0, width);
    setDirty();
  }
}


int KstViewEllipse::borderWidth() const {
  return _borderWidth;
}


void KstViewEllipse::setForegroundColor(const QColor& color) {
  KstViewObject::setForegroundColor(color);  
}


QColor KstViewEllipse::foregroundColor() const {
  return KstViewObject::foregroundColor();
}


QMap<QString, QVariant> KstViewEllipse::widgetHints(const QString& propertyName) const {
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
  } else if (propertyName == "fillColor") {
    map.insert(QString("_kst_widgetType"), QString("KColorButton"));
    map.insert(QString("_kst_label"), i18n("Fill Color"));    
  } else if (propertyName == "transparentFill") {
    map.insert(QString("_kst_widgetType"), QString("QCheckBox"));
    map.insert(QString("_kst_label"), QString::null);   
    map.insert(QString("text"), i18n("Transparent fill")); 
  }  
  return map;
}


signed int KstViewEllipse::directionFor(const QPoint& pos) {
  signed int direction = KstViewObject::directionFor(pos);
  if (direction != 0) {
    // not moving, so in any resize direction, we want it centred
    direction |= CENTEREDRESIZE;  
  }  
  return direction;
}


void KstViewEllipse::drawShadow(KstPainter& p, const QPoint& pos) {
  QRect rect(geometry());
  rect.moveTopLeft(pos);
  p.drawEllipse(rect);  
}

    
void KstViewEllipse::setTransparentFill(bool yes) {
  if (_transparentFill != yes) {
    _transparentFill = yes;
    setDirty();
  }
}


bool KstViewEllipse::transparentFill() const {
  return _transparentFill;
}


namespace {
KstViewObject *create_KstViewEllipse() {
  return new KstViewEllipse;
}


KstGfxMouseHandler *handler_KstViewEllipse() {
  return new KstGfxEllipseMouseHandler;
}

KST_REGISTER_VIEW_OBJECT(Ellipse, create_KstViewEllipse, handler_KstViewEllipse)
}


#include "kstviewellipse.moc"
// vim: ts=2 sw=2 et
