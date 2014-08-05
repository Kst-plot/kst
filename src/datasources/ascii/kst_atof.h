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

#ifndef KST_ATOF_H
#define KST_ATOF_H

#include <QByteArray>
#include <QString>
#include <stdlib.h>

#include "math_kst.h"

#ifdef Q_CC_MSVC
  #define KST_THREAD_LOCAL __declspec(thread)
#else
 #ifndef KST_NO_THREAD_LOCAL
   #define KST_THREAD_LOCAL __thread
  #else
   #define KST_THREAD_LOCAL
  #endif
#endif

class LexicalCast
{
public:

  static LexicalCast& instance();


  enum NaNMode {
    NullValue,
    NaNValue,
    PreviousValue
  };

  struct AutoReset {
    AutoReset(bool useDot, NaNMode);
    ~AutoReset();
  };

  void setUseDotAsDecimalSeparator(bool useDot);

  char localSeparator() const;

#ifdef KST_USE_KST_ATOF
  double fromDouble(const char* p) const;
#else
  inline double fromDouble(const char* p) const { return atof(p); }
#endif
  double fromTime(const char*) const;
  inline double toDouble(const char* p) const { return _isFormattedTime ? fromTime(p) : fromDouble(p); }

  void setTimeFormat(const QString& format);
  
  double nanValue() const;

private:
  LexicalCast();
  ~LexicalCast();

  NaNMode _nanMode;
  static KST_THREAD_LOCAL double _previousValue;
  char _separator;

  QByteArray _originalLocal;
  QString _timeFormat;
  int _timeFormatLength;
  bool _isFormattedTime;
  bool _timeWithDate;

  void resetLocal();

  inline bool isDigit(const char c) const {
    return (c >= 48) && (c <= 57) ? true : false;
  }
};


//-------------------------------------------------------------------------------------------
inline double LexicalCast::nanValue() const
{
  switch (_nanMode) {
  case NullValue:     return 0;
  case NaNValue:      return Kst::NOPOINT;
  case PreviousValue: return _previousValue;
  default:            return 0;
  }
}


#endif

// vim: ts=2 sw=2 et
