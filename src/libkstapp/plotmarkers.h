/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PLOTMARKERS_H
#define _PLOTMARKERS_H

#include "kst_export.h"

#include "vector.h"
#include "curve.h"

#include <QColor>
#include <QList>

namespace Kst {

class PlotMarkers {

  public:
    enum CurveMarkerMode {
      RisingEdge = 0,
      FallingEdge = 1,
      BothEdges = 2
    };

    PlotMarkers(bool xAxis = true);
    virtual ~PlotMarkers();

    QList<double> markers();

    QList<double> manualMarkers() const { return _manualMarkers; }
    void setManualMarkers(const QList<double> &markers) { _manualMarkers = markers; }

    Qt::PenStyle lineStyle() const { return _lineStyle; }
    void setLineStyle(Qt::PenStyle style) { _lineStyle = style; }

    QColor lineColor() const { return _lineColor; }
    void setLineColor(const QColor &color) { _lineColor = color; }

    double lineWidth() const { return _lineWidth; }
    void setLineWidth(const double width) { _lineWidth = width; }

    bool isVectorSource() const { return (_vector != 0); }
    VectorPtr vector() const { return _vector; }
    void setVector(VectorPtr vector) { _vector = vector; }

    bool isCurveSource() const { return (_curve != 0); }
    CurvePtr curve() const { return _curve; }
    void setCurve(CurvePtr curve) { _curve = curve; }

    CurveMarkerMode curveMarkerMode() const { return _curveMode; }
    void setCurveMarkerMode(const CurveMarkerMode mode) { _curveMode = mode; }

    void saveInPlot(QXmlStreamWriter &xml);
    bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

  private:
    bool _xAxis;
    QColor _lineColor;
    Qt::PenStyle _lineStyle;
    double _lineWidth;
    CurveMarkerMode _curveMode;
    VectorPtr _vector;
    CurvePtr _curve;
    QList<double> _manualMarkers;
};

}
#endif

// vim: ts=2 sw=2 et
