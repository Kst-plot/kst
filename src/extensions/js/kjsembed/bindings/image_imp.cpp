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

#include <qfile.h>
#include <kjsembed/global.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>
#include <qvariant.h>

#include "image_imp.h"

namespace KJSEmbed {
namespace Bindings {


ImageImp::ImageImp( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

ImageImp::~ImageImp()
{
}

void ImageImp::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    if( !JSProxy::checkType(object, JSProxy::ValueProxy, "QImage") ) return;
   
    JSProxy::MethodTable methods[] = {
    { Methodwidth, "width" },
    { Methodheight, "height" },
    { Methoddepth, "depth" },
    { MethodisOk, "isOk" },
    { Methodpixmap, "pixmap" },
    { Methodload, "load" },
    { Methodsave, "save" },
    { MethodsetFormat, "setFormat" },
    { MethodsmoothScale, "smoothScale" },
    { MethodsmoothScaleMin, "smoothScaleMin" },
    { MethodsetPixmap, "setPixmap" },
    { MethodinvertPixels, "invertPixels" },
    { Methodpixel, "pixel"},
    { MethodsetPixel, "setPixel"},
    { Methodmirror, "mirror" },
    { 0, 0 }
    };
    JSProxy::addMethods<ImageImp>(exec, methods, object);

}

KJS::Value ImageImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
    if( !JSProxy::checkType(self, JSProxy::ValueProxy, "QImage") ) return KJS::Value();
    JSValueProxy *op = JSProxy::toValueProxy( self.imp() );
    img = op->toVariant().toImage();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
//    case Methodfilename:
//        retValue = KJS::String(filename());
//        break;
//    case Methodformat:
//        retValue = KJS::String(format());
//        break;
    case Methodwidth:
        retValue = KJS::Number(width());
        break;
    case Methodheight:
        retValue = KJS::Number(height());
        break;
    case Methoddepth:
        retValue = KJS::Number(depth());
        break;
    case MethodisOk:
        retValue = KJS::Boolean(isOk());
        break;
    case Methodpixmap:
    {
	retValue = convertToValue(exec, pixmap());
        break;
    }

    case Methodload: {
            QString arg0 = extractQString( exec, args, 0);
            retValue = KJS::Boolean( load( arg0 ));
            break;
        }
    case Methodsave: {
            QString arg0 = extractQString( exec, args, 0);
            QString arg1 = extractQString( exec, args, 1);
            retValue = KJS::Boolean( save( arg0, arg1 ));
            break;
        }
    case MethodsetFormat: {
            QString arg0 = extractQString( exec, args, 0);
            setFormat( arg0 );
            break;
        }
    case MethodsmoothScale: {
            int arg0 = extractInt( exec, args, 0);
            int arg1 = extractInt( exec, args, 1);
            smoothScale(arg0, arg1);
            break;
        }
    case MethodsmoothScaleMin: {
            int arg0 = extractInt( exec, args, 0);
            int arg1 = extractInt( exec, args, 1);
            smoothScaleMin(arg0, arg1);
            break;
        }
    case MethodsetPixmap: {
            QPixmap pix = extractQPixmap(exec, args, 0);
            setPixmap(pix);
            break;
        }
    case MethodinvertPixels: {
            bool alpha = extractBool( exec, args, 0);
            img.invertPixels(alpha);
            break;
    }
    case Methodpixel: {
            int x = extractInt(exec, args, 0);
	    int y = extractInt(exec, args, 1);
	    retValue = KJS::Number( img.pixel(x, y) );
            break;
    }
    case MethodsetPixel: {
            int x = extractInt(exec, args, 0);
	    int y = extractInt(exec, args, 1);
	    uint col = extractUInt(exec, args, 2);
	    img.setPixel(x, y, col);
            break;
    }
    case Methodmirror:
    {
	if ( img.isNull())
	    break;

	bool arg0 = extractBool( exec, args, 0);
	bool arg1 = extractBool( exec, args, 1);
	img = img.mirror( arg0, arg1 );
    }
    default:
        kdWarning() << "Image has no method " << mid << endl;
        break;
    }

    op->setValue(img);
    return retValue;
}

int ImageImp::width() const {

    return !img.isNull() ? img.width() : 0;
}

int ImageImp::height() const {
    return !img.isNull() ? img.height() : 0;
}

int ImageImp::depth() const {
    return !img.isNull() ? img.depth() : 0;
}

bool ImageImp::isOk() const {
    return !img.isNull();
}

QPixmap ImageImp::pixmap() const {
    if ( img.isNull() )
        return QPixmap();

    QPixmap pix;
    pix.convertFromImage( img );
    return pix;
}

void ImageImp::setPixmap( const QPixmap &pix ) {
    if ( img.isNull() )
        img = QImage();

    img = pix.convertToImage();
}

QString ImageImp::format() const {
    return fmt;
}

QString ImageImp::filename() const {
    return nm;
}

void ImageImp::setFormat( const QString &f ) {
    fmt = f;
}

bool ImageImp::load( const QString &filename ) {

    if ( !img.isNull() )
        img = QImage();

    nm = filename;
    fmt = QImageIO::imageFormat( filename );
    bool ok = img.load( filename );
    if ( !ok ) {
    	kdWarning() << "Error loading " << filename << endl;
        img = QImage();
    }
    return ok;
}

bool ImageImp::save( const QString &filename, const QString &fmt ) {

    if ( img.isNull() ) {
	kdWarning() << "Attempt to save a null image to '" << filename << "'" << endl;
        return false;
    }

    bool ok = img.save( filename, fmt.isNull() ? "PNG" : fmt.ascii() );
    if ( ok )
        nm = filename;

    return ok;
}

void ImageImp::smoothScale( int w, int h ) {
    if ( img.isNull())
        return;
    img = img.smoothScale( w, h );
}

void ImageImp::smoothScaleMin( int w, int h ) {
    if ( img.isNull())
        return;
    img = img.smoothScale( w, h, QImage::ScaleMin );
}


} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

