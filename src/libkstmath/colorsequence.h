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

#ifndef COLORSEQUENCE_H
#define COLORSEQUENCE_H

#include "kstmath_export.h"

#include "nextcolor.h"
#include "curve.h"


class KPalette;

namespace Kst {

class KSTMATH_EXPORT ColorSequence : public NextColor
{
  public:
    enum ColorMode { MonoChrome, GrayScale, Color };
    void createPalette();
    QColor next();
    QColor current();
    QColor next(const QColor& badColor);
    QColor next(const CurveList& Curves, const QColor& badColor);
    bool colorsTooClose(const QColor& color, const QColor& badColor);
    ColorMode colorMode();
    void setColorMode(ColorMode mode);
    int count();
    void reset();
    QColor entry(int ptr);

    static ColorSequence& self();
    
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
