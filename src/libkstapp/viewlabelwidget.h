/***************************************************************************
                   viewlabelwidget.h
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

#ifndef VIEWLABELWIDGET_H
#define VIEWLABELWIDGET_H

#include <QWidget>
#include "ui_viewlabelwidget4.h"

class ViewLabelWidget : public QWidget, public Ui::ViewLabelWidget {
  Q_OBJECT

public:
  ViewLabelWidget(QWidget *parent = 0);
  ~ViewLabelWidget();

  void init();

public slots:
  void insertScalarInText(const QString &S);
  void insertStringInText(const QString &S);
};

#endif
// vim: ts=2 sw=2 et
