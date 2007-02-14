// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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

#ifndef KJSEMBED_RESOURCES_H
#define KJSEMBED_RESOURCES_H

#include <qpixmap.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

namespace KJSEmbed {
namespace BuiltIns {

/**
 * KJS binding for KStandardDirs.
 *
 * @author Ian Reinhart Geiser geiseri@kde.org
 */
class StdDirsImp : public JSProxyImp
{
	
public:
	/** Identifiers for the various methods. */
    enum DirMethods { 
    	findResource, addResourceType, kde_default, 
    	addResourceDir, findResourceDir, saveLocation, 
			makeDir, exists, realPath, currentDirPath,
			homeDirPath, applicationDirPath  };
    StdDirsImp( KJS::ExecState *exec, uint id );
    virtual ~StdDirsImp();

    /** Adds the StdActions object to the specified parent. */
    static void addBindings( KJS::ExecState *exec, KJS::Object &parent );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    uint id;
    JSFactory *jsfact;
};


/**
 * KJS binding for KIconLoader.
 *
 * @author Ian Reinhart Geiser geiseri@kde.org
 */
class StdIconsImp : public JSProxyImp
{
public:
    enum IconMethods { BarIcon, DesktopIcon, SmallIcon, MainBarIcon, UserIcon, MimeSourceIcon };
    StdIconsImp(JSFactory *fac,  KJS::ExecState *exec, uint id );
    virtual ~StdIconsImp();

    /** Adds the StdActions object to the specified parent. */
    static void addBindings( JSFactory *fac, KJS::ExecState *exec, KJS::Object &parent );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
	/** wrap a QPixmap **/
    KJS::Value wrapPixmap(KJS::ExecState *exec,const QPixmap &icon);
    uint id;
    JSFactory *jsfact;
};

} // namespace BuiltIns
} // namespace KJSEmbed

#endif // KJSEMBED_RESOURCES_H
