// -*- c++ -*-

/*
 *  Copyright (C) 2001-2003, Richard J. Moore <rich@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KJSEMBEDJSCONSOLEPLUGIN_H
#define KJSEMBEDJSCONSOLEPLUGIN_H

#include <kparts/plugin.h>

class KJSEmbedPart;

namespace KJSEmbed {

/**
 * A KParts::Plugin that provides a console for executing Javascript commands.
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
class JSConsolePlugin : public KParts::Plugin
{
    Q_OBJECT

public:
    JSConsolePlugin( QObject *parent, const char *name, const QStringList & );
    ~JSConsolePlugin();

private slots:
    void showConsole();

private:
    void init();

    KJSEmbedPart *js;
    class Private *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSCONSOLEPLUGIN_H

// Local Variables:
// c-basic-offset: 4
// End:
