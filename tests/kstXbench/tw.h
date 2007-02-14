/***************************************************************************
                                  tw.h
                             -------------------
    begin                : November, 2004
    copyright            : (C) 2004 by Barth Netterfield
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
#ifndef TW_H
#define TW_H
#include <qtimer.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qdatetime.h>
#include <qapplication.h>

class TW : public QWidget {
  Q_OBJECT
 public:
  TW(QApplication *a, QWidget *parent = 0, const char *name=0);
  void testPixMapLines(int length);
  void testPixMapFill(int size);
  void testDrawPixmap();
  void testDrawImage();
  void testPlotBench(bool ppressure = false);
  QApplication *_a;
};

#endif
