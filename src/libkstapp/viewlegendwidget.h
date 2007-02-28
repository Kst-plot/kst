/***************************************************************************
                   viewlegendwidget.h
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

#ifndef VIEWLEGENDWIDGET_H
#define VIEWLEGENDWIDGET_H

#include <QWidget>
#include "ui_viewlegendwidget4.h"

class ViewLegendWidget : public QWidget, public Ui::ViewLegendWidget {
  Q_OBJECT

public:
  ViewLegendWidget(QWidget *parent = 0);
  ~ViewLegendWidget();

public slots:
};

#endif
// vim: ts=2 sw=2 et
