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

#include "global.h"
#include "jsfactory.h"
#include "kjsembedpart.h"

#include "kjsembedpart_imp.h"

//
// KJSEmbedPart Bindings
//
namespace KJSEmbed {
namespace Bindings {

KJSEmbedPartImp::KJSEmbedPartImp( KJS::ExecState *exec, KJSEmbedPart *jsp, int mid, const QString &p )
    : JSProxyImp(exec), part(jsp), id(mid), param(p)
{
}

KJSEmbedPartImp::~KJSEmbedPartImp() {}

KJS::Value KJSEmbedPartImp::call( KJS::ExecState *exec, KJS::Object &/*self*/, const KJS::List &args )
{
    QString arg0 = (args.size() > 0) ? args[0].toString(exec).qstring() : QString::null;
//    QString arg1 = (args.size() > 1) ? args[1].toString(exec).qstring() : QString::null;

    if ( id == MethodCreate ) {
	return part->factory()->create( exec, arg0.latin1(), args.copyTail() );
    }
    else {
	kdWarning() << "KJSEmbedPartImp has no method " << id << endl;
    }

    QString msg = i18n( "KJSEmbedPartImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
