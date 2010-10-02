/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2004 by University of British Columbia
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "linestyle.h"

namespace Kst {

const unsigned int LINESTYLE_MAXTYPE = 5;

Qt::PenStyle LineStyle[LINESTYLE_MAXTYPE] = {
  Qt::SolidLine, 
  Qt::DashLine, 
  Qt::DotLine, 
  Qt::DashDotLine, 
  Qt::DashDotDotLine
};

}
// vim: ts=2 sw=2 et
