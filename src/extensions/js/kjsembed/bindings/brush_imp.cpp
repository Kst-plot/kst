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
#include <qbrush.h>

#include "brush_imp.h"

namespace KJSEmbed {
namespace Bindings {

BrushImp::BrushImp( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

BrushImp::~BrushImp()
{
}

void BrushImp::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType(object, JSProxy::ValueProxy, "QBrush") ) return;

    JSProxy::MethodTable methods[] = {
    { Methodstyle,  "style"},
    { MethodsetStyle, "setStyle"},
    { MethodColor, "color"},
    { MethodsetColor, "setColor"},
    { 0, 0 }
    };

    JSProxy::addMethods<BrushImp>(exec, methods, object );

    JSProxy::EnumTable enums[] = {
    // BrushType
    { "NoBrush", 0 },
    { "SolidBrush", 1 },
    { "Dense1Pattern", 2 },
    { "Dense2Pattern", 3 },
    { "Dense3Pattern", 4 },
    { "Dense4Pattern", 5 },
    { "Dense5Pattern", 6 },
    { "Dense6Pattern", 7 },
    { "Dense7Pattern", 8 },
    { "HorPattern", 9 },
    { "VerPattern", 10 },
    { "CrossPattern", 11},
    { "BDiagPattern", 12 },
    { "FDiagPattern", 13 },
    { "DiagCrossPattern", 14},
    
    { 0, 0 }
    };

    JSProxy::addEnums(exec, enums, object);

}

KJS::Value BrushImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
 
    if( !JSProxy::checkType(self, JSProxy::ValueProxy, "QBrush") ) return KJS::Value();
    JSValueProxy *op = JSProxy::toValueProxy( self.imp() );
    QBrush brush = op->toVariant().toBrush();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case Methodstyle:
    {
    	return KJS::Number((int) brush.style() );
	break;
    }
    case MethodsetStyle:
    {
    	int style = extractInt(exec, args, 0);
	brush.setStyle((Qt::BrushStyle)style);
	break;
    }
    case MethodColor:
    {
    	return convertToValue(exec, brush.color());
	break;
    }
    case MethodsetColor:
    {
    	QColor color = extractQColor(exec, args, 0);
	brush.setColor(color);
	break;
    }
    default:
        kdWarning() << "Brush has no method " << mid << endl;
        break;
    }

    op->setValue(brush);
    return retValue;
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed
