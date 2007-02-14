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

#include <qpixmap.h>
#include <qbitmap.h>

#include <kjsembed/global.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>
#include <qvariant.h>

#include "pixmap_imp.h"

namespace KJSEmbed {
namespace Bindings {

Pixmap::Pixmap( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

Pixmap::~Pixmap()
{
}

void Pixmap::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType( object, JSProxy::ValueProxy, "QPixmap") )
        return;
    
    JSProxy::MethodTable methods[] = {
    { MethodisNull, "isNull" },
    { Methodwidth, "width" },
    { Methodheight, "height" },
    { Methodsize, "size" },
    { Methodrect, "rect" },
    { Methoddepth, "depth" },
    { Methodresize, "resize" },
    { Methodfill, "fill" },
    { Methodmask, "mask" },
    { MethodsetMask, "setMask" },
    { MethodcreateHeuristicMask, "createHeuristicMask" },
    { MethodgrabWindow, "grabWindow" },
    { 0, 0 }
    };

    JSProxy::addMethods<Pixmap>(exec, methods, object);

}

KJS::Value Pixmap::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {

    if( !JSProxy::checkType( self, JSProxy::ValueProxy, "QPixmap") )
        return KJS::Value();

    JSValueProxy *op = JSProxy::toValueProxy( self.imp() );
    pix = op->toVariant().toPixmap();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case Methodwidth:
        retValue = KJS::Number(width());
        break;
    case Methodheight:
        retValue = KJS::Number(height());
        break;
    case Methoddepth:
        retValue = KJS::Number(depth());
        break;
    case MethodisNull:
        retValue = KJS::Boolean(isNull());
        break;
    case Methodsize:
        retValue = convertToValue(exec, size());
        break;
    case Methodrect:
        retValue = convertToValue(exec, rect());
        break;
    case Methodresize:
    {
        if( args.size() == 2)
		resize(extractInt(exec, args, 0), extractInt(exec, args, 1));
	else if( args.size() == 1)
	    resize(extractQSize(exec, args, 0) );
	break;
    }
    case Methodfill:
        fill( extractQColor(exec, args, 0));
        break;
    case Methodmask:
    {
        retValue = convertToValue(exec, mask() );
        break;
    }
    case MethodsetMask:
    {
       setMask(extractQPixmap(exec, args, 0));
       break;
    }
    case MethodcreateHeuristicMask:
    {
       retValue = convertToValue(exec, createHeuristicMask(extractBool(exec, args, 0)));
       break;
    }
    case MethodgrabWindow:
    {
    	int winid = extractInt(exec, args,0);
	int x = extractInt(exec, args,1);
	int y = extractInt(exec, args,2);
	int w = extractInt(exec, args,3);
	int h = extractInt(exec, args,4);
    	grabWindow(winid,x,y,w,h);
	break;
    }
    default:
        kdWarning() << "Image has no method " << mid << endl;
        break;
    }

    op->setValue(pix);
    return retValue;
}

void Pixmap::resize( int w, int h )
{
    pix.resize( w, h );
}

void Pixmap::resize( const QSize &size )
{
    pix.resize( size );
}

void Pixmap::fill( const QColor &c )
{
    pix.fill( c );
}

void Pixmap::grabWindow(int winID, int x, int y, int w, int h)
{
  pix = QPixmap::grabWindow((WId)winID, x, y, w, h);
}

QPixmap Pixmap::mask()
{
	return *(pix.mask());
}
void Pixmap::setMask(const QPixmap& mask)
{
	QBitmap bm;
	bm = mask;
	pix.setMask(bm);
}

QPixmap Pixmap::createHeuristicMask ( bool clipTight )
{
	return (QPixmap)pix.createHeuristicMask(clipTight);
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

