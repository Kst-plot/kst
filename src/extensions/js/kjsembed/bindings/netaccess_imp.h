// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; -*-
/**
 * netaccess.h
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
#ifndef KJSEMBED_NETACCESS_IMP_H
#define KJSEMBED_NETACCESS_IMP_H

#include <qstring.h>
#include "bindingobject.h"

class KURL;

namespace KJSEmbed {
namespace Bindings {

/**
 * Provides a binding to KIO::NetAccess.
 */
class NetAccess : public BindingObject
{
    Q_OBJECT
public:
    NetAccess( QObject *parent, const char *name=0 );
    virtual ~NetAccess();
public slots:
    bool download( const KURL& src, const QString & loc );
    QString createTempFile(const QString& prefix, const QString& ext, uint mode );
    void removeTempFile( const QString& name );
    bool upload( const QString& src, const KURL& dest );
    bool copy( const KURL& src, const KURL& target );
    bool dircopy( const KURL& src, const KURL& target );
    bool move( const KURL& src, const KURL& target );
    bool exists( const KURL& url, bool source );
    bool del( const KURL& url );
    int propertiesDialog( const KURL &url );

    QString fish_execute( const KURL& url, const QString& command );
    QString mimetype( const KURL& url );
    QString lastErrorString();
};

} // namespace Bindings
} // namespace KJSEmbed

#endif
