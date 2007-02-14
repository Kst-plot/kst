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

#ifndef KJSEMBED_PIXMAP_IMP_H
#define KJSEMBED_PIXMAP_IMP_H

#include <kjsembed/jsobjectproxy_imp.h>

#include <qpixmap.h>

namespace KJSEmbed {
namespace Bindings {

/**
 * QObject Binding for QPixmap.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */
class Pixmap : public JSProxyImp
{

    /** Identifiers for the methods provided by this class. */
    enum MethodId { MethodisNull, Methodwidth, Methodheight, Methodsize, Methodrect, Methoddepth, Methodresize, Methodfill, Methodmask, MethodsetMask, MethodcreateHeuristicMask, MethodgrabWindow };

public:
    Pixmap( KJS::ExecState *exec, int id  );
    virtual ~Pixmap();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );



private:
    bool isNull() const { return pix.isNull(); }

    int width() const { return pix.width(); }
    int height() const { return pix.height(); }
    QSize size() const { return pix.size(); }
    QRect rect() const { return pix.rect(); }

    int depth() const { return pix.depth(); }

    void resize( int w, int h );
    void resize( const QSize &size );
    void fill( const QColor &color );

    QPixmap mask();
    void setMask(const QPixmap& mask);
    QPixmap createHeuristicMask ( bool clipTight );

    // Image Capture
    void grabWindow(int winID, int x =0, int y = 0, int w = -1, int h = -1);

private:
    int mid;
    QPixmap pix;
};

} // namespace
} // namespace

#endif // KJSEMBED_PIXMAP_IMP_H
