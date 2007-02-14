/***************************************************************************
                     kstmath.cpp  -  Math portability tools
                             -------------------
    begin                : Oct 20, 2004
    copyright            : (C) 2004 by The University of Toronto
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

#include "kstmath.h"

namespace KST {
#ifdef NAN
KST_EXPORT const double NOPOINT = NAN;
#else
KST_EXPORT const double NOPOINT = 0.0/0.0; // NaN
#endif
}

// vim: ts=2 sw=2 et
