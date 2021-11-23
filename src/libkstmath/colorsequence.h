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
    QColor next();
    QColor current();
    QColor entry(int ptr);

    static ColorSequence& self();
    
  private:
    ColorSequence();
    ~ColorSequence();

    static ColorSequence* _self;
    static void cleanup();
    QVector<QColor> _colors;

    int _ptr;  // pointer to the next color
    int _count;
};

}
#endif

// vim: ts=2 sw=2 et
