// -*- c++ -*-

/*
 *  Copyright (C) 2004 Ian Reinhart Geiser <geiseri@kde.org>
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
#include "size_imp.h"
#include <qsize.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>

namespace KJSEmbed {
namespace Bindings {

Size::Size( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

Size::~Size()
{
}
void Size::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType(object, JSProxy::ValueProxy, "QSize") ) return;

    JSProxy::MethodTable methods[] = {
    { Methodwidth, "width"},
    { MethodsetWidth, "setWidth"},
    { MethodHeight, "height"},
    { MethodsetHeight, "setHeight"},
    { Methodscale, "scale"},
    { Methodtranspose, "transpose"},
    { MethodexpandedTo, "expandedTo"},
    { MethodboundedTo, "boundedTo"},
    { 0, 0 }
    };

    JSProxy::addMethods<Size>(exec, methods, object );
    //
    // Define the enum constants
    //

    JSProxy::EnumTable enums[] = {
    { "ScaleFree", (int)QSize::ScaleFree },
    { "ScaleMin", (int)QSize::ScaleMin },
    { "ScaleMax", (int)QSize::ScaleMax },
    { 0, 0 }
    };

    JSProxy::addEnums(exec, enums, object);
}

KJS::Value Size::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
    if( !JSProxy::checkType(self, JSProxy::ValueProxy, "QSize") ) return KJS::Value();
    KJS::Value retValue = KJS::Value();
    JSValueProxy *vp = JSProxy::toValueProxy( self.imp() );
    QSize val = vp->toVariant().toSize();

    switch ( mid ) {
        case Methodwidth:
            retValue = KJS::Number(val.width());
            break;
        case MethodsetWidth:
            val.setWidth(extractInt(exec,args,0));
            break;
        case MethodHeight:
            retValue = KJS::Number(val.height());
            break;
        case MethodsetHeight:
            val.setHeight(extractInt(exec,args,0));
            break;
        case Methodscale:
        {
            if( args.size() == 2 ) {
                QSize sz = extractQSize(exec, args, 0);
                int mode = extractInt(exec, args, 1);
                val.scale(sz, (QSize::ScaleMode)mode);
            } else {
                int x = extractInt(exec, args, 0);
                int y = extractInt(exec, args, 1);
                int mode = extractInt(exec, args, 2);
                val.scale(x,y, (QSize::ScaleMode)mode);
            }
            break;
        }
        case Methodtranspose:
            val.transpose();
            break;
        case MethodexpandedTo:
        {
            QSize sz = extractQSize(exec, args, 0);
            retValue = convertToValue( exec, val.expandedTo(sz) );
            break;
        }
        case MethodboundedTo:
        {
            QSize sz = extractQSize(exec, args, 0);
            retValue = convertToValue( exec, val.boundedTo(sz) );
            break;
        }
        default:
            QString msg = i18n( "Size has no method %1" ).arg(mid);
            return throwError(exec, msg);
            break;
    }

    vp->setValue(val);
    return retValue;
}

} // namespace Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
