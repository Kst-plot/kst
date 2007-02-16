/***************************************************************************
                      kstcolorsequence.cpp  -  Part of KST
                             -------------------
    begin                : Mon Jul 07 2003
    copyright            : (C) 2003 The University of Toronto
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

#include <math.h>

// include files for Qt

// include files for KDE
#include <kpalette.h>

// application specific includes
#include "kstcolorsequence.h"
#include "kstsettings.h"
//Added by qt3to4:
#include <Q3MemArray>

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
static KStaticDeleter<KstColorSequence> sdColorSequence;


KstColorSequence::KstColorSequence()
: _ptr(0), _mode(Color) {
  _pal = 0L;

  createPalette();
}


KstColorSequence::~KstColorSequence() {
  delete _pal;
  _pal = 0L;
}


KstColorSequence *KstColorSequence::_self = 0L;


void KstColorSequence::createPalette( ) {
  if (_palette != KstSettings::globalSettings()->curveColorSequencePalette) {
    delete _pal;  
    _palette = KstSettings::globalSettings()->curveColorSequencePalette;
    _pal = new KPalette(_palette);

    if (_pal->nrColors() <= 0) {
      for (int i = 0; i < colorcnt; i++) {
        _pal->addColor(QColor(colors[i]));
      }
    }

    _count = _pal->nrColors();
    _ptr = 0;
  }
}


QColor KstColorSequence::next(const KstVCurveList& curves, const QColor& badColor) {
  QColor color;
  int dark_factor;
  int ptrMin;
  int start;

  if (!_self) {
    _self = sdColorSequence.setObject(_self, new KstColorSequence);
  }
  _self->createPalette();
  
  Q3MemArray<int> usage(_self->_count*2);
  
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
    color = _self->_pal->color( _self->_ptr % _self->_count).dark(dark_factor);

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
  color = _self->_pal->color( _self->_ptr++ % _self->_count).dark(dark_factor);

  return color;
}


QColor KstColorSequence::next() {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }
  _self->createPalette();

  if (_self->_ptr >= _self->_count * 2) {
    _self->_ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
  return _self->_pal->color( _self->_ptr++ % _self->_count).dark(dark_factor);
}


QColor KstColorSequence::next(const QColor& badColor) {
  QColor color;
  int dark_factor;

  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
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
      color = _self->_pal->color( _self->_ptr++ % _self->_count).dark(dark_factor);
    } while (colorsTooClose(color, badColor) && start != _self->_ptr);
  }

  // if we couldn't find one then just use the next color in the sequence.
  if (start == _self->_ptr) {
    if (_self->_ptr >= _self->_count * 2) {
      _self->_ptr = 0;
    }
    dark_factor = 100 + ( 50 * ( _self->_ptr / _self->_count ) );
    color = _self->_pal->color( _self->_ptr++ % _self->_count).dark(dark_factor);
  }

  return color;
}


bool KstColorSequence::colorsTooClose(const QColor& color, const QColor& badColor) {
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

  color.getHsv(sugH,sugS,sugV);
  badColor.getHsv(badH, badS, badV);

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


KstColorSequence::ColorMode KstColorSequence::colorMode() {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }

  return _self->_mode;
}


void KstColorSequence::setColorMode(KstColorSequence::ColorMode mode) {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }

  _self->_mode = mode;
}


int KstColorSequence::count() {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }
  _self->createPalette();

  return _self->_count * 2;
}


void KstColorSequence::reset() {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }

  _self->_ptr = 0;
}


QColor KstColorSequence::entry(int ptr) {
  if (!_self) {
    sdColorSequence.setObject(_self, new KstColorSequence);
  }
  _self->createPalette();

  if (ptr >= _self->_count * 2) {
    ptr = 0;
  }

  int dark_factor = 100 + ( 50 * ( ptr / _self->_count ) );
  return _self->_pal->color( ptr % _self->_count).dark(dark_factor);
}


// vim: ts=2 sw=2 et
