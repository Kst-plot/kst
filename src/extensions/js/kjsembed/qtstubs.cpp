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

#include "qtstubs.h"

#ifndef QT_ONLY
#include "qtstubs.moc"
#endif // QT_ONLY

#ifdef QT_ONLY

namespace KParts {

ReadOnlyPart::ReadOnlyPart( QObject *parent, const char *name )
  : QObject( parent, name ), m_widget(0)
{
}

ReadOnlyPart::~ReadOnlyPart()
{
}

ReadWritePart::ReadWritePart( QObject *parent, const char *name )
  : ReadOnlyPart( parent, name )
{
}

ReadWritePart::~ReadWritePart()
{
}

} // namespace KParts

#endif // QT_ONLY
