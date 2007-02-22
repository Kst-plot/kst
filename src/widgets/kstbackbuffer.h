/***************************************************************************
                               kstbackbuffer.h
                             -------------------
    begin                : Apr 12, 2004
    copyright            : (C) 2004 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTBACKBUFFER_H
#define KSTBACKBUFFER_H

#include <qpainter.h>
#include <qpixmap.h>

class KstBackBuffer {
  public:
    KstBackBuffer();
    ~KstBackBuffer();

    QSize size() const { return _buffer.size(); }
    QPixmap& buffer();
    void paintInto(QPainter& p, const QRect& geom);
    void paintInto(QPainter& p, const QRect& geom, const QPoint& from);

  private:
    QPixmap _buffer;
};


#endif
// vim: ts=2 sw=2 et
