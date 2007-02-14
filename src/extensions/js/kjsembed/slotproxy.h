// -*- c++ -*-

/*
 *  Copyright (C) 2003-2004, Richard J. Moore <rich@kde.org>
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
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KJSEMBED_SLOTPROXY_H
#define KJSEMBED_SLOTPROXY_H
#include <kjsembed/global.h>

#include <kjs/object.h>
#include <qobject.h>

#ifndef QT_ONLY
#include <kurl.h>
#else
#include <kjsembed/qtstubs.h>
#endif // QT_ONLY

class QDate;
class QTime;
class QDateTime;

namespace KJS {
class Interpreter;
}

namespace KJSEmbed {

class JSObjectProxy;

/**
 * Allows slots to be connected to methods of a JS object.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSSlotProxy : public QObject
{
    Q_OBJECT

public:
    JSSlotProxy( QObject *parent, const char *name=0 );
    JSSlotProxy( QObject *parent, const char *name, JSObjectProxy *prx );

    virtual ~JSSlotProxy();

    void setInterpreter( KJS::Interpreter *js ) { this->js = js; }
    void setProxy( JSObjectProxy *proxy ) { this->proxy = proxy; }
    void setObject( const KJS::Object &obj ) { this->obj = obj; }
    void setMethod( const QString &method ) { this->method = method; }

public slots:
    void slot_void();
    void slot_bool( bool b );
    void slot_string( const QString &s );
    void slot_int( int i );
    void slot_uint( uint i );
    void slot_long( long i );
    void slot_ulong( ulong i );
    void slot_double( double d );
    void slot_font( const QFont &font );
    void slot_color( const QColor &color );
    void slot_point( const QPoint &point );
    void slot_rect( const QRect &rec );
    void slot_size( const QSize &size );
    void slot_pixmap( const QPixmap &pix );
    void slot_url( const KURL &url );
    void slot_intint( int , int );
    void slot_intbool( int , bool );
    void slot_intintint( int , int , int );

    void slot_date(const QDate& date);
    void slot_time(const QTime& time);
    void slot_datetime( const QDateTime &dateTime );
    void slot_datedate( const QDate &date1, const QDate &date2 );
    void slot_colorstring( const QColor &color, const QString &string);
    void slot_image( const QImage &image );

    void slot_variant( const QVariant &variant );
    
    void slot_widget( QWidget *widget );

private:
    void execute( const KJS::List &args );
    JSObjectProxy *proxy;
    KJS::Interpreter *js;
    KJS::Object obj;
    QString method;
    class SlotProxyPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBED_SLOTPROXY_H

