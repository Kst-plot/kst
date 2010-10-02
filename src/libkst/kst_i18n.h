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

#ifndef KST_I18N_H
#define KST_I18N_H

// #define USE_KDE_I18N

#ifdef USE_KDE_I18N
#include <klocale.h>
#else

#include <QString>

#ifndef I18N_NOOP
#define I18N_NOOP(x) x
#endif

#ifndef I18N_NOOP2
#define I18N_NOOP2(comment,x) x
#endif

//i18n
inline QString i18n (const char *text)
{ return text; }

template <typename A1>
inline QString i18n (const char *text, const A1 &a1)
{ return QString(text).arg(a1); }

template <typename A1, typename A2>
inline QString i18n (const char *text, const A1 &a1, const A2 &a2)
{ return QString(text).arg(a1, a2); }

template <typename A1, typename A2, typename A3>
inline QString i18n (const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{ return QString(text).arg(a1, a2, a3); }

//i18nc
inline QString i18nc (const char *, const char *text)
{ return i18n(text); }

template <typename A1>
inline QString i18nc (const char *, const char *text, const A1 &a1)
{ return i18n(text, a1); }

template <typename A1, typename A2>
inline QString i18nc (const char *, const char *text, const A1 &a1, const A2 &a2)
{ return i18n(text, a1, a2); }

template <typename A1, typename A2, typename A3>
inline QString i18nc (const char *, const char *text, const A1 &a1, const A2 &a2, const A3 &a3)
{ return i18n(text, a1, a2, a3); }

#endif

#endif

// vim: ts=2 sw=2 et
