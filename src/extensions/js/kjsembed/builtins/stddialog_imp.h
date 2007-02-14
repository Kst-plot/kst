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

#ifndef KJSEMBED_STDDIALOG_IMP_H
#define KJSEMBED_STDDIALOG_IMP_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace BuiltIns {

/**
 * KJS bindings to the standard KDE dialogs.
 *
 * @author Richard Moore, rich@kde.org
 */
class StdDialogImp : public JSProxyImp
{
public:
    /** Identifiers for the various methods. */
    enum MethodId {
	// KFileDialog
	MethodGetOpenFileName, MethodGetSaveFileName,
	MethodGetOpenURL, MethodGetSaveURL,
	MethodGetExistingDirectory, MethodGetExistingURL,
	MethodGetImageOpenURL,

	// KColorDialog
	MethodGetColor,
	// KPropertiesDialog
	MethodShowProperties
    };

    StdDialogImp( KJS::ExecState *exec, int id );
    ~StdDialogImp();

    /** Adds the StdDialogs object to the specified parent. */
    static void addBindings( KJS::ExecState *exec, KJS::Object &parent );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    uint id;
};

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

#endif // KJSEMBED_STDDIALOG_IMP_H

// Local Variables:
// c-basic-offset: 4
// End:
