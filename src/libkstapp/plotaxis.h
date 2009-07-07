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

#ifndef PLOTAXIS_H
#define PLOTAXIS_H

#include <QObject>

#include "plotitem.h"
#include "plotmarkers.h"

namespace Kst {

class PlotAxis : public QObject
{
  Q_OBJECT
  public:

    enum MajorTickMode {
      None = 0,
      Coarse = 2,
      Normal = 5,
      Fine = 10,
      VeryFine = 15
    };

    enum ZoomMode { Auto, AutoBorder, FixedExpression, SpikeInsensitive, MeanCentered };

    PlotAxis(PlotItem *plotItem, Qt::Orientation orientation);
    ~PlotAxis();

    PlotItem* plotItem() { return _plotItem; }

    bool isAxisVisible() const;
    void setAxisVisible(bool visible);

    MajorTickMode axisMajorTickMode() const;
    void setAxisMajorTickMode(MajorTickMode mode);

    int axisMinorTickCount() const;
    void setAxisMinorTickCount(const int count);

    bool axisAutoMinorTicks() const;
    void setAxisAutoMinorTicks(const bool enabled);

    bool drawAxisMajorTicks() const;
    void setDrawAxisMajorTicks(const bool draw);

    bool drawAxisMinorTicks() const;
    void setDrawAxisMinorTicks(const bool draw);

    bool drawAxisMajorGridLines() const;
    void setDrawAxisMajorGridLines(const bool draw);

    bool drawAxisMinorGridLines() const;
    void setDrawAxisMinorGridLines(const bool draw);

    QColor axisMajorGridLineColor() const;
    void setAxisMajorGridLineColor(const QColor &color);

    QColor axisMinorGridLineColor() const;
    void setAxisMinorGridLineColor(const QColor &color);

    Qt::PenStyle axisMajorGridLineStyle() const;
    void setAxisMajorGridLineStyle(const Qt::PenStyle style);

    Qt::PenStyle axisMinorGridLineStyle() const;
    void setAxisMinorGridLineStyle(const Qt::PenStyle style);

    int axisSignificantDigits() const;
    void setAxisSignificantDigits(const int digits);

    ZoomMode axisZoomMode() const;
    void setAxisZoomMode(ZoomMode mode);

    bool axisLog() const;
    void setAxisLog(bool log);

    bool axisReversed() const;
    void setAxisReversed(const bool enabled);

    bool axisAutoBaseOffset() const;
    void setAxisAutoBaseOffset(const bool enabled);

    bool axisBaseOffset() const;
    void setAxisBaseOffset(const bool enabled);

    bool axisInterpret() const;
    void setAxisInterpret(const bool enabled);

    AxisDisplayType axisDisplay() const;
    void setAxisDisplay(const AxisDisplayType display);

    AxisInterpretationType axisInterpretation() const;
    void setAxisInterpretation(const AxisInterpretationType interpret);

    int axisLabelRotation() const;
    void setAxisLabelRotation(const int rotation);

    PlotMarkers axisPlotMarkers() { return _axisPlotMarkers; }
    void setAxisPlotMarkers(const PlotMarkers &plotMarkers) { _axisPlotMarkers = plotMarkers; _ticksUpdated = true; }

    QMap<qreal, QString> axisLabels() { return _axisLabels; }
    QList<qreal> axisMajorTicks() { return _axisMajorTicks; }
    QList<qreal> axisMinorTicks() { return _axisMinorTicks; }
    QString baseLabel() { return _baseLabel; }

    void saveInPlot(QXmlStreamWriter &xml, QString axisId);
    bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

    void validateDrawingRegion(QPainter *painter);

    // return the value and reset.
    bool ticksUpdated() { bool bReturn = _ticksUpdated; _ticksUpdated = false; return bReturn; }
    bool isDirty() { return _dirty; }

  public Q_SLOTS:
    void updateTicks(bool useOverrideTicks = false);
    void setTicksUpdated() { _ticksUpdated = true; }

  private:

    QString interpretLabel(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double base, double lastValue);
    double convertTimeValueToJD(AxisInterpretationType axisInterpretation, double valueIn);
    QString convertJDToDateString(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double dJD);
    double convertTimeDiffValueToDays(AxisInterpretationType axisInterpretation, double offsetIn);
    double interpretOffset(AxisInterpretationType axisInterpretation, AxisDisplayType axisDisplay, double base, double value);

    qreal computedMajorTickSpacing(MajorTickMode majorTickCount, Qt::Orientation orientation, bool enforceMin = true);
    void computeLogTicks(QList<qreal> *MajorTicks, QList<qreal> *MinorTicks, QMap<qreal, QString> *Labels, qreal min, qreal max, MajorTickMode tickMode);

    MajorTickMode convertToMajorTickMode(int tickCount);

  private:

    PlotItem *_plotItem;
    Qt::Orientation _orientation;

    bool _dirty;

    ZoomMode _axisZoomMode;
    bool _isAxisVisible;

    bool _ticksUpdated;

    QRectF _labelRect;
    QMap<qreal, QString> _axisLabels;
    QList<qreal> _axisMajorTicks;
    QList<qreal> _axisMinorTicks;

    QString _baseLabel;

    bool _axisLog;
    bool _axisReversed;
    bool _axisAutoBaseOffset;
    bool _axisBaseOffset;
    bool _axisBaseOffsetOverride;

    bool _axisInterpret;
    AxisDisplayType _axisDisplay;
    AxisInterpretationType _axisInterpretation;

    MajorTickMode _axisMajorTickMode;
    MajorTickMode _axisOverrideMajorTicks;

    int _axisMinorTickCount;
    bool _automaticMinorTicks;
    int _automaticMinorTickCount;

    int _axisSignificantDigits;

    bool _drawAxisMajorTicks;
    bool _drawAxisMinorTicks;
    bool _drawAxisMajorGridLines;
    bool _drawAxisMinorGridLines;

    QColor _axisMajorGridLineColor;
    QColor _axisMinorGridLineColor;

    Qt::PenStyle _axisMajorGridLineStyle;
    Qt::PenStyle _axisMinorGridLineStyle;

    PlotMarkers _axisPlotMarkers;

    int _labelRotation;
};


}

#endif

// vim: ts=2 sw=2 et
