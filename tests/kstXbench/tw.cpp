/***************************************************************************
                                  tw.cpp
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
#include <qcolor.h>
#include <qimage.h>
#include <qpaintdevice.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>

#include <stdio.h>

#include "tw.h"

#define WIDTH 1280
#define HEIGHT 1024

#define NITR 1000

TW::TW(QApplication *a, QWidget *parent, const char *name):
  QWidget(parent, name) {
  setFixedSize(WIDTH, HEIGHT);
  _a = a;
}

void TW::testPixMapLines(int length) {
  int i, j, et;
  QTime t;
  QPixmap pm(WIDTH, HEIGHT);
  QPainter p( &pm );

  t.start();
  j = 0;
  do {
    for ( i=0; i<1000; i++,j++ ) {
      p.drawLine(1, 500, length+1,  500);
    }
    _a->processEvents();
  } while ( ( et=t.elapsed() )<1000 );
  printf( "Lines into Pixmaps: length %6d  Rate: %g/s\n",
          length, 1000.0*double( j )/double( et ) );
  _a->processEvents();
}

void TW::testPixMapFill(int length) {
  int i, j, et;
  QTime t;
  QPixmap pm(WIDTH, HEIGHT);
  QPainter p( &pm );

  t.start();
  j = 0;
  do {
    for ( i=0; i<100; i++,j++ ) {
      p.fillRect( 1, 1, length, length, Qt::blue );
    }
    _a->processEvents();
  } while ( ( et=t.elapsed() )<1000 );
  printf( "fillRect into Pixmaps: size %6d  Rate: %g/s\n",
          length, 1000.0*double( j )/double( et ) );
  _a->processEvents();
}

void TW::testDrawPixmap() {
  QPixmap pm(WIDTH, HEIGHT);
  QTime t;
  int i, j, et;
  QPainter p( this );

  pm.fill(Qt::red);
  t.start();

  j = 0;
  do {
    for ( i=0; i<5; i++,j++ ) {
      p.drawPixmap(0,0,pm);
      _a->processEvents();
    }
  } while ( ( et=t.elapsed() )<1000 );

  printf( "Draw pixmap into widget (%d x %d). Rate: %g/s\n",
          WIDTH,  HEIGHT, 1000.0*double( j )/double( et ) );
}

void TW::testPlotBench(bool ppressure) {
  QPixmap full(WIDTH, HEIGHT);
  QPixmap *px[4];
  QTime t;
  int i,j, l, w, h, et;
  int i_plot;

  if (ppressure) {
    QPixmap d1(WIDTH, HEIGHT);
    QPixmap d2(WIDTH, HEIGHT);
    QPixmap d3(WIDTH, HEIGHT);
    QPixmap d4(WIDTH, HEIGHT);
    QPixmap d5(WIDTH, HEIGHT);
    QPixmap d6(WIDTH, HEIGHT);
    QPixmap d7(WIDTH, HEIGHT);
    QPixmap d8(WIDTH, HEIGHT);
  }

  w = WIDTH/2;
  h = HEIGHT/2;

  for (i=0; i<4; i++) {
    px[i] = new QPixmap(w,h);
  }

  t.start();

  j = 0;
  do {
    for (i=0; i<5; i++) {
      for (i_plot=0; i_plot<4; i_plot++) {
	QPainter p(px[i_plot]);
	p.fillRect( 1, 1, w-2, h-2, Qt::white );
	p.setPen(QPen(Qt::black, 2));
	for (l = 0; l<w; l++) {
	  p.drawLine((l+j*5)%(w-5), 1, (l+j*5)%(w-5), l%h);
	  //printf("%d\n", (l+j*5)%(w-5));
	}
	p.setPen(QPen(Qt::blue, 2));
	for (l = 0; l<w; l++) {
	  p.drawLine((l+j*5)%(w-5), 1, (l+j*5)%(w-5), l%h/4);
	  //printf("%d\n", (l+j*5)%(w-5));
	}
	for (l=0; l<15; l++) {
	  p.drawText(10, l*20+h/4, QString("Label"));
	}
      }
      {
	QPainter p(&full);

	p.drawPixmap(0,0,*px[0]);
	p.drawPixmap(w,0,*px[1]);
	p.drawPixmap(0,h,*px[2]);
	p.drawPixmap(w,h,*px[3]);
	p.flush();
      }
      QPainter q(this);
      q.drawPixmap(0,0,full);
      j++;
      _a->processEvents();
    }
  } while ( ( et=t.elapsed() )<1000 );

  for (i=0; i<4; i++) {
    delete px[i];
  }

  if (ppressure) printf("pressured ");
  printf( "simulated plots (%d x %d). Rate: %g/s\n",
          WIDTH,  HEIGHT, 1000.0*double( j )/double( et ) );
}

void TW::testDrawImage() {
  QImage im(WIDTH, HEIGHT, 32);
  QTime t;
  int i, j, et;
  QPainter p( this );

  im.fill(0x0000ff11);

  t.start();

  j = 0;
  do {
    for ( i=0; i<5; i++,j++ ) {
      p.drawImage(0,0,im);
      _a->processEvents();
    }
  } while ( ( et=t.elapsed() )<1000 );

  printf( "Draw image into widget (%d x %d). Rate: %g/s\n",
          WIDTH,  HEIGHT, 1000.0*double( j )/double( et ) );
}

//#include "tw.moc"
