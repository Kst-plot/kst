/***************************************************************************
                              kstalignment.cpp
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
#include "kstalignment.h"

KstAlignment KST::alignment;

int operator<(const QPoint &p1, const QPoint &p2) {
  return p1.x() < p2.x() || (p1.x() == p2.x() && p1.y() < p2.y());
}


KstAlignment::KstAlignment() {
}


void KstAlignment::reset() {
  _xAlignments.clear();
  _yAlignments.clear();
}


void KstAlignment::setPosition(const QRect& geometry, const QRect& plotRegion) {
  QPoint xAllRegion(geometry.left(), geometry.right());
  QPoint yAllRegion(geometry.top(), geometry.bottom());
  QPoint xPlotRegion(plotRegion.left(), plotRegion.right());
  QPoint yPlotRegion(plotRegion.top(), plotRegion.bottom());

  if (_xAlignments.contains(xAllRegion)) {
    QPoint xPlotRegionCurrent = _xAlignments[xAllRegion];

    if (xPlotRegion.x() > xPlotRegionCurrent.x()) {
      xPlotRegionCurrent.setX(xPlotRegion.x());
    }
    if (xPlotRegion.y() > xPlotRegionCurrent.y()) {
      xPlotRegionCurrent.setY(xPlotRegion.y());
    }
    _xAlignments.insert(xAllRegion, xPlotRegionCurrent);
  } else {
    _xAlignments.insert(xAllRegion, xPlotRegion);
  }

  if (_yAlignments.contains(yAllRegion)) {
    QPoint yPlotRegionCurrent = _yAlignments[yAllRegion];

    if (yPlotRegion.x() > yPlotRegionCurrent.x()) {
      yPlotRegionCurrent.setX(yPlotRegion.x());
    }
    if (yPlotRegion.y() > yPlotRegionCurrent.y()) {
      yPlotRegionCurrent.setY(yPlotRegion.y());
    }
    _yAlignments.insert(yAllRegion, yPlotRegionCurrent);
  } else {
    _yAlignments.insert(yAllRegion, yPlotRegion);
  }
}


void KstAlignment::limits(const QRect& geometry, double& xleft, double& xright, double& ytop, double& ybottom, double dFactor) {
  QPoint xAllRegion(geometry.left(), geometry.right());
  QPoint yAllRegion(geometry.top(), geometry.bottom());

  alignmentMap::ConstIterator allRef = _xAlignments.find(xAllRegion);
  if (allRef != _xAlignments.end()) {
    xleft = double((*allRef).x()) * dFactor;
    xright = double((*allRef).y()) * dFactor;
  } else {
    xleft = 0.0;
    xright = 0.0;
  }

  allRef = _yAlignments.find(yAllRegion);
  if (allRef != _yAlignments.end()) {
    ytop = double((*allRef).x()) * dFactor;
    ybottom = double((*allRef).y()) * dFactor;
  } else {
    ytop = 0.0;
    ybottom = 0.0;
  }
}


QRect KstAlignment::limits(const QRect& geometry) {
  QPoint xAllRegion(geometry.left(), geometry.right());
  QPoint yAllRegion(geometry.top(), geometry.bottom());
  QRect plotRegion;

  alignmentMap::ConstIterator allRef = _xAlignments.find(xAllRegion);
  if (allRef != _xAlignments.end()) {
    plotRegion.setLeft((*allRef).x());
    plotRegion.setRight((*allRef).y());
  } else {
    plotRegion.setLeft(0);
    plotRegion.setRight(0);
  }

  allRef = _yAlignments.find(yAllRegion);
  if (allRef != _yAlignments.end()) {
    plotRegion.setTop((*allRef).x());
    plotRegion.setBottom((*allRef).y());
  } else {
    plotRegion.setTop(0);
    plotRegion.setBottom(0);
  }

  return plotRegion;
}

// vim: ts=2 sw=2 et
