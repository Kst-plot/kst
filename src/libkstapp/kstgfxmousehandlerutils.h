/***************************************************************************
                  kstgfxmousehandlerutils.h  -  Part of KST
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 The University of British Columbia
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

#ifndef KSTGFXMOUSEHANDLERUTILS_H
#define KSTGFXMOUSEHANDLERUTILS_H

class QRect;
class QPoint;

namespace KstGfxMouseHandlerUtils {
    // from the line described by fromPoint and toPoint, returns the point on the line (and inside bounds) with the shortest distance to pos. fromPoint must be inside bounds already.
    QPoint findNearestPtOnLine(const QPoint& fromPoint, const QPoint& toPoint, const QPoint& pos, const QRect &bounds);
    // resizes the rect described by anchorPoint and movePoint to pos, keeping anchorPoint fixed. anchorPoint and movePoint must be inside bounds already.
    QRect resizeRectFromCorner(const QPoint& anchorPoint, const QPoint& movePoint, const QPoint& pos, const QRect &bounds, bool maintainAspect);
    // resizes originalRect to pos, keeping the center fixed. anchorPoint must be inside bounds.
    QRect resizeRectFromCornerCentered(const QRect& originalRect, const QPoint& pos, const QRect& bounds, bool maintainAspect);
    // resizes a rect from an edge, keeping anchorPoint fixed. movePoint = center of edge being dragged. anchorPoint = center of opposite edge. anchorPoint and movePoint must be inside bounds already.
    QRect resizeRectFromEdge(const QRect& originalSize, const QPoint& anchorPoint, const QPoint& movePoint, const QPoint& pos, const QRect &bounds, bool maintainAspect);
    // returns a new rectangle. mouseOrigin must be inside bounds already.
    QRect newRect(const QPoint& pos, const QPoint& mouseOrigin, const QRect& bounds, bool squareAspect);
    // returns a new rectangle, mouseOrigin must be inside bounds already.
    QRect newRectCentered(const QPoint& pos, const QPoint& mouseOrigin, const QRect& bounds, bool squareAspect);
    // returns a rectangle with topLeft = from, and bottomRight = to. mouseOrigin must be inside bounds already.
    QRect newLine(const QPoint& pos, const QPoint& mouseOrigin, bool specialAspect, const QRect& bounds);
    // returns -1 if pos is in 1st or 3rd quadrant with origin mouseOrigin, 1 otherwise
    int quadrantSign(const QPoint& pos, const QPoint& mouseOrigin);

    
}

#endif

// vim: ts=2 sw=2 et
