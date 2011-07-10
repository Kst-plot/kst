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

#ifndef CURVEAPPEARANCE_H
#define CURVEAPPEARANCE_H

#include <QWidget>
#include "ui_curveappearance.h"

#include "kstwidgets_export.h"

namespace Kst {

class KSTWIDGETS_EXPORT CurveAppearance : public QWidget, public Ui::CurveAppearance {
  Q_OBJECT
  public:
    CurveAppearance(QWidget *parent = 0);
    virtual ~CurveAppearance();

    bool showLines() const;
    bool showLinesDirty() const;
    void setShowLines(const bool showLines);

    bool showPoints() const;
    bool showPointsDirty() const;
    void setShowPoints(const bool showPoints);

    bool showHead() const;
    bool showHeadDirty() const;
    void setShowHead(const bool showHead);

    bool showBars() const;
    bool showBarsDirty() const;
    void setShowBars(const bool showBars);

    QColor color() const;
    bool colorDirty() const;
    void setColor(const QColor &c);

    QColor headColor() const;
    bool headColorDirty() const;
    void setHeadColor(const QColor &c);

    int pointType() const;
    bool pointTypeDirty() const;
    void setPointType(const int pointType);

    int headType() const;
    bool headTypeDirty() const;
    void setHeadType(const int headType);

    int lineStyle() const;
    bool lineStyleDirty() const;
    void setLineStyle(int lineStyle);

    int lineWidth() const;
    bool lineWidthDirty() const;
    void setLineWidth(const int lineWidth);

    QColor barFillColor() const;
    bool barFillColorDirty() const;
    void setBarFillColor(const QColor &c);

    int pointDensity() const;
    bool pointDensityDirty() const;
    void setPointDensity(int pointDensity);

    void clearValues();

    void setWidgetDefaults(bool nextColor = true);
    void loadWidgetDefaults();

  private slots:
    void enableSettings();
    void drawSampleLine();
    void populateSymbolCombos();
    void populateSymbolCombo(QComboBox *combo, QColor color);
    void populateLineStyleCombo();

  Q_SIGNALS:
    void modified();
};

}

#endif

// vim: ts=2 sw=2 et
