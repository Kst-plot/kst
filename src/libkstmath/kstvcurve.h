/***************************************************************************
                          kstvcurve.h: defines a curve for kst
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

#ifndef KSTVCURVE_H
#define KSTVCURVE_H

#include "kstrelation.h"
#include "kstpainter.h"
#include "kstcurvepointsymbol.h"
#include "kst_export.h"

#include <QStack>

/**A class for handling curves for kst
 *@author C. Barth Netterfield
 */

class KST_EXPORT KstVCurve: public KstRelation {
  public:
    KstVCurve(const QString &in_tag, Kst::VectorPtr in_X, Kst::VectorPtr in_Y,
        Kst::VectorPtr in_EX, Kst::VectorPtr in_EY,
        Kst::VectorPtr in_EXMinus, Kst::VectorPtr in_EYMinus,
        const QColor &in_color);
    
    KstVCurve(QDomElement &e);
    virtual ~KstVCurve();

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

    Kst::ObjectTag xVTag() const;
    Kst::ObjectTag yVTag() const;
    Kst::ObjectTag xETag() const;
    Kst::ObjectTag yETag() const;
    Kst::ObjectTag xEMinusTag() const;
    Kst::ObjectTag yEMinusTag() const;

    void setXVector(Kst::VectorPtr new_vx);
    void setYVector(Kst::VectorPtr new_vy);
    void setXError(Kst::VectorPtr new_ex);
    void setYError(Kst::VectorPtr new_ey);
    void setXMinusError(Kst::VectorPtr new_ex);
    void setYMinusError(Kst::VectorPtr new_ey);

    /** Save curve information */
    void save(QTextStream &ts, const QString& indent = QString::null);

    QString xLabel() const;
    QString yLabel() const;
    QString topLabel() const;

    virtual KstCurveType curveType() const;

    virtual bool xIsRising() const;

    virtual double maxX() const;
    virtual double minX() const;
    virtual double meanX() const { return MeanX; } 
    virtual double meanY() const { return MeanY; }
    virtual void yRange(double xFrom, double xTo, double* yMin, double* yMax);

    virtual int samplesPerFrame() const;

    virtual void showNewDialog();
    virtual void showEditDialog();

    Kst::VectorPtr xVector() const;
    Kst::VectorPtr yVector() const;
    Kst::VectorPtr xErrorVector() const;
    Kst::VectorPtr yErrorVector() const;
    Kst::VectorPtr xMinusErrorVector() const;
    Kst::VectorPtr yMinusErrorVector() const;
    
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
    
    virtual int lineWidth()     const { return LineWidth; }
    virtual int lineStyle()     const { return LineStyle; }
    virtual int barStyle()      const { return BarStyle; }
    virtual int pointDensity()  const { return PointDensity; }

    virtual QColor color() const { return Color; }
    virtual void setColor(const QColor& new_c);
    
    
    void pushColor(const QColor& c) { _colorStack.push(color()); setColor(c); }
    void popColor() { setColor(_colorStack.pop()); }
    void pushLineWidth(int w) { _widthStack.push(lineWidth()); setLineWidth(w); }
    void popLineWidth() { setLineWidth(_widthStack.pop()); }
    void pushLineStyle(int s) { _lineStyleStack.push(lineStyle()); setLineStyle(s); }
    void popLineStyle() { setLineStyle(_lineStyleStack.pop()); }
    void pushPointStyle(int s) { _pointStyleStack.push(pointType); pointType = s; }
    void popPointStyle() { pointType = _pointStyleStack.pop(); }
    void pushHasPoints(bool h) { _hasPointsStack.push(hasPoints()); setHasPoints(h); }
    void popHasPoints() { setHasPoints(_hasPointsStack.pop()); }
    void pushHasLines(bool h) { _hasLinesStack.push(hasLines()); setHasLines(h); }
    void popHasLines() { setHasLines(_hasLinesStack.pop()); }
    void pushPointDensity(int d) { _pointDensityStack.push(pointDensity()); setPointDensity(d); }
    void popPointDensity() { setPointDensity(_pointDensityStack.pop()); } 
    
    int pointType;
    
#if 0
    virtual KstRelationPtr makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap);
#endif
    
    // render this vcurve
    virtual void paint(const KstCurveRenderContext& context);
    
    // render the legend symbol for this curve
    virtual void paintLegendSymbol(KstPainter *p, const QRect& bound);
    
    // see KstRelation::distanceToPoint
    virtual double distanceToPoint(double xpos, double dx, double ypos) const;
    
    // see KstRelation::providerDataObject
    virtual Kst::DataObjectPtr providerDataObject() const;

  private:
    inline void commonConstructor(const QString& in_tag, const QColor& in_color);

    double MeanY;
    
    int BarStyle;
    int LineWidth;
    int LineStyle;
    int PointDensity;
    
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

typedef Kst::SharedPtr<KstVCurve> KstVCurvePtr;
typedef Kst::ObjectList<KstVCurvePtr> KstVCurveList;

#endif
// vim: ts=2 sw=2 et
