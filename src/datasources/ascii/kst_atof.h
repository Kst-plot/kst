/***************************************************************************
 *                                                                         *
 *   Copyright : (C) 2010 The University of Toronto                        *
 *   email     : netterfield@astro.utoronto.ca                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef  KST_ATOF_H
#define KST_ATOF_H

#include <QByteArray>
#include <stdlib.h>


struct LexicalCast
{
  LexicalCast();
  ~LexicalCast();

  char localSeparator() const;

  // use second parameter when useDot is false
  void setDecimalSeparator(bool useDot);

#ifdef KST_USE_KST_ATOF
  double toDouble(const char* p) const;
#else
  inline double toDouble(const char* p) const { return atof(p); }
#endif

private:
  char _separator;
  QByteArray _originalLocal;
  void resetLocal();
  bool isDigit(const char) const;
};




#endif

// vim: ts=2 sw=2 et
