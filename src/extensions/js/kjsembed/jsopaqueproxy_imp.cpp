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

#include <qvariant.h>
#include "global.h"

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "jsobjectproxy.h"
#include "jsobjectproxy_imp.h"

#include "jsopaqueproxy.h"
#include "jsopaqueproxy_imp.h"

using namespace KJS;
namespace KJSEmbed {
namespace Bindings {

JSOpaqueProxyImp::JSOpaqueProxyImp( KJS::ExecState *exec, int mid, const JSOpaqueProxy *parent )
    : JSProxyImp(exec), id(mid), proxy(parent)
{
}

JSOpaqueProxyImp::~JSOpaqueProxyImp()
{
}

KJS::Value JSOpaqueProxyImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    switch( id ) {
	case MethodTypeName:
	{
	    return KJS::String( proxy->typeName() );
	}
	break;
    }

    return ObjectImp::call( exec, self, args );
}

} // namespace Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

