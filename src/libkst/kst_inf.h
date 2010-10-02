/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KST_INF_H
#define KST_INF_H

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __sun
#include <ieeefp.h>
#endif

#if !defined(INF)
#include <qconfig.h>
static double inf__()
{
  /* work around some strict alignment requirements
     for double variables on some architectures (e.g. PA-RISC) */
  typedef union { unsigned char b[8]; double d; } kjs_double_t;
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
  static const kjs_double_t Inf_Bytes = { { 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 } };
#elif defined(arm)
  static const kjs_double_t Inf_Bytes = { { 0, 0, 0xf0, 0x7f, 0, 0, 0, 0 } };
#else
  static const kjs_double_t Inf_Bytes = { { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f } };
#endif

  const double Inf = Inf_Bytes.d;
  return Inf;
}
#define INF (::inf__())
#endif

#endif // KST_INF_H
