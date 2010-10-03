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
#ifndef KST_WIDGETS_EXPORT_H
#define KST_WIDGETS_EXPORT_H

#include <qglobal.h>

#if (defined(Q_OS_WIN32) || defined(Q_OS_WIN64))
#  if defined(BUILD_KSTWIDGETS)
#    define KSTWIDGETS_EXPORT Q_DECL_EXPORT
#  else
#    define KSTWIDGETS_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define KSTWIDGETS_EXPORT
#endif

#endif

// vim: ts=2 sw=2 et
