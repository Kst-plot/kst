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

#ifndef KJSEMBEDJSVALUEPROXY_H
#define KJSEMBEDJSVALUEPROXY_H
#include "global.h"
#include <qcstring.h>
#include <qvariant.h>
#include <kjs/object.h>
#include <kjsembed/jsproxy.h>

namespace KJSEmbed {

/**
 * Provides a binding to an opaque value.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSValueProxy : public JSProxy
{
public:
    JSValueProxy();
    virtual ~JSValueProxy();

    QString typeName() const;
    /** Returns true iff the content of this proxy inherits the specified base-class. */
    bool inherits( const char *clazz );

    void setValue( const QVariant &var );

    QVariant toVariant() const;

    virtual void addBindings( KJS::ExecState *state, KJS::Object &object );

    /** Reimplemented to return the name and class of the target. */
    virtual KJS::UString toString( KJS::ExecState *exec ) const;

private:
    QVariant val;
    class JSValueProxyPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSVALUEPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
