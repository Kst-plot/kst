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

#ifndef AXIS_H
#define AXIS_H

namespace Kst {

// A representation of a plot axis.  This has nothing to do with the way the
// axis is drawn - only the properties of it.
class Axis {
  public:
    Axis();

    bool reversed;
    bool isLog;
    double logBase;
};


// A representation of a style used to render an axis.
class AxisStyle {
  public:
    AxisStyle();

    bool showMinorTicks;
    bool showMajorTicks;
    double minorTickWidth;
    double majorTickWidth;
};

}

#endif

// vim: ts=2 sw=2 et
