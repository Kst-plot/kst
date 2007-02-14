// -*- c++ -*-

/*
 *  Copyright (C) 2001-2003, Richard J. Moore <rich@kde.org>
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

#ifndef KJSEMBEDSLOTUTILS_H
#define KJSEMBEDSLOTUTILS_H
#include "global.h"

#include <kjs/object.h>
#include <kjsembed/jsbinding.h>

class QObject;

namespace KJS { class Interpreter; }

namespace KJSEmbed {
namespace Bindings {

class JSObjectProxyImp;

/**
 * Provides utility methods for slot invocation.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */
class KJSEMBED_EXPORT JSSlotUtils
{
public:
    /**
     * Identifiers for fast slot signatures.
     */
    enum SignatureId {
	SignatureNotSupported = -1,
	SignatureUnknown,

	SignatureNone,

	SignatureInt, SignatureUInt, SignatureLong, SignatureULong,
	SignatureBool, SignatureDouble, SignatureDateTime,
	SignatureString, SignatureCString, SignatureDate, SignatureTime,

	SignatureURL, SignatureColor, SignaturePoint, SignatureRect,
	SignatureSize, SignaturePixmap, SignatureFont, SignatureImage,

	SignatureQWidget,

	SignatureIntInt, SignatureStringInt, SignatureRectBool,
	SignatureStringString, SignatureDateDate, SignatureColorString,
	SignatureStringBool, SignatureIntColor, SignatureIntBool,
	SignatureURLURL, SignatureURLString, SignatureStringURL,

	SignatureStringStringString, SignatureStringBoolBool, SignatureStringIntInt,
	SignatureIntColorBool, SignatureIntIntBool, SignatureIntIntString,
	SignatureStringBoolInt,

	SignatureIntIntIntInt,SignatureIntIntFloatFloat, SignatureStringBoolBoolBool,

	SignatureIntIntIntIntBool, SignatureIntIntIntIntInt, SignatureIntIntIntIntIntInt,
        SignatureCustom

    };

    /**
     * Returns the method id for the specified slot signature. If there is no match
     * then -1 is returned.
     */
    static int findSignature( const QString &sig );

    /** Connects the specified signal to the specified slot. */
    static bool connect( QObject *sender, const char *sig, QObject *recv, const char *dest );

    static KJS::Boolean disconnect( KJS::ExecState *exec, KJS::Object &self,
			     QObject *sender, const char *sig, QObject *recv, const char *dest );

    static KJS::Value invokeSlot( KJS::ExecState *exec, KJS::Object &self, const KJS::List&args,
				  JSObjectProxyImp *proxyimp );

    /** Returns the value of the QUObject or KJS::Null. */
    static KJS::Value extractValue( KJS::ExecState *exec, QUObject *uo, JSObjectProxy *parent=0 );

    /** Implant an int into a QUObject. */
    static void implantInt( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );
    /** Implant a QString into a QUObject. */
    static void implantQString( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );
    /** Implant a bool into a QUObject. */
    static void implantBool( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );
    /** Implant a double into a QUObject. */
    static void implantDouble( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );
    /** Implant a C string into a QUObject. */
    static void implantCString( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );

    /** Implant a KURL into a QUObject. */
    static void implantURL( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, KURL *url );
    /** Implant a QColor into a QUObject. */
    static void implantColor( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QColor *color );
    /** Implant a QPoint into a QUObject. */
    static void implantPoint( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QPoint *point );
    /** Implant a QRect into a QUObject. */
    static void implantRect( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QRect *rect );
    /** Implant a QSize into a QUObject. */
    static void implantSize( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QSize *size );

    /** Implant a QDate into a QUObject. */
    static void implantDate( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QDate *date );
    /** Implant a QTime into a QUObject. */
    static void implantTime( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QTime *time );
    /** Implant a QDateTime into a QUObject. */
    static void implantDateTime( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QDateTime *datetime );
    /** Implant a QPixmap into a QUObject. */
    static void implantPixmap( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QPixmap *pix );

    /**
     * Implant a value proxy into a QUObject. The type specified in the clazz parameter
     * must match the type of the proxy'd value for the method to succeed.
     */
    static bool implantValueProxy( KJS::ExecState *exec, QUObject *uo,
				   const KJS::Value &v, const QString &clazz);

    /** Implant a QVariant into a QUObject. */
    static bool implantQVariant( KJS::ExecState *exec, QUObject *uo,const KJS::Value &v);

    /**
     * Implant an opaque proxy into a QUObject. The type specified in the
     * clazz parameter must match the type of the proxy'd value for the method
     * to succeed.
     */
    static bool implantOpaqueProxy( KJS::ExecState *, QUObject *uo,
				    const KJS::Value &v, const QString &clazz );

    /**
     * Implant an object proxy into a QUObject. The object proxy'd by the specified value
     * must inherit the class specified for the converstion to succeed.
     */
    static bool implantObjectProxy( KJS::ExecState *exec, QUObject *uo,
				    const KJS::Value &v, const QString &clazz );
    /** Implant a QString into a QUObject. */ 
    static void implantStringList(KJS::ExecState *exec, QUObject *uo, 
				  const KJS::Value &v, QStringList *lst );
    
    /** Implant a char* into a QUObject. */ 
    static void implantCharStar( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v );

private:
    JSSlotUtils();
    ~JSSlotUtils();

    class JSSlotUtilsPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDSLOTUTILS_H

// Local Variables:
// c-basic-offset: 4
// End:
