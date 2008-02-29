/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// application specific includes
#include "plotmarkers.h"

#include <QDebug>

namespace Kst {

PlotMarkers::PlotMarkers(bool xAxis) :
  _xAxis(xAxis),
  _lineStyle(Qt::SolidLine),
  _lineWidth(1.0),
  _vector(0), 
  _curve(0) {
}


PlotMarkers::~PlotMarkers() {
}


void PlotMarkers::updateMarkers() {
  _markers.clear();
  _markers = _manualMarkers;
  if (_vector != 0) {
    _vector->readLock();
    for (int i = 0; i < _vector->length(); ++i) {
      _markers << _vector->value(i);
    }
    _vector->unlock();
  }
  if (_curve != 0) {
    _curve->readLock();

    int count = _curve->sampleCount();

    if (count > 0) {
      double prevX, prevY;
      double curX, curY;

      // scan through the whole curve
      _curve->point(0, prevX, prevY);
      for (int i = 1; i < count; i++) {
        _curve->point(i, curX, curY);
        if (_xAxis) {
          if ((_curveMode == RisingEdge || _curveMode == BothEdges) && prevY == 0.0 && curY > 0.0) {
            _markers << curX;
          }
          if ((_curveMode == FallingEdge || _curveMode == BothEdges) && prevY > 0.0 && curY == 0.0) {
            _markers << prevX;
          }
        } else {
          if ((_curveMode == RisingEdge || _curveMode == BothEdges) && prevX == 0.0 && curX > 0.0) {
            _markers << curY;
          }
          if ((_curveMode == FallingEdge || _curveMode == BothEdges) && prevX > 0.0 && curX == 0.0) {
            _markers << prevY;
          }
        }
        prevX = curX;
        prevY = curY;
      }
    }
    _curve->unlock();
  }
}

}
// vim: ts=2 sw=2 et
