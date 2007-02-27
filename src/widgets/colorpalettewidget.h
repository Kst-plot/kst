/***************************************************************************
                   colorpalettewidget.h
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

#ifndef COLORPALETTEWIDGET_H
#define COLORPALETTEWIDGET_H

#include <QWidget>
#include "ui_colorpalettewidget4.h"

class ColorPaletteWidget : public QWidget, public Ui::ColorPaletteWidget {
  Q_OBJECT

public:
  ColorPaletteWidget(QWidget *parent = 0);
  ~ColorPaletteWidget();

  QString selectedPalette();
  void refresh();
  void refresh(const QString &palette);
  int currentPaletteIndex();

public slots:
  void updatePalette(const QString &palette);

private:
  void init();
};

#endif
// vim: ts=2 sw=2 et
