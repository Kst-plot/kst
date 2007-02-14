/*
 *  Copyright (C) 2003, Richard J. Moore <rich@kde.org>
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

#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kdebug.h>
#include <jsproxy.h>
#include <jsbinding.h>
#include <jsfactory.h>
#include <qdir.h>
#include <qapplication.h>
#include "resources.h"

namespace KJSEmbed {
namespace BuiltIns {


StdDirsImp::StdDirsImp( KJS::ExecState *exec, uint mid )
    : JSProxyImp(exec), id(mid)
{

}

StdDirsImp::~StdDirsImp() {}

void StdDirsImp::addBindings( KJS::ExecState *exec, KJS::Object &parent )
{

    JSProxy::MethodTable methods[] = {
    			{ StdDirsImp::findResource, "findResource" },
			{ StdDirsImp::addResourceType, "addResourceType" },
			{ StdDirsImp::kde_default, "kde_default" },
			{ StdDirsImp::addResourceDir, "addResourceDir" },
			{ StdDirsImp::findResourceDir, "findResourceDir" },
			{ StdDirsImp::saveLocation, "saveLocation" },
			{ StdDirsImp::makeDir, "makeDir" },
			{ StdDirsImp::exists, "exists" },
			{ StdDirsImp::realPath, "realPath" },
			{ StdDirsImp::currentDirPath, "currentDirPath" },
			{ StdDirsImp::homeDirPath, "homeDirPath" },
			{ StdDirsImp::applicationDirPath, "applicationDirPath" },
			{ 0, 0 }
		     };

    int idx = 0;
    do {
	StdDirsImp *sdi = new StdDirsImp( exec, idx );
	parent.put( exec , methods[idx].name, KJS::Object(sdi) );
	++idx;
    } while( methods[idx].id );

}

KJS::Value StdDirsImp::call( KJS::ExecState *exec, KJS::Object &/*self*/, const KJS::List &args )
{
//    QString arg0 = extractQString(exec, args, 0);
//    QString arg1 = extractQString(exec, args, 1);

    switch( id ) {
	case StdDirsImp::findResource:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    QString path = KGlobal::dirs ()->findResource(arg0.latin1(), arg1);
	    return KJS::String( path );
	}
	case StdDirsImp::addResourceType:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    bool isOk = KGlobal::dirs ()->addResourceType(arg0.latin1(), arg1);
	    return KJS::Boolean( isOk );
	}
	case StdDirsImp::kde_default:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString path = KGlobal::dirs ()->kde_default(arg0.latin1());
	    return KJS::String( path );
	}
	case StdDirsImp::addResourceDir:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    bool isOk = KGlobal::dirs ()->addResourceDir(arg0.latin1(), arg1);
	    return KJS::Boolean( isOk );
	}
	case StdDirsImp::findResourceDir:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    QString path = KGlobal::dirs ()->findResourceDir(arg0.latin1(), arg1);
	    return KJS::String( path );
	}
	case StdDirsImp::saveLocation:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    bool arg2 = (args.size() > 2) ? args[2].toBoolean(exec) : true;
	    QString path = KGlobal::dirs ()->saveLocation(arg0.latin1(), arg1, arg2);
	    return KJS::String( path );
	}
	case StdDirsImp::makeDir:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    int arg1 = (args.size() > 1) ? args[1].toInteger(exec) : 0755;
	    bool isOk = KStandardDirs::makeDir(arg0, arg1);
	    return KJS::Boolean( isOk );
	}
	case StdDirsImp::exists:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    bool isOk = KStandardDirs::exists(arg0);
	    return KJS::Boolean( isOk );
	}
	case StdDirsImp::realPath:
	{
	    QString arg0 = extractQString(exec, args, 0);
	    QString path = KStandardDirs::realPath(arg0);
	    return KJS::String( path );
	}
	case StdDirsImp::currentDirPath:
		return KJS::String(QDir::currentDirPath());
	case StdDirsImp::homeDirPath:
		return KJS::String(QDir::homeDirPath());
	case StdDirsImp::applicationDirPath:
		return KJS::String(qApp->applicationDirPath());
	default:
	    kdWarning() << "StdDirsImp has no method " << id << endl;
    }

    kdWarning() << "StdDirsImp returning null" << endl;

    return KJS::Null();
}

StdIconsImp::StdIconsImp( JSFactory *fact, KJS::ExecState *exec, uint mid )
    : JSProxyImp(exec), id(mid), jsfact(fact)
{
}

StdIconsImp::~StdIconsImp() {}

void StdIconsImp::addBindings( JSFactory *fact,  KJS::ExecState *exec, KJS::Object &parent )
{
    JSProxy::MethodTable methods[] = {
    			{ StdIconsImp::BarIcon, "BarIcon" },
			{ StdIconsImp::DesktopIcon, "DesktopIcon" },
			{ StdIconsImp::SmallIcon, "SmallIcon" },
			{ StdIconsImp::MainBarIcon, "MainBarIcon" },
			{ StdIconsImp::UserIcon, "UserIcon" },
			{ StdIconsImp::MimeSourceIcon, "MimeSourceIcon" },
			{ 0, 0 }
		     };

    int idx = 0;
    do {
	StdIconsImp *sdi = new StdIconsImp( fact, exec, idx );
	parent.put( exec , methods[idx].name, KJS::Object(sdi) );
	++idx;
    } while( methods[idx].id );

}

KJS::Value StdIconsImp::wrapPixmap(KJS::ExecState *exec, const QPixmap &icon)
{
	return convertToValue(exec, icon);
}

KJS::Value StdIconsImp::call( KJS::ExecState *exec, KJS::Object &/*obj*/, const KJS::List &args )
{
    QString arg0 = extractQString(exec, args, 0);  // Name
		int arg1 = extractInt(exec, args, 1); // Size 0, 16, 24, 32, 48, 64, 128
		int arg2 = extractInt(exec, args, 2);
    // State 0 DefaultState, 1 ActiveState, 2 DisabledState, 3 LastState
    switch( id ) {
	case StdIconsImp::BarIcon:
	{
	    return wrapPixmap(exec, ::BarIcon(arg0, arg1, arg2));
	}
	case StdIconsImp::DesktopIcon:
	{
	    return wrapPixmap(exec, ::DesktopIcon(arg0, arg1, arg2));
	}
	case StdIconsImp::SmallIcon:
	{
	    return wrapPixmap(exec, ::SmallIcon(arg0, arg1, arg2));
	}
	case StdIconsImp::MainBarIcon:
	{
	    return wrapPixmap(exec, ::MainBarIcon(arg0, arg1, arg2));
	}
	case StdIconsImp::UserIcon:
	{
	    return wrapPixmap(exec, ::UserIcon(arg0, arg2));
	}
	case StdIconsImp::MimeSourceIcon:
	{
	    return wrapPixmap( exec, QPixmap::fromMimeSource(arg0));
	}
	default:
	    kdWarning() << "StdDirsImp has no method " << id << endl;
    }

    kdWarning() << "StdDirsImp returning null" << endl;

    return KJS::Null();
}

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
