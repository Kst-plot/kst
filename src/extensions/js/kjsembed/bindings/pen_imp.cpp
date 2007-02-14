// -*- c++ -*-

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

#include <qpainter.h>
#include <qpixmap.h>
#include <kjsembed/global.h>
#include <qpen.h>
#include "pen_imp.h"
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>

namespace KJSEmbed {
namespace Bindings {

Pen::Pen( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

Pen::~Pen()
{
}
void Pen::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType(object, JSProxy::ValueProxy, "QPen") ) return;
    
    JSProxy::MethodTable methods[] = {
    { Methodwidth,  "width"},
    { MethodsetWidth, "setWidth"},
    { MethodColor, "color"},
    { MethodsetColor, "setColor"},
    { 0, 0 }
    };

    JSProxy::addMethods<Pen>(exec, methods, object );

    JSProxy::EnumTable enums[] = {
    // PenStyle
    { "NoPen", 0 },
    { "SolidLine", 1 },
    { "DashLine", 2 },
    { "DotLine", 3 },
    { "DashDotLine", 4 },
    { "DashDotDotLine", 5 },
    { "MPenStyle", 6 },
    // Pen Join Style
    { "MiterJoin", 7 },
    { "BevelJoin", 8 },
    { "RoundJoin", 9 },
    { "MPenJoinStyle", 10 },
    // Pen Cap Style
    { "FlatCap", 11},
    { "SquareCap", 12 },
    { "RoundCap", 13 },
    { "MPenCapStyle", 14},
    
    { 0, 0 }
    };

    JSProxy::addEnums(exec, enums, object);
}

KJS::Value Pen::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {

    if( !JSProxy::checkType(self, JSProxy::ValueProxy, "QPen") ) return KJS::Value();
    JSValueProxy *op = JSProxy::toValueProxy( self.imp() );
    QPen pen = op->toVariant().toPen();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case Methodwidth:
    {
    	return KJS::Number((int) pen.width() );
	break;
    }
    case MethodsetWidth:
    {
    	uint w = extractUInt(exec, args, 0);
	pen.setWidth(w);
	break;
    }
    case MethodColor:
    {
    	return convertToValue(exec, pen.color());
	break;
    }
    case MethodsetColor:
    {
    	QColor color = extractQColor(exec, args, 0);
	pen.setColor(color);
	break;
    }
    default:
        kdWarning() << "Pen has no method " << mid << endl;
        break;
    }

    op->setValue(pen);
    return retValue;
}

} // namespace Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


