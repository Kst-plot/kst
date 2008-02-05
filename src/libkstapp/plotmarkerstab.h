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

#ifndef PLOTMARKERSTAB_H
#define PLOTMARKERSTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "ui_plotmarkerstab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT PlotMarkersTab : public DialogTab, Ui::PlotMarkersTab {
  Q_OBJECT
  public:
    PlotMarkersTab(QWidget *parent = 0);
    virtual ~PlotMarkersTab();

    bool drawXAxisMajorTicks() const;
    void setDrawXAxisMajorTicks(const bool enabled);

    bool drawXAxisMajorGridLines() const;
    void setDrawXAxisMajorGridLines(const bool enabled);

    bool drawXAxisMinorTicks() const;
    void setDrawXAxisMinorTicks(const bool enabled);

    bool drawXAxisMinorGridLines() const;
    void setDrawXAxisMinorGridLines(const bool enabled);

    PlotItem::MajorTickMode xAxisMajorTickSpacing() const;
    void setXAxisMajorTickSpacing(PlotItem::MajorTickMode spacing);

    bool drawYAxisMajorTicks() const;
    void setDrawYAxisMajorTicks(const bool enabled);

    bool drawYAxisMajorGridLines() const;
    void setDrawYAxisMajorGridLines(const bool enabled);

    bool drawYAxisMinorTicks() const;
    void setDrawYAxisMinorTicks(const bool enabled);

    bool drawYAxisMinorGridLines() const;
    void setDrawYAxisMinorGridLines(const bool enabled);

    PlotItem::MajorTickMode yAxisMajorTickSpacing() const;
    void setYAxisMajorTickSpacing(PlotItem::MajorTickMode spacing);

    Qt::PenStyle xAxisMajorGridLineStyle() const;
    void setXAxisMajorGridLineStyle(Qt::PenStyle style);

    QColor xAxisMajorGridLineColor() const;
    void setXAxisMajorGridLineColor(const QColor &color);

    Qt::PenStyle xAxisMinorGridLineStyle() const;
    void setXAxisMinorGridLineStyle(Qt::PenStyle style);

    QColor xAxisMinorGridLineColor() const;
    void setXAxisMinorGridLineColor(const QColor &color);

    Qt::PenStyle yAxisMajorGridLineStyle() const;
    void setYAxisMajorGridLineStyle(Qt::PenStyle style);

    QColor yAxisMajorGridLineColor() const;
    void setYAxisMajorGridLineColor(const QColor &color);

    Qt::PenStyle yAxisMinorGridLineStyle() const;
    void setYAxisMinorGridLineStyle(Qt::PenStyle style);

    QColor yAxisMinorGridLineColor() const;
    void setYAxisMinorGridLineColor(const QColor &color);

    int xAxisMinorTickCount() const;
    void setXAxisMinorTickCount(const int count);

    int yAxisMinorTickCount() const;
    void setYAxisMinorTickCount(const int count);

  public Q_SLOTS:
    void update();
};

}

#endif

// vim: ts=2 sw=2 et
