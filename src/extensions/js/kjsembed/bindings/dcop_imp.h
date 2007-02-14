// $Id$
// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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

#ifndef KJSEMBED_DCOP_IMP_H
#define KJSEMBED_DCOP_IMP_H

#include "bindingobject.h"
#include <kjsembed/jsobjectproxy_imp.h>
#include <kjsembed/jsbindingbase.h>
#include <dcopobject.h>

class DCOPClient;
#include <kdatastream.h>
#include <qvariant.h>
#include <qstring.h>

namespace KJS
{
	class Interpreter;
}

namespace KJSEmbed {
namespace Bindings {
class JSDCOPInterfacerLoader : public JSBindingBase
{
    public:
        KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
        void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
};

/**
* QObject Binding for a DCOP interface
*/
class JSDCOPInterface : public BindingObject, public DCOPObject
{
    Q_OBJECT

public:
    JSDCOPInterface( KJS::Interpreter *js, QObject *parent, const char *name=0 );
    virtual ~JSDCOPInterface();
    QCStringList functionsDynamic();
    bool processDynamic (const QCString &fun, const QByteArray &data, QCString &replyType, QByteArray &replyData);
public slots:
    void publish(const QString& signature);

private:
   QCStringList m_Members;
   KJS::Interpreter *m_js;
};

class JSDCOPRefLoader : public JSBindingBase
{
    public:
        KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
        void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
};

/**
 * DCOPRef binding.
*/
class JSDCOPRef : public JSProxyImp
{
    enum MethodId { Methodcall, Methodsend, Methodapp, Methodobj, Methodtype, MethodsetRef  };

public:
    JSDCOPRef( KJS::ExecState *exec, int id );
    virtual ~JSDCOPRef();
    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    int mid;

};

class JSDCOPClientLoader : public JSBindingBase
{
    public:
        KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
        void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
};

/**
 * Binding for DCOPClient.
 */
class JSDCOPClient : public JSProxyImp
{
	enum MethodId { Methodattach,  Methoddetach, MethodisAttached,  MethodregisteredApplications, MethodremoteObjects, MethodremoteInterfaces, MethodremoteFunctions, MethodconnectDCOPSignal, MethoddisconnectDCOPSignal, Methodcall, Methodsend, MethodDCOPStart, MethodappId, MethodisApplicationRegistered };

public:
    JSDCOPClient( KJS::ExecState *exec, int id );
    virtual ~JSDCOPClient();

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    /** Returns true iff this object implements the call function. */
    virtual bool implementsCall() const { return true; }

    /** Invokes the call function. */
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    static KJS::Value demarshall( KJS::ExecState * exec, const QCString& type, QDataStream& data);
    static QVariant demarshall( const QCString &type, QDataStream &ds);
    static void marshall( const QVariant& data, const QString &typeName, QByteArray& buffer );
    static QStringList getTypes( const QString& function);

    bool attach() const;
    bool detach() const;

    bool isAttached() const;

    QString dcopStart( const QString& appName, const QStringList& args);
    QStringList registeredApplications();
    QStringList remoteObjects( const QString& remApp);
    QStringList remoteInterfaces( const QString& remApp, const QString& remObj );
    QStringList remoteFunctions( const QString& remApp, const QString& remObj);
    bool connectDCOPSignal (const QString &sender, const QString &senderObj, const QString &signal, const QString &receiverObj, const QString &slot, bool Volatile = false);
    bool disconnectDCOPSignal (const QString &sender, const QString &senderObj, const QString &signal, const QString &receiverObj, const QString &slot);
    KJS::Value dcopCall(KJS::ExecState *exec, KJS::Object &, const KJS::List &args);
    KJS::Value dcopSend(KJS::ExecState *exec, KJS::Object &, const KJS::List &args);
    private:
    	int mid;
};

} // namespace Bindings
} // namespace KJSEmbed

#endif // KJSEMBED_TEXTSTREAM_IMP_H
