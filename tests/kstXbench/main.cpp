/***************************************************************************
                                  main.cpp
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

#include <qapplication.h>
#include <qtimer.h>
#include <qpixmap.h>

#include <stdio.h>
#include <stdlib.h>

#include "tw.h"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  TW tw( &a );

  tw.show();

//   tw.testPixMapLines(1000);
//   tw.testPixMapLines(100);
//   tw.testPixMapLines(10);

//   tw.testPixMapFill( 1000 );

//   tw.testDrawImage();
//   tw.testDrawPixmap();

  tw.testPlotBench();
  tw.testPlotBench(true);
  tw.testPlotBench();
  tw.testPlotBench(true);

  return( 0 );
  return a.exec();
}

