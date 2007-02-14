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

#ifndef KJSEMBEDJSPROXYIMP_H
#define KJSEMBEDJSPROXYIMP_H

#include <kjs/function.h>
#include <kjsembed/global.h>

namespace KJSEmbed {

/**
 * Base class of custom KJS::FunctionImp classes.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSProxyImp :  public KJS::InternalFunctionImp
{
public:
    /** Creates the function using the builtin function prototype of the interpreter. */
    JSProxyImp( KJS::ExecState *exec );
    /** Cleans up */
    virtual ~JSProxyImp();

private:
    class JSProxyImpPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
