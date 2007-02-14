// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
/**
 * netaccess_imp.cpp
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA
 * 02111-1307  USA
 */
#include "netaccess_imp.h"

#include <kio/netaccess.h>
#include <kpropertiesdialog.h>

#include <kurl.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <ktempfile.h>

namespace KJSEmbed {
namespace Bindings {

NetAccess::NetAccess( QObject *parent, const char *name )
    : BindingObject( parent, name )
{
}

NetAccess::~NetAccess()
{
}

bool NetAccess::download( const KURL& src, const QString& loc )
{
    QString mloc = loc;
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::download( src, mloc, 0 );
#else
    return KIO::NetAccess::download( src, mloc );
#endif
}

QString NetAccess::createTempFile( const QString& prefix, const QString& ext, uint mode )
{
	KTempFile tmpFile = KTempFile(prefix, ext, mode);
	tmpFile.setAutoDelete(false);
	return tmpFile.name();
}

void NetAccess::removeTempFile( const QString& name )
{
    KIO::NetAccess::removeTempFile( name );
}

bool NetAccess::upload( const QString& src, const KURL& dest )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::upload( src, dest, 0 );
#else
    return KIO::NetAccess::upload( src, dest );
#endif
}

bool NetAccess::copy( const KURL& src, const KURL& target )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::copy( src, target, 0 );
#else
    return KIO::NetAccess::copy( src, target );
#endif
}

bool NetAccess::dircopy( const KURL& src, const KURL& target )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::dircopy( src, target, 0 );
#else
    return KIO::NetAccess::dircopy( src, target );
#endif
}

bool NetAccess::move( const KURL& src, const KURL& target )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::move( src, target, 0 );
#else
    kdWarning() << "NetAccess::move(src,target) is not supported on 3.1" << endl;
    return false;
#endif
}

bool NetAccess::exists( const KURL& url, bool source )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::exists( url, source, 0 );
#else
    return KIO::NetAccess::exists( url, source );
#endif
}

bool NetAccess::del( const KURL & url )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::del( url, 0 );
#else
    return KIO::NetAccess::del( url );
#endif
}

int NetAccess::propertiesDialog( const KURL &url )
{
    KPropertiesDialog dlg( url, 0, "file_properties", true, false );
    return dlg.exec();
}

QString NetAccess::fish_execute( const KURL& url, const QString& command )
{
#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::fish_execute( url, command, 0 );
#else
    kdWarning() << "NetAccess::fish_execute(url,command) is not supported on 3.1" << endl;
    return QString::null;
#endif
}

QString NetAccess::mimetype( const KURL& url )
{
    kdDebug()<<"mimetype CALLED "<<endl;

#if KDE_IS_VERSION(3,1,90)
    return KIO::NetAccess::mimetype( url, 0 );
#else
    return KIO::NetAccess::mimetype( url );
#endif
}

QString NetAccess::lastErrorString()
{
    return KIO::NetAccess::lastErrorString();
}

}
}

#include "netaccess_imp.moc"
