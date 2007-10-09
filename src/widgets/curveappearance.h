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

#ifndef CURVEAPPEARANCE_H
#define CURVEAPPEARANCE_H

#include <QWidget>
#include "ui_curveappearance.h"

#include "kst_export.h"

namespace Kst {

class KST_EXPORT CurveAppearance : public QWidget, public Ui::CurveAppearance {
  Q_OBJECT
  public:
    CurveAppearance(QWidget *parent = 0);
    virtual ~CurveAppearance();

    bool showLines();
    bool showPoints();
    bool showBars();
    QColor color();
    int pointType();
    int lineStyle();
    int lineWidth();
    int barStyle();
    int pointDensity();

  private slots:
    void enableSettings();
    void drawSampleLine();
    void populatePointSymbolCombo();
    void populateLineStyleCombo();
};

}

#endif

// vim: ts=2 sw=2 et
