/*
 *  Copyright (C) 2001-2002, Richard J. Moore <rich@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#include <kjs/ustring.h>
#include <kjs/types.h>
#include <kjs/value.h>

#include <qstring.h>

namespace KJS {

    UString::UString( const QString &d )
    {
	uint len = d.length();
	UChar *dat = new UChar[len];
	memcpy( dat, d.unicode(), len * sizeof(UChar) );
	rep = UString::Rep::create(dat, len);
    }

    QString UString::qstring() const
    {
	return QString((QChar*) data(), size());
    }

    QConstString UString::qconststring() const
    {
	return QConstString((QChar*) data(), size());
    }

    QString Identifier::qstring() const
    {
	return QString((QChar*) data(), size());
    }

}


