/***************************************************************************
                              kstviewarrow.cpp
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

#include "kstgfxarrowmousehandler.h"
#include "kstmath.h"
#include "kstviewarrow.h"
#include "kstviewobjectfactory.h"

#include <math.h>

#include <klocale.h>

#include <qbitmap.h>
#include <qmetaobject.h>
#include <qpainter.h>
#include <qvariant.h>
//Added by qt3to4:
#include <Q3PointArray>

KstViewArrow::KstViewArrow()
: KstViewLine("Arrow") {
  _hasFromArrow = false;
  _hasToArrow = true;
  _fromArrowScaling = 1.0;
  _toArrowScaling = 1.0;
  _standardActions |= Delete | Edit;
}


KstViewArrow::KstViewArrow(const QDomElement& e)
: KstViewLine(e) {
  _hasFromArrow = false;
  _hasToArrow = true;
  _fromArrowScaling = 1.0;
  _toArrowScaling = 1.0;
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement(); 
    if (!el.isNull()) {
      if (metaObject()->indexOfProperty(el.tagName().toLatin1()) > -1) {
        setProperty(el.tagName().toLatin1(), QVariant(el.text()));
      }
    }
    n = n.nextSibling();
  }

  // always has this value
  _type = "Arrow";
  _standardActions |= Delete | Edit;
}


KstViewArrow::KstViewArrow(const KstViewArrow& arrow)
: KstViewLine(arrow) {
  _hasFromArrow = arrow._hasFromArrow;
  _hasToArrow = arrow._hasToArrow;
  _fromArrowScaling = arrow._fromArrowScaling;
  _toArrowScaling = arrow._toArrowScaling;
  
  // these always have these values
  _type = "Arrow";
  _standardActions |= Delete | Edit;
}


KstViewArrow::~KstViewArrow() {
}


KstViewObject* KstViewArrow::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  Q_UNUSED(name)

  KstViewArrow *viewArrow = new KstViewArrow(*this);
  parent.appendChild(viewArrow, true);

  return viewArrow;
}


void KstViewArrow::paintArrow(KstPainter& p, const QPoint& to, const QPoint &from, int w, double scaling) {
  double deltax = scaling * 2.0 * double(w);
  double theta = atan2(double(from.y() - to.y()), double(from.x() - to.x())) - M_PI / 2.0;
  double sina = sin(theta);
  double cosa = cos(theta);
  double yin = sqrt(3.0) * deltax;
  double x1, y1, x2, y2;
  QMatrix m(cosa, sina, -sina, cosa, 0.0, 0.0);
  
  m.map( deltax, yin, &x1, &y1);
  m.map(-deltax, yin, &x2, &y2);
  
  Q3PointArray pts(3);
  pts[0] = to;
  pts[1] = to + QPoint(d2i(x1), d2i(y1));
  pts[2] = to + QPoint(d2i(x2), d2i(y2));
  
  p.drawPolygon(pts);
}


QRegion KstViewArrow::clipRegion() {
  if (_clipMask.isNull()) {
    _myClipMask = QRegion();
    QBitmap bm1(_geom.bottomRight().x(), _geom.bottomRight().y(), true);
    if (!bm1.isNull()) {
      KstPainter p;
      p.setMakingMask(true);
      p.begin(&bm1);
      p.setViewXForm(true);
      KstViewLine::paintSelf(p, QRegion());
      p.end();
      _clipMask = QRegion(bm1);
    }
    QBitmap bm2(_geom.bottomRight().x(), _geom.bottomRight().y(), true);
    if (!bm2.isNull()) {
      KstPainter p;
      p.setMakingMask(true);
      p.begin(&bm2);
      p.setViewXForm(true);
      paintSelf(p, QRegion());
      p.end();
      _myClipMask = QRegion(bm2);
    }
  }

  return _myClipMask | _clipMask;
}


void KstViewArrow::paintSelf(KstPainter& p, const QRegion& bounds) {
  p.save();
  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    if (p.makingMask()) {
      p.setCompositionMode(QPainter::CompositionMode_Source);
    } else {
      const QRegion clip(clipRegion());
      KstViewLine::paintSelf(p, bounds - _myClipMask);
      p.setClipRegion(bounds & clip);
    }
  } else {
      KstViewLine::paintSelf(p, bounds);
  }
  
  if (hasArrow()) {
    QPoint to = KstViewLine::to();
    QPoint from = KstViewLine::from();    
    const int w = width() * p.lineWidthAdjustmentFactor();
    QPen pen(_foregroundColor, w);
    
    pen.setCapStyle(capStyle());
    p.setPen(pen);
    p.setBrush(_foregroundColor);
    
    if (_hasToArrow) {      
      paintArrow(p, to, from, w, _toArrowScaling);
    }
    if (_hasFromArrow) {      
      paintArrow(p, from, to, w, _fromArrowScaling);
    }
  }
  p.restore();
}


void KstViewArrow::save(QTextStream& ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  ts << indent + "  " << "<orientation>" << _orientation << "</orientation>" << endl;
  KstViewObject::save(ts, indent + "  ");
  ts << indent << "</" << type() << ">" << endl;
}


bool KstViewArrow::hasArrow() const {
  return _hasToArrow || _hasFromArrow;
}


QMap<QString, QVariant> KstViewArrow::widgetHints(const QString& propertyName) const {
  QMap<QString, QVariant> map = KstViewLine::widgetHints(propertyName);
  if (!map.empty()) {
    return map;  
  }
  if (propertyName == "hasFromArrow") {
    map.insert(QString("_kst_widgetType"), QString("QCheckBox"));
    map.insert(QString("_kst_label"), QVariant());
    map.insert(QString("text"), i18n("Arrow at start"));
  } else if (propertyName == "hasToArrow") {
    map.insert(QString("_kst_widgetType"), QString("QCheckBox"));
    map.insert(QString("_kst_label"), QVariant());
    map.insert(QString("text"), i18n("Arrow at end"));
  } else if (propertyName == "fromArrowScaling") {
    map.insert(QString("_kst_widgetType"), QString("KDoubleSpinBox"));
    map.insert(QString("_kst_label"), i18n("Start arrow scaling")); 
    map.insert(QString("minimum"), 1.0);
    map.insert(QString("maximum"), 100.0);
  } else if (propertyName == "toArrowScaling") {
    map.insert(QString("_kst_widgetType"), QString("KDoubleSpinBox"));
    map.insert(QString("_kst_label"), i18n("End arrow scaling")); 
    map.insert(QString("minimum"), 1.0);
    map.insert(QString("maximum"), 100.0);
  }
  return map;
}


bool KstViewArrow::hasFromArrow() const {
  return _hasFromArrow;  
}


void KstViewArrow::setHasFromArrow(bool yes) {
  if (_hasFromArrow != yes) {
    _hasFromArrow = yes;  
    setDirty();
  }
}


bool KstViewArrow::hasToArrow() const {
  return _hasToArrow;  
}


void KstViewArrow::setHasToArrow(bool yes) {
  if (_hasToArrow != yes) {
    _hasToArrow = yes;  
    setDirty();
  }
}


double KstViewArrow::fromArrowScaling() const {
  return _fromArrowScaling;  
}


void KstViewArrow::setFromArrowScaling(double scaling) {
  if (scaling < 1.0) {
    scaling = 1.0;  
  }  
  if (_fromArrowScaling != scaling) {
    _fromArrowScaling = scaling;
    setDirty();
  }
}


double KstViewArrow::toArrowScaling() const {
  return _toArrowScaling;  
}


void KstViewArrow::setToArrowScaling(double scaling) {
  if (scaling < 1.0) {
    scaling = 1.0;  
  }  
  if (_toArrowScaling != scaling) {
    _toArrowScaling = scaling;
    setDirty();
  }
}


namespace {
KstViewObject *create_KstViewArrow() {
  return new KstViewArrow;
}


KstGfxMouseHandler *handler_KstViewArrow() {
  return new KstGfxArrowMouseHandler;
}

KST_REGISTER_VIEW_OBJECT(Arrow, create_KstViewArrow, handler_KstViewArrow)
}


#include "kstviewarrow.moc"
// vim: ts=2 sw=2 et
