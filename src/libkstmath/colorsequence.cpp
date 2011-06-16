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

// application specific includes
#include "colorsequence.h"
#include <QVector>
#include <qapplication.h>
#include <math_kst.h>

namespace Kst {

static const QLatin1String& KstColorSequenceName = QLatin1String("KstColors");

// Default palette that is used if "Kst Colors" is not found.
static const char *const colors[] = { "red",
                                      "blue",
                                      "green",
                                      "black",
                                      "magenta",
                                      "steelblue",
                                      "#501010",
                                      "#105010"
                                      };
static const int colorcnt = sizeof(colors) / sizeof(char*);


ColorSequence *ColorSequence::_self = &ColorSequence::self();


ColorSequence& ColorSequence::self() {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  return *_self;
}


void ColorSequence::cleanup() {
    delete _self;
    _self = 0;
}


ColorSequence::ColorSequence()
: _ptr(0), _mode(Color) {
  createPalette();
}


ColorSequence::~ColorSequence() {
}


void ColorSequence::createPalette( ) {
  if (_palette != KstColorSequenceName) {
    _pal.clear();
    _palette = KstColorSequenceName;

    for (int i = 0; i < colorcnt; i++) {
      _pal.insert(i, QColor(colors[i]));
    }

    _count = _pal.count();
    _ptr = 0;
  }
}





QColor ColorSequence::next(const CurveList& curves, const QColor& badColor) {
  QColor color;
  int dark_factor;
  int ptrMin;
  int start;

  createPalette();

  QVector<int> usage(_count*2);

  for (int i = 0; i < _count*2; i++) {
    usage[i] = 0;
  }

  // check we are not already using this color, but if
  //  we are then count the number of usages of each color
  //  in the palette.
  start = _ptr;
  if (start >= _count * 2) {
    start = 0;
  }

  while (_ptr != start) {
    if (_ptr >= _count * 2) {
      _ptr = 0;
    }

    dark_factor = 100 + ( 50 * ( _ptr / _count ) );
    color = _pal.value( _ptr % _count).dark(dark_factor);

    // if we are too close to the bad color then increase the usage count
    //  to try and not use it.
    if (badColor.isValid() && colorsTooClose(color, badColor) ) {
      usage[_ptr] += 100;
    }

    for (int i = 0; i < (int)curves.count(); i++) {
      if (color == curves[i]->color()) {
        usage[_ptr]++;
      }
    }

    if (usage[_ptr] == 0) {
      break;
    }

    _ptr++;
  }

  // if we are already using this color then use the least used color for all the curves.
  if (usage[_ptr] != 0) {
    ptrMin = _ptr;

    while (_ptr != start) {
      if (_ptr >= _count * 2) {
        _ptr = 0;
      }

      if (usage[_ptr] < usage[ptrMin]) {
        ptrMin = _ptr;
      }

      _ptr++;
    }

    _ptr = ptrMin;
  }

  dark_factor = 100 + ( 50 * ( _ptr / _count ) );
  color = _pal.value( _ptr++ % _count).dark(dark_factor);

  return color;
}


QColor ColorSequence::next() {
  createPalette();

  if (_ptr >= _count * 2) {
    _ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( _ptr / _count ) );
  return _pal.value( _ptr++ % _count).dark(dark_factor);
}

QColor ColorSequence::current() {
  createPalette();

  if (_ptr >= _count * 2) {
    _ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( _ptr / _count ) );
  return _pal.value( _ptr % _count).dark(dark_factor);
}


QColor ColorSequence::next(const QColor& badColor) {
  QColor color;
  int dark_factor;

  createPalette();

  int start = _ptr;

  // find the next color in the sequence that it not too close to the bad color.
  if (badColor.isValid()) {
    do {
      if (_ptr >= _count * 2) {
        _ptr = 0;
      }
      dark_factor = 100 + ( 50 * ( _ptr / _count ) );
      color = _pal.value( _ptr++ % _count).dark(dark_factor);
    } while (colorsTooClose(color, badColor) && start != _ptr);
  }

  // if we couldn't find one then just use the next color in the sequence.
  if (start == _ptr) {
    if (_ptr >= _count * 2) {
      _ptr = 0;
    }
    dark_factor = 100 + ( 50 * ( _ptr / _count ) );
    color = _pal.value( _ptr++ % _count).dark(dark_factor);
  }

  return color;
}


bool ColorSequence::colorsTooClose(const QColor& color, const QColor& badColor) {
  double r1, h1, f1, x1, y1, z1;
  double r2, h2, f2, x2, y2, z2;
  double dc;
  int sugH, sugS, sugV;
  int badH, badS, badV;

  // make sure that the new color is not close to badColor.
  // to do this imagine HSV as defining a cone.
  // The distance from the point of the cone is R = V / 255
  // Angle of rotational symetry is Theta = H * 2PI/360
  // The 2nd angle is phi = S*(PI/4)/255
  // a color is acceptable if |C1-C2|>dcMin

  color.getHsv(&sugH,&sugS,&sugV);
  badColor.getHsv(&badH, &badS, &badV);

  r1 = badV/255.0;
  h1 = badH*M_PI/180.0;
  f1 = badS*M_PI/4.0/255.0;
  x1 = r1*sin( h1 )*sin( f1 );
  y1 = r1*cos( h1 )*sin( f1 );
  z1 = r1*cos( f1 );
  r2 = sugV/255.0;
  h2 = sugH*M_PI/180.0;
  f2 = sugS*M_PI/4.0/255.0;
  x2 = r2*sin( h2 )*sin( f2 );
  y2 = r2*cos( h2 )*sin( f2 );
  z2 = r2*cos( f2 );
  dc = sqrt( ( x1-x2 )*( x1-x2 ) + ( y1-y2 )*( y1-y2 ) + ( z1-z2 )*( z1-z2 ) );

  return dc < 0.3;
}


ColorSequence::ColorMode ColorSequence::colorMode() {
  return _mode;
}


void ColorSequence::setColorMode(ColorSequence::ColorMode mode) {
  _mode = mode;
}


int ColorSequence::count() {
  createPalette();

  return _count * 2;
}


void ColorSequence::reset() {
  _ptr = 0;
}


QColor ColorSequence::entry(int ptr) {
  createPalette();

  if (ptr >= _count * 2) {
    ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( ptr / _count ) );
  return _pal.value( ptr % _count).dark(dark_factor);
}

}
// vim: ts=2 sw=2 et
