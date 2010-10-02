/***************************************************************************
                           curvepointsymbol.cpp
                             -------------------
    begin                : Sunday June 17, 2001
    copyright            : (C) 2001 by cbn
    email                : netterfield@astro.utoronto.ca
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


#include "curvepointsymbol.h"
#include <QPolygon>

namespace Kst {

namespace CurvePointSymbol {

void draw(int Type, QPainter *p, double x, double y, int lineSize, int size) {
  Q_UNUSED(size)
  
  double s;

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
      p->drawLine(QLineF(x-s, y-s, x+s, y+s));
      p->drawLine(QLineF(x-s, y+s, x+s, y-s));
      break;
    case 1:
      p->setBrush(Qt::NoBrush);
      p->drawRect(QRectF(x-s, y-s, 2*s, 2*s));
      break;
    case 2:
      p->setBrush(Qt::NoBrush);
      p->drawEllipse(x-s, y-s, 2*s, 2*s);
      break;
    case 3:
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawEllipse(x-s, y-s, 2*s, 2*s);
      break;
    case 4:
      {
        QPolygonF pts;

        pts << QPointF(x-s, y-s) << QPointF(x, y+s) << QPointF(x+s, y-s);
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 5:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, int(x-s), int(y+s), int(x), int(y-s), int(x+s), int(y+s) );
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 6:
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawRect(QRectF(x-s, y-s, 2*s+1, 2*s+1));
      break;
    case 7:
      p->drawLine(QLineF(x-s, y, x+s, y));
      p->drawLine(QLineF(x, y-s, x, y+s));
      break;
    case 8:
      p->drawLine(QLineF(x-s, y-s, x+s, y+s));
      p->drawLine(QLineF(x-s, y+s, x+s, y-s));
      p->drawLine(QLineF(x-s, y, x+s, y));
      p->drawLine(QLineF(x, y-s, x, y+s));
      break;    
    case 9:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, int(x-s), int(y-s), int(x), int(y+s), int(x+s), int(y-s) );
        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 10:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, int(x-s), int(y+s), int(x), int(y-s), int(x+s), int(y+s) );
        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 11:
      {
        QPolygon pts(4);

        pts.putPoints( 0, 4,   int(x+s), int(y),
                               int(x), int(y+s),
                               int(x-s), int(y),
                               int(x), int(y-s) );
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 12:
      {    
        QPolygon pts(4);

        pts.putPoints( 0, 4,   int(x+s), int(y),
                               int(x), int(y+s),
                               int(x-s), int(y),
                               int(x), int(y-s) );
        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
  }

  p->setBrush(Qt::NoBrush);
}


int dim(const QRect &window) {
  return qMax(1, ((window.width() + window.height()) / 400));
}

}

}
// vim: ts=2 sw=2 et
