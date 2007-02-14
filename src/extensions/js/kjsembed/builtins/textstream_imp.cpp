/*
 *  Copyright (C) 2003, Richard J. Moore <rich@kde.org>
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

#include <qtextstream.h>
#include <qiodevice.h>

#include <kjsembed/global.h>
#include <kjsembed/jsbinding.h>
#include "jsopaqueproxy.h"
#include "textstream_imp.h"

namespace KJSEmbed {
namespace BuiltIns {

TextStreamImp::TextStreamImp( KJS::ExecState *exec, int mid, QTextStream *textstream )
    : JSProxyImp(exec), id(mid), ts(textstream)
{
}

TextStreamImp::~TextStreamImp()
{
}

void TextStreamImp::addBindings( KJS::ExecState *exec, KJS::Object &parent )
{
    kdDebug() << "TextStreamImp::addBindings()" << endl;

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( parent.imp() );
    if ( !op ) {
	kdWarning() << "TextStreamImp::addBindings() failed, not a JSOpaqueProxy" << endl;
	return;
    }

    QTextStream *ts = op->toTextStream();
    if ( !ts ) {
	kdWarning() << "TextStreamImp::addBindings() failed, type is " << op->typeName() << endl;
	return;
    }

    JSProxy::MethodTable methods[] = { 
	{ MethodIsReadable, "isReadable" },
	{ MethodIsWritable, "isWritable" },
	{ MethodPrint, "print" },
	{ MethodPrintLn, "println" },
	{ MethodReadLine, "readLine" },
	{ MethodFlush, "flush" },
	{ 0, 0 }
    };

    int idx = 0;
    do {
	TextStreamImp *tsi = new TextStreamImp( exec, idx, ts );
	parent.put( exec , methods[idx].name, KJS::Object(tsi) );
	++idx;
    } while( methods[idx].id );
}

KJS::Value TextStreamImp::call( KJS::ExecState *exec, KJS::Object &/*self*/, const KJS::List &args )
{
    QString not_readable = i18n( "Attempt to read from a write-only text stream." );
    QString not_writable = i18n( "Attempt to write to a read-only text stream." );

		QString arg0 = extractQString(exec, args, 0);

    QIODevice *dev = ts->device();
    KJS::Object err;

    switch ( id ) {
	case MethodIsReadable:
	    return KJS::Boolean( dev->isReadable() );
	    break;
	case MethodIsWritable:
	    return KJS::Boolean( dev->isWritable() );
	    break;
	case MethodPrint:
	    if ( !dev->isWritable() ) {
          return throwError(exec, not_writable.utf8());
	    }
	    (*ts) << arg0; 
	    break;
	case MethodPrintLn:
	    if ( !dev->isWritable() ) {
          return throwError(exec, not_writable.utf8());
	    }
	    (*ts) << arg0 << endl;
	    break;
	case MethodReadLine:
	    if ( dev->isReadable() ) {
		QString line = ts->readLine();
		if ( line.isNull() )
		    return KJS::Null();
		else
		    return KJS::String( line );
	    }
	    else {
          return throwError(exec, not_readable.utf8());
	    }
	    break;
	case MethodFlush:
	    if ( !dev->isWritable() ) {
          return throwError(exec, not_writable.utf8());
	    }
	    (*ts) << flush; 
	    break;
	default:
	    kdWarning() << "TextStreamImp has no method " << id << endl;
	    break;
    }

    return KJS::Value();
}

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
