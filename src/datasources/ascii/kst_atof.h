#ifndef  KST_ATOF_H
#define KST_ATOF_H

#include <QByteArray>
#include <math.h>


struct LexicalCast
{
  LexicalCast();
  ~LexicalCast();

  // use second parameter when useDot is false
  void setDecimalSeparator(bool useDot, char separator);

#ifdef KST_USE_KST_ATOF
  double toDouble(const char* p) const;
#else
  inline double toDouble(const char* p) const { return atof(p); }
#endif

private:
  bool _useDot;  
  char _separator;
  QByteArray _originalLocal;
};




#endif

// vim: ts=2 sw=2 et
