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

#ifndef KJSEMBEDBUILTINIMP_H
#define KJSEMBEDBUILTINIMP_H

#include <kjsembed/jsproxy_imp.h>

#include <kjsembed/jsbuiltin.h>

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

class KJSEmbedPart;

namespace Bindings {

/**
 * Implements KJS bindings for JSBuiltIn.
 *
 * @author Richard Moore, rich@kde.org
 */
class JSBuiltInImp : public JSProxyImp
{
public:
    /** Identifiers for the various methods. */
    enum MethodId {
	MethodInternalError,

	MethodPrint, MethodPrintLn, MethodWarn,
	MethodReadLine, MethodOpenFile, MethodReadFile, MethodWriteFile,
	MethodLoadScript, MethodDumpObject, MethodRunKSpy,
	MethodExit, MethodDumpCompletion, MethodSaxLoadFile,
	MethodAlert, MethodConfirm, MethodPrompt, MethodI18n, MethodImport,
	MethodShell,CustomMethod=0x1000
    };

    JSBuiltInImp( JSBuiltIn *builtin, int id, const QString &param=QString::null );
    ~JSBuiltInImp();

    QString parameter() const { return param; }

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    KJSEmbedPart *part;
    uint id;
    QString param;
    class JSBuiltInImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDBUILTINIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
