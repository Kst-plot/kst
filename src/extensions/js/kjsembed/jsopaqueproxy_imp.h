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

#ifndef KJSEMBEDJSOPAQUEPROXYIMP_H
#define KJSEMBEDJSOPAQUEPROXYIMP_H

#include <qcstring.h>
#include <kjs/object.h>
#include <kjsembed/jsproxy_imp.h>

namespace KJSEmbed { class JSOpaqueProxy; }

namespace KJSEmbed {
namespace Bindings {

/**
 * Provides a binding to an opaque value.
 *
 * @author Richard Moore, rich@kde.org
 */
class JSOpaqueProxyImp : public JSProxyImp
{
public:
    enum Methods {
	MethodInternalError,
	MethodTypeName
    };

    JSOpaqueProxyImp( KJS::ExecState *exec, int id, const JSOpaqueProxy *parent );
    virtual ~JSOpaqueProxyImp();

    virtual bool implementsCall() const { return true; }
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    int id;
    const JSOpaqueProxy *proxy;
    class JSOpaqueProxyImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDJSOPAQUEPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
