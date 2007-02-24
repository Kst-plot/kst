/***************************************************************************
                           kstcurvepointsymbol.cpp
                             -------------------
    begin                : Sunday June 17, 2001
    copyright            : (C) 2001 by cbn
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/


#include "kstcurvepointsymbol.h"
#include <kglobal.h>
#include <QPolygon>

namespace KstCurvePointSymbol {

void draw(int Type, QPainter *p, int x, int y, int lineSize, int size) {
  Q_UNUSED(size)
  
  int s;

  if (Type < 0 || Type > KSTPOINT_MAXTYPE) {
    Type = 0;
  }

  if (lineSize == 0 || lineSize == 1) {
    s = 3;
  } else {
    s = ( 3 * lineSize ) / 2;
  }
  
  switch (Type) {
    case 0:
      p->drawLine(x-s, y-s, x+s, y+s);
      p->drawLine(x-s, y+s, x+s, y-s);
      break;
    case 1:
      p->setBrush(Qt::NoBrush);
      p->drawRect(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 2:
      p->setBrush(Qt::NoBrush);
      p->drawEllipse(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 3:
      p->setBrush(Qt::SolidPattern);
      p->drawEllipse(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 4:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y-s, x, y+s, x+s, y-s );
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 5:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y+s, x, y-s, x+s, y+s );
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 6:
      p->setBrush(Qt::SolidPattern);
      p->drawRect(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 7:
      p->drawLine(x-s, y, x+s, y);
      p->drawLine(x, y-s, x, y+s);
      break;
    case 8:
      p->drawLine(x-s, y-s, x+s, y+s);
      p->drawLine(x-s, y+s, x+s, y-s);
      p->drawLine(x-s, y, x+s, y);
      p->drawLine(x, y-s, x, y+s);
      break;    
    case 9:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y-s, x, y+s, x+s, y-s );
        p->setBrush(Qt::SolidPattern);
        p->drawPolygon(pts);
      }
      break;
    case 10:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y+s, x, y-s, x+s, y+s );
        p->setBrush(Qt::SolidPattern);
        p->drawPolygon(pts);
      }
      break;
    case 11:
      {
        QPolygon pts(4);

        pts.putPoints( 0, 4,   x+s, y,
                               x, y+s,
                               x-s, y,
                               x, y-s );
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 12:
      {    
        QPolygon pts(4);

        pts.putPoints( 0, 4,   x+s, y,
                               x, y+s,
                               x-s, y,
                               x, y-s );
        p->setBrush(Qt::SolidPattern);
        p->drawPolygon(pts);
      }
      break;
  }

  p->setBrush(Qt::NoBrush);
}


int dim(QPainter *p) {
  QRect r = p->window();
  return qMax(1, ((r.width() + r.height()) / 400));
}

}

// vim: ts=2 sw=2 et
