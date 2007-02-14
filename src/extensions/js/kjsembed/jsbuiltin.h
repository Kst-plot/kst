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

#ifndef KJSEMBED_JSBUILTIN_H
#define KJSEMBED_JSBUILTIN_H
#include "global.h"
#include <kjs/object.h>

namespace KJSEmbed {

namespace Bindings { class JSBuiltInImp; }

class KJSEmbedPart;

/**
 * Container class for the built-in objects of KJSEmbed.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSBuiltIn
{
public:
    /** Create builtin objects for the specified KJSEmbedPart. */
    JSBuiltIn( KJSEmbedPart *part );

    /** Cleans up. */
    ~JSBuiltIn();

    void add( KJS::ExecState *exec, KJS::Object &parent );

    /** Adds the global functions to the specified parent object. */
    void addGlobals( KJS::ExecState *exec, KJS::Object &parent );

    /** Returns the builtin Factory object. */
    KJS::Object builtinFactory() const { return factory; }

    /** Returns the builtin System object. */
    KJS::Object builtinSystem() const { return system; }

    /** Returns the builtin StdDialog object. */
    KJS::Object builtinStdDialog() const { return stddialog; }

    /** Returns the builtin StdAction object. */
    KJS::Object builtinStdAction() const { return stdaction; }

    /** Returns the builtin main config object */
    KJS::Object builtinConfig() const { return config; }

    /** Returns the builtin standard directories object */
    KJS::Object builtinStdDirs() const { return stddrs; }

    /** Returns the builtin icon loader object */
    KJS::Object builtinStdIcons() const { return stdicns; }

    /** Returns the builtin Qt object. */
    KJS::Object builtinQt() const { return qt; }

    /** Returns the part the builtins are using. */
    KJSEmbedPart *part() const { return jspart; }

private:
    /** Creates the objects and methods for the builtin types. */
    void init( KJS::ExecState *exec );

private:
    KJSEmbedPart *jspart;

    KJS::Object factory;
    KJS::Object system;
    KJS::Object stddialog;
    KJS::Object stdaction;
    KJS::Object config;
    KJS::Object stddrs;
    KJS::Object stdicns;
    KJS::Object qt;
    class JSBuiltInPrivate *d;
};

}

#endif // KJSEMBED_JSBUILTIN_H
