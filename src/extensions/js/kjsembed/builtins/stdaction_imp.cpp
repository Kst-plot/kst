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

#include <kaction.h>
#include <kdebug.h>
#include <kstdaction.h>

#include <jsfactory.h>
#include <jsobjectproxy.h>

#include "stdaction_imp.h"

namespace KJSEmbed {
namespace BuiltIns {

StdActionImp::StdActionImp( KJS::ExecState *exec, JSFactory *fact, uint actid )
    : JSProxyImp(exec), id(actid), jsfact( fact )
{
}

StdActionImp::~StdActionImp()
{
}

static uint KSTDACTION_START = KStdAction::New;
static uint KSTDACTION_END = KStdAction::ConfigureNotifications+1;

const char *factoryMethodNames[] = {
    "START_MARKER",
    "fileNew", "open", "openRecent", "save", "saveAs", "revert", "close",
    "print", "printPreview", "mail", "quit",
    "undo", "redo", "cut", "copy", "paste", "selectAll", "deselect", "find", "findNext", "findPrev",
    "replace",
    "actualSize", "fitToPage", "fitToWidth", "fitToHeight", "zoomIn", "zoomOut",
    "zoom", "redisplay",
    "up", "back", "forward", "home", "prior", "next", "goGoto", "gotoPage", "gotoLine",
    "firstPage", "lastPage",
    "addBookmark", "editBookmarks",
    "spelling",
    "showMenubar", "showToolbar", "showStatusbar", "saveOptions", "keyBindings",
    "preferences", "configureToolbars",
    "help", "helpContents", "whatsThis", "reportBug", "aboutApp", "aboutKDE",
    "tipofDay",
    "configureNotifications",
    "END_MARKER"
};

void StdActionImp::addBindings( JSFactory *fact, KJS::ExecState *exec, KJS::Object &parent )
{
    for ( uint i = KSTDACTION_START; i < KSTDACTION_END; i++ ) {
	StdActionImp *sai = new StdActionImp( exec, fact, i );
	parent.put( exec, factoryMethodNames[i], KJS::Object( sai ) );
    }
}

KJS::Value StdActionImp::call( KJS::ExecState *exec, KJS::Object &/*self*/, const KJS::List &args )
{
    if ( ( args.size() < 3 ) || ( args.size() > 4 ) )
	return KJS::Null();

    // recv, slot, parent, name?
    QObject *recv = extractQObject( exec, args, 0);

    QString sl = extractQString( exec, args, 1 );
    const char *slc = sl.isNull() ? "" : sl.ascii();

    QObject *parentobj = extractQObject( exec, args, 2 );
    KActionCollection *parent = parentobj ? dynamic_cast<KActionCollection *>( parentobj ) : 0;

    KStdAction::StdAction actid = static_cast<KStdAction::StdAction>(id);
    const char *arg3 = (args.size() == 4) ? args[3].toString(exec).ascii() : KStdAction::name( actid );

    // Create the object
    KAction *act = KStdAction::create( actid, arg3, recv, slc, parent );
    return jsfact->createProxy( exec, act );
}

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
