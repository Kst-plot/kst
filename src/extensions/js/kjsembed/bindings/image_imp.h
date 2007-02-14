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

#ifndef KJSEMBED_IMAGE_IMP_H
#define KJSEMBED_IMAGE_IMP_H

#include <kjsembed/jsobjectproxy_imp.h>

#include <qimage.h>
#include <qpixmap.h>


namespace KJSEmbed {
namespace Bindings {

/**
 * Methods for the QImage binding.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */

class ImageImp : public JSProxyImp {

    /** Identifiers for the methods provided by this class. */
    enum MethodId {
	/*Methodfilename, Methodformat,*/Methodwidth, Methodheight, Methoddepth,
	MethodisOk, Methodpixmap, Methodload, Methodsave,MethodsetFormat,
	MethodsmoothScale, MethodsmoothScaleMin, MethodsetPixmap,  MethodinvertPixels,
	Methodpixel, MethodsetPixel, Methodmirror
    };


public:
    ImageImp( KJS::ExecState *exec, int id );
    virtual ~ImageImp();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const {
        return true;
    }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );


private:

    QString filename() const;
    QString format() const;
    int width() const;
    int height() const;
    int depth() const;

    bool isOk() const;
    QPixmap pixmap() const;

    bool load( const QString &filename );
    bool save( const QString &filename, const QString &fmt );
    void setFormat( const QString &fmt );
    void smoothScale( int w, int h );
    void smoothScaleMin( int w, int h );
    void setPixmap( const QPixmap &pix );


    int mid;
    QImage img; // this is a temp!!!
    QString nm;
    QString fmt;
};

} // namespace
} // namespace

#endif // KJSEMBED_IMAGE_IMP_H
