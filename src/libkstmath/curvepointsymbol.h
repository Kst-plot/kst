/***************************************************************************
                            curvepointsymbol.h
                             -------------------
    begin                : June 3 2001
    copyright            : (C) 2001 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CURVEPOINTSYMBOL_H
#define CURVEPOINTSYMBOL_H

#include "kstmath_export.h"
#include <qpainter.h>

#define KSTPOINT_MAXTYPE 13

namespace Kst {

namespace  CurvePointSymbol {
    /** draw the point on a painter the scale of the point is based on size */
    KSTMATH_EXPORT void draw(int type, QPainter *p, double x, double y, double pointSize=4);
    /** Get the dimension of a point */
    KSTMATH_EXPORT int dim(const QRect &window);
}

}
#endif
// vim: ts=2 sw=2 et
