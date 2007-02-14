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

#ifndef KJSEMBED_MOVIE_IMP_H
#define KJSEMBED_MOVIE_IMP_H

#include "bindingobject.h"
#include <qmovie.h>
#include <qimage.h>

namespace KJSEmbed {
namespace Bindings {

/**
 * QObject Binding for QMovie.
 *
 * @author Ian Reinhart Geiser, geiseri@kde.org
 * @version $Id$
 */
class Movie : public BindingObject
{
    Q_OBJECT

    Q_PROPERTY( QColor setBackgroundColor READ backgroundColor )
    Q_PROPERTY( int setSpeed READ speed )

public:
    Movie( QObject *parent=0, const char *name=0 );
    virtual ~Movie();
    QColor backgroundColor() const;
    void setBackgroundColor( const QColor &c );
    int speed() const;
    void setSpeed ( int percent );

public slots:
    bool load( const QString &filename, int bufsize = 1024);
    QRect getValidRect();
    QPixmap framePixmap();
    QImage frameImage();
    bool isNull();
    int frameNumber();
    int steps();
    bool paused();
    bool finished();
    bool running();
    void unpause();
    void pause();
    void step();
    void step( int steps );
    void restart();

private:
    QMovie movie;
};

} // namespace
} // namespace

#endif // KJSEMBED_MOVIE_IMP_H
