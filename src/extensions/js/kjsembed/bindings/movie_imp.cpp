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

#include <qpixmap.h>
#include <kjsembed/global.h>
#include "movie_imp.h"


namespace KJSEmbed {
namespace Bindings {

Movie::Movie( QObject *parent, const char *name ) : BindingObject(parent, name)
{
	movie = QMovie();
}
Movie::~Movie()
{

}
QColor Movie::backgroundColor() const
{
	if( movie.isNull() )
		return QColor();
	return movie.backgroundColor();
}
void Movie::setBackgroundColor( const QColor &c )
{
	if( movie.isNull() )
		return;
	movie.setBackgroundColor(c);
}
int Movie::speed() const
{
	if( movie.isNull() )
		return 0;
	return movie.speed();
}
void Movie::setSpeed( int percent )
{
	if( movie.isNull() )
		return;
	movie.setSpeed(percent);
}
bool Movie::load( const QString &filename, int bufsize)
{
	movie = QMovie( filename, bufsize);
	if ( movie.isNull() )
		return false;
	else
		return true;
}
QRect Movie::getValidRect()
{
	if( movie.isNull() )
		return QRect();
	return movie.getValidRect();
}
QPixmap Movie::framePixmap()
{
	if( movie.isNull() )
		return QPixmap();
	return movie.framePixmap();
}
QImage Movie::frameImage()
{
	if( movie.isNull() )
		return QImage();
	return movie.frameImage();
}
bool Movie::isNull()
{
	return movie.isNull();
}
int Movie::frameNumber()
{
	if( movie.isNull() )
		return 0;
	return movie.frameNumber();
}
int Movie::steps()
{
	if( movie.isNull() )
		return 0;
	return movie.steps();
}
bool Movie::paused()
{
	if( movie.isNull() )
		return false;
	return movie.paused();
}
bool Movie::finished()
{
	if( movie.isNull() )
		return false;
	return movie.finished();
}
bool Movie::running()
{
	if( movie.isNull() )
		return false;
	return movie.running();
}
void Movie::unpause()
{
	if( movie.isNull() )
		return;
	movie.unpause();
}
void Movie::pause()
{
	if( movie.isNull() )
		return;
	movie.pause();
}
void Movie::step()
{
	if( movie.isNull() )
		return;
	movie.step();
}
void Movie::step( int steps )
{
	if( movie.isNull() )
		return;
	movie.step(steps);
}
void Movie::restart()
{
	if( movie.isNull() )
		return;
	movie.restart();
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#ifndef QT_ONLY
#include "movie_imp.moc"
#endif
