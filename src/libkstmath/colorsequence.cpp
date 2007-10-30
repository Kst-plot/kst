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

// application specific includes
#include "colorsequence.h"
#include "settings.h"
#include <QVector>
#include <qapplication.h>
#include <math_kst.h>

namespace Kst {

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


ColorSequence::ColorSequence()
: _ptr(0), _mode(Color) {
  createPalette();
}


ColorSequence::~ColorSequence() {
}


ColorSequence *ColorSequence::_self = 0L;


void ColorSequence::createPalette( ) {
  if (_palette != Settings::globalSettings()->curveColorSequencePalette) {
    _pal.clear();
    _palette = Settings::globalSettings()->curveColorSequencePalette;

    for (int i = 0; i < colorcnt; i++) {
      _pal.insert(i, QColor(colors[i]));
    }

    _count = _pal.count();
    _ptr = 0;
  }
}


void ColorSequence::cleanup() {
    delete _self;
    _self = 0;
}


QColor ColorSequence::next(const CurveList& curves, const QColor& badColor) {
  QColor color;
  int dark_factor;
  int ptrMin;
  int start;

  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  _self->createPalette();

  QVector<int> usage(_self->_count*2);

  for (int i = 0; i < _self->_count*2; i++) {
    usage[i] = 0;
  }

  // check we are not already using this color, but if
  //  we are then count the number of usages of each color
  //  in the palette.
  start = _self->_ptr;
  if (start >= _self->_count * 2) {
    start = 0;
  }

  while (_self->_ptr != start) {
    if (_self->_ptr >= _self->_count * 2) {
      _self->_ptr = 0;
    }

    dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
    color = _self->_pal.value( _self->_ptr % _self->_count).dark(dark_factor);

    // if we are too close to the bad color then increase the usage count
    //  to try and not use it.
    if (badColor.isValid() && colorsTooClose(color, badColor) ) {
      usage[_self->_ptr] += 100;
    }

    for (int i = 0; i < (int)curves.count(); i++) {
      if (color == curves[i]->color()) {
        usage[_self->_ptr]++;
      }
    }

    if (usage[_self->_ptr] == 0) {
      break;
    }

    _self->_ptr++;
  }

  // if we are already using this color then use the least used color for all the curves.
  if (usage[_self->_ptr] != 0) {
    ptrMin = _self->_ptr;

    while (_self->_ptr != start) {
      if (_self->_ptr >= _self->_count * 2) {
        _self->_ptr = 0;
      }

      if (usage[_self->_ptr] < usage[ptrMin]) {
        ptrMin = _self->_ptr;
      }

      _self->_ptr++;
    }

    _self->_ptr = ptrMin;
  }

  dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
  color = _self->_pal.value( _self->_ptr++ % _self->_count).dark(dark_factor);

  return color;
}


QColor ColorSequence::next() {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  _self->createPalette();

  if (_self->_ptr >= _self->_count * 2) {
    _self->_ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
  return _self->_pal.value( _self->_ptr++ % _self->_count).dark(dark_factor);
}


QColor ColorSequence::next(const QColor& badColor) {
  QColor color;
  int dark_factor;

  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  _self->createPalette();

  int start = _self->_ptr;

  // find the next color in the sequence that it not too close to the bad color.
  if (badColor.isValid()) {
    do {
      if (_self->_ptr >= _self->_count * 2) {
        _self->_ptr = 0;
      }
      dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
      color = _self->_pal.value( _self->_ptr++ % _self->_count).dark(dark_factor);
    } while (colorsTooClose(color, badColor) && start != _self->_ptr);
  }

  // if we couldn't find one then just use the next color in the sequence.
  if (start == _self->_ptr) {
    if (_self->_ptr >= _self->_count * 2) {
      _self->_ptr = 0;
    }
    dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
    color = _self->_pal.value( _self->_ptr++ % _self->_count).dark(dark_factor);
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
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }

  return _self->_mode;
}


void ColorSequence::setColorMode(ColorSequence::ColorMode mode) {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }

  _self->_mode = mode;
}


int ColorSequence::count() {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  _self->createPalette();

  return _self->_count * 2;
}


void ColorSequence::reset() {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }

  _self->_ptr = 0;
}


QColor ColorSequence::entry(int ptr) {
  if (!_self) {
    _self = new ColorSequence;
    qAddPostRoutine(ColorSequence::cleanup);
  }
  _self->createPalette();

  if (ptr >= _self->_count * 2) {
    ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( ptr / _self->_count ) );
  return _self->_pal.value( ptr % _self->_count).dark(dark_factor);
}

}
// vim: ts=2 sw=2 et
