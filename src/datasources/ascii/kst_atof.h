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
#include <QString>
#include <stdlib.h>


class LexicalCast
{
public:
  static LexicalCast& instance();
  
  struct AutoReset {
    AutoReset(bool useDot);
    ~AutoReset();
  };
  
  // use second parameter when useDot is false
  void setDecimalSeparator(bool useDot);

  char localSeparator() const;

#ifdef KST_USE_KST_ATOF
  double fromDouble(const char* p) const;
#else
  inline double fromDouble(const char* p) const { return atof(p); }
#endif
  double fromTime(const char*) const;
  inline double toDouble(const char* p) const { return _isTime ? fromTime(p) : fromDouble(p); }

  void setTimeFormat(const QString& format);
  
private:
  LexicalCast();
  ~LexicalCast();

  char _separator;
  QByteArray _originalLocal;
  QString _timeFormat;
  bool _isTime;
  bool _timeWithDate;
  bool _timeInTwoColumns;

  void resetLocal();

  inline bool isDigit(const char c) const {
    return (c >= 48) && (c <= 57) ? true : false;
  }

};


#endif

// vim: ts=2 sw=2 et
