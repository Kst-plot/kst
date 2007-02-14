// -*- c++ -*-

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

#ifndef KJSEMBEDTEXTSTREAMIMP_H
#define KJSEMBEDTEXTSTREAMIMP_H

#include <kjs/object.h>
#include <kjsembed/jsbinding.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QTextStream;

namespace KJSEmbed {
namespace BuiltIns {

/**
 * Implements the JS methods of text stream object bindings.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */
class TextStreamImp : public JSProxyImp
{
public:
    /** Identifiers for the methods provided by this class. */
    enum MethodId {
	MethodIsReadable,
	MethodIsWritable,
	MethodPrint,
	MethodPrintLn,
	MethodReadLine,
	MethodFlush
    };

    TextStreamImp( KJS::ExecState *exec, int id, QTextStream *ts );
    ~TextStreamImp();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    int id;
    QTextStream *ts;

    class TextStreamImpPrivate *d;
};

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

#endif // KJSEMBEDTEXTSTREAMIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
