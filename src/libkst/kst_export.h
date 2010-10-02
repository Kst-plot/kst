/***************************************************************************
                                kst_export.h
                             -------------------
    begin                : Feb 07 2005
    copyright            : (C) 2005 Dirk Mueller <mueller@kde.org>
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KST_EXPORT_H
#define KST_EXPORT_H

#include <qglobal.h>

#if (defined(Q_OS_WIN32) || defined(Q_OS_WIN64))
#  if defined(BUILD_KSTCORE)
#    define KSTCORE_EXPORT Q_DECL_EXPORT
#  else
#    define KSTCORE_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define KSTCORE_EXPORT
#endif

#endif

// vim: ts=2 sw=2 et
