/***************************************************************************
                   view2dplotwidget.h
                             -------------------
    begin                : 02/28/07
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

#ifndef VIEW2DPLOTWIDGET_H
#define VIEW2DPLOTWIDGET_H

#include <QWidget>
#include "ui_view2dplotwidget4.h"

class View2DPlotWidget : public QWidget, public Ui::View2DPlotWidget {
  Q_OBJECT

public:
  View2DPlotWidget(QWidget *parent = 0);
  ~View2DPlotWidget();

public slots:
};

#endif
// vim: ts=2 sw=2 et
