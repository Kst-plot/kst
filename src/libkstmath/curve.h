/***************************************************************************
                          vcurve.h: defines a curve for kst
                             -------------------
    begin                : Fri Oct 22 2000
    copyright            : (C) 2000 by C. Barth Netterfield
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

#ifndef CURVE_H
#define CURVE_H

#include "relation.h"
#include "painter.h"
#include "curvepointsymbol.h"
#include "kst_export.h"

#include <QStack>

/**A class for handling curves for kst
 *@author C. Barth Netterfield
 */

namespace Kst {

class KST_EXPORT Curve: public Relation {
  Q_OBJECT

  public:
    static const QString staticTypeString;
    const QString& typeString() const { return staticTypeString; }
    static const QString staticTypeTag;

    virtual UpdateType update(int update_counter = -1);
    virtual QString propertyString() const;

    virtual int getIndexNearXY(double x, double dx, double y) const;

    virtual bool hasXError() const;
    virtual bool hasYError() const;
    virtual bool hasXMinusError() const;
    virtual bool hasYMinusError() const;

    // Note: these are -expensive-.  Don't use them on inner loops!
    virtual void point(int i, double &x1, double &y1) const;
    virtual void getEXPoint(int i, double &x1, double &y1, double &ex1);
    virtual void getEYPoint(int i, double &x1, double &y1, double &ey1);
    virtual void getEXMinusPoint(int i, double &x1, double &y1, double &ex1);
    virtual void getEYMinusPoint(int i, double &x1, double &y1, double &ey1);
    virtual void getEXPoints(int i, double &x, double &y, double &ex, double &exminus);
    virtual void getEYPoints(int i, double &x, double &y, double &ey, double &eyminus);

    ObjectTag xVTag() const;
    ObjectTag yVTag() const;
    ObjectTag xETag() const;
    ObjectTag yETag() const;
    ObjectTag xEMinusTag() const;
    ObjectTag yEMinusTag() const;

    void setXVector(VectorPtr new_vx);
    void setYVector(VectorPtr new_vy);
    void setXError(VectorPtr new_ex);
    void setYError(VectorPtr new_ey);
    void setXMinusError(VectorPtr new_ex);
    void setYMinusError(VectorPtr new_ey);

    /** Save curve information */
    void save(QXmlStreamWriter &s);

    QString xLabel() const;
    QString yLabel() const;
    QString topLabel() const;

    virtual CurveType curveType() const;

    virtual bool xIsRising() const;

    virtual double maxX() const;
    virtual double minX() const;
    virtual double meanX() const { return MeanX; }
    virtual double meanY() const { return MeanY; }
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax);

    virtual int samplesPerFrame() const;

    virtual void showNewDialog();
    virtual void showEditDialog();

    VectorPtr xVector() const;
    VectorPtr yVector() const;
    VectorPtr xErrorVector() const;
    VectorPtr yErrorVector() const;
    VectorPtr xMinusErrorVector() const;
    VectorPtr yMinusErrorVector() const;

    virtual bool hasPoints()    const { return HasPoints; }
    virtual bool hasLines()     const { return HasLines; }
    virtual bool hasBars()      const { return HasBars; }
    virtual void setHasPoints(bool in_HasPoints);
    virtual void setHasLines(bool in_HasLines);
    virtual void setHasBars(bool in_HasBars);
    virtual void setLineWidth(int in_LineWidth);
    virtual void setLineStyle(int in_LineStyle);
    virtual void setBarStyle( int in_BarStyle);
    virtual void setPointDensity(int in_PointDensity);
    virtual void setPointType(int in_PointType);

    virtual int lineWidth()     const { return LineWidth; }
    virtual int lineStyle()     const { return LineStyle; }
    virtual int barStyle()      const { return BarStyle; }
    virtual int pointDensity()  const { return PointDensity; }
    virtual int pointType()  const { return PointType; }

    virtual QColor color() const { return Color; }
    virtual void setColor(const QColor& new_c);


    void pushColor(const QColor& c) { _colorStack.push(color()); setColor(c); }
    void popColor() { setColor(_colorStack.pop()); }
    void pushLineWidth(int w) { _widthStack.push(lineWidth()); setLineWidth(w); }
    void popLineWidth() { setLineWidth(_widthStack.pop()); }
    void pushLineStyle(int s) { _lineStyleStack.push(lineStyle()); setLineStyle(s); }
    void popLineStyle() { setLineStyle(_lineStyleStack.pop()); }
    void pushPointStyle(int s) { _pointStyleStack.push(PointType); PointType = s; }
    void popPointStyle() { PointType = _pointStyleStack.pop(); }
    void pushHasPoints(bool h) { _hasPointsStack.push(hasPoints()); setHasPoints(h); }
    void popHasPoints() { setHasPoints(_hasPointsStack.pop()); }
    void pushHasLines(bool h) { _hasLinesStack.push(hasLines()); setHasLines(h); }
    void popHasLines() { setHasLines(_hasLinesStack.pop()); }
    void pushPointDensity(int d) { _pointDensityStack.push(pointDensity()); setPointDensity(d); }
    void popPointDensity() { setPointDensity(_pointDensityStack.pop()); }

    virtual RelationPtr makeDuplicate(QMap<RelationPtr, RelationPtr> &duplicatedRelations);

    // render this vcurve
    virtual void paint(const CurveRenderContext& context);

    // render the legend symbol for this curve
    virtual void paintLegendSymbol(Painter *p, const QRect& bound);

    // see KstRelation::distanceToPoint
    virtual double distanceToPoint(double xpos, double dx, double ypos) const;

    // see KstRelation::providerDataObject
    virtual DataObjectPtr providerDataObject() const;

  protected:
    Curve(ObjectStore *store, const ObjectTag &in_tag,
        VectorPtr in_X=0L, VectorPtr in_Y=0L,
        VectorPtr in_EX=0L, VectorPtr in_EY=0L,
        VectorPtr in_EXMinus=0L, VectorPtr in_EYMinus=0L,
        const QColor &in_color=QColor());

    Curve(ObjectStore *store, QDomElement &e);

    virtual ~Curve();

    friend class ObjectStore;

  private:
    inline void commonConstructor(const QColor& in_color);

    double MeanY;

    int BarStyle;
    int LineWidth;
    int LineStyle;
    int PointDensity;
    int PointType;

    bool HasPoints;
    bool HasLines;
    bool HasBars;

    QColor Color;
    QStack<int> _widthStack;
    QStack<QColor> _colorStack;
    QStack<int> _pointStyleStack;
    QStack<int> _lineStyleStack;
    QStack<bool> _hasPointsStack;
    QStack<bool> _hasLinesStack;
    QStack<int> _pointDensityStack;

};

typedef SharedPtr<Curve> CurvePtr;
typedef ObjectList<Curve> CurveList;

}
#endif
// vim: ts=2 sw=2 et
