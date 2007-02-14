// -*- c++ -*-
/***************************************************************************
 *   Copyright (C) 2004 by Richard Moore                                   *
 *   rich@kde.org                                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QTSTUBS_H
#define QTSTUBS_H
#include "global.h"
#ifdef QT_ONLY

#include <qobject.h>
#include <qurl.h>

class QWidget;

typedef QUrl KURL;

namespace KParts {

class KJSEMBED_EXPORT ReadOnlyPart : public QObject
{
   Q_OBJECT

public:
   ReadOnlyPart( QObject *parent, const char *name=0 );
   virtual ~ReadOnlyPart();

   void setWidget( QWidget *w ) { m_widget = w; }

private:
   QWidget *m_widget;
};

class KJSEMBED_EXPORT ReadWritePart : public ReadOnlyPart
{
   Q_OBJECT

public:
   ReadWritePart( QObject *parent, const char *name=0 );
   virtual ~ReadWritePart();
};

} // namespace KParts

namespace KJSEmbed {

class KJSEMBED_EXPORT XMLActionClientStub
{
public:
   XMLActionClientStub() {}
   ~XMLActionClientStub() {}
};

typedef XMLActionClientStub XMLActionClient;

} // namespace KJSEmbed

#endif // QT_ONLY

#endif // QTSTUBS_H
