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

void draw(int Type, QPainter *p, double x, double y, int lineSize) {

  double s;

  s = (p->window().width()+p->window().height())*0.003;
  if (s<3.0) s = 3.0;

  if (Type < 0 || Type > KSTPOINT_MAXTYPE) {
    Type = 0;
  }

  if (lineSize == 0 || lineSize == 1) {
  } else {
    s = ( s * lineSize ) / 2.0;
  }

  switch (Type) {
    case 0:
      p->drawLine(QLineF(x-s, y-s, x+s, y+s));
      p->drawLine(QLineF(x-s, y+s, x+s, y-s));
      break;
    case 1:
      p->setBrush(Qt::NoBrush);
      p->drawRect(QRectF(x-s, y-s, 2.0*s, 2.0*s));
      break;
    case 2:
      p->setBrush(Qt::NoBrush);
      p->drawEllipse(QPointF(x,y), s, s);
      break;
    case 3:
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawEllipse(QPointF(x,y), s, s);
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
        QPolygonF pts;

        pts << QPointF(x-s, y+s) <<  QPointF(x,y-s) <<  QPointF(x+s,y+s);
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 6:
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawRect(QRectF(x-s, y-s, 2.0*s+1.0, 2.0*s+1.0));
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
        QPolygonF pts;
        pts << QPointF(x-s, y-s) <<  QPointF(x, y+s) <<  QPointF(x+s, y-s);
        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 10:
      {
        QPolygonF pts;

        pts << QPointF(x-s, y+s) <<  QPointF(x, y-s) <<  QPointF(x+s,y+s);

        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 11:
      {
        QPolygonF pts;

        pts << QPointF(x+s, y)
            << QPointF(x, y+s)
            << QPointF(x-s, y)
            << QPointF(x, y-s);

        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 12:
      {    
        QPolygonF pts;

        pts << QPointF(x+s, y)
            << QPointF(x, y+s)
            << QPointF(x-s, y)
            << QPointF(x, y-s);

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
