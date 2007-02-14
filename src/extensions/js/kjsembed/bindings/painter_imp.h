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

#ifndef KJSEMBED_PAINTER_IMP_H
#define KJSEMBED_PAINTER_IMP_H

#include <kjsembed/jsbindingbase.h>
#include <kjsembed/jsproxy_imp.h>
#include <qpen.h>
#include <qpainter.h>

class QPixmap;

namespace KJSEmbed {
namespace Bindings {

    class PainterLoader : public JSBindingBase
    {
        public:
            KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
            void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
    };

    
class PainterRef
{
	public:
		PainterRef();
		~PainterRef();

		QPainter *painter();

		QPaintDevice *device();
		void setDevice(QPaintDevice *d);

    void setCanDelete(bool can ) {m_canDelete = can;}
    bool canDelete() const { return m_canDelete;}
    
	private:
		QPainter *m_painter;
		QPaintDevice *m_device;
    bool m_canDelete;
};

/**
 * JSProxyImp binding for painting on a QPixmap.
 *
 * @author Richard Moore, rich@kde.org
 */

class Painter : public JSProxyImp
{
	/** Identifiers for the methods provided by this class. */
	enum MethodId {Methodbegin, Methodend, Methodpixmap, MethodsetPixmap, MethodsetPen, Methodpen, MethodtextBox, MethodmoveTo, MethodlineTo, MethoddrawPoint,  MethoddrawLine, MethoddrawRect,  MethoddrawRoundRect, MethoddrawEllipse, MethoddrawText, MethoddrawArc, MethoddrawPie, MethoddrawPixmap, MethoddrawImage, MethoddrawTiledPixmap, Methodscale, Methodshear, Methodrotate, Methodtranslate, MethodsetFont, Methodfont, MethodsetBrush, Methodbrush, MethodbackgroundColor, MethodsetBackgroundColor};
public:
    Painter( KJS::ExecState *exec, int id );
    virtual ~Painter();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
private:

    bool begin();
    bool begin(QPaintDevice *dev);
    bool end();
    QPixmap pixmap() const;
    void setPen( const QPen& pn);
    QPen pen() const;
    QRect textBox(const QString &text);
    void moveTo( int x, int y );
    void lineTo( int x, int y );
    void drawPoint( int x, int y );
    void drawLine( int x1, int y1, int x2, int y2 );
    void drawRect( int x, int y, int w, int h );
    void drawRoundRect( int x, int y, int w, int h, int, int);
    void drawEllipse( int x, int y, int w, int h );
    void drawText( int x, int y, const QString &txt );
    void drawArc( int x, int y, int w, int h, int a, int alen );
    void drawPie( int x, int y, int w, int h, int a, int alen );
    void drawPixmap ( int x, int y, const QPixmap &pixmap, int sx, int sy, int sw, int sh );
    void drawImage ( int x, int y, const QImage &image, int sx, int sy, int sw, int sh, int conversionFlags );
    void drawTiledPixmap ( int x, int y, int w, int h, const QPixmap &pixmap, int sx, int sy);
    void scale ( double sx, double sy );
    void shear ( double sh, double sv );
    void rotate ( double a );
    void translate ( double dx, double dy );
    void setFont( const QFont &font);
    QFont font() const;
    void setBrush( const QColor &brush );
    QColor brush() const;
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

private:
    int mid;
    PainterRef *pr;  // temps now

};

} // namespace Bindings
} // namespace KJSEmbed

#endif // KJSEMBED_PAINTER_IMP_H

