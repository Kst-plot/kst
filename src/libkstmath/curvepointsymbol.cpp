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
#include <math.h>

namespace Kst {

namespace CurvePointSymbol {

void draw(int Type, QPainter *p, double x, double y, double pointSize) {

  double s = pointSize;
  double lw;
  bool int_draw = !(p->testRenderHint(QPainter::Antialiasing));

  p->save();
  QPen pen = p->pen();
  lw = p->pen().widthF();
  if (lw<=1.0) lw = 1.01; // QT4 line drawing is buggy for width < 1.0
  pen.setWidthF(lw);
  p->setPen(pen);

  if (Type < 0 || Type > KSTPOINT_MAXTYPE) {
    Type = 0;
  }

  if (int_draw) {
    x = floor(x+0.5);
    y = floor(y+0.5);
    s = floor(s+0.5);
    if (p->transform().isTranslating()) { // translate to nearest pixel
      qreal dx = p->transform().dx();
      qreal dy = p->transform().dy();
      QTransform T;
      T.translate(floor(dx+0.5), floor(dy+0.5));
      p->setTransform(T);
    }
  }

  switch (Type) {
    case 0: // X
        p->drawLine(QPointF(x-s, y-s), QPointF(x+s, y+s));
        p->drawLine(QPointF(x-s, y+s), QPointF(x+s, y-s));
      break;
    case 1: // unfilled square
      p->setBrush(Qt::NoBrush);
      p->drawRect(QRectF(x-s, y-s, 2.0*s, 2.0*s));
      break;
    case 2: // unfilled circle
      p->setBrush(Qt::NoBrush);
      p->drawEllipse(QPointF(x,y), s, s);
      break;
    case 3: // filled circle
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawEllipse(QPointF(x,y), s, s);
      break;
    case 4: // unfilled down triangle
      {
        QPolygonF pts;

        pts << QPointF(x-s, y-s) << QPointF(x, y+s) << QPointF(x+s, y-s);
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 5: // unfilled up triangle
      {
        QPolygonF pts;

        pts << QPointF(x-s, y+s) <<  QPointF(x,y-s) <<  QPointF(x+s,y+s);
        p->setBrush(Qt::NoBrush);
        p->drawPolygon(pts);
      }
      break;
    case 6: // filled square
      p->setBrush(Qt::SolidPattern);
      p->setBrush(p->pen().color());
      p->drawRect(QRectF(x-s, y-s, 2.0*s+1.0, 2.0*s+1.0));
      break;
    case 7: // +
      p->drawLine(QPointF(x-s, y), QPointF(x+s, y));
      p->drawLine(QPointF(x, y-s), QPointF(x, y+s));
      break;
    case 8: // *
      if (int_draw) { // diagonal lines need to be even num pix wide w/out antialiasing
        lw = floor(lw*0.5) * 2.0;
      }
      pen = p->pen();
      pen.setWidthF(lw);
      p->setPen(pen);
      p->drawLine(QPointF(x-s, y-s), QPointF(x+s, y+s));
      p->drawLine(QPointF(x-s, y+s), QPointF(x+s, y-s));
      if (int_draw) { // vertical lines need to be odd num pix wide w/out antialiasing
        lw-=1;
        if (lw<0) lw = 0;
        pen = p->pen();
        pen.setWidthF(lw);
        p->setPen(pen);
      }
      s*=1.4;
      p->drawLine(QPointF(x-s, y), QPointF(x+s, y));
      p->drawLine(QPointF(x, y-s), QPointF(x, y+s));
      break;
    case 9: // filled down triangle
      {
        QPolygonF pts;
        pts << QPointF(x-s, y-s) <<  QPointF(x, y+s) <<  QPointF(x+s, y-s);
        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 10: // filled up triangle
      {
        QPolygonF pts;

        pts << QPointF(x-s, y+s) <<  QPointF(x, y-s) <<  QPointF(x+s,y+s);

        p->setBrush(Qt::SolidPattern);
        p->setBrush(p->pen().color());
        p->drawPolygon(pts);
      }
      break;
    case 11: // unfilled diamond
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
    case 12: // filled diamond
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
  p->restore();
}

}

}
// vim: ts=2 sw=2 et
