/***************************************************************************
                   curveappearancewidget.h
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
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

#ifndef CURVEAPPEARANCEWIDGET_H
#define CURVEAPPEARANCEWIDGET_H

#include <QWidget>
#include "ui_curveappearancewidget4.h"

#include "kst_export.h"

class CurveAppearanceWidget : public QWidget, public Ui::CurveAppearanceWidget {
  Q_OBJECT

public:
  CurveAppearanceWidget(QWidget *parent = 0);
  ~CurveAppearanceWidget();

  bool showLines();
  bool showPoints();
  bool showBars();
  QColor color();
  int pointType();
  int lineStyle();
  int lineWidth();
  int barStyle();
  int pointDensity();

public slots:
  void init();
  void setColor(QColor c);
  void drawLine();
  void reset(QColor newColor);
  void reset();
  void setUsePoints(bool usePoints);
  void setMustUseLines(bool bMustUseLines);
  void redrawCombo();
  void setValue(bool hasLines, bool hasPoints, bool hasBars, const QColor &c, int pointType,
                int lineWidth, int lineStyle, int barStyle, int pointDensity);
  void enableSettings();

private slots:
  void fillCombo();
  void comboChanged();
  void fillLineStyleCombo();

protected:
  void resizeEvent(QResizeEvent *pEvent);
} KST_EXPORT;

#endif
// vim: ts=2 sw=2 et
