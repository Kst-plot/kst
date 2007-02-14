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
#include <qimage.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsvalueproxy.h>
#include <kjsembed/jsbinding.h>
#include <qfont.h>
#include <kjsembed/global.h>

#include "painter_imp.h"

namespace KJSEmbed {
namespace Bindings {

    KJS::Object PainterLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
    {
        PainterRef * p = new PainterRef();
        /*
        QWidget *w = extractQWidget(exec, args, 0);
        QPixmap pix = extractQPixmap(exec, args, 0);
        if( w )
        {
            p->setDevice(w);
            p->setCanDelete(false);
        }
        else if( !pix.isNull() )
            p->setDevice( new QPixmap(pix) );
        */
        JSOpaqueProxy *prx = new JSOpaqueProxy( p, "Painter" );
        prx->setOwner( JSProxy::JavaScript );
        KJS::Object proxyObj( prx );
	Painter::addBindings( exec, proxyObj );
        return proxyObj;
    }

    void PainterLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
    {
        Painter::addBindings( exec, proxy );
    }
        
PainterRef::PainterRef()
{
	m_device = 0L;
	m_painter = new QPainter;
  m_canDelete = true;
}
PainterRef::~PainterRef()
{
	kdDebug() << "Painter ref going away..." << endl;
  if( m_device && m_canDelete)
		delete m_device;
	delete m_painter;
}

QPainter *PainterRef::painter()
{
	return m_painter;
}

QPaintDevice *PainterRef::device()
{
	return m_device;
}

void PainterRef::setDevice(QPaintDevice *d)
{
  if( m_device && m_canDelete)
	{
		delete m_device;
		m_device = 0L;
	}
	m_device = d;

}

void Painter::addBindings( KJS::ExecState *exec, KJS::Object &object ) {
 
    if( !JSProxy::checkType(object, JSProxy::OpaqueProxy, "Painter") ) return;
    
    JSProxy::MethodTable methods[] = {
    { Methodbegin, "begin" },
    { Methodend, "end" },
    { Methodpixmap, "pixmap" },
    { MethodsetPixmap, "setPixmap" },
    { MethodsetPen, "setPen" },
    { Methodpen, "pen" },
    { MethodtextBox, "textBox" },
    { MethodmoveTo, "moveTo" },
    { MethodlineTo, "lineTo" },
    { MethoddrawPoint, "drawPoint" },
    { MethoddrawLine, "drawLine" },
    { MethoddrawRect, "drawRect" },
    { MethoddrawRoundRect, "drawRoundRect" },
    { MethoddrawEllipse, "drawEllipse" },
    { MethoddrawText, "drawText" },
    { MethoddrawArc, "drawArc" },
    { MethoddrawPie, "drawPie" },
    { MethoddrawPixmap, "drawPixmap" },
    { MethoddrawImage, "drawImage" },
    { MethoddrawTiledPixmap, "drawTiledPixmap" },
    { Methodscale, "scale" },
    { Methodshear, "shear" },
    { Methodrotate, "rotate" },
    { Methodtranslate, "translate" },
    { MethodsetFont, "setFont" },
    { Methodfont, "font" },
    { MethodsetBrush, "setBrush" },
    { Methodbrush, "brush" },
    { MethodbackgroundColor, "backgroundColor" },
    { MethodsetBackgroundColor, "setBackgroundColor" },
    { 0, 0 }
    };

    JSProxy::addMethods<Painter>(exec, methods, object );

}

KJS::Value Painter::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
    if( !JSProxy::checkType(self, JSProxy::OpaqueProxy, "Painter") ) return KJS::Boolean(false);
    
    KJS::Value retValue = KJS::Boolean(false);
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );

    pr = op->toNative<PainterRef>();

    switch ( mid ) {
    case Methodbegin:
    {
        if ( args.size() == 1 ) {
            JSObjectProxy *objp = JSProxy::toObjectProxy(args[0].imp() );
            JSValueProxy *vp = JSProxy::toValueProxy(args[0].imp() );
            JSOpaqueProxy *op = JSProxy::toOpaqueProxy(args[0].imp() );
            if ( objp )
            {
                if( objp->widget() )
                {
                    pr->setDevice( objp->widget() );
                    pr->setCanDelete(false);
                    retValue = KJS::Boolean( begin() );
                }
                else
                {
                    QString msg = i18n( "Paintdevice was not a valid widget." );
                    throwError(exec,msg);
                    retValue =  KJS::Boolean(false);
                }
            }
            else if ( vp )
            {
                QPixmap *p = new QPixmap(vp->toVariant().toPixmap());
                if( p->isNull() )
                {
                    QString msg = i18n( "Cannot paint to a null pixmap." ).arg( op->typeName()  );
                    throwError(exec,msg);
                    retValue =  KJS::Boolean(false);
                }
                else
                {
                    pr->setDevice( p );
                    pr->setCanDelete(true);
                    retValue = KJS::Boolean( begin() );
                }
            }
            else if ( op && op->typeName() == "QPaintDevice" )
            {
                pr->setDevice( op->toNative<QPaintDevice>() );
                pr->setCanDelete(false);
                retValue = KJS::Boolean( begin() );
            }
            else
            {
                QString msg = i18n( "Paintdevice of type '%1' is not supported." ).arg( op->typeName()  );
                throwError(exec,msg);
                retValue =  KJS::Boolean(false);
            }
        }
        else
        {
            QString msg = i18n( "Incorrect number of arguments '%1'." ).arg( args.size() );
            throwError(exec,msg);
            retValue =  KJS::Boolean(false);
        }
        break;
    }
    case Methodend:
        retValue = KJS::Boolean( end() );
        break;
    case Methodpixmap:
    {
	retValue = convertToValue(exec, pixmap());
        break;
    }
    case MethodsetPixmap:
        break;
    case MethodsetPen:
    {
        QPen pn = extractQPen(exec, args, 0);
	setPen(pn);
        break;
    }
    case Methodpen:
    {
        retValue = convertToValue(exec, pen());
        break;
    }
    case MethodtextBox:
    {
        retValue = convertToValue(exec, textBox(extractQString(exec, args, 0)));
	break;
    }
    case MethodmoveTo:
    {
        moveTo(extractInt(exec, args, 0), extractInt(exec, args, 1));
	break;
    }
    case MethodlineTo:
    {
        lineTo(extractInt(exec, args, 0), extractInt(exec, args, 1));
	break;
    }
    case MethoddrawPoint:
    {
        drawPoint(extractInt(exec, args, 0), extractInt(exec, args, 1));
	break;
    }
    case MethoddrawLine:
    {
        drawLine(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3));
        break;
    }
    case MethoddrawRect:
    {
        drawRect(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3));
	break;
    }
    case MethoddrawRoundRect:
    {
        drawRoundRect(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3),extractInt(exec, args, 4), extractInt(exec, args, 5));
	break;
    }
    case MethoddrawEllipse:
    {
        drawEllipse(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3));
	break;
    }
    case MethoddrawText:
    {
        drawText(extractInt(exec, args, 0), extractInt(exec, args, 1), extractQString(exec, args, 2));
	break;
    }
    case MethoddrawArc:
    {
        drawArc(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3),extractInt(exec, args, 4), extractInt(exec, args, 5));
	break;
    }
    case MethoddrawPie:
    {
        drawPie(extractInt(exec, args, 0), extractInt(exec, args, 1),extractInt(exec, args, 2), extractInt(exec, args, 3),extractInt(exec, args, 4), extractInt(exec, args, 5));
	break;
    }
    case MethoddrawPixmap:
    {
        drawPixmap(extractInt(exec, args, 0), extractInt(exec, args, 1), extractQPixmap(exec, args, 2), extractInt(exec, args, 3), extractInt(exec, args, 4), extractInt(exec, args, 5), extractInt(exec, args, 6));
	break;
    }
    case MethoddrawImage:
    {
        drawImage( extractInt(exec, args, 0), extractInt(exec, args, 1), extractQImage(exec, args, 2), extractInt(exec, args, 3), extractInt(exec, args, 4), extractInt(exec, args, 5), extractInt(exec, args, 6), extractInt(exec, args, 7) );
	break;
    }
    case MethoddrawTiledPixmap:
    {
        drawTiledPixmap(extractInt(exec, args, 0), extractInt(exec, args, 1), extractInt(exec, args, 2), extractInt(exec, args, 3), extractQPixmap(exec, args, 4), extractInt(exec, args, 5), extractInt(exec, args, 6));
	break;
    }
    case Methodscale:
    {
        scale(extractDouble(exec, args, 0), extractDouble(exec, args, 1));
	break;
    }
    case Methodshear:
    {
        shear(extractDouble(exec, args, 0), extractDouble(exec, args, 1));
	break;
    }
    case Methodrotate:
    {
        rotate(extractDouble(exec, args, 0));
	break;
    }
    case Methodtranslate:
    {
        translate(extractDouble(exec, args, 0), extractDouble(exec, args, 1));
        break;
    }
    case MethodsetFont:
    {
        setFont(extractQFont(exec, args, 0));
        break;
    }
    case Methodfont:
    {
        retValue = convertToValue(exec, font());
	break;
    }
    case MethodsetBrush:
    {
    	pr->painter()->setBrush(extractQBrush(exec, args, 0));
        break;
    }
    case Methodbrush:
    {
        retValue = convertToValue(exec, brush());
	break;
    }
    case MethodbackgroundColor:
    {
        retValue = convertToValue(exec, backgroundColor());
	break;
    }
    case MethodsetBackgroundColor:
    {
        setBackgroundColor(extractQColor(exec, args, 0));
	break;
    }
    default:
        kdWarning() << "Painter has no method " << mid << endl;
        break;
    }

    //op->setValue(pr, "Painter");
    return retValue;
}

Painter::Painter( KJS::ExecState *exec, int id )
	: JSProxyImp(exec), mid(id),  pr(0)
{
}

Painter::~Painter()
{
}

bool Painter::begin()
{
    return pr->painter()->begin(pr->device());
}
bool Painter::begin(QPaintDevice *dev)
{
    return pr->painter()->begin(dev);
}

bool Painter::end()
{
    return pr->painter()->end();
}

QPixmap Painter::pixmap() const
{
   QPixmap *pix = dynamic_cast<QPixmap *>(pr->device());
   if( pix )
   {
	return *pix;
   }
   return QPixmap();
}

void Painter::setPen( const QPen& pn )
{
    pr->painter()->setPen(pn);
}

QPen Painter::pen() const
{
    return pr->painter()->pen();
}

QRect Painter::textBox(const QString &text)
{
	QFontMetrics fnt = pr->painter()->fontMetrics();
	return fnt.boundingRect(text);
}

void Painter::moveTo( int x, int y )
{
    pr->painter()->moveTo( x, y );
}

void Painter::lineTo( int x, int y )
{
    pr->painter()->lineTo( x, y );
}

void Painter::drawPoint( int x, int y )
{
    pr->painter()->drawPoint( x, y );
}

void Painter::drawLine( int x1, int y1, int x2, int y2 )
{
    pr->painter()->drawLine( x1, y1, x2, y2 );
}

void Painter::drawRect( int x, int y, int w, int h )
{
    pr->painter()->drawRect( x, y, w, h );
}

void Painter::drawRoundRect( int x, int y, int w, int h, int xrnd, int yrnd )
{
     pr->painter()->drawRoundRect( x, y, w, h, xrnd, yrnd );
}

void Painter::drawEllipse( int x, int y, int w, int h )
{
    pr->painter()->drawEllipse( x, y, w, h );
}

void Painter::drawText( int x, int y, const QString &txt )
{
    pr->painter()->drawText( x, y, txt );
}

void Painter::drawArc ( int x, int y, int w, int h, int a, int alen )
{
    pr->painter()->drawArc( x, y, w, h, a, alen );
}
void Painter::drawPie ( int x, int y, int w, int h, int a, int alen )
{
    pr->painter()->drawPie( x, y, w, h, a, alen);
}
void Painter::drawPixmap ( int x, int y, const QPixmap & pixmap, int sx, int sy, int sw, int sh )
{
    pr->painter()->drawPixmap(  x, y, pixmap, sx, sy, sw, sh );
}
void Painter::drawImage ( int x, int y, const QImage &image, int sx , int sy, int sw , int sh, int conversionFlags)
{
    pr->painter()->drawImage( x, y, image, sx, sy, sw, sh, conversionFlags );
}
void Painter::drawTiledPixmap ( int x, int y, int w, int h, const QPixmap & pixmap, int sx, int sy )
{
    pr->painter()->drawTiledPixmap( x, y, w, h, pixmap, sx, sy );
}

void Painter::scale ( double sx, double sy )
{
    pr->painter()->scale( sx,sy );
}
void Painter::shear ( double sh, double sv )
{
    pr->painter()->shear( sh, sv );
}
void Painter::rotate ( double a )
{
    pr->painter()->rotate( a );
}
void Painter::translate ( double dx, double dy )
{
    pr->painter()->translate( dx, dy );
}

void KJSEmbed::Bindings::Painter::setFont( const QFont & font )
{
	pr->painter()->setFont(font);
}

QFont KJSEmbed::Bindings::Painter::font( ) const
{
	return pr->painter()->font();
}

QColor KJSEmbed::Bindings::Painter::backgroundColor( ) const
{
	return pr->painter()->backgroundColor();
}

void KJSEmbed::Bindings::Painter::setBackgroundColor( const QColor & color )
{
	pr->painter()->setBackgroundColor(color);
}

} // namespace Bindings
} // namespace KJSEmbed


void KJSEmbed::Bindings::Painter::setBrush( const QColor & brush )
{
	pr->painter()->setBrush(brush);
}

QColor KJSEmbed::Bindings::Painter::brush( ) const
{
	return pr->painter()->brush().color();
}
// Local Variables:
// c-basic-offset: 4
// End:


