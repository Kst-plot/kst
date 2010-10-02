/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AXIS_H
#define AXIS_H

#include <QtGlobal>

namespace Kst {

// A representation of a plot axis.  This has nothing to do with the way the
// axis is drawn - only the properties of it.
class Axis {
  public:
    Axis();
    ~Axis();

    bool reversed;
    bool isLog;
    qreal logBase;
};


// A representation of a style used to render an axis.
class AxisStyle {
  public:
    AxisStyle();
    ~AxisStyle();

    bool showMinorTicks;
    bool showMajorTicks;
    qreal minorTickWidth;
    qreal majorTickWidth;
};

}

#endif

// vim: ts=2 sw=2 et
