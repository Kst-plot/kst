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
#include <QPolygon>
#include <QPainterPath>

namespace KstCurvePointSymbol {

void draw(int Type, QPainterPath *path, int x, int y, int lineSize, int size) {
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
      path->moveTo(x-s, y-s);
      path->lineTo(x+s, y+s);
      path->moveTo(x-s, y+s);
      path->lineTo(x+s, y-s);
      break;
    case 1:
//       p->setBrush(Qt::NoBrush);
      path->addRect(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 2:
//       p->setBrush(Qt::NoBrush);
      path->addEllipse(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 3:
//       p->setBrush(Qt::SolidPattern);
      path->addEllipse(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 4:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y-s, x, y+s, x+s, y-s );
//        p->setBrush(Qt::NoBrush);
        path->addPolygon(QPolygonF(pts));
      }
      break;
    case 5:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y+s, x, y-s, x+s, y+s );
//        p->setBrush(Qt::NoBrush);
        path->addPolygon(QPolygonF(pts));
      }
      break;
    case 6:
//      p->setBrush(Qt::SolidPattern);
      path->addRect(x-s, y-s, 2*s+1, 2*s+1);
      break;
    case 7:
      path->moveTo(x-s, y);
      path->lineTo(x+s, y);
      path->moveTo(x, y-s);
      path->lineTo(x, y+s);
      break;
    case 8:
      path->moveTo(x-s, y-s);
      path->lineTo(x+s, y+s);
      path->moveTo(x-s, y+s);
      path->lineTo(x+s, y-s);
      path->moveTo(x-s, y);
      path->lineTo(x+s, y);
      path->moveTo(x, y-s);
      path->lineTo(x, y+s);
      break;
    case 9:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y-s, x, y+s, x+s, y-s );
//        p->setBrush(Qt::SolidPattern);
        path->addPolygon(QPolygonF(pts));
      }
      break;
    case 10:
      {
        QPolygon pts(3);

        pts.putPoints( 0, 3, x-s, y+s, x, y-s, x+s, y+s );
//        p->setBrush(Qt::SolidPattern);
        path->addPolygon(QPolygonF(pts));
      }
      break;
    case 11:
      {
        QPolygon pts(4);

        pts.putPoints( 0, 4,   x+s, y,
                               x, y+s,
                               x-s, y,
                               x, y-s );
//        p->setBrush(Qt::NoBrush);
        path->addPolygon(QPolygonF(pts));
      }
      break;
    case 12:
      {    
        QPolygon pts(4);

        pts.putPoints( 0, 4,   x+s, y,
                               x, y+s,
                               x-s, y,
                               x, y-s );
//        p->setBrush(Qt::SolidPattern);
        path->addPolygon(QPolygonF(pts));
      }
      break;
  }

//  p->setBrush(Qt::NoBrush);
}


int dim(const QRect &rect) {
  return qMax(1, ((rect.width() + rect.height()) / 400));
}

}

// vim: ts=2 sw=2 et
