/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _COLORSEQUENCE_H
#define _COLORSEQUENCE_H

#include <qcolor.h>
#include "curve.h"
#include "kstmath_export.h"

class KPalette;

namespace Kst {

class KSTMATH_EXPORT ColorSequence 
{
  public:
    enum ColorMode { MonoChrome, GrayScale, Color };
    void createPalette();
    static QColor next();
    static QColor current();
    static QColor next(const QColor& badColor);
    static QColor next(const CurveList& Curves, const QColor& badColor);
    static bool colorsTooClose(const QColor& color, const QColor& badColor);
    static ColorMode colorMode();
    static void setColorMode(ColorMode mode);
    static int count();
    static void reset();
    static QColor entry(int ptr);
    
  private:
    ColorSequence();
    ~ColorSequence();
    static ColorSequence* _self;
    static void cleanup();
    QHash<int, QColor> _pal;
    int _count;
    int _ptr;  // pointer to the next color
    ColorMode _mode;
    QString _palette;
};

}
#endif

// vim: ts=2 sw=2 et
