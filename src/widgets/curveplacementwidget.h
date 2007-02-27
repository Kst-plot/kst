/***************************************************************************
                   curveplacementwidget.h
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

#ifndef CURVEPLACEMENTWIDGET_H
#define CURVEPLACEMENTWIDGET_H

#include <QWidget>
#include "ui_curveplacementwidget4.h"

class CurvePlacementWidget : public QWidget, public Ui::CurvePlacementWidget {
  Q_OBJECT

public:
  CurvePlacementWidget(QWidget *parent = 0);
  ~CurvePlacementWidget();

  bool existingPlot();
  bool newPlot();
  QString plotName();
  int columns();
  bool reGrid();

public slots:
  void setExistingPlot(bool existingPlot);
  void setNewPlot(bool newPlot);
  void setCols(int c);
  void setCurrentPlot(const QString &p);
  void newWindow();
  void update();
  void updatePlotList();
  void updateEnabled();
  void updateGrid();
};

#endif
// vim: ts=2 sw=2 et
