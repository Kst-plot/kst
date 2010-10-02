/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "math_kst.h"

namespace Kst {
#ifdef NAN
const double NOPOINT = NAN;
#else
const double NOPOINT = 0.0/0.0; // NaN
#endif
}

// vim: ts=2 sw=2 et
