/***************************************************************************
                              kstalignment.h
                             -------------
    begin                : June 24, 2004
    copyright            : (C) 2004 The University of Toronto
                           (C) 2004 The University of British Columbia
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

#ifndef KSTALIGNMENT_H
#define KSTALIGNMENT_H

#include <qmap.h>
#include <qpoint.h>
#include <qrect.h>

Q_EXPORT int operator<(const QPoint &p1, const QPoint &p2);

// this class keeps track of the plot line alignment (eg, xaxis lineup, etc)
class KstAlignment {
 public:
  KstAlignment();
  void reset();
  void setPosition(const QRect& geometry, const QRect& plotRegion);
  void limits(const QRect& geometry, double& xleft, double& xright, double& ytop, double& ybottom, double dFactor);
  QRect limits(const QRect& geometry);

private:
  typedef QMap<QPoint, QPoint> alignmentMap;
  alignmentMap  _xAlignments;
  alignmentMap  _yAlignments;
};

namespace KST {
  extern KstAlignment alignment;
}

#endif
// vim: ts=2 sw=2 et
