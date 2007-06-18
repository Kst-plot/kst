/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTRENDERER2D_H
#define PLOTRENDERER2D_H

#include <QPainterPath>
#include "axis.h"
#include "kstbasecurve.h"

namespace Kst {

enum RenderType2D { Cartesian, Polar, Sinusoidal };

class PlotRenderer2D {
  public:
    PlotRenderer2D(const QString &name);
    virtual ~PlotRenderer2D();
    // Set data range in terms of projected coordinates
    virtual void setRangeXY(const QRectF& range);
    // Set data range in terms of native coordinates
    virtual void setRangeUV(const QRectF& range);
    // Get data range in terms of projected coordinates
    virtual void rangeXY(QRectF *range);
    // Get data range in terms of native coordinates
    virtual void rangeUV(QRectF *range);
    // Take a PainterPath in native coordinates and project
    virtual void projectPath(QPainterPath *path);
    // Project a single point
    virtual void projectPoint(const QPointF& pold, QPointF *pnew);
    // Inverse projection of a single point
    virtual void projectPointInv(const QPointF& pold, QPointF *pnew);
    
    // Set and get the renderer type
    void setType(RenderType2D type);
    RenderType2D type();
    
    // The list of things to render
    KstBaseCurveList sources;

  protected:
    // Recompute auxilliary range information if the
    // properties of the full plot have changed.
    void refreshRange(); 
    
    // Range of plot in projected coordinates
    QRectF _xyRange;
    // Range of plot in native coordinates
    QRectF _uvRange;
    
    // Axes 
    Axis _xAxis, _yAxis;
    // Axis styles
    AxisStyle _xAxisStyle, _yAxisStyle;
    
  private:
    // Name of the plot
    QString _name;
    
    // Type of the plot
    RenderType2D _type;
    
};

}

#endif

// vim: ts=2 sw=2 et
