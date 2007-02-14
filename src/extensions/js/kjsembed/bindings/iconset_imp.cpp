// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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

#include <kjsembed/global.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>
#include <qvariant.h>
#include <qiconset.h>
#include <qpixmap.h>

#include "iconset_imp.h"

namespace KJSEmbed {
namespace Bindings {

IconsetImp::IconsetImp( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

IconsetImp::~IconsetImp()
{
}

void IconsetImp::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    JSValueProxy *op = JSProxy::toValueProxy( object.imp() );
    if ( !op ) {
        kdWarning() << "IconsetImp::addBindings() failed, not a JSValueProxy" << endl;
        return;
    }

    if ( op->typeName() != "QIconset" ) {
	kdWarning() << "IconsetImp::addBindings() failed, type is " << op->typeName() << endl;
	return;
    }

    JSProxy::MethodTable methods[] = {
	{ MethodReset,  "reset"},
	{ MethodSetPixmap, "setPixmap"},
	{ MethodPixmap, "pixmap"},
	{ MethodIsGenerated, "isGenerated"},
	{ MethodClearGenerated, "clearGenerated"},
	{ MethodIsNull, "isNull"},
	{ MethodDetach, "detach"},
	{ 0, 0 }
    };

    int idx = 0;
    do {
        IconsetImp *meth = new IconsetImp( exec, methods[idx].id );
        object.put( exec , methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {
	// Size
	{ "Automatic", 0 },
	{ "Small", 1 },
	{ "Large", 2 },
	// Mode
	{ "Normal", 0 },
	{ "Disabled", 1 },
	{ "Active", 2 },
	// State
	{ "On", 0 },
	{ "Off", 1 },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );
}

KJS::Value IconsetImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {

    JSValueProxy *op = JSProxy::toValueProxy( self.imp() );
    if ( !op ) {
        kdWarning() << "IconsetImp::call() failed, not a JSValueProxy" << endl;
        return KJS::Value();
    }

    if ( op->typeName() != "QIconSet" ) {
	kdWarning() << "IconsetImp::call() failed, type is " << op->typeName() << endl;
	return KJS::Value();
    }

    QIconSet iconset = op->toVariant().toIconSet();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case MethodReset:
    {
    	QPixmap pix = extractQPixmap(exec, args, 0);
	QIconSet::Size size = (QIconSet::Size) extractInt(exec, args, 1 );
	iconset.reset(pix,size);
	break;
    }
    case MethodSetPixmap:
    {
	QPixmap pix = extractQPixmap( exec, args, 0);
	QString fname = extractQString( exec, args, 0);
	QIconSet::Size size = (QIconSet::Size) extractInt(exec, args, 1 );
	QIconSet::Mode mode = (QIconSet::Mode) extractInt( exec, args, 2 );
	QIconSet::State state = (QIconSet::State) extractInt( exec, args, 3);
	if( pix.isNull() )
		iconset.setPixmap( fname, size, mode, state );
	else
		iconset.setPixmap( pix, size, mode, state );
	break;
    }
    case MethodPixmap:
    {
	QPixmap pix;

	if( args.size() == 3 )
	{
		QIconSet::Size size = (QIconSet::Size)extractInt( exec, args, 0 );
		QIconSet::Mode mode = (QIconSet::Mode)extractInt( exec, args, 1 );
		QIconSet::State state = (QIconSet::State)extractInt( exec, args, 1 );
		pix = iconset.pixmap( size, mode, state );
	}
	else
		pix = iconset.pixmap();

	break;
    }
    default:
        kdWarning() << "Iconset has no method " << mid << endl;
        break;
    }

    op->setValue(iconset);
    return retValue;
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed
