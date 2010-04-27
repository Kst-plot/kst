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

#ifndef MARKERSTAB_H
#define MARKERSTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_markerstab.h"

#include "kst_export.h"

namespace Kst {

class MarkersTab : public DialogTab, Ui::MarkersTab {
  Q_OBJECT
  public:
    MarkersTab(QWidget *parent = 0);
    virtual ~MarkersTab();

    void setObjectStore(ObjectStore *store);

    QList<double> markers() const;
    void setMarkers(const QList<double> &markers);

    Qt::PenStyle lineStyle() const;
    void setLineStyle(Qt::PenStyle style);

    QColor lineColor() const;
    void setLineColor(const QColor &color);

    double lineWidth() const;
    void setLineWidth(const double width);

    bool isCurveSource() const;
    void setCurveSource(const bool enabled);

    bool isVectorSource() const;
    void setVectorSource(const bool enabled);

    VectorPtr vector() const;
    void setVector(VectorPtr vector);
    void setVector(const QString &vector);

    CurvePtr curve() const;
    void setCurve(CurvePtr curve);

    PlotMarkers::CurveMarkerMode curveMarkerMode() const;
    void setCurveMarkerMode(const PlotMarkers::CurveMarkerMode mode);

    PlotMarkers plotMarkers() const;
    void setPlotMarkers(const PlotMarkers &plotMarkers);

    bool markersDirty() const { return _dirty; }
    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

  public Q_SLOTS:
    void add();
    void remove();
    void clear();
    void update();
    void setDirty() { _dirty = true; }

  private:
    PlotMarkers _plotMarkers;
    bool _dirty;
};

}

#endif

// vim: ts=2 sw=2 et
