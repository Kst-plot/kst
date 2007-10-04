/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
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
#include "kstvcurve.h"
#include "kst_export.h"

class KPalette;

namespace Kst {

class ColorSequence {
  public:
    enum ColorMode { MonoChrome, GrayScale, Color };
    KST_EXPORT void createPalette();
    static KST_EXPORT QColor next();
    static KST_EXPORT QColor next(const QColor& badColor);
    static KST_EXPORT QColor next(const KstVCurveList& Curves, const QColor& badColor);
    static KST_EXPORT bool colorsTooClose(const QColor& color, const QColor& badColor);
    static KST_EXPORT ColorMode colorMode();
    static KST_EXPORT void setColorMode(ColorMode mode);
    static KST_EXPORT int count();
    static KST_EXPORT void reset();
    static KST_EXPORT QColor entry(int ptr);
    
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
