// -*- c++ -*-

/*
 *  Copyright (C) 2001-2004, Richard J. Moore <rich@kde.org>
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

#ifndef KJSEMBED_JSFACTORYIMP_H
#define KJSEMBED_JSFACTORYIMP_H

#include <kjsembed/jsproxy_imp.h>

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

class JSFactory;

namespace Bindings {

/**
 * Implements KJS bindings for JSFactory.
 *
 * @author Richard Moore, rich@kde.org
 */
class JSFactoryImp : public JSProxyImp
{
public:
    /** Identifiers for the various methods. */
    enum MethodId {
	MethodInternalError,

	NewInstance,
	MethodLoadUI,
	MethodCreateROPart,
	MethodConstructors,
	MethodWidgets,
	MethodTypes,
	MethodIsSupported,
	MethodIsQObject,
	MethodIsValue,
	MethodIsOpaque,
	MethodListPlugins,
	MethodCreateRWPart,
	MethodCreateObject
    };

    JSFactoryImp( KJS::ExecState *exec, JSFactory *fact, int id, const QString &param=QString::null );
    ~JSFactoryImp();

    QString parameter() const { return param; }

    /** Returns true iff this object implements the construct function. */
    virtual bool implementsConstruct() const { return true; }

    /** Invokes the construct function. */
    virtual KJS::Object construct( KJS::ExecState *exec, const KJS::List &args );

    void setDefaultValue( const KJS::Value &value );
    virtual KJS::Value defaultValue( KJS::ExecState *exec, KJS::Type hint ) const;

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    /** Returns the JSFactory for which this is the implementation. */
    JSFactory *factory() const { return fact; }

private:
    void publishChildren(QObject *obj, KJS::Object &parent);
    JSFactory *fact;
    uint id;
    QString param;
    KJS::Value defaultVal;
    class JSFactoryImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBED_JSFACTORYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
