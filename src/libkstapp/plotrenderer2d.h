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

#ifndef PLOTRENDER2D_H
#define PLOTRENDER2D_H

#include "axis.h"

namespace Kst {

class PlotRenderer2D {
  public:
    PlotRenderer2D();

  private:
    Axis _xAxis, _yAxis;
    AxisStyle _xAxisStyle, _yAxisStyle;
};

}

#endif

// vim: ts=2 sw=2 et
