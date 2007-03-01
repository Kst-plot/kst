/***************************************************************************
                 kstgfxmousehandlerutils.cpp  -  description
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 by University of British Columbia
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

#include <stdlib.h>
#include <math.h>

#include <qrect.h>
#include <qpoint.h>

#include <kglobal.h>

#include <qdebug.h>
#include "kstgfxmousehandlerutils.h"

QPoint KstGfxMouseHandlerUtils::findNearestPtOnLine(const QPoint& fromPoint, const QPoint& toPoint, const QPoint& pos, const QRect &bounds) {
  QPoint npos = pos;
  
  if (fromPoint.y() == toPoint.y()) {
    npos.setY(fromPoint.y());
    npos.setX(qMax(npos.x(), bounds.left()));
    npos.setX(qMin(npos.x(), bounds.right()));
  } else if (fromPoint.x() == toPoint.x()) {
    npos.setX(fromPoint.x());
    npos.setY(qMax(npos.y(), bounds.top()));
    npos.setY(qMin(npos.y(), bounds.bottom()));
  } else {
    double newxpos, newypos;
    double slope = double(toPoint.y() - fromPoint.y()) / double(toPoint.x() - fromPoint.x());

    newxpos = (double(pos.y()) + slope*double(fromPoint.x()) + double(pos.x())/slope -double(fromPoint.y())) / (slope + 1.0/slope); //we want the tip of our new line to be as close as possible to the original line (while still maintaining aspect). 

    newxpos = qMin(newxpos, double(bounds.right())); //ensure that our x is inside the bounds.
    newxpos = qMax(newxpos, double(bounds.left())); // ""
    newypos = slope*(newxpos - double(fromPoint.x())) + double(fromPoint.y()); //consistency w/ x.

    newypos = qMin(newypos, double(bounds.bottom())); //ensure that our y is inside the bounds.
    newypos = qMax(newypos, double(bounds.top())); // ""*/
    newxpos = double(fromPoint.x()) + (newypos - double(fromPoint.y()))/slope; // x will still be inside the bounds because we have just moved newypos closer to fromPoint.y(), which will send newxpos closer to fromPoint.x(), ie. in the direction further 'into' the bounds.

    npos.setX(int(newxpos));
    npos.setY(int(newypos));
  }

  return npos;
}


QRect KstGfxMouseHandlerUtils::resizeRectFromCorner(const QPoint& anchorPoint, const QPoint& movePoint, const QPoint& pos, const QRect& bounds, bool maintainAspect) {
  QRect newSize;
  QPoint npos = pos;

  if (maintainAspect) {
    QPoint fakeMovePoint = anchorPoint + QPoint(quadrantSign(pos,anchorPoint)*abs((movePoint - anchorPoint).x()),abs((movePoint - anchorPoint).y())); // allow the rectangle to flip.
    npos = findNearestPtOnLine(anchorPoint, fakeMovePoint, pos, bounds);
  }
  
  newSize.setTopLeft(anchorPoint);
  newSize.setBottomRight(npos);

  return bounds.intersect(newSize.normalize());
}


QRect KstGfxMouseHandlerUtils::resizeRectFromCornerCentered(const QRect& originalRect, const QPoint& pos, const QRect& bounds, bool maintainAspect) {
  QRect newRect;
  QPoint anchorPoint = originalRect.center();

  int newHalfWidth, newHalfHeight;

  newHalfWidth = abs((pos - anchorPoint).x());
  newHalfHeight = abs((pos - anchorPoint).y());

  if (maintainAspect) {
    newHalfWidth = qMin(newHalfWidth,anchorPoint.x() - bounds.left());
    newHalfWidth = qMin(newHalfWidth,bounds.right() - anchorPoint.x());

    newHalfHeight = qMin(newHalfHeight,anchorPoint.y() - bounds.top());
    newHalfHeight = qMin(newHalfHeight,bounds.bottom() - anchorPoint.y());

    QSize newSize(originalRect.size());
    newSize.scale(2*newHalfWidth,2*newHalfHeight,QSize::ScaleMin);

    newRect.setSize(newSize);
    newRect.moveCenter(anchorPoint);
  } else {
    newRect = QRect(0,0,2*newHalfWidth,2*newHalfHeight);
    newRect.moveCenter(anchorPoint);
    newRect = newRect.intersect(bounds);
  }

  return newRect;
}


QRect KstGfxMouseHandlerUtils::resizeRectFromEdge(const QRect& originalSize, const QPoint& anchorPoint, const QPoint& movePoint, const QPoint& pos, const QRect &bounds, bool maintainAspect) {
  QRect newSize(originalSize);

  if (movePoint.y() == anchorPoint.y()) {
      int newWidth = pos.x() - anchorPoint.x(); //defined differently than in QRect.

      if (maintainAspect) {
        double newHalfHeight = originalSize.height() * (abs(newWidth) + 1) / originalSize.width() / 2.0; //defined with the QRect convention (height = bot - top + 1)

        newHalfHeight = qMin(double(movePoint.y() - bounds.top()) + 1, newHalfHeight); // ensure we are still within the bounds.
        newHalfHeight = qMin(double(bounds.bottom() - movePoint.y()) + 1, newHalfHeight);

        if (newWidth == 0) { // anything better to be done?
          newWidth = 1;
        }

        newWidth = (int(originalSize.width() * (newHalfHeight * 2.0) / originalSize.height()) - 1)*newWidth/abs(newWidth); // consistency of width w/ the newly calculated height.

        newSize.setTop(anchorPoint.y() + int(newHalfHeight - 0.5));
        newSize.setBottom(anchorPoint.y() - int(newHalfHeight - 0.5));
      }

      newSize.setLeft(anchorPoint.x());
      newSize.setRight(anchorPoint.x() + newWidth); // +1 for the way widths are defined in QRect.

    } else if (movePoint.x() == anchorPoint.x()) {
      // mimic the case for (movePoint.y() == anchorPoint.y()). comments are there.
      int newHeight = pos.y() - anchorPoint.y();

      if (maintainAspect) {
        double newHalfWidth = originalSize.width() * (abs(newHeight) + 1) / originalSize.height() / 2.0;

        newHalfWidth = qMin(double(movePoint.x() - bounds.left() + 1), newHalfWidth);
        newHalfWidth = qMin(double(bounds.right() - movePoint.x() + 1), newHalfWidth);

        if (newHeight == 0) {
          newHeight = 1;
        }

        newHeight = (int(originalSize.height() * newHalfWidth * 2.0 / originalSize.width()) - 1)*newHeight/abs(newHeight);
        newSize.setLeft(anchorPoint.x() + int(newHalfWidth - .5));
        newSize.setRight(anchorPoint.x() - int(newHalfWidth - .5));
      }

      newSize.setTop(anchorPoint.y());
      newSize.setBottom(anchorPoint.y() + newHeight);
    }

    return newSize.normalize();
}


QRect KstGfxMouseHandlerUtils::newRect(const QPoint& pos, const QPoint& mouseOrigin, const QRect& bounds, bool squareAspect) {
  return resizeRectFromCorner(mouseOrigin, mouseOrigin + QPoint(1,1), pos, bounds, squareAspect);
}


QRect KstGfxMouseHandlerUtils::newRectCentered(const QPoint& pos, const QPoint& mouseOrigin, const QRect& bounds, bool squareAspect) {
  QRect originalRect;
  originalRect.setSize(QSize(1,1));
  originalRect.moveCenter(mouseOrigin);

  return resizeRectFromCornerCentered(originalRect, pos, bounds, squareAspect);
}


QRect KstGfxMouseHandlerUtils::newLine(const QPoint& pos, const QPoint& mouseOrigin, bool specialAspect, const QRect& bounds) {

  if (KDE_ISLIKELY(!specialAspect)) {
    QPoint npos = KstGfxMouseHandlerUtils::findNearestPtOnLine(mouseOrigin, pos, pos, bounds);
    return QRect(mouseOrigin, npos);
  } else { //want special 45deg, or vertical, or horizontal line. 
    QPoint npos;
    QPoint mouseDisplacement(pos - mouseOrigin); // for picking type of line..

    if (mouseDisplacement.x() == 0) { // vertical line.
      npos = findNearestPtOnLine(mouseOrigin, mouseOrigin + QPoint(0,1), pos, bounds);
    } else if (mouseDisplacement.y() == 0) { // horizontal line.
      npos = findNearestPtOnLine(mouseOrigin, mouseOrigin + QPoint(1,0), pos, bounds);
    } else { // 45deg or vertical or horizontal.
      int dx = int(rint(2.0*mouseDisplacement.x()/abs(mouseDisplacement.y())));
      int dy = int(rint(2.0*mouseDisplacement.y()/abs(mouseDisplacement.x())));

      if (dx != 0) {
        dx /= abs(dx);
      }
      if (dy != 0) { // type of line picked.
        dy /= abs(dy);
      }
  
      npos = findNearestPtOnLine(mouseOrigin, mouseOrigin + QPoint(dx,dy), pos, bounds);
    }
    return QRect(mouseOrigin, npos);
  }
}


int KstGfxMouseHandlerUtils::quadrantSign(const QPoint& pos, const QPoint& mouseOrigin) {
  if ((pos.y() < mouseOrigin.y() && pos.x() > mouseOrigin.x()) ||
       (pos.y() > mouseOrigin.y() && pos.x() < mouseOrigin.x())) {
    return -1;
  } else {
    return 1;
  }
}


// vim: ts=2 sw=2 et
