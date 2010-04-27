/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2004 by University of British Columbia
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qnamespace.h>

#ifndef LINESTYLE_H
#define LINESTYLE_H

#include "kstmath_export.h"

namespace Kst {

KSTMATH_EXPORT extern const unsigned int LINESTYLE_MAXTYPE;
KSTMATH_EXPORT extern Qt::PenStyle LineStyle[];

#define POINTDENSITY_MAXTYPE 4
#define LINEWIDTH_MAX        5

}

#endif
// vim: ts=2 sw=2 et
