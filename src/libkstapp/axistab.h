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

#ifndef AXISTAB_H
#define AXISTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "plotdefines.h"
#include "ui_axistab.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT AxisTab : public DialogTab, Ui::AxisTab {
  Q_OBJECT
  public:
    AxisTab(QWidget *parent = 0);
    virtual ~AxisTab();

    bool drawAxisMajorTicks() const;
    void setDrawAxisMajorTicks(const bool enabled);

    bool drawAxisMajorGridLines() const;
    void setDrawAxisMajorGridLines(const bool enabled);

    bool drawAxisMinorTicks() const;
    void setDrawAxisMinorTicks(const bool enabled);

    bool drawAxisMinorGridLines() const;
    void setDrawAxisMinorGridLines(const bool enabled);

    PlotAxis::MajorTickMode axisMajorTickSpacing() const;
    void setAxisMajorTickSpacing(PlotAxis::MajorTickMode spacing);

    Qt::PenStyle axisMajorGridLineStyle() const;
    void setAxisMajorGridLineStyle(Qt::PenStyle style);

    QColor axisMajorGridLineColor() const;
    void setAxisMajorGridLineColor(const QColor &color);

    Qt::PenStyle axisMinorGridLineStyle() const;
    void setAxisMinorGridLineStyle(Qt::PenStyle style);

    QColor axisMinorGridLineColor() const;
    void setAxisMinorGridLineColor(const QColor &color);

    int axisMinorTickCount() const;
    void setAxisMinorTickCount(const int count);

    int significantDigits() const;
    void setSignificantDigits(const int digits);

    bool isLog() const;
    void setLog(const bool enabled);

    bool isBaseOffset() const;
    void setBaseOffset(const bool enabled);

    bool isReversed() const;
    void setReversed(const bool enabled);

    bool isInterpret() const;
    void setInterpret(const bool enabled);

    KstAxisDisplay axisDisplay() const;
    void setAxisDisplay(KstAxisDisplay display);

    KstAxisInterpretation axisInterpretation() const;
    void setAxisInterpretation(KstAxisInterpretation interpretation);

  public Q_SLOTS:
    void update();
};

}

#endif

// vim: ts=2 sw=2 et
