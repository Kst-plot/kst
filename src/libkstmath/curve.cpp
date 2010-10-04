/***************************************************************************
                          vcurve.cpp: holds info for a curve
                             -------------------
    begin                : Fri Nov 3 2000
    copyright            : (C) 2000 by cbn
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// includes for KDE
#include "kst_i18n.h"
#include <qdebug.h>
#include <QPolygonF>
#include <QXmlStreamWriter>

// application specific includes
#include "dialoglauncher.h"
#include "colorsequence.h"
#include "datacollection.h"
#include "debug.h"
#include "linestyle.h"
#include "math_kst.h"
#include "datavector.h"
#include "curve.h"
#include "ksttimers.h"
#include "objectstore.h"

#include <time.h>
#include <iostream>

// #define DEBUG_VECTOR_CURVE
// #define BENCHMARK

#ifndef KDE_IS_LIKELY
#if __GNUC__ - 0 >= 3
# define KDE_ISLIKELY( x )    __builtin_expect(!!(x),1)
# define KDE_ISUNLIKELY( x )  __builtin_expect(!!(x),0)
#else
# define KDE_ISLIKELY( x )   ( x )
# define KDE_ISUNLIKELY( x )  ( x )
#endif
#endif

// for painting
#define MAX_NUM_POLYLINES       1000

namespace Kst {

const QString Curve::staticTypeString = I18N_NOOP("Curve");
const QString Curve::staticTypeTag = I18N_NOOP("curve");

static const QLatin1String& XVECTOR = QLatin1String("X");
static const QLatin1String& YVECTOR = QLatin1String("Y");
static const QLatin1String& EXVECTOR = QLatin1String("EX");
static const QLatin1String& EYVECTOR = QLatin1String("EY");
static const QLatin1String& EXMINUSVECTOR = QLatin1String("EXMinus");
static const QLatin1String& EYMINUSVECTOR = QLatin1String("EYMinus");

Curve::Curve(ObjectStore *store)
: Relation(store) {
  setHasPoints(false);
  setHasBars(false);
  setHasLines(true);
  setLineWidth(1);
  setLineStyle(0);
  setBarStyle(0);
  setPointDensity(0);

  MaxX = MinX = MeanX = MaxY = MinY = MeanY = MinPosX = MinPosY = 0;
  NS = 0;
  _typeString = i18n("Curve");
  _type = "Curve";
  _initializeShortName();
  Color = QColor();
}

void Curve::_initializeShortName() {
  _shortName = 'C'+QString::number(_cnum);
  if (_cnum>max_cnum)
    max_cnum = _cnum;
  _cnum++;
}


Curve::~Curve() {
}


void Curve::internalUpdate() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  VectorPtr cxV = *_inputVectors.find(XVECTOR);
  VectorPtr cyV = *_inputVectors.find(YVECTOR);
  if (!cxV || !cyV) {
    return;
  }

  writeLockInputsAndOutputs();

  MaxX = cxV->max();
  MinX = cxV->min();
  MeanX = cxV->mean();
  MinPosX = cxV->minPos();
  _ns_maxx = cxV->ns_max();
  _ns_minx = cxV->ns_min();

  if (MinPosX > MaxX) {
    MinPosX = 0;
  }
  MaxY = cyV->max();
  MinY = cyV->min();
  MeanY = cyV->mean();
  MinPosY = cyV->minPos();
  _ns_maxy = cyV->ns_max();
  _ns_miny = cyV->ns_min();

  if (MinPosY > MaxY) {
    MinPosY = 0;
  }

  NS = qMax(cxV->length(), cyV->length());

  unlockInputsAndOutputs();

  _redrawRequired = true;

  return;
}


void Curve::point(int i, double &x, double &y) const {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
}


void Curve::getEXPoint(int i, double &x, double &y, double &ex) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr exv = xErrorVector();
  if (exv) {
    ex = exv->interpolate(i, NS);
  }
}


void Curve::getEXMinusPoint(int i, double &x, double &y, double &ex) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr exmv = xMinusErrorVector();
  if (exmv) {
    ex = exmv->interpolate(i, NS);
  }
}


void Curve::getEXPoints(int i, double &x, double &y, double &exminus, double &explus) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr exv = xErrorVector();
  if (exv) {
    explus = exv->interpolate(i, NS);
  }
  VectorPtr exmv = xMinusErrorVector();
  if (exmv) {
    exminus = exmv->interpolate(i, NS);
  }
}


void Curve::getEYPoint(int i, double &x, double &y, double &ey) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr eyv = yErrorVector();
  if (eyv) {
    ey = eyv->interpolate(i, NS);
  }
}


void Curve::getEYMinusPoint(int i, double &x, double &y, double &ey) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr eyv = yMinusErrorVector();
  if (eyv) {
    ey = eyv->interpolate(i, NS);
  }
}


void Curve::getEYPoints(int i, double &x, double &y, double &eyminus, double &eyplus) {
  VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  VectorPtr eyv = yErrorVector();
  if (eyv) {
    eyplus = eyv->interpolate(i, NS);
  }
  VectorPtr eymv = yMinusErrorVector();
  if (eymv) {
    eyminus = eymv->interpolate(i, NS);
  }
}


bool Curve::hasXError() const {
  return _inputVectors.contains(EXVECTOR);
}


bool Curve::hasYError() const {
  return _inputVectors.contains(EYVECTOR);
}


bool Curve::hasXMinusError() const {
  return _inputVectors.contains(EXMINUSVECTOR);
}


bool Curve::hasYMinusError() const {
  return _inputVectors.contains(EYMINUSVECTOR);
}


void Curve::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  s.writeAttribute("xvector", _inputVectors[XVECTOR]->Name());
  s.writeAttribute("yvector", _inputVectors[YVECTOR]->Name());
  if (_inputVectors.contains(EXVECTOR)) {
    s.writeAttribute("errorxvector", _inputVectors[EXVECTOR]->Name());
  }
  if (_inputVectors.contains(EYVECTOR)) {
    s.writeAttribute("erroryvector", _inputVectors[EYVECTOR]->Name());
  }
  if (_inputVectors.contains(EXMINUSVECTOR)) {
    s.writeAttribute("errorxminusvector", _inputVectors[EXMINUSVECTOR]->Name());
  }
  if (_inputVectors.contains(EYMINUSVECTOR)) {
    s.writeAttribute("erroryminusvector", _inputVectors[EYMINUSVECTOR]->Name());
  }
  s.writeAttribute("color", Color.name());

  s.writeAttribute("haslines", QVariant(HasLines).toString());
  s.writeAttribute("linewidth", QString::number(LineWidth));
  s.writeAttribute("linestyle", QString::number(LineStyle));

  s.writeAttribute("haspoints", QVariant(HasPoints).toString());
  s.writeAttribute("pointtype", QString::number(PointType));
  s.writeAttribute("pointdensity", QString::number(PointDensity));

  s.writeAttribute("hasbars", QVariant(HasBars).toString());
  s.writeAttribute("barstyle", QString::number(BarStyle));
  s.writeAttribute("ignoreautoscale", QVariant(_ignoreAutoScale).toString());

  saveNameInfo(s, CNUM);

  s.writeEndElement();
}


void Curve::setXVector(VectorPtr new_vx) {
  if (new_vx) {
    _inputVectors[XVECTOR] = new_vx;
  } else {
    _inputVectors.remove(XVECTOR);
  }
}


void Curve::setYVector(VectorPtr new_vy) {
  if (new_vy) {
    _inputVectors[YVECTOR] = new_vy;
  } else {
    _inputVectors.remove(YVECTOR);
  }
}


void Curve::setXError(VectorPtr new_ex) {
  if (new_ex) {
    _inputVectors[EXVECTOR] = new_ex;
  } else {
    _inputVectors.remove(EXVECTOR);
  }
}


void Curve::setYError(VectorPtr new_ey) {
  if (new_ey) {
    _inputVectors[EYVECTOR] = new_ey;
  } else {
    _inputVectors.remove(EYVECTOR);
  }
}


void Curve::setXMinusError(VectorPtr new_ex) {
  if (new_ex) {
    _inputVectors[EXMINUSVECTOR] = new_ex;
  } else {
    _inputVectors.remove(EXMINUSVECTOR);
  }
}


void Curve::setYMinusError(VectorPtr new_ey) {
  if (new_ey) {
    _inputVectors[EYMINUSVECTOR] = new_ey;
  } else {
    _inputVectors.remove(EYMINUSVECTOR);
  }
}


QString Curve::xLabel() const {
  return _inputVectors[XVECTOR]->label();
}


QString Curve::yLabel() const {
  return _inputVectors[YVECTOR]->label();
}


QString Curve::topLabel() const {
  return _inputVectors[YVECTOR]->descriptiveLabel();
}


QString Curve::propertyString() const {
  return i18n("%1 vs %2").arg(yVector()->Name()).arg(xVector()->Name());
}


void Curve::showNewDialog() {
  DialogLauncher::self()->showCurveDialog();
}


void Curve::showEditDialog() {
  DialogLauncher::self()->showCurveDialog(this);
}


int Curve::samplesPerFrame() const {
  const DataVector *rvp = dynamic_cast<const DataVector*>(_inputVectors[YVECTOR].data());
  return rvp ? rvp->samplesPerFrame() : 1;
}


VectorPtr Curve::xVector() const {
  return *_inputVectors.find(XVECTOR);
}


VectorPtr Curve::yVector() const {
  return *_inputVectors.find(YVECTOR);
}


VectorPtr Curve::xErrorVector() const {
  return *_inputVectors.find(EXVECTOR);
}


VectorPtr Curve::yErrorVector() const {
  return *_inputVectors.find(EYVECTOR);
}


VectorPtr Curve::xMinusErrorVector() const {
  return *_inputVectors.find(EXMINUSVECTOR);
}


VectorPtr Curve::yMinusErrorVector() const {
  return *_inputVectors.find(EYMINUSVECTOR);
}


bool Curve::xIsRising() const {
  return _inputVectors[XVECTOR]->isRising();
}


inline int indexNearX(double x, VectorPtr& xv, int NS) {
  // monotonically rising: we can do a binary search
  // should be reasonably fast
  if (xv->isRising()) {
    int i_top = NS - 1;
    int i_bot = 0;

    // don't pre-check for x outside of the curve since this is not
    // the common case.  It will be correct - just slightly slower...
    while (i_bot + 1 < i_top) {
      int i0 = (i_top + i_bot)/2;
      double rX = xv->interpolate(i0, NS);
      if (x < rX) {
        i_top = i0;
      } else {
        i_bot = i0;
      }
    }
    double xt = xv->interpolate(i_top, NS);
    double xb = xv->interpolate(i_bot, NS);
    if (xt - x < x - xb) {
      return i_top;
    } else {
      return i_bot;
    }
  } else {
    // Oh Oh... not monotonically rising - we have to search the entire curve!
    // May be unbearably slow for large vectors
    double rX = xv->interpolate(0, NS);
    double dx0 = fabs(x - rX);
    int i0 = 0;

    for (int i = 1; i < NS; ++i) {
      rX = xv->interpolate(i, NS);
      double dx = fabs(x - rX);
      if (dx < dx0) {
        dx0 = dx;
        i0 = i;
      }
    }
    return i0;
  }
}


/** getIndexNearXY: return index of point within (or closest too)
    x +- dx which is closest to y **/
int Curve::getIndexNearXY(double x, double dx_per_pix, double y) const {
  VectorPtr xv = *_inputVectors.find(XVECTOR);
  VectorPtr yv = *_inputVectors.find(YVECTOR);
  if (!xv || !yv) {
    return 0; // anything better we can do?
  }

  double xi, yi, dx, dxi, dy, dyi;
  bool first = true;
  int i,i0, iN, index;
  int sc = sampleCount();

  if (xv->isRising()) {
    iN = i0 = indexNearX(x, xv, NS);

    xi = xv->interpolate(i0, NS);
    while (i0 > 0 && x-dx_per_pix < xi) {
      xi = xv->interpolate(--i0, NS);
    }

    xi = xv->interpolate(iN, NS);
    while (iN < sc-1 && x+dx_per_pix > xi) {
      xi = xv->interpolate(++iN, NS);
    }
  } else {
    i0 = 0;
    iN = sampleCount()-1;
  }

  index = i0;
  xi = xv->interpolate(index, NS);
  yi = yv->interpolate(index, NS);
  dx = fabs(x - xi);
  dy = fabs(y - yi);

  for (i = i0 + 1; i <= iN; i++) {
    xi = xv->interpolate(i, NS);
    dxi = fabs(x - xi);
    if (dxi < dx_per_pix) {
      dx = dxi;
      yi = yv->interpolate(i, NS);
      dyi = fabs(y - yi);
      if (first || dyi < dy) {
        first = false;
        index = i;
        dy = dyi;
      }
    } else if (dxi < dx) {
      dx = dxi;
      index = i;
    }
  }
  return index;
}


void Curve::setHasPoints(bool in_HasPoints) {
  HasPoints = in_HasPoints;
}


void Curve::setHasLines(bool in_HasLines) {
  HasLines = in_HasLines;
}


void Curve::setHasBars(bool in_HasBars) {
  HasBars = in_HasBars;
}


void Curve::setLineWidth(int in_LineWidth) {
#ifdef Q_WS_WIN32
  if (in_LineWidth == 0) {
    in_LineWidth = 1;
  }
#endif
  LineWidth = in_LineWidth;
}


void Curve::setLineStyle(int in_LineStyle) {
  LineStyle = in_LineStyle;
}


void Curve::setBarStyle(int in_BarStyle) {
  BarStyle = in_BarStyle;
}


void Curve::setPointDensity(int in_PointDensity) {
  PointDensity = in_PointDensity;
}


void Curve::setPointType(int in_PointType) {
  PointType = in_PointType;
}


void Curve::setColor(const QColor& new_c) {
  Color = new_c;
}


double Curve::maxX() const {
  if (hasBars() && sampleCount() > 0) {
    return MaxX + (MaxX - MinX)/(2*(sampleCount()-1));
  }
  return MaxX;
}


double Curve::minX() const {
  if (hasBars() && sampleCount() > 0) {
    return MinX - (MaxX - MinX)/(2*(sampleCount()-1));
  }
  return MinX;
}


RelationPtr Curve::makeDuplicate(QMap<RelationPtr, RelationPtr> &duplicatedRelations) {
  CurvePtr curve = store()->createObject<Curve>();

  if (descriptiveNameIsManual()) {
    curve->setDescriptiveName(descriptiveName());
  }
  curve->setXVector(xVector());
  curve->setYVector(yVector());
  if (hasXError()) {
    curve->setXError(xErrorVector());
  }
  if (hasYError()) {
    curve->setYError(yErrorVector());
  }
  if (hasXMinusError()) {
    curve->setXMinusError(xMinusErrorVector());
  }
  if (hasYMinusError()) {
    curve->setYMinusError(yMinusErrorVector());
  }

  curve->setColor(Color);
  curve->setHasPoints(HasPoints);
  curve->setHasLines(HasLines);
  curve->setHasBars(HasBars);
  curve->setLineWidth(LineWidth);
  curve->setLineStyle(LineStyle);
  curve->setPointType(PointDensity);
  curve->setPointDensity(PointDensity);
  curve->setBarStyle(BarStyle);

  curve->writeLock();
  curve->registerChange();
  curve->unlock();

  duplicatedRelations.insert(this, RelationPtr(curve));
  return RelationPtr(curve);
}


void Curve::paintObjects(const CurveRenderContext& context) {
  QPainter *p = context.painter;
  p->save();

  //p->setRenderHint(QPainter::Antialiasing);
  Qt::PenStyle style = Kst::LineStyle[lineStyle()];

  if (hasBars()) {
    if (barStyle() == 1) { // filled
      p->setPen(QPen(context.foregroundColor, _width, style));
    } else {
      p->setPen(QPen(color(), _width, style));
    }

    foreach(const QRectF& rect, _filledRects) {
        p->fillRect(rect, color());
    }
  }
  p->setPen(QPen(color(), _width, style));

  foreach(const QPolygonF& poly, _polygons) {
    p->drawPolyline(poly);
  }
  foreach(const QLineF& line, _lines) {
    p->drawLine(line);
  }
  foreach(const QRectF& rect, _rects) {
    p->drawRect(rect);
  }
  foreach(const QPointF& point, _points) {
    CurvePointSymbol::draw(PointType, p, point.x(), point.y(), _width);
  }
  p->restore();
}


void Curve::updatePaintObjects(const CurveRenderContext& context) {
  _polygons.clear();
  _lines.clear();
  _points.clear();
  _filledRects.clear();
  _rects.clear();

  VectorPtr xv = *_inputVectors.find(XVECTOR);
  VectorPtr yv = *_inputVectors.find(YVECTOR);
  if (!xv || !yv) {
    return;
  }

  double Lx = context.Lx, Hx = context.Hx, Ly = context.Ly, Hy = context.Hy;
  double m_X = context.m_X, m_Y = context.m_Y;
  double b_X = context.b_X, b_Y = context.b_Y;
  double XMin = context.XMin, XMax = context.XMax;
  bool xLog = context.xLog, yLog = context.yLog;
  double xLogBase = context.xLogBase;
  double yLogBase = context.yLogBase;
  double maxY = 0.0, minY = 0.0;
  double rX = 0.0, rY, rEX, rEY;
  double X1 = 0.0, Y1 = 0.0;
  double X2 = 0.0, Y2 = 0.0;
  double last_x1, last_y1;
  bool overlap = false;
  int i_pt;

#ifdef BENCHMARK
  QTime bench_time, benchtmp;
  int b_1 = 0, b_2 = 0, b_3 = 0, b_4 = 0;
  bench_time.start();
  benchtmp.start();
  int numberOfLinesDrawn = 0;
  int numberOfPointsDrawn = 0;
  int numberOfBarsDrawn = 0;
#endif

  if (lineWidth() == 0) {
    _width = context.penWidth;
  } else if (context.penWidth > 0) {
    _width = lineWidth() * context.penWidth;
  } else {
    _width = lineWidth();
  }

  int pointDim = CurvePointSymbol::dim(context.window);
  if (sampleCount() > 0) {
    int i0, iN;

    if (xv->isRising()) {
      i0 = indexNearX(XMin, xv, NS);
      if (i0 > 0) {
        --i0;
      }
      iN = indexNearX(XMax, xv, NS);
      if (iN < sampleCount() - 1) {
        ++iN;
      }
    } else {
      i0 = 0;
      iN = sampleCount() - 1;
    }

#ifdef BENCHMARK
    clock_t linesStart = clock();
#endif
    if (hasLines()) {
      QPolygonF points;
      points.reserve(MAX_NUM_POLYLINES);

      double lastPlottedX = 0;
      double lastPlottedY = 0;
      int index = 0;
      int i0Start = i0;

// optimize - isnan seems expensive, at least in gcc debug mode
//            cachegrind backs this up.
#undef isnan
#define isnan(x) (x != x)
      rX = xv->interpolate(i0, NS);
      rY = yv->interpolate(i0, NS);
      // if invalid point then look backward for the last valid point.
      while (i0 > 0 && (isnan(rX) || isnan(rY))) {
        --i0;
        rX = xv->interpolate(i0, NS);
        rY = yv->interpolate(i0, NS);
      }

      // if invalid point then look forward for the next valid point...
      if (isnan(rX) || isnan(rY)) {
        i0 = i0Start;
        while (i0 < iN && (isnan(rX) || isnan(rY))) {
          ++i0;
          rX = xv->interpolate(i0, NS);
          rY = yv->interpolate(i0, NS);
        }
      }
 
      if (xLog) {
        rX = logXLo(rX, xLogBase);
      }
      if (yLog) {
        rY = logYLo(rY, yLogBase);
      }
      last_x1 = m_X*rX + b_X;
      last_y1 = m_Y*rY + b_Y;

      i_pt = i0;

      while (i_pt < iN) {
        X2 = last_x1;
        Y2 = last_y1;

        ++i_pt;
        rX = xv->interpolate(i_pt, NS);
        rY = yv->interpolate(i_pt, NS);
        bool foundNan = false;

        // if necessary continue looking for the first valid point...
        while (i_pt < iN && (isnan(rX) || isnan(rY))) {
#undef isnan
          foundNan = true;
          ++i_pt;
          rX = xv->interpolate(i_pt, NS);
          rY = yv->interpolate(i_pt, NS);
        }

        if (KDE_ISUNLIKELY(foundNan)) {
          if (points.size()>0) {
            _polygons.append(points);
#ifdef BENCHMARK
            ++numberOfLinesDrawn;
#endif
          }
          points.resize(0);
          if (overlap) {
            if (X2 >= Lx && X2 <= Hx) {
              if (maxY > Hy && minY <= Hy)
                maxY = Hy;
              if (minY < Ly && maxY >= Ly)
                minY = Ly;
              if (minY >= Ly && minY <= Hy && maxY >= Ly && maxY <= Hy) {
#ifdef BENCHMARK
                ++numberOfLinesDrawn;
#endif
                _lines.append(QLineF(X2, minY, X2, maxY));
              }
            }
            overlap = false;
          }
        }

        if (xLog) {
          rX = logXLo(rX, xLogBase);
        }
        if (yLog) {
          rY = logYLo(rY, yLogBase);
        }
        X1 = m_X*rX + b_X;
        Y1 = m_Y*rY + b_Y;
        last_x1 = X1;
        last_y1 = Y1;

        if (KDE_ISLIKELY(!foundNan)) {
          if (KDE_ISLIKELY(samePixel(X1, X2))) {
            if (KDE_ISLIKELY(overlap)) {
              if (KDE_ISUNLIKELY(Y1 > maxY)) {
                maxY = Y1;
              }
              if (KDE_ISUNLIKELY(Y1 < minY)) {
                minY = Y1;
              }
            } else {
              if (Y1 < Y2) {
                minY = Y1;
                maxY = Y2;
              } else {
                maxY = Y1;
                minY = Y2;
              }
              overlap = true;
            }
          } else {
            if (KDE_ISLIKELY(overlap)) {
              if (KDE_ISLIKELY(X2 >= Lx && X2 <= Hx)) {
                if (KDE_ISUNLIKELY(maxY <= Hy && minY >= Ly)) {

                  if (points.size()>MAX_NUM_POLYLINES-2) {
                    _polygons.append(points);
                    points.resize(0);
#ifdef BENCHMARK
                    ++numberOfLinesDrawn;
#endif
                  }

                  if (KDE_ISUNLIKELY(minY == maxY)) {
                    points.append(QPointF(X2, maxY));
                  } else if (KDE_ISUNLIKELY(Y2 == minY)) {
                    points.append(QPointF(X2, maxY));
                    points.append(QPointF(X2, minY));
                  } else if (KDE_ISUNLIKELY(Y2 == maxY)) {
                    points.append(QPointF(X2, minY));
                    points.append(QPointF(X2, maxY));
                  } else {
                    points.append(QPointF(X2, minY));
                    points.append(QPointF(X2, maxY));
                    if (KDE_ISLIKELY(Y2 >= Ly && Y2 <= Hy)) {
                      points.append(QPointF(X2, Y2));
                    }
                  }
                  lastPlottedX = X2;
                  lastPlottedY = Y2;
                } else {
                  if (KDE_ISUNLIKELY(maxY > Hy && minY <= Hy)) {
                    maxY = Hy;
                  }
                  if (KDE_ISUNLIKELY(minY < Ly && maxY >= Ly)) {
                    minY = Ly;
                  }
                  if (KDE_ISUNLIKELY(minY >= Ly && minY <= Hy && maxY >= Ly && maxY <= Hy)) {
                    if (points.size()>0) {
                      _polygons.append(points);
                      points.resize(0);
#ifdef BENCHMARK
                      ++numberOfLinesDrawn;
#endif
                    }
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                    _lines.append(QLineF(X2, minY, X2, maxY));
                  }
                }
              }
              overlap = false;
            }

            if (KDE_ISLIKELY(!((X1 < Lx && X2 < Lx) || (X1 > Hx && X2 > Hx)))) {

              // trim the line to be within the plot...
              if (KDE_ISUNLIKELY(isinf(X1))) {
                Y1 = Y2;
                if (X1 > 0.0) {
                  X1 = Hx;
                } else {
                  X1 = Lx;
                }
              }

              if (KDE_ISUNLIKELY(isinf(X2))) {
                Y2 = Y1;
                if (X2 > 0.0) {
                  X2 = Hx;
                } else {
                  X2 = Lx;
                }
              }

              if (KDE_ISUNLIKELY(isinf(Y1))) {
                X1 = X2;
                if (Y1 > 0.0) {
                  Y1 = Hy;
                } else {
                  Y1 = Ly;
                }
              }

              if (KDE_ISUNLIKELY(isinf(Y2))) {
                X2 = X1;
                if (Y2 > 0.0) {
                  Y2 = Hy;
                } else {
                  Y2 = Ly;
                }
              }

              if (KDE_ISUNLIKELY(X1 < Lx && X2 > Lx)) {
                Y1 = (Y2 - Y1) / (X2 - X1) * (Lx - X1) + Y1;
                X1 = Lx;
              } else if (KDE_ISUNLIKELY(X2 < Lx && X1 > Lx)) {
                Y2 = (Y1 - Y2) / (X1 - X2) * (Lx - X2) + Y2;
                X2 = Lx;
              }

              if (KDE_ISUNLIKELY(X1 < Hx && X2 > Hx)) {
                Y2 = (Y2 - Y1) / (X2 - X1) * (Hx - X1) + Y1;
                X2 = Hx;
              } else if (KDE_ISUNLIKELY(X2 < Hx && X1 > Hx)) {
                Y1 = (Y1 - Y2) / (X1 - X2) * (Hx - X2) + Y2;
                X1 = Hx;
              }

              if (KDE_ISUNLIKELY(Y1 < Ly && Y2 > Ly)) {
                X1 = (X2 - X1) / (Y2 - Y1) * (Ly - Y1) + X1;
                Y1 = Ly;
              } else if (KDE_ISUNLIKELY(Y2 < Ly && Y1 > Ly)) {
                X2 = (X1 - X2) / (Y1 - Y2) * (Ly - Y2) + X2;
                Y2 = Ly;
              }

              if (KDE_ISUNLIKELY(Y1 < Hy && Y2 > Hy)) {
                X2 = (X2 - X1) / (Y2 - Y1) * (Hy - Y1) + X1;
                Y2 = Hy;
              } else if (KDE_ISUNLIKELY(Y2 < Hy && Y1 > Hy)) {
                X1 = (X1 - X2) / (Y1 - Y2) * (Hy - Y2) + X2;
                Y1 = Hy;
              }


#ifdef DEBUG_VECTOR_CURVE
bool xInBounds = (X1 >= Lx && X1 <= Hx && X2 >= Lx && X2 <= Hx);
bool yInBounds = (Y1 >= Ly && Y1 <= Hy && Y2 >= Ly && Y2 <= Hy);

if (!xInBounds)
qDebug() << "x not in bounds"
         << "X1:" << X1
         << "X2:" << X2
         << "Lx:" << Lx
         << "Hx:" << Hx
         << endl;
if (!yInBounds)
qDebug() << "y not in bounds"
         << "Y1:" << Y1
         << "Y2:" << Y2
         << "Ly:" << Ly
         << "Hy:" << Hy
         << endl;
#endif

              if (X1 >= Lx && X1 <= Hx && X2 >= Lx && X2 <= Hx &&
                  Y1 >= Ly && Y1 <= Hy && Y2 >= Ly && Y2 <= Hy) {


                if (KDE_ISUNLIKELY(points.size()==0)) {
                    points.append(QPointF(X2, Y2));
                    points.append(QPointF(X1, Y1));
                } else if (samePixel(lastPlottedX, X2) &&
                    samePixel(lastPlottedY,Y2) &&
                    index < MAX_NUM_POLYLINES) {
                  points.append(QPointF(X1, Y1));
                } else {
                  if (KDE_ISLIKELY(points.size()>1)) {
                    _polygons.append(points);
                    points.resize(0);
#ifdef BENCHMARK
                    ++numberOfLinesDrawn;
#endif
                  }
                  points.append(QPointF(X2, Y2));
                  points.append(QPointF(X1, Y1));
                }
                lastPlottedX = X1;
                lastPlottedY = Y1;
              }
            }
          } // end if (X1 == X2)
        } // end if (!foundNan)
      } // end while

      // we might a have polyline left undrawn...
      if (points.size()>1) {
        _polygons.append(points);
        points.resize(0);
#ifdef BENCHMARK
        ++numberOfLinesDrawn;
#endif
      }

      // we might have some overlapping points still unplotted...
      if (overlap) {
        if (X2 >= Lx && X2 <= Hx) {
          if (maxY > Hy && minY <= Hy) {
            maxY = Hy;
          }
          if (minY < Ly && maxY >= Ly) {
            minY = Ly;
          }
          if (minY >= Ly && minY <= Hy && maxY >= Ly && maxY <= Hy) {
#ifdef BENCHMARK
            ++numberOfLinesDrawn;
#endif
           _lines.append(QLineF(X2, minY, X2, maxY));
          }
        }
        overlap = false;
      }
    } // end if hasLines()
#ifdef BENCHMARK
    clock_t linesEnd = clock();
    std::cout << "\n        Lines clocks: " << (linesEnd - linesStart) << "\n";
    b_1 = benchtmp.elapsed();
#endif

    VectorPtr exv = _inputVectors.contains(EXVECTOR) ? *_inputVectors.find(EXVECTOR) : 0;
    VectorPtr eyv = _inputVectors.contains(EYVECTOR) ? *_inputVectors.find(EYVECTOR) : 0;
    VectorPtr exmv = _inputVectors.contains(EXMINUSVECTOR) ? *_inputVectors.find(EXMINUSVECTOR) : 0;
    VectorPtr eymv = _inputVectors.contains(EYMINUSVECTOR) ? *_inputVectors.find(EYMINUSVECTOR) : 0;
    // draw the bargraph bars, if any...
    if (hasBars()) {
      bool visible = true;
      double rX2 = 0.0;
      double drX = 0.0;
      QRectF lastRect;

      if (!exv) {
        // determine the bar position width. NOTE: This is done
        //  only if xv->isRising() as in this case the calculation
        //  is simple...
        drX = (maxX() - minX())/double(sampleCount());
        if (xv->isRising()) {
          double oldX = 0.0;

          for (i_pt = i0; i_pt <= iN; i_pt++) {
            rX = xv->interpolate(i_pt, NS);
            if (i_pt > i0) {
              if (rX - oldX < drX) {
                drX = rX - oldX;
              }
            }
            oldX = rX; 
          }
        }
      }

      for (i_pt = i0; i_pt <= iN; i_pt++) {
        visible = true;

        if (exv) {
          drX = exv->interpolate(i_pt, NS);
        }
        rX = xv->interpolate(i_pt, NS);
        rY = yv->interpolate(i_pt, NS);
        rX -= drX/2.0;
        rX2 = rX + drX;
        if (xLog) {
          rX = logXLo(rX, xLogBase);
          rX2 = logXLo(rX2, xLogBase);
        }
        if (yLog) {
          rY = logYLo(rY, yLogBase);
          Y2 = Hy;
        } else {
          Y2 = b_Y;
        }

        X1 = m_X * rX + b_X;
        X2 = m_X * rX2 + b_X;
        Y1 = m_Y * rY + b_Y;

        if ((X1 > Hx) || (X2 < Lx) || (Y1 == Ly && Y2 == Ly) || (Y1 == Hy && Y2 == Hy)) {
          visible = false;
        }

        if (visible) {
          QRectF rect(X1, Y1, X2 - X1, Y2 - Y1);
          if (!lastRect.contains(rect)) {
            if (barStyle() == 1) { // filled
              _filledRects.append(rect);
            } else {
              _rects.append(rect);
            }
            lastRect = rect;
#ifdef BENCHMARK
            ++numberOfBarsDrawn;
#endif
          }
        }
      }
    }

#ifdef BENCHMARK
    b_2 = benchtmp.elapsed();
#endif

    // draw the points, if any...
    if (hasPoints()) {
      const double w = Hx - Lx;
      const double h = Hy - Ly;
      int size = 0;
      if (hasLines() && pointDensity() != 0) {
        size = int(qMax(w, h)) / int(pow(3.0, POINTDENSITY_MAXTYPE - pointDensity()));
      }

      QRectF rect(Lx, Ly, w, h);
      QPointF pt, lastPt;

      for (i_pt = i0; i_pt <= iN; ++i_pt) {
        rX = xv->interpolate(i_pt, NS);
        rY = yv->interpolate(i_pt, NS);
        if (xLog) {
          rX = logXLo(rX, xLogBase);
        }
        if (yLog) {
          rY = logYLo(rY, yLogBase);
        }

        pt.setX(m_X * rX + b_X);
        pt.setY(m_Y * rY + b_Y);
        if (rect.contains(pt) && pt != lastPt &&
            (lastPt.isNull() || !((abs(pt.x() - lastPt.x()) < size) || (abs(pt.y() - lastPt.y()) < size)))) {
#ifdef BENCHMARK
          ++numberOfPointsDrawn;
#endif
            lastPt = pt;
            _points.append(pt);
        }
      }
    }

#ifdef BENCHMARK
    b_3 = benchtmp.elapsed();
#endif

    // draw the x-errors, if any...
    if ((exv || exmv) && !hasBars()) {
      double rX1;
      double rX2;
      bool do_low_flag = true;
      bool do_high_flag = true;
      bool errorSame = false;

      if (exv && exmv) {
        if (exv->shortName() == exmv->shortName()) {
          errorSame = true;
        }
      }

      for (i_pt = i0; i_pt <= iN; i_pt++) {
        do_low_flag = true;
        do_high_flag = true;

        rX = xv->interpolate(i_pt, NS);
        rY = yv->interpolate(i_pt, NS);
        if (errorSame) {
          rEX = fabs(exv->interpolate(i_pt, NS));
          if (xLog) {
            rX1 = logXLo(rX - rEX, xLogBase);
            rX2 = logXLo(rX + rEX, xLogBase);
          } else {
            rX1 = rX - rEX;
            rX2 = rX + rEX;
          }
        } else if (exv && exmv) {
          double rEXHi = fabs(exv->interpolate(i_pt, NS));
          double rEXLo = fabs(exmv->interpolate(i_pt, NS));
          if (xLog) {
            rX1 = logXLo(rX - rEXLo, xLogBase);
            rX2 = logXLo(rX + rEXHi, xLogBase);
          } else {
            rX1 = rX - rEXLo;
            rX2 = rX + rEXHi;
          }
        } else if (exv) {
          rEX = exv->interpolate(i_pt, NS);
          if (xLog) {
            rX1 = logXLo(rX, xLogBase);
            rX2 = logXLo(rX + fabs(rEX), xLogBase);
          } else {
            rX1 = rX;
            rX2 = rX + fabs(rEX);
          }
          do_low_flag = false;
        } else {
          rEX = fabs(exmv->interpolate(i_pt, NS));
          if (xLog) {
            rX1 = logXLo(rX - rEX, xLogBase);
            rX2 = logXLo(rX, xLogBase);
          } else {
            rX1 = rX - rEX;
            rX2 = rX;
          }
          do_high_flag = false;
        }

        if (yLog) {
          rY = logYLo(rY, yLogBase);
        }

        X1 = m_X * rX1 + b_X;
        X2 = m_X * rX2 + b_X;
        Y1 = m_Y * rY + b_Y;

        if (X1 < Lx && X2 > Lx) {
          X1 = Lx;
          do_low_flag = false;
        }
        if (X1 < Hx && X2 > Hx) {
          X2 = Hx;
          do_high_flag = false;
        }

        if (X1 >= Lx && X2 <= Hx && Y1 >= Ly && Y1 <= Hy) {
          _lines.append(QLineF(X1, Y1, X2, Y1));
          if (do_low_flag) {
            _lines.append(QLineF(X1, Y1 + pointDim, X1, Y1 - pointDim));
          }
          if (do_high_flag) {
            _lines.append(QLineF(X2, Y1 + pointDim, X2, Y1 - pointDim));
          }
        }
      }
    }

    // draw the y-errors, if any...
    if (eyv || eymv) {
      double rY1;
      double rY2;
      bool do_low_flag = true;
      bool do_high_flag = true;
      bool errorSame = false;

      if (eyv && eymv) {
        if (eyv->shortName() == eymv->shortName()) {
          errorSame = true;
        }
      }

      for (i_pt = i0; i_pt <= iN; i_pt++) {
        do_low_flag = true;
        do_high_flag = true;

        rX = xv->interpolate(i_pt, NS);
        rY = yv->interpolate(i_pt, NS);
        if (errorSame) {
          rEY = eyv->interpolate(i_pt, NS);
          if (yLog) {
            rY1 = logYLo(rY-fabs(rEY), yLogBase);
            rY2 = logYLo(rY+fabs(rEY), yLogBase);
          } else {
            rY1 = rY-fabs(rEY);
            rY2 = rY+fabs(rEY);
          }
        } else if (eyv && eymv) {
          double rEYHi = fabs(eyv->interpolate(i_pt, NS));
          double rEYLo = fabs(eymv->interpolate(i_pt, NS));
          if (yLog) {
            rY1 = logYLo(rY - rEYLo, yLogBase);
            rY2 = logYLo(rY + rEYHi, yLogBase);
          } else {
            rY1 = rY - rEYLo;
            rY2 = rY + rEYHi;
          }
        } else if (eyv) {
          rEY = fabs(eyv->interpolate(i_pt, NS));
          if (yLog) {
            rY1 = logYLo(rY, yLogBase);
            rY2 = logYLo(rY + rEY, yLogBase);
          } else {
            rY1 = rY;
            rY2 = rY + rEY;
          }
          do_low_flag = false;
        } else {
          rEY = fabs(eymv->interpolate(i_pt, NS));
          if (yLog) {
            rY1 = logYLo(rY - rEY, yLogBase);
            rY2 = logYLo(rY, yLogBase);
          } else {
            rY1 = rY - rEY;
            rY2 = rY;
          }
          do_high_flag = false;
        }

        if (xLog) {
          rX = logXLo(rX, xLogBase);
        }

        X1 = m_X * rX + b_X;
        Y1 = m_Y * rY1 + b_Y;
        Y2 = m_Y * rY2 + b_Y;

        if (Y1 < Ly && Y2 > Ly) {
          Y1 = Ly;
          do_low_flag = false;
        }
        if (Y1 < Hy && Y2 > Hy) {
          Y2 = Hy;
          do_high_flag = false;
        }

        if (X1 >= Lx && X1 <= Hx && Y1 >= Ly && Y2 <= Hy) {
          _lines.append(QLineF(X1, Y1, X1, Y2));
          if (do_low_flag) {
          _lines.append(QLineF(X1 + pointDim, Y1, X1 - pointDim, Y1));
          }
          if (do_high_flag) {
          _lines.append(QLineF(X1 + pointDim, Y2, X1 - pointDim, Y2));
          }
        }
      }
    } // end if (hasYError())
  } // end if (sampleCount() > 0)

#ifdef BENCHMARK
  b_4 = benchtmp.elapsed();
#endif

#ifdef BENCHMARK
  int i = bench_time.elapsed();
  //qDebug() << endl << "Plotting curve " << (void *)this << ": " << i << "ms";
  //qDebug() << "    Without locks: " << b_4 << "ms";
  //qDebug() << "    Number of lines drawn:" << numberOfLinesDrawn;
  //qDebug() << "    Number of points drawn:" << numberOfPointsDrawn;
  //qDebug() << "    Number of bars drawn:" << numberOfBarsDrawn;
  std::cout << "\nPlotting curve " << (void *)this << ": " << i << "ms";
  std::cout << "\n    Without locks: " << b_4 << "ms";
  std::cout << "\n    Number of lines drawn:" << numberOfLinesDrawn;
  std::cout << "\n    Number of points drawn:" << numberOfPointsDrawn;
  std::cout << "\n    Number of bars drawn:" << numberOfBarsDrawn;
  if (b_1 > 0)       std::cout << "\n            Lines: " << b_1 << "ms\n";
  if (b_2 - b_1 > 0) std::cout << "\n             Bars: " << (b_2 - b_1) << "ms\n";
  if (b_3 - b_2 > 0) std::cout << "\n           Points: " << (b_3 - b_2) << "ms\n";
  if (b_4 - b_3 > 0) std::cout << "\n           Errors: " << (b_4 - b_3) << "ms\n";
#endif
}


void Curve::yRange(double xFrom, double xTo, double* yMin, double* yMax) {
  if (!yMin || !yMax) {
    return;
  }

  VectorPtr xv = *_inputVectors.find(XVECTOR);
  VectorPtr yv = *_inputVectors.find(YVECTOR);
  if (!xv || !yv) {
    *yMin = *yMax = 0;
    return;
  }

  // get range of the curve to search for min/max
  int i0, iN;
  if (xv->isRising()) {
    i0 = indexNearX(xFrom, xv, NS);
    iN = indexNearX(xTo, xv, NS);
  } else {
    i0 = 0;
    iN = sampleCount() - 1;
  }
  // search for min/max
  bool first = true;
  double newYMax = 0, newYMin = 0;
  for (int i_pt = i0; i_pt <= iN; i_pt++) {
    double rX = xv->interpolate(i_pt, NS);
    double rY = yv->interpolate(i_pt, NS);
    // make sure this point is visible
    if (rX >= xFrom && rX <= xTo) {
      // update min/max
      if (first || rY > newYMax) {
        newYMax = rY;
      }
      if (first || rY < newYMin) {
        newYMin = rY;
      }
      first = false;
    }
  }
  *yMin = newYMin;
  *yMax = newYMax;
}


DataObjectPtr Curve::providerDataObject() const {
  DataObjectPtr provider = 0L;
  // FIXME: fix this.. I don't know what's going on here
#if 0
  vectorList.lock().readLock();
  VectorPtr vp = *vectorList.findTag(yVTag().tag());  // FIXME: should use full tag
  vectorList.lock().unlock();
  if (vp) {
    vp->readLock();
    provider = kst_cast<DataObject>(vp->provider());
    vp->unlock();
  }
#endif
  return provider;
}


double Curve::distanceToPoint(double xpos, double dx, double ypos) const {
// find the y distance between the curve and a point. return 1.0E300 if this distance is undefined. i don't want to use -1 because it will make the code which uses this function messy.
  VectorPtr xv = *_inputVectors.find(XVECTOR);
  if (!xv) {
    return 1.0E300; // anything better we can do?
  }

  double distance = 1.0E300;

  int i_near_x = getIndexNearXY(xpos, dx, ypos);
  double near_x, near_y;
  point(i_near_x, near_x, near_y);

  if (fabs(near_x - xpos) < dx) {
    distance = fabs(ypos - near_y); // initial estimate.
  }

  if (hasLines() && xv->isRising()) {
    // if hasLines then we should find the distance between the curve and the point, not the data and 
    //  the point. if isRising because it is (probably) to slow to use this technique if the data is 
    //  unordered. borrowed from indexNearX. use binary search to find the indices immediately above 
    //  and below our xpos.
    int i_top = NS - 1;
    int i_bot = 0;

    while (i_bot + 1 < i_top) {
      int i0 = (i_top + i_bot)/2;

      double rX = xv->interpolate(i0, NS);
      if (xpos < rX) {
        i_top = i0;
      } else {
        i_bot = i0;
      }
    }
    // end borrowed

    double x_bot, y_bot, x_top, y_top;
    point(i_bot, x_bot, y_bot);
    point(i_top, x_top, y_top);

    if (x_bot <= xpos && x_top >= xpos) {
      near_y = (y_top - y_bot) / (x_top - x_bot) * (xpos - x_bot) + y_bot; // calculate y value for line segment between x_bot and x_top.
      
      if (fabs(ypos - near_y) < distance) {
        distance = fabs(ypos - near_y);
      }
    }
  }

  return distance;
}


void Curve::paintLegendSymbol(QPainter *p, const QRectF& bound) {
  int width;
  
  if (lineWidth() == 0) {
    width = 1;
  } else {  
    width = lineWidth();
  }
  
  p->save();
  if (hasLines()) {
    // draw a line from left to right centered vertically
    p->setPen(QPen(color(), width, Kst::LineStyle[lineStyle()]));
    p->drawLine(QLineF(bound.left(), bound.top() + bound.height()*.5,
                bound.right(), bound.top() + bound.height()*.5));
  }
  if (hasPoints()) {
    // draw a point in the middle
    p->setPen(QPen(color(), width));
    CurvePointSymbol::draw(PointType, p, bound.left() + bound.width()*.5, bound.top() + bound.height()*.5, width, 600);
  }
  p->restore();
}

QString Curve::_automaticDescriptiveName() const {
  return i18n("%1 vs %2").arg(yVector()->descriptiveName()).arg(xVector()->descriptiveName());
}

QString Curve::descriptionTip() const {
  QString tip;

  tip = i18n("Curve: %1\nX: %2\nY: %3").arg(Name()).arg(xVector()->descriptionTip()).arg(yVector()->descriptionTip());

  if (hasXError()) {
    tip += i18n("\nX+ Error: %1").arg(xErrorVector()->Name());
  }

  if (hasXMinusError()) {
    tip += i18n("\nX- Error: %1").arg(xMinusErrorVector()->Name());
  }

  if (hasYError()) {
    tip += i18n("\nY+ Error: %1").arg(yErrorVector()->Name());
  }

  if (hasYMinusError()) {
    tip += i18n("\nY- Error: %1").arg(yMinusErrorVector()->Name());
  }

  if (hasLines()) {
    tip += i18n("\nLines: Width %1 and Style %2").arg(lineWidth()).arg(lineStyle());
  }

  if (hasPoints()) {
    tip += i18n("\nPoints: Style %1").arg(pointType());
  }

  if (hasBars()) {
    tip += i18n("\nBars");
  }

  return tip;
}

}
// vim: ts=2 sw=2 et
