/***************************************************************************
                               kstviewline.cpp
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
#include "kstgfxlinemousehandler.h"
#include "kstviewline.h"
#include "kstviewobjectfactory.h"

#include <klocale.h>

#include <qmetaobject.h>
#include <qpainter.h>
#include <qvariant.h>

#include <math.h>

KstViewLine::KstViewLine(const QString& type)
: KstViewObject(type), _width(0) {
  setTransparent(true);
  _container = false;
  _capStyle = Qt::FlatCap; 
  _penStyle = Qt::SolidLine; 
  setMinimumSize(QSize(1, 1));
  _standardActions |= Delete | Edit;
}


KstViewLine::KstViewLine(const QDomElement& e)
: KstViewObject(e) {
  _width = 0;
  _container = false;
  _capStyle = Qt::FlatCap; 
  _penStyle = Qt::SolidLine;
  int orientationInt = 0;
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement el = n.toElement(); 
    if (!el.isNull()) {
      if (el.tagName() == "orientation") {
        orientationInt = el.text().toInt();
      } else if (metaObject()->findProperty(el.tagName().toLatin1(), true) > -1) {
        setProperty(el.tagName().toLatin1(), QVariant(el.text()));  
      }
    }
    n = n.nextSibling();
  }
  
  switch (orientationInt) {
    case 1:
      _orientation = UpRight;
      break;
    case 2:
      _orientation = DownLeft;
      break;
    case 3:
      _orientation = DownRight;
      break;
    case 0:
    default:
      _orientation = UpLeft;  
  }

  // always these values
  setTransparent(true);
  _type = "Line";
  setMinimumSize(QSize(1, 1));
  _standardActions |= Delete | Edit;
}


KstViewLine::KstViewLine(const KstViewLine& line)
: KstViewObject(line) {
  _capStyle = line._capStyle;
  _penStyle = line._penStyle;
  _orientation = line._orientation;
  _width = line._width;
  
  // these always have these values
  _type = "Line";
  _standardActions |= Delete | Edit;
}


KstViewLine::~KstViewLine() {
}


KstViewObject* KstViewLine::copyObjectQuietly(KstViewObject& parent, const QString& name) const {
  Q_UNUSED(name)

  KstViewLine* viewLine = new KstViewLine(*this);
  parent.appendChild(viewLine, true);
  
  return viewLine;
}


void KstViewLine::paintSelf(KstPainter& p, const QRegion& bounds) {
  p.save();
  if (p.type() != KstPainter::P_PRINT && p.type() != KstPainter::P_EXPORT) {
    if (p.makingMask()) {
      p.setCompositionMode(QPainter::CompositionMode_Source);
      KstViewObject::paintSelf(p, geometry());
    } else {
      const QRegion clip(clipRegion());
      KstViewObject::paintSelf(p, bounds - clip);
      p.setClipRegion(bounds & clip);
    }
  }

  // figure out which direction to draw the line
  const int w(_width * p.lineWidthAdjustmentFactor());
  QPen pen(_foregroundColor, w);
  pen.setCapStyle(_capStyle);
  pen.setStyle(_penStyle);
  p.setPen(pen);

  const QRect geom(geometry());
  int u = 0, v = 0;
  
  // Adjust for large widths.  We don't want the line clipped because it goes
  // out of the bounding box.
  if (w > 1 && geom.height() > 0) {
    double theta = atan(geom.width()/geom.height());
    if (theta >= 0 && theta <= M_PI/4) {
      u = int(fabs((w / 2.0) * (sin(theta) + cos(theta))));
      v = int(fabs((w / 2.0) * (1.5*sin(theta) + 0.5*cos(theta))));
    } else {
      u = int(fabs((w / 2.0) * (1.5*sin(theta) + 0.5*cos(theta))));
      v = int(fabs((w / 2.0) * (sin(theta) + cos(theta))));
    }
  }

  switch (_orientation) {
    case UpLeft:
    case DownRight:
      p.drawLine(geom.bottomRight() + QPoint(-u, -v), geom.topLeft() + QPoint(u, v));
      break;
    case UpRight:
    case DownLeft:
      p.drawLine(geom.bottomLeft() + QPoint(u, -v), geom.topRight() + QPoint(-u, v));
      break;
  }
  p.restore();
}


void KstViewLine::save(QTextStream& ts, const QString& indent) {
  ts << indent << "<" << type() << ">" << endl;
  // and save the orientation
  ts << indent + "  " << "<orientation>" << _orientation << "</orientation>" << endl;
  KstViewObject::save(ts, indent + "  ");
  ts << indent << "</" << type() << ">" << endl;
}


void KstViewLine::setFrom(const QPoint& from) {
  if (_from != from) {
    _from = from;
    // line drawing finished; update size, origin, and orientation
    updateOrientation();
    setDirty();
  }
}


QPoint KstViewLine::from() const {
  switch (_orientation) {
    case DownRight:
      return geometry().topLeft();
    case DownLeft:
      return geometry().topRight();
    case UpRight:
      return geometry().bottomLeft();
    case UpLeft:
      return geometry().bottomRight();
  }
  return QPoint(-1,-1);
}


void KstViewLine::setTo(const QPoint& to) {
  if (_to != to) {
    _to = to;
    // line drawing finished; update size, origin, and orientation
    updateOrientation();
    setDirty();
  }
}


QPoint KstViewLine::to() const {
  switch (_orientation) {
    case DownRight:
      return geometry().bottomRight();
    case DownLeft:
      return geometry().bottomLeft();
    case UpRight:
      return geometry().topRight();
    case UpLeft:
      return geometry().topLeft();
  }
  return QPoint(-1,-1);
}


void KstViewLine::setWidth(int width) {
  if (_width != width) {
    _width = width;
    //updateOrientation();
    setDirty();
  }
}


int KstViewLine::width() const {
  return _width;
}


void KstViewLine::setCapStyle(Qt::PenCapStyle style) {
  if (_capStyle != style) {
    _capStyle = style;
    setDirty();
  }
}


Qt::PenCapStyle KstViewLine::capStyle() const {
  return _capStyle;
}


void KstViewLine::setPenStyle(Qt::PenStyle style) {
  if (_penStyle != style) {
    _penStyle = style;
    setDirty();
  }
}


Qt::PenStyle KstViewLine::penStyle() const {
  return _penStyle;
}


void KstViewLine::move(const QPoint& pos) {
  KstViewObject::move(pos);
  if (_from.x() < _to.x()) {
    if (_from.y() < _to.y()) {
      _from = _geom.topLeft();
      _to = _geom.bottomRight();
    } else {
      _from = _geom.bottomLeft();
      _to = _geom.topRight();
    }
  } else {
    if (_from.y() < _to.y()) {
      _from = _geom.topRight();
      _to = _geom.bottomLeft();
    } else {
      _from = _geom.bottomRight();
      _to = _geom.topLeft();
    }
  }
}


void KstViewLine::updateOrientation() {
  if (_from.x() < _to.x()) {
    if (_from.y() < _to.y()) {
      _orientation = DownRight;  
      KstViewObject::move(_from);
      KstViewObject::resize(QSize(qMax(_width, _to.x() - _from.x() + 1), qMax(_width, _to.y() - _from.y() + 1)));
    } else {
      _orientation = UpRight;  
      KstViewObject::move(QPoint(_from.x(), _to.y()));
      KstViewObject::resize(QSize(qMax(_width, _to.x() - _from.x() + 1), qMax(_width, _from.y() - _to.y() + 1)));
    }
  } else {
    if (_from.y() < _to.y()) {
      _orientation = DownLeft;  
      KstViewObject::move(QPoint(_to.x(), _from.y()));
      KstViewObject::resize(QSize(qMax(_width, _from.x() - _to.x() + 1), qMax(_width, _to.y() - _from.y() + 1)));
    } else {
      _orientation = UpLeft;  
      KstViewObject::move(_to);
      KstViewObject::resize(QSize(qMax(_width, _from.x() - _to.x() + 1), qMax(_width, _from.y() - _to.y() + 1)));
    }
  }
}


void KstViewLine::drawFocusRect(KstPainter& p) {
  // draw the hotpoints
  QPoint point1, point2;
  
  const int dx = KST_RESIZE_BORDER_W/2;

  const QRect geom(geometry());
  if (_orientation == UpLeft || _orientation == DownRight) {
    point1 = QPoint(geom.left() - dx, geom.top() - dx);
    point2 = QPoint(geom.right() - dx, geom.bottom() - dx);
  } else {
    point1 = QPoint(geom.right() - dx, geom.top() - dx);
    point2 = QPoint(geom.left() - dx, geom.bottom() - dx);
  }
  QRect rect;
  rect.setSize(QSize(2 * dx + 1, 2 * dx + 1));
  rect.moveTopLeft(point1);
  p.drawRect(rect);
  rect.moveTopLeft(point2);
  p.drawRect(rect);
}


void KstViewLine::drawSelectRect(KstPainter& p) {
  p.setPen(QPen(Qt::black, 0));
  p.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  drawFocusRect(p);
}


inline bool linePointsCloseEnough(const QPoint& point1, const QPoint& point2) {
  const int dx = KST_RESIZE_BORDER_W/2;
  return point1.x() <= point2.x() + dx &&
      point1.x() >= point2.x() - dx &&
      point1.y() <= point2.y() + dx &&
      point1.y() >= point2.y() - dx;
}


signed int KstViewLine::directionFor(const QPoint& pos) {
  if (!isSelected()) {
    return NONE;  
  }
  
  const QRect geom(geometry());
  switch (_orientation) {
    case UpLeft:
      if (linePointsCloseEnough(pos, geom.topLeft())) {
        return ENDPOINT | DOWN;
      } else if (linePointsCloseEnough(pos, geom.bottomRight())) {
        return ENDPOINT | UP;
      }
      break;
    case DownRight:
      if (linePointsCloseEnough(pos, geom.bottomRight())) {
        return ENDPOINT | DOWN;
      } else if (linePointsCloseEnough(pos, geom.topLeft())) {
        return ENDPOINT | UP;
      }
      break;
    case UpRight:
      if (linePointsCloseEnough(pos, geom.topRight())) {
        return ENDPOINT | DOWN;
      } else if (linePointsCloseEnough(pos, geom.bottomLeft())) {
        return ENDPOINT | UP;
      }
      break;
    case DownLeft:
      if (linePointsCloseEnough(pos, geom.bottomLeft())) {
        return ENDPOINT | DOWN;
      } else if (linePointsCloseEnough(pos, geom.topRight())) {
        return ENDPOINT | UP;
      }
      break;
    default:
      break;
  }
  return NONE;
}


QMap<QString, QVariant> KstViewLine::widgetHints(const QString& propertyName) const {
  QMap<QString, QVariant> map = KstViewObject::widgetHints(propertyName);
  if (!map.empty()) {
    return map;  
  }
  
  if (propertyName == "width") {
    map.insert(QString("_kst_widgetType"), QString("QSpinBox"));
    map.insert(QString("_kst_label"), i18n("Line width"));  
    map.insert(QString("minimum"), 0);
  } else if (propertyName == "lineStyle") {
    map.insert(QString("_kst_widgetType"), QString("PenStyleWidget"));
    map.insert(QString("_kst_label"), i18n("Line style"));  
  } else if (propertyName == "lineColor") {
    map.insert(QString("_kst_widgetType"), QString("KColorButton"));
    map.insert(QString("_kst_label"), i18n("Line color"));    
  }
  return map;
}


int KstViewLine::penStyleWrap() const {
  switch (penStyle()) {
    case Qt::DashLine:
      return 1;
    case Qt::DotLine:
      return 2;
    case Qt::DashDotLine:
      return 3;
    case Qt::DashDotDotLine:
      return 4;
    case Qt::SolidLine:
    default:
      return 0;  
  }
}


void KstViewLine::setPenStyleWrap(int style) {
  switch (style) {
    case 1:
      setPenStyle(Qt::DashLine);
      break;
    case 2:
      setPenStyle(Qt::DotLine);
      break;
    case 3:
      setPenStyle(Qt::DashDotLine);
      break;
    case 4:
      setPenStyle(Qt::DashDotDotLine);
      break;  
    case 0:
    default:
      setPenStyle(Qt::SolidLine);
      break;
  }
}


void KstViewLine::setForegroundColor(const QColor& color) {
  KstViewObject::setForegroundColor(color);  
}


QColor KstViewLine::foregroundColor() const {
  return KstViewObject::foregroundColor();  
}


void KstViewLine::drawShadow(KstPainter& p, const QPoint& pos) {
  QPoint point1, point2;
  QRect rect = geometry();
  rect.moveTopLeft(pos);
  if (_orientation == UpLeft || _orientation == DownRight) {
    point1 = pos;
    point2 = rect.bottomRight();
  } else {
    point1 = rect.topRight();
    point2 = rect.bottomLeft();
  }
  p.drawLine(point1, point2);
}


QRect KstViewLine::surroundingGeometry() const {
  QRect geom(geometry());
  if (from().x() == to().x()) {
    //vertical line
    geom.setLeft(geom.left() - width()/2 - 1);
    geom.setRight(geom.right() + width()/2 + 1);
  } else if (from().y() == to().y()) {
    //horizontal line
    geom.setTop(geom.top() - width()/2 - 1);
    geom.setBottom(geom.bottom() + width()/2 + 1);
  }
  return geom;
}


namespace {
KstViewObject *create_KstViewLine() {
  return new KstViewLine;
}


KstGfxMouseHandler *handler_KstViewLine() {
  return new KstGfxLineMouseHandler;
}

KST_REGISTER_VIEW_OBJECT(Line, create_KstViewLine, handler_KstViewLine)
}


#include "kstviewline.moc"
// vim: ts=2 sw=2 et
