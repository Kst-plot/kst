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

// patched BSD V7 code
// http://www.bsdlover.cn/study/UnixTree/V7/usr/src/libc/gen/atof.c.html

#include "kst_atof.h"
#include "math_kst.h"

#include <math.h>
#include <ctype.h>
#include <locale.h>

#include <QLocale>
#include <QTime>
#include <QDateTime>
#include <QDebug>

#define LOGHUGE 39


//-------------------------------------------------------------------------------------------
#ifdef KST_USE_KST_ATOF
double LexicalCast::fromDouble(const char* signedp) const
{
  unsigned char* p = (unsigned char*)signedp;
  unsigned char c;
  double fl, flexp, exp5;
  double big = 72057594037927936.;  /*2^56*/
  int nd;
  int eexp, exp, neg, negexp, bexp;

  neg = 1;
  while((c = *p++) == ' ')
    ;
  if (c == '-')
    neg = -1;
  else if (c=='+')
    ;
  else
    --p;

  if (_nanMode != NullValue && c != '-' && c != '+' && c != _separator && !isDigit(c)) {
    return nanValue();
  }


  exp = 0;
  fl = 0;
  nd = 0;
  while ((c = *p++), isDigit(c)) {
    if (fl<big)
      fl = 10*fl + (c-'0');
    else
      exp++;
    nd++;
  }

  if (c == _separator) {
    while ((c = *p++), isDigit(c)) {
      if (fl<big) {
        fl = 10*fl + (c-'0');
        exp--;
      }
    nd++;
    }
  }

  negexp = 1;
  eexp = 0;
  if ((c == 'E') || (c == 'e')) {
    if ((c= *p++) == '+')
      ;
    else if (c=='-')
      negexp = -1;
    else
      --p;

    while ((c = *p++), isDigit(c)) {
      eexp = 10*eexp+(c-'0');
    }
    if (negexp<0)
      eexp = -eexp;
    exp = exp + eexp;
  }

  negexp = 1;
  if (exp<0) {
    negexp = -1;
    exp = -exp;
  }


  if((nd+exp*negexp) < -LOGHUGE){
    fl = 0;
    exp = 0;
  }
  flexp = 1;
  exp5 = 5;
  bexp = exp;
  for (;;) {
    if (exp&01)
      flexp *= exp5;
    exp >>= 1;
    if (exp==0)
      break;
    exp5 *= exp5;
  }
  if (negexp<0)
    fl /= flexp;
  else
    fl *= flexp;
  fl = ldexp(fl, negexp*bexp);
  if (neg<0)
    fl = -fl;
  _previousValue = fl;
  return(fl);
}
#endif


//-------------------------------------------------------------------------------------------
LexicalCast::AutoReset::AutoReset(bool useDot, NaNMode mode)
{
  instance().setUseDotAsDecimalSeparator(useDot);
  instance()._nanMode = mode;
}

//-------------------------------------------------------------------------------------------
LexicalCast::AutoReset::~AutoReset()
{
  instance().resetLocal();
  instance()._isFormattedTime = false;
  instance()._timeFormat.clear();
  instance()._nanMode = NullValue;
}

//-------------------------------------------------------------------------------------------
LexicalCast& LexicalCast::instance()
{
  static LexicalCast lexcInstance;
  return lexcInstance;
}

//-------------------------------------------------------------------------------------------
LexicalCast::LexicalCast() :
  _nanMode(NullValue),
  _isFormattedTime(false),
  _timeWithDate(false)
{
}

KST_THREAD_LOCAL double LexicalCast::_previousValue = 0;

//-------------------------------------------------------------------------------------------
LexicalCast::~LexicalCast() 
{
  resetLocal();
}

//-------------------------------------------------------------------------------------------
void LexicalCast::resetLocal() 
{
  if (!_originalLocal.isEmpty()) {
    setlocale(LC_NUMERIC, _originalLocal.constData());
    _originalLocal.clear();
  }
}

//-------------------------------------------------------------------------------------------
void LexicalCast::setUseDotAsDecimalSeparator(bool useDot)
{
  useDot ? _separator = '.' : _separator = ',';

  if (_separator != localSeparator()) {
    _originalLocal = QByteArray((const char*) setlocale(LC_NUMERIC, 0));
    if (useDot) {
      setlocale(LC_NUMERIC, "C");
    } else {
      setlocale(LC_NUMERIC, "de");
    }
  } else {
    resetLocal();
  }
}

//-------------------------------------------------------------------------------------------
char LexicalCast::localSeparator() const
{
  return *setlocale(LC_NUMERIC, 0);
}

//-------------------------------------------------------------------------------------------
void LexicalCast::setTimeFormat(const QString& format)
{
  _timeFormat = format.trimmed(); // remove space at start/end
  _isFormattedTime = !format.isEmpty();
  _timeWithDate = format.contains("d") || format.contains("M") || format.contains("y");
  _timeFormatLength = _timeFormat.size();
} 

//-------------------------------------------------------------------------------------------
double LexicalCast::fromTime(const char* p) const
{
  for (int i = 0; i < _timeFormatLength; i++) {
    if (*(p + i) == '\0')
      return nanValue();
  }

  const QString time = QString::fromLatin1(p, _timeFormatLength);
  double sec = nanValue();
  if (_timeWithDate) {
    QDateTime t = QDateTime::fromString(time, _timeFormat);
    if (!t.isValid()) {
      return nanValue();
    }
    t.setTimeSpec(Qt::UTC);
    sec = t.toMSecsSinceEpoch() / 1000.0;
  } else {
    const QTime t = QTime::fromString(time, _timeFormat);
    if (t.isValid())
      sec = QTime(0, 0, 0).msecsTo(t) / 1000.0;
  }
  _previousValue = sec;
  return sec;
}



// vim: ts=2 sw=2 et
