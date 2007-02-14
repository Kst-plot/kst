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

#ifndef KJSEMBEDPARTIMP_H
#define KJSEMBEDPARTIMP_H

#include <kjs/object.h>
#include <kjsembed/jsobjectproxy_imp.h>

namespace KJSEmbed { class KJSEmbedPart; }

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {

/**
 * Implements KJS bindings for KJSEmbedPart.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEmbedPartImp : public JSProxyImp
{
public:
    /** Identifiers for the various methods. */
    enum MethodId {
	MethodCreate,
	CustomMethod=0x1000
    };

    KJSEmbedPartImp( KJS::ExecState *exec, KJSEmbedPart *part, int id, const QString &param=QString::null );
    ~KJSEmbedPartImp();

    QString parameter() const { return param; }

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    KJSEmbedPart *part;
    uint id;
    QString param;
    class KJSEmbedPartImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDPARTIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
