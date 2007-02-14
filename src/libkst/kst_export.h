/***************************************************************************
                                kst_export.h
                             -------------------
    begin                : Feb 07 2005
    copyright            : (C) 2005 Dirk Mueller <mueller@kde.org>
    email                :
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

#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#include <kdemacros.h>

#if KDE_IS_VERSION( 3,3,90 )
/* life is great */
#else
/* workaround typo that breaks compilation with newer gcc */
#undef KDE_EXPORT
#define KDE_EXPORT
#undef KDE_NO_EXPORT
#define KDE_NO_EXPORT
#endif

#define KST_EXPORT KDE_EXPORT
#define KSTMDI_EXPORT KDE_EXPORT
#define KJSEMBED_EXPORT KDE_EXPORT
#else
/* no kdemacros */
#define KST_EXPORT
#define KSTMDI_EXPORT
#define KJSEMBED_EXPORT
#endif

#endif

// vim: ts=2 sw=2 et
