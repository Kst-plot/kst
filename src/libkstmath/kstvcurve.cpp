/***************************************************************************
                          kstcurve.cpp: holds info for a curve
                             -------------------
    begin                : Fri Nov 3 2000
    copyright            : (C) 2000 by cbn
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

// includes for KDE
#include "kst_i18n.h"
#include <qdebug.h>
#include <QPolygon>
#include <QTextDocument>

// application specific includes
#include "dialoglauncher.h"
#include "kstcolorsequence.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include "kstlinestyle.h"
#include "kstmath.h"
#include "datavector.h"
#include "kstvcurve.h"
#include "ksttimers.h"

#include <time.h>

// #define DEBUG_VECTOR_CURVE

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

static const QLatin1String& COLOR_XVECTOR = QLatin1String("X");
static const QLatin1String& COLOR_YVECTOR = QLatin1String("Y");
static const QLatin1String& EXVECTOR = QLatin1String("EX");
static const QLatin1String& EYVECTOR = QLatin1String("EY");
static const QLatin1String& EXMINUSVECTOR = QLatin1String("EXMinus");
static const QLatin1String& EYMINUSVECTOR = QLatin1String("EYMinus");

KstVCurve::KstVCurve(const QString &in_tag, Kst::VectorPtr in_X, Kst::VectorPtr in_Y,
                      Kst::VectorPtr in_EX, Kst::VectorPtr in_EY,
                      Kst::VectorPtr in_EXMinus, Kst::VectorPtr in_EYMinus,
                      const QColor &in_color)
: KstRelation() {
  setHasPoints(false);
  setHasBars(false);
  setHasLines(true);
  setLineWidth(1);
  setLineStyle(0);
  setBarStyle(0);
  setPointDensity(0);

  commonConstructor(in_tag, in_color);
  if (in_X) {
    _inputVectors[COLOR_XVECTOR] = in_X;
  }

  if (in_Y) {
    _inputVectors[COLOR_YVECTOR] = in_Y;
  }

  if (in_EX) {
    _inputVectors[EXVECTOR] = in_EX;
  }

  if (in_EY) {
    _inputVectors[EYVECTOR] = in_EY;
  }

  if (in_EXMinus) {
    _inputVectors[EXMINUSVECTOR] = in_EXMinus;
  }

  if (in_EYMinus) {
    _inputVectors[EYMINUSVECTOR] = in_EYMinus;
  }

  setDirty();
}


KstVCurve::KstVCurve(QDomElement &e)
: KstRelation(e) {
  QString in_tag, xname, yname, exname, eyname, exminusname, eyminusname;
  // QColor in_color(KstColorSequence::next(-1));
  QColor in_color("red"); // the above line is invalid.
  bool hasMinus = false;

  setHasPoints(false);
  setHasLines(false);
  setHasBars(false);
  setLineWidth(1);
  setLineStyle(0);
  setBarStyle(0);
  setPointDensity(0);

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (!e.isNull()) { // the node was really an element.
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "hasMinus") {
        hasMinus = true;
      } else if (e.tagName() == "xvectag") {
        xname = e.text();
      } else if (e.tagName() == "yvectag") {
        yname = e.text();
      } else if (e.tagName() == "exVectag") {
        exname = e.text();
        if (!hasMinus) {
          exminusname = e.text();
        }
      } else if (e.tagName() == "eyVectag") {
        eyname = e.text();
        if (!hasMinus) {
          eyminusname = e.text();
        }
      } else if (e.tagName() == "exMinusVectag") {
        exminusname = e.text();
      } else if (e.tagName() == "eyMinusVectag") {
        eyminusname = e.text();
      } else if (e.tagName() == "color") {
        in_color.setNamedColor(e.text());
      } else if (e.tagName() == "legend") {
        setLegendText(e.text());
      // the following options are only needed to change from the default
      } else if (e.tagName() == "hasLines") {
        HasLines = e.text() != "0";
      } else if (e.tagName() == "hasPoints") {
        HasPoints = e.text() != "0";
      } else if (e.tagName() == "hasBars") {
        HasBars = e.text() != "0";
      } else if (e.tagName() == "pointType") {
        pointType = e.text().toInt();
      } else if (e.tagName() == "lineWidth") {
        LineWidth = e.text().toInt();
      } else if (e.tagName() == "lineStyle") {
        LineStyle = e.text().toInt();
      } else if (e.tagName() == "barStyle") {
        BarStyle = e.text().toInt();
      } else if (e.tagName() == "pointDensity") {
        PointDensity = e.text().toInt();
      } else if (e.tagName() == "ignoreAutoScale") {
        _ignoreAutoScale = true;
      }
    }
    n = n.nextSibling();
  }

  if (!xname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(COLOR_XVECTOR), xname));
  }
  if (!yname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(COLOR_YVECTOR), yname));
  }
  if (!exname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(EXVECTOR), exname));
  }
  if (!eyname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(EYVECTOR), eyname));
  }
  if (!exminusname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(EXMINUSVECTOR), exminusname));
  }
  if (!eyminusname.isEmpty()) {
    _inputVectorLoadQueue.append(qMakePair(QString(EYMINUSVECTOR), eyminusname));
  }
  commonConstructor(in_tag, in_color);
}


void KstVCurve::commonConstructor(const QString &in_tag, const QColor &in_color) {
  MaxX = MinX = MeanX = MaxY = MinY = MeanY = MinPosX = MinPosY = 0;
  NS = 0;
  _typeString = i18n("Curve");
  _type = "Curve";
  Color = in_color;
  setTagName(KstObjectTag::fromString(in_tag));
  updateParsedLegendTag();
}


KstVCurve::~KstVCurve() {
}


KstObject::UpdateType KstVCurve::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  Kst::VectorPtr cxV = *_inputVectors.find(COLOR_XVECTOR);
  Kst::VectorPtr cyV = *_inputVectors.find(COLOR_YVECTOR);
  if (!cxV || !cyV) {
    return setLastUpdateResult(NO_CHANGE);
  }

  writeLockInputsAndOutputs();

  bool depUpdated = force;

  depUpdated = UPDATE == cxV->update(update_counter) || depUpdated;
  depUpdated = UPDATE == cyV->update(update_counter) || depUpdated;

  Kst::VectorPtr exV = _inputVectors.contains(EXVECTOR) ? *_inputVectors.find(EXVECTOR) : 0;
  if (exV) {
    depUpdated = UPDATE == exV->update(update_counter) || depUpdated;
  }

  Kst::VectorPtr eyV = _inputVectors.contains(EYVECTOR) ? *_inputVectors.find(EYVECTOR) : 0;
  if (eyV) {
    depUpdated = UPDATE == eyV->update(update_counter) || depUpdated;
  }

  Kst::VectorPtr exmV = _inputVectors.contains(EXMINUSVECTOR) ? *_inputVectors.find(EXMINUSVECTOR) : 0;
  if (exmV) {
    depUpdated = UPDATE == exmV->update(update_counter) || depUpdated;
  }

  Kst::VectorPtr eymV = _inputVectors.contains(EYMINUSVECTOR) ? *_inputVectors.find(EYMINUSVECTOR) : 0;
  if (eymV) {
    depUpdated = UPDATE == eymV->update(update_counter) || depUpdated;
  }

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

  return setLastUpdateResult(depUpdated ? UPDATE : NO_CHANGE);
}


void KstVCurve::point(int i, double &x, double &y) const {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
}


void KstVCurve::getEXPoint(int i, double &x, double &y, double &ex) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr exv = xErrorVector();
  if (exv) {
    ex = exv->interpolate(i, NS);
  }
}


void KstVCurve::getEXMinusPoint(int i, double &x, double &y, double &ex) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr exmv = xMinusErrorVector();
  if (exmv) {
    ex = exmv->interpolate(i, NS);
  }
}


void KstVCurve::getEXPoints(int i, double &x, double &y, double &exminus, double &explus) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr exv = xErrorVector();
  if (exv) {
    explus = exv->interpolate(i, NS);
  }
  Kst::VectorPtr exmv = xMinusErrorVector();
  if (exmv) {
    exminus = exmv->interpolate(i, NS);
  }
}


void KstVCurve::getEYPoint(int i, double &x, double &y, double &ey) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr eyv = yErrorVector();
  if (eyv) {
    ey = eyv->interpolate(i, NS);
  }
}


void KstVCurve::getEYMinusPoint(int i, double &x, double &y, double &ey) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr eyv = yMinusErrorVector();
  if (eyv) {
    ey = eyv->interpolate(i, NS);
  }
}


void KstVCurve::getEYPoints(int i, double &x, double &y, double &eyminus, double &eyplus) {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    x = xv->interpolate(i, NS);
  }
  Kst::VectorPtr yv = yVector();
  if (yv) {
    y = yv->interpolate(i, NS);
  }
  Kst::VectorPtr eyv = yErrorVector();
  if (eyv) {
    eyplus = eyv->interpolate(i, NS);
  }
  Kst::VectorPtr eymv = yMinusErrorVector();
  if (eymv) {
    eyminus = eymv->interpolate(i, NS);
  }
}


KstObjectTag KstVCurve::xVTag() const {
  Kst::VectorPtr xv = xVector();
  if (xv) {
    return xv->tag();
  }
  return KstObjectTag::invalidTag;
}


KstObjectTag KstVCurve::yVTag() const {
  Kst::VectorPtr yv = yVector();
  if (yv) {
    return yv->tag();
  }
  return KstObjectTag::invalidTag;
}


KstObjectTag KstVCurve::xETag() const {
  Kst::VectorPtr v = xErrorVector();
  if (v) {
    return v->tag();
  }
  return KstObjectTag::invalidTag;
}


KstObjectTag KstVCurve::yETag() const {
  Kst::VectorPtr v = yErrorVector();
  if (v) {
    return v->tag();
  }
  return KstObjectTag::invalidTag;
}


KstObjectTag KstVCurve::xEMinusTag() const {
  Kst::VectorPtr v = xMinusErrorVector();
  if (v) {
    return v->tag();
  }
  return KstObjectTag::invalidTag;
}


KstObjectTag KstVCurve::yEMinusTag() const {
  Kst::VectorPtr v = yMinusErrorVector();
  if (v) {
    return v->tag();
  }
  return KstObjectTag::invalidTag;
}


bool KstVCurve::hasXError() const {
  return _inputVectors.contains(EXVECTOR);
}


bool KstVCurve::hasYError() const {
  return _inputVectors.contains(EYVECTOR);
}


bool KstVCurve::hasXMinusError() const {
  return _inputVectors.contains(EXMINUSVECTOR);
}


bool KstVCurve::hasYMinusError() const {
  return _inputVectors.contains(EYMINUSVECTOR);
}


void KstVCurve::save(QTextStream &ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<curve>" << endl;
  ts << l2 << "<tag>" << Qt::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<xvectag>" << Qt::escape(_inputVectors[COLOR_XVECTOR]->tag().tagString()) << "</xvectag>" << endl;
  ts << l2 << "<yvectag>" << Qt::escape(_inputVectors[COLOR_YVECTOR]->tag().tagString()) << "</yvectag>" << endl;
  ts << l2 << "<legend>" << Qt::escape(legendText()) << "</legend>" << endl;
  ts << l2 << "<hasMinus/>" << endl;
  if (_inputVectors.contains(EXVECTOR)) {
    ts << l2 << "<exVectag>" << Qt::escape(_inputVectors[EXVECTOR]->tag().tagString()) << "</exVectag>" << endl;
  }
  if (_inputVectors.contains(EYVECTOR)) {
    ts << l2 << "<eyVectag>" << Qt::escape(_inputVectors[EYVECTOR]->tag().tagString()) << "</eyVectag>" << endl;
  }
  if (_inputVectors.contains(EXMINUSVECTOR)) {
    ts << l2 << "<exMinusVectag>" << Qt::escape(_inputVectors[EXMINUSVECTOR]->tag().tagString()) << "</exMinusVectag>" << endl;
  }
  if (_inputVectors.contains(EYMINUSVECTOR)) {
    ts << l2 << "<eyMinusVectag>" << Qt::escape(_inputVectors[EYMINUSVECTOR]->tag().tagString()) << "</eyMinusVectag>" << endl;
  }
  ts << l2 << "<color>" << Color.name() << "</color>" << endl;
  if (HasLines) {
    ts << l2 << "<hasLines/>" << endl;
  }
  ts << l2 << "<lineWidth>" << LineWidth << "</lineWidth>" << endl;
  ts << l2 << "<lineStyle>" << LineStyle << "</lineStyle>" << endl;
  if (HasPoints) {
    ts << l2 << "<hasPoints/>" << endl;
  }
  ts << l2 << "<pointType>" << pointType << "</pointType>" << endl;
  ts << l2 << "<pointDensity>" << PointDensity << "</pointDensity>" << endl;
  if (HasBars) {
    ts << l2 << "<hasBars/>" << endl;
  }
  ts << l2 << "<barStyle>" << BarStyle << "</barStyle>" << endl;
  if (_ignoreAutoScale) {
    ts << l2 << "<ignoreAutoScale/>" << endl;
  }
  ts << indent << "</curve>" << endl;
}


void KstVCurve::setXVector(Kst::VectorPtr new_vx) {
  if (new_vx) {
    _inputVectors[COLOR_XVECTOR] = new_vx;
  } else {
    _inputVectors.remove(COLOR_XVECTOR);
  }
  setDirty();
}


void KstVCurve::setYVector(Kst::VectorPtr new_vy) {
  if (new_vy) {
    _inputVectors[COLOR_YVECTOR] = new_vy;
  } else {
    _inputVectors.remove(COLOR_YVECTOR);
  }
  setDirty();
}


void KstVCurve::setXError(Kst::VectorPtr new_ex) {
  if (new_ex) {
    _inputVectors[EXVECTOR] = new_ex;
  } else {
    _inputVectors.remove(EXVECTOR);
  }
  setDirty();
}


void KstVCurve::setYError(Kst::VectorPtr new_ey) {
  if (new_ey) {
    _inputVectors[EYVECTOR] = new_ey;
  } else {
    _inputVectors.remove(EYVECTOR);
  }
  setDirty();
}


void KstVCurve::setXMinusError(Kst::VectorPtr new_ex) {
  if (new_ex) {
    _inputVectors[EXMINUSVECTOR] = new_ex;
  } else {
    _inputVectors.remove(EXMINUSVECTOR);
  }
  setDirty();
}


void KstVCurve::setYMinusError(Kst::VectorPtr new_ey) {
  if (new_ey) {
    _inputVectors[EYMINUSVECTOR] = new_ey;
  } else {
    _inputVectors.remove(EYMINUSVECTOR);
  }
  setDirty();
}


QString KstVCurve::xLabel() const {
  return _inputVectors[COLOR_XVECTOR]->label();
}


QString KstVCurve::yLabel() const {
  return _inputVectors[COLOR_YVECTOR]->label();
}


QString KstVCurve::topLabel() const {
  return QString::null;
  //return VY->fileLabel();
}


KstCurveType KstVCurve::curveType() const {
  return KST_VCURVE;
}


QString KstVCurve::propertyString() const {
  return i18n("%1 vs %2").arg(yVTag().displayString()).arg(xVTag().displayString());
}


void KstVCurve::showNewDialog() {
  Kst::DialogLauncher::self()->showCurveDialog();
}


void KstVCurve::showEditDialog() {
  Kst::DialogLauncher::self()->showCurveDialog(this);
}


int KstVCurve::samplesPerFrame() const {
  const Kst::DataVector *rvp = dynamic_cast<const Kst::DataVector*>(_inputVectors[COLOR_YVECTOR].data());
  return rvp ? rvp->samplesPerFrame() : 1;
}


Kst::VectorPtr KstVCurve::xVector() const {
  return *_inputVectors.find(COLOR_XVECTOR);
}


Kst::VectorPtr KstVCurve::yVector() const {
  return *_inputVectors.find(COLOR_YVECTOR);
}


Kst::VectorPtr KstVCurve::xErrorVector() const {
  return *_inputVectors.find(EXVECTOR);
}


Kst::VectorPtr KstVCurve::yErrorVector() const {
  return *_inputVectors.find(EYVECTOR);
}


Kst::VectorPtr KstVCurve::xMinusErrorVector() const {
  return *_inputVectors.find(EXMINUSVECTOR);
}


Kst::VectorPtr KstVCurve::yMinusErrorVector() const {
  return *_inputVectors.find(EYMINUSVECTOR);
}


bool KstVCurve::xIsRising() const {
  return _inputVectors[COLOR_XVECTOR]->isRising();
}


inline int indexNearX(double x, Kst::VectorPtr& xv, int NS) {
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
int KstVCurve::getIndexNearXY(double x, double dx_per_pix, double y) const {
  Kst::VectorPtr xv = *_inputVectors.find(COLOR_XVECTOR);
  Kst::VectorPtr yv = *_inputVectors.find(COLOR_YVECTOR);
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


void KstVCurve::setHasPoints(bool in_HasPoints) {
  HasPoints = in_HasPoints;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setHasLines(bool in_HasLines) {
  HasLines = in_HasLines;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setHasBars(bool in_HasBars) {
  HasBars = in_HasBars;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setLineWidth(int in_LineWidth) {
  LineWidth = in_LineWidth;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setLineStyle(int in_LineStyle) {
  LineStyle = in_LineStyle;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setBarStyle(int in_BarStyle) {
  BarStyle = in_BarStyle;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setPointDensity(int in_PointDensity) {
  PointDensity = in_PointDensity;
  setDirty();
  emit modifiedLegendEntry();
}


void KstVCurve::setColor(const QColor& new_c) {
  setDirty();
  Color = new_c;
  emit modifiedLegendEntry();
}


double KstVCurve::maxX() const {
  if (hasBars() && sampleCount() > 0) {
    return MaxX + (MaxX - MinX)/(2*(sampleCount()-1));
  }
  return MaxX;
}


double KstVCurve::minX() const {
  if (hasBars() && sampleCount() > 0) {
    return MinX - (MaxX - MinX)/(2*(sampleCount()-1));
  }
  return MinX;
}


#if 0
KstRelationPtr KstVCurve::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  Kst::VectorPtr VX = *_inputVectors.find(COLOR_XVECTOR);
  Kst::VectorPtr VY = *_inputVectors.find(COLOR_YVECTOR);
  Kst::VectorPtr EX = *_inputVectors.find(EXVECTOR);
  Kst::VectorPtr EY = *_inputVectors.find(EYVECTOR);
  Kst::VectorPtr EXMinus = *_inputVectors.find(EXMINUSVECTOR);
  Kst::VectorPtr EYMinus = *_inputVectors.find(EYMINUSVECTOR);

  QString name(tagName() + '\'');
  while (KstData::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  KstVCurvePtr vcurve = new KstVCurve(name, VX, VY, EX, EY, EXMinus, EYMinus, Color);
  // copy some other properties as well
  vcurve->setHasPoints(HasPoints);
  vcurve->setHasLines(HasLines);
  vcurve->setHasBars(HasBars);
  vcurve->setBarStyle(BarStyle);
  vcurve->setLineWidth(LineWidth);
  vcurve->setLineStyle(LineStyle);
  vcurve->setPointDensity(PointDensity);

  duplicatedMap.insert(this, KstRelationPtr(vcurve));
  return KstRelationPtr(vcurve);
}
#endif


void KstVCurve::paint(const KstCurveRenderContext& context) {
  Kst::VectorPtr xv = *_inputVectors.find(COLOR_XVECTOR);
  Kst::VectorPtr yv = *_inputVectors.find(COLOR_YVECTOR);
  if (!xv || !yv) {
    return;
  }

  /*Kst*/QPainter *p = context.painter;
  QColor foregroundColor = context.foregroundColor;
  double Lx = context.Lx, Hx = context.Hx, Ly = context.Ly, Hy = context.Hy;
  double m_X = context.m_X, m_Y = context.m_Y;
  double b_X = context.b_X, b_Y = context.b_Y;
  double XMin = context.XMin, XMax = context.XMax;
  bool xLog = context.xLog, yLog = context.yLog;
  double xLogBase = context.xLogBase;
  double yLogBase = context.yLogBase;
  int penWidth = context.penWidth;
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
#endif

  int pointDim = KstCurvePointSymbol::dim(context.window);
  if (sampleCount() > 0) {
    Qt::PenStyle style = KstLineStyle[lineStyle()];
    int i0, iN;
    int width;
    
    if (lineWidth() == 0) {
      width = penWidth;
    } else {
      width = lineWidth() * penWidth;
    }
    
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
      QPolygon points(MAX_NUM_POLYLINES);
      int lastPlottedX = 0;
      int lastPlottedY = 0;
      int index = 0;
      int i0Start = i0;

      p->setPen(QPen(color(), width, style));

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
          if (index > 0) {
            QPolygon poly;
            poly.putPoints(0, index, points);
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawPolyline" << poly << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
            p->drawPolyline(poly);
          }
          index = 0;
          if (overlap) {
            if (X2 >= Lx && X2 <= Hx) {
              if (maxY > Hy && minY <= Hy)
                maxY = Hy;
              if (minY < Ly && maxY >= Ly)
                minY = Ly;
              if (minY >= Ly && minY <= Hy && maxY >= Ly && maxY <= Hy) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawLine" << QLine(d2i(X2), d2i(minY), d2i(X2), d2i(maxY)) << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                p->drawLine(d2i(X2), d2i(minY), d2i(X2), d2i(maxY));
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
          int X1i = d2i(X1);
          int X2i = d2i(X2);
          if (KDE_ISLIKELY(X1i == X2i)) {
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
                  int Y2i = d2i(Y2);
                  int maxYi = d2i(maxY);
                  int minYi = d2i(minY);

                  if (index >= MAX_NUM_POLYLINES-2) {
                    QPolygon poly;
                    poly.putPoints(0, index, points);
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawPolyline" << poly << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                    p->drawPolyline(poly);
                    index = 0;
                  }
                  if (KDE_ISUNLIKELY(minYi == maxYi)) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                    points.setPoint(index++, X2i, maxYi);
                  } else if (KDE_ISUNLIKELY(Y2 == minY)) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                    points.setPoint(index++, X2i, maxYi);
                    points.setPoint(index++, X2i, minYi);
                  } else if (KDE_ISUNLIKELY(Y2 == maxY)) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                    points.setPoint(index++, X2i, minYi);
                    points.setPoint(index++, X2i, maxYi);
                  } else {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                    points.setPoint(index++, X2i, minYi);
                    points.setPoint(index++, X2i, maxYi);
                    if (KDE_ISLIKELY(Y2 >= Ly && Y2 <= Hy)) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                      points.setPoint(index++, X2i, Y2i);
                    }
                  }
                  lastPlottedX = X2i;
                  lastPlottedY = Y2i;
                } else {
                  if (KDE_ISUNLIKELY(maxY > Hy && minY <= Hy)) {
                    maxY = Hy;
                  }
                  if (KDE_ISUNLIKELY(minY < Ly && maxY >= Ly)) {
                    minY = Ly;
                  }
                  if (KDE_ISUNLIKELY(minY >= Ly && minY <= Hy && maxY >= Ly && maxY <= Hy)) {
                    if (index > 0) {
                      QPolygon poly;
                      poly.putPoints(0, index, points);
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawPolyline" << poly << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                      p->drawPolyline(poly);
                      index = 0;
                    }
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawLine" << QLine(X2i, d2i(minY), X2i, d2i(maxY)) << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                    p->drawLine(X2i, d2i(minY), X2i, d2i(maxY));
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
                int X1i = d2i(X1);
                int Y1i = d2i(Y1);
                int X2i = d2i(X2);
                int Y2i = d2i(Y2);

#ifdef DEBUG_VECTOR_CURVE
qDebug() << "MY POINTS ARE GOOD!!" << index << endl;
#endif

                if (KDE_ISUNLIKELY(index == 0)) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                  points.setPoint(index++, X2i, Y2i);
                  points.setPoint(index++, X1i, Y1i);
                } else if (lastPlottedX == X2i &&
                    lastPlottedY == Y2i &&
                    index < MAX_NUM_POLYLINES) {
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                  points.setPoint(index++, X1i, Y1i);
                } else {
                  if (KDE_ISLIKELY(index > 1)) {
                    QPolygon poly;
                    poly.putPoints(0, index, points);
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawPolyline" << poly << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
                    p->drawPolyline(poly);
                  }
                  index = 0;
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "index++" << index << endl;
#endif
                  points.setPoint(index++, X2i, Y2i);
                  points.setPoint(index++, X1i, Y1i);
                }
                lastPlottedX = X1i;
                lastPlottedY = Y1i;
              }
            }
          } // end if (X1 == X2)
        } // end if (!foundNan)
      } // end while

      // we might a have polyline left undrawn...
      if (index > 1) {
        QPolygon poly;
        poly.putPoints(0, index, points);
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawPolyline" << poly << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
        p->drawPolyline(poly);
        index = 0;
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
#ifdef DEBUG_VECTOR_CURVE
qDebug() << __LINE__ << "drawLine" << QLine(d2i(X2), d2i(minY), d2i(X2), d2i(maxY)) << endl;
#endif
#ifdef BENCHMARK
  ++numberOfLinesDrawn;
#endif
            p->drawLine(d2i(X2), d2i(minY), d2i(X2), d2i(maxY));
          }
        }
        overlap = false;
      }
    } // end if hasLines()
#ifdef BENCHMARK
    clock_t linesEnd = clock();
    qDebug() << "        Lines clocks: " << (linesEnd - linesStart) << endl;
    b_1 = benchtmp.elapsed();
#endif

    Kst::VectorPtr exv = _inputVectors.contains(EXVECTOR) ? *_inputVectors.find(EXVECTOR) : 0;
    Kst::VectorPtr eyv = _inputVectors.contains(EYVECTOR) ? *_inputVectors.find(EYVECTOR) : 0;
    Kst::VectorPtr exmv = _inputVectors.contains(EXMINUSVECTOR) ? *_inputVectors.find(EXMINUSVECTOR) : 0;
    Kst::VectorPtr eymv = _inputVectors.contains(EYMINUSVECTOR) ? *_inputVectors.find(EYMINUSVECTOR) : 0;
    // draw the bargraph bars, if any...
    if (hasBars()) {
      bool has_top = true;
      bool has_bot = true;
      bool has_left = true;
      bool has_right = true;
      bool visible = true;
      double rX2 = 0.0;
      double drX = 0.0;

      if (barStyle() == 1) { // filled
        p->setPen(QPen(foregroundColor, width, style));
      } else {
        p->setPen(QPen(color(), width, style));
      }

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
        visible = has_bot = has_top = has_left = has_right = true;

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
        }

        X1 = m_X * rX + b_X;
        X2 = m_X * rX2 + b_X;
        if (X1 > Hx || X2 < Lx) {
          visible = false;
        } else {
          if (X1 < Lx) {
            has_left = false;
            X1 = Lx;
          }
          if (X2 > Hx) {
            has_right = false;
            X2 = Hx;
          }
        }

        // determine where the top of the bar is and whether
        // to draw the top line
        Y1 = m_Y * rY + b_Y;
        if (Y1 < Ly) {
          Y1 = Ly;
          has_top = false;
        }
        if (Y1 > Hy) {
          Y1 = Hy;
          has_top = false;
        }

        // determine where the bottom of the bar is and whether
        // to draw the bottom line
        if (yLog) {
          Y2 = Hy;
          has_bot = false;
        } else {
          Y2 = b_Y;
          if (Y2 < Ly) {
            Y2 = Ly;
            has_bot = false;
          }
          if (Y2 > Hy) {
            Y2 = Hy;
            has_bot = false;
          }
        }

        if (Y1 == Ly && Y2 == Ly) {
          visible = false;
        }
        else if (Y1 == Hy && Y2 == Hy) {
          visible = false;
        }

        if (visible) {
          if (barStyle() == 1) { // filled
            int X1i = d2i(X1);
            int Y1i = d2i(Y1);
            p->fillRect(X1i, Y1i, d2i(X2) - X1i, d2i(Y2) - Y1i, color());
          }
          if (has_top) {
            int Y1i = d2i(Y1);
            p->drawLine(d2i(X1-(width/2)), Y1i, d2i(X2+(width/2)), Y1i);
          }
          if (has_bot) {
            int Y2i = d2i(Y2);
            p->drawLine(d2i(X1-(width/2)), Y2i, d2i(X2-(width/2)), Y2i);
          }
          if (has_left) {
            int X1i = d2i(X1);
            p->drawLine(X1i, d2i(Y1-(width/2)), X1i, d2i(Y2+(width/2)));
          }
          if (has_right) {
            int X2i = d2i(X2);
            p->drawLine(X2i, d2i(Y1-(width/2)), X2i, d2i(Y2+(width/2)));
          }
        }
      }
    }

#ifdef BENCHMARK
    b_2 = benchtmp.elapsed();
#endif

    p->setPen(QPen(color(), width));

    // draw the points, if any...
    if (hasPoints()) {
      if (hasLines() && pointDensity() != 0) {
        const double w = Hx - Lx;
        const double h = Hy - Ly;
        QRegion rgn((int)Lx, (int)Ly, (int)w, (int)h);
        const int size = int(qMax(w, h)) / int(pow(3.0, KSTPOINTDENSITY_MAXTYPE - pointDensity()));
        QPoint pt;
        for (i_pt = i0; i_pt <= iN; ++i_pt) {
          rX = xv->interpolate(i_pt, NS);
          rY = yv->interpolate(i_pt, NS);
          if (xLog) {
            rX = logXLo(rX, xLogBase);
          }
          if (yLog) {
            rY = logYLo(rY, yLogBase);
          }

          pt.setX(d2i(m_X * rX + b_X));
          pt.setY(d2i(m_Y * rY + b_Y));
          if (rgn.contains(pt)) {
            KstCurvePointSymbol::draw(pointType, p, pt.x(), pt.y(), width);
            rgn -= QRegion(pt.x()-(size/2), pt.y()-(size/2), size, size, QRegion::Ellipse);
          }
        }
      } else {
        for (i_pt = i0; i_pt <= iN; ++i_pt) {
          rX = xv->interpolate(i_pt, NS);
          rY = yv->interpolate(i_pt, NS);
          if (xLog) {
            rX = logXLo(rX, xLogBase);
          }
          if (yLog) {
            rY = logYLo(rY, yLogBase);
          }

          X1 = m_X * rX + b_X;
          Y1 = m_Y * rY + b_Y;
          if (X1 >= Lx && X1 <= Hx && Y1 >= Ly && Y1 <= Hy) {
            KstCurvePointSymbol::draw(pointType, p, d2i(X1), d2i(Y1), width);
          }
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

      if (exv && exmv && xETag() == xEMinusTag()) {
        errorSame = true;
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
          int X1i = d2i(X1);
          int X2i = d2i(X2);
          int Y1i = d2i(Y1);
          p->drawLine(X1i, Y1i, X2i, Y1i);
          if (do_low_flag) {
            p->drawLine(X1i, Y1i + pointDim, X1i, Y1i - pointDim);
          }
          if (do_high_flag) {
            p->drawLine(X2i, Y1i + pointDim, X2i, Y1i - pointDim);
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

      if (eyv && eymv && yETag() == yEMinusTag()) {
        errorSame = true;
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
          int X1i = d2i(X1);
          int Y1i = d2i(Y1);
          int Y2i = d2i(Y2);
          p->drawLine(X1i, Y1i, X1i, Y2i);
          if (do_low_flag) {
            p->drawLine(X1i + pointDim, Y1i, X1i - pointDim, Y1i);
          }
          if (do_high_flag) {
            p->drawLine(X1i + pointDim, Y2i, X1i - pointDim, Y2i);
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
  qDebug() << "Plotting curve " << (void *)this << ": " << i << "ms" << endl;
  qDebug() << "    Without locks: " << b_4 << "ms" << endl;
  qDebug() << "    Nnumber of lines drawn:" << numberOfLinesDrawn << endl;
  if (b_1 > 0)       qDebug() << "            Lines: " << b_1 << "ms" << endl;
  if (b_2 - b_1 > 0) qDebug() << "             Bars: " << (b_2 - b_1) << "ms" << endl;
  if (b_3 - b_2 > 0) qDebug() << "           Points: " << (b_3 - b_2) << "ms" << endl;
  if (b_4 - b_3 > 0) qDebug() << "           Errors: " << (b_4 - b_3) << "ms" << endl;
#endif
}


void KstVCurve::yRange(double xFrom, double xTo, double* yMin, double* yMax) {
  if (!yMin || !yMax) {
    return;
  }

  Kst::VectorPtr xv = *_inputVectors.find(COLOR_XVECTOR);
  Kst::VectorPtr yv = *_inputVectors.find(COLOR_YVECTOR);
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


Kst::DataObjectPtr KstVCurve::providerDataObject() const {
  KST::vectorList.lock().readLock();
  Kst::VectorPtr vp = *KST::vectorList.findTag(yVTag().tag());  // FIXME: should use full tag
  KST::vectorList.lock().unlock();
  Kst::DataObjectPtr provider = 0L;
  if (vp) {
    vp->readLock();
    provider = kst_cast<Kst::DataObject>(vp->provider());
    vp->unlock();
  }
  return provider;
}


double KstVCurve::distanceToPoint(double xpos, double dx, double ypos) const {
// find the y distance between the curve and a point. return 1.0E300 if this distance is undefined. i don't want to use -1 because it will make the code which uses this function messy.
  Kst::VectorPtr xv = *_inputVectors.find(COLOR_XVECTOR);
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


void KstVCurve::paintLegendSymbol(KstPainter *p, const QRect& bound) {
  int width;
  
  if (lineWidth() == 0) {
    width = p->lineWidthAdjustmentFactor();
  } else {  
    width = lineWidth() * p->lineWidthAdjustmentFactor();
  }
  
  p->save();
  if (hasLines()) {
    // draw a line from left to right centered vertically
    p->setPen(QPen(color(), width, KstLineStyle[lineStyle()]));
    p->drawLine(bound.left(), bound.top() + bound.height()/2,
                bound.right(), bound.top() + bound.height()/2);
  }
  if (hasPoints()) {
    // draw a point in the middle
    p->setPen(QPen(color(), width));
    KstCurvePointSymbol::draw(pointType, p, bound.left() + bound.width()/2, bound.top() + bound.height()/2, width, 600);
  }
  p->restore();
}


// vim: ts=2 sw=2 et
