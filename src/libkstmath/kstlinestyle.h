/***************************************************************************
                          kstlinestyle.h: defines line styles for kst
                             -------------------
    begin                : Apr 22 2004
    copyright            : (C) 2001 The University of British Columbia
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
#include <qnamespace.h>

#ifndef KSTLINESTYLE_H
#define KSTLINESTYLE_H

#include "kst_export.h"

KST_EXPORT extern const unsigned int KSTLINESTYLE_MAXTYPE;
KST_EXPORT extern Qt::PenStyle KstLineStyle[];

#define KSTPOINTDENSITY_MAXTYPE 4
#define KSTLINEWIDTH_MAX        5

#endif
// vim: ts=2 sw=2 et
