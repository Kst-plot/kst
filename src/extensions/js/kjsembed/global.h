// -*- c++ -*-
/***************************************************************************
 *   Copyright (C) 2004 by Richard Moore                                   *
 *   rich@kde.org                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef KJSEMBED_GLOBAL_H
#define KJSEMBED_GLOBAL_H

#ifndef WIN32
#include <ctype.h>
#include <stdlib.h>
char *itoa(int num, char *str, int radix = 10);
#endif

#include <kst_export.h>

#include <qtextstream.h>
namespace KJSEmbed {
KJSEMBED_EXPORT QTextStream *conin();
KJSEMBED_EXPORT QTextStream *conout();
KJSEMBED_EXPORT QTextStream *conerr();
}

#include <kdebug.h>
#include <klocale.h>

#endif // KJSEMBED_GLOBAL_H

