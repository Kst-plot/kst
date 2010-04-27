#ifndef KST_MATH_EXPORT_H
#define KST_MATH_EXPORT_H

#include <qglobal.h>

#if (defined(Q_OS_WIN32) || defined(Q_OS_WIN64))
#  if defined(BUILD_KSTMATH)
#    define KSTMATH_EXPORT Q_DECL_EXPORT
#  else
#    define KSTMATH_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define KSTMATH_EXPORT
#endif

#endif

// vim: ts=2 sw=2 et
