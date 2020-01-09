/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// NOTE: only include this from .cpp files

#ifndef MATH_KST_H
#define MATH_KST_H

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __sun
#include <ieeefp.h>
#endif

#include "kst_export.h"

#ifdef _MSC_VER
inline double pow(float a, double b) { return pow((double)a, b); }
inline double pow(int a, qreal b) { return pow((double)a, (double)b); }
#endif

#if defined(__OpenBSD__) && !defined(__clang__)
#include <cmath>
using std::isnan;
using std::isinf;
using std::isfinite;
#endif

namespace Kst {
/*
** For systems without NAN, this is a NAN in IEEE double format.
** Code from KDE's kjs libarary.
*/
#if !defined(NAN)
#include <qconfig.h>
static double nan__()
{
  /* work around some strict alignment requirements
     for double variables on some architectures (e.g. PA-RISC) */
  typedef union { unsigned char b[8]; double d; } kjs_double_t;
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
  static const kjs_double_t NaN_Bytes = { { 0x7f, 0xf8, 0, 0, 0, 0, 0, 0 } };
#elif defined(arm)
  static const kjs_double_t NaN_Bytes = { { 0, 0, 0xf8, 0x7f, 0, 0, 0, 0 } };
#else
  static const kjs_double_t NaN_Bytes = { { 0, 0, 0, 0, 0, 0, 0xf8, 0x7f } };
#endif

  const double NaN = NaN_Bytes.d;
  return NaN;
}
#define NAN (Kst::nan__())
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
** Both Solaris and FreeBSD-current do weird things with the
** isnan() defined in math.h - in particular on FreeBSD it
** gets #undeffed by the C++ math routines later. Use the
** std:: version in those cases.
*/
#ifdef isnan
#define KST_ISNAN(a)    isnan(a)
#elif defined(__APPLE__)
#define KST_ISNAN(a)    (a == NAN ? 1 : 0)
#else
  // HUH? Ok let's get rid of the silliness here sometime.
#define KST_ISNAN(a)    isnan(a)
#endif


KSTCORE_EXPORT extern const double NOPOINT;

inline int d2i(double x) {
  return int(floor(x+0.5));
}

inline bool samePixel(double x1, double x2) {
  //return (int(floor(x1)) == int(floor(x2)));
  return (int(x1) == int(x2));
}

#if defined(__SVR4) && defined(__sun)
inline int isinf(double x) { return x == x && !finite(x); }
#endif

#ifdef Q_CC_MSVC
#ifndef isnan
#define isnan _isnan
#endif
#ifndef finite
#define finite _finite
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323
#endif
#ifndef isinf
#define isinf !_finite
#endif
template<class T>
bool isfinite(T value) { return !isinf(value); }
#endif

#if 0
#define isnan _isnan
#define finite _finite
#ifndef M_PI
#define M_PI 3.14159265358979323
#endif
#define isinf !_finite
#endif

inline double logXLo(double x, double base = 10.0) {
  if (base == 10.0) {
    return x > 0.0 ? log10(x) : -350.0;
  } else {
    return x > 0.0 ? log10(x)/log10(base) : -350.0;
  }
}

inline double logXHi(double x, double base = 10.0) {
  if (base == 10.0) {
    return x > 0.0 ? log10(x) : -340.0;
  } else {
    return x > 0.0 ? log10(x)/log10(base) : -340.0;
  }
}

inline double logYLo(double x, double base = 10.0) {
  if (base == 10.0) {
    return x > 0.0 ? log10(x) : -350.0;
  } else {
    return x > 0.0 ? log10(x)/log10(base) : -350.0;
  }
}

inline double logYHi(double x, double base = 10.0) {
  if (base == 10.0) {
    return x > 0.0 ? log10(x) : -340.0;
  } else {
    return x > 0.0 ? log10(x)/log10(base) : -340.0;
  }
}

inline double roundDouble (double x) {
#ifdef rint
  return rint(x);
#else
  int i = (int) x;
  if (x >= 0.0) {
    return ((x-i) >= 0.5) ? (i + 1) : (i);
  } else {
    return (-x+i >= 0.5) ? (i - 1) : (i);
  }
#endif
}
}
#endif

// vim: ts=2 sw=2 et
