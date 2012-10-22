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

#ifndef AXISTAB_H
#define AXISTAB_H

#include "dialogtab.h"
#include "plotitem.h"
#include "plotdefines.h"
#include "ui_axistab.h"

#include "kst_export.h"

namespace Kst {

class AxisTab : public DialogTab, Ui::AxisTab {
  Q_OBJECT
  public:
    AxisTab(QWidget *parent = 0);
    virtual ~AxisTab();

    bool drawAxisMajorTicks() const;
    bool drawAxisMajorTicksDirty() const;
    void setDrawAxisMajorTicks(const bool enabled);

    bool drawAxisMajorGridLines() const;
    bool drawAxisMajorGridLinesDirty() const;
    void setDrawAxisMajorGridLines(const bool enabled);

    bool drawAxisMinorTicks() const;
    bool drawAxisMinorTicksDirty() const;
    void setDrawAxisMinorTicks(const bool enabled);

    bool drawAxisMinorGridLines() const;
    bool drawAxisMinorGridLinesDirty() const;
    void setDrawAxisMinorGridLines(const bool enabled);

    MajorTickMode axisMajorTickSpacing() const;
    bool axisMajorTickSpacingDirty() const;
    void setAxisMajorTickSpacing(MajorTickMode spacing);

    Qt::PenStyle axisMajorGridLineStyle() const;
    bool axisMajorGridLineStyleDirty() const;
    void setAxisMajorGridLineStyle(Qt::PenStyle style);

    QColor axisMajorGridLineColor() const;
    bool axisMajorGridLineColorDirty() const;
    void setAxisMajorGridLineColor(const QColor &color);

    qreal axisMajorGridLineWidth() const;
    bool axisMajorGridLineWidthDirty() const;
    void setAxisMajorGridLineWidth(qreal width);

    Qt::PenStyle axisMinorGridLineStyle() const;
    bool axisMinorGridLineStyleDirty() const;
    void setAxisMinorGridLineStyle(Qt::PenStyle style);

    QColor axisMinorGridLineColor() const;
    bool axisMinorGridLineColorDirty() const;
    void setAxisMinorGridLineColor(const QColor &color);

    qreal axisMinorGridLineWidth() const;
    bool axisMinorGridLineWidthDirty() const;
    void setAxisMinorGridLineWidth(qreal width);

    bool isAutoMinorTickCount() const;
    bool isAutoMinorTickCountDirty() const;
    void setAutoMinorTickCount(const bool enabled);

    int axisMinorTickCount() const;
    bool axisMinorTickCountDirty() const;
    void setAxisMinorTickCount(const int count);

    int significantDigits() const;
    bool significantDigitsDirty() const;
    void setSignificantDigits(const int digits);

    bool isLog() const;
    bool isLogDirty() const;
    void setLog(const bool enabled);

    bool isAutoBaseOffset() const;
    bool isAutoBaseOffsetDirty() const;
    void setAutoBaseOffset(const bool enabled);

    bool isBaseOffset() const;
    bool isBaseOffsetDirty() const;
    void setBaseOffset(const bool enabled);

    bool isReversed() const;
    bool isReversedDirty() const;
    void setReversed(const bool enabled);

    bool isInterpret() const;
    bool isInterpretDirty() const;
    void setInterpret(const bool enabled);

    AxisDisplayType axisDisplay() const;
    bool axisDisplayDirty() const;
    void setAxisDisplay(AxisDisplayType display);

    QString axisDisplayFormatString() const;
    bool axisDisplayFormatStringDirty() const;
    void setAxisDisplayFormatString(const QString& formatString);

    QString timezone() const;
    bool timezoneDirty() const;
    void setTimezone(QString timezone);

    AxisInterpretationType axisInterpretation() const;
    bool axisInterpretationDirty() const;
    void setAxisInterpretation(AxisInterpretationType interpretation);

    int labelRotation() const;
    bool labelRotationDirty() const;
    void setLabelRotation(const int rotation);

    bool hideTopRight() const;
    bool hideTopRightDirty() const;
    void setHideTopRight( bool hide);

    bool hideBottomLeft() const;
    bool hideBottomLeftDirty() const;
    void setHideBottomLeft( bool hide);

    void enableSingleEditOptions(bool enabled);
    void clearTabValues();

    void setAsYAxis();

  public Q_SLOTS:
    void updateButtons();

};

}

#endif

// vim: ts=2 sw=2 et
