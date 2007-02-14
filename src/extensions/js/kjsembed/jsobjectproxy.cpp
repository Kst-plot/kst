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

#include <qobject.h>
#include <qobjectlist.h>
#include <qmetaobject.h>
#include <qregexp.h>
#include <qsignal.h>
#include <qstrlist.h>
#include <qvariant.h>

#include <private/qucomextra_p.h>

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jssecuritypolicy.h"
#include "jsfactory.h"
#include "global.h"

#include "jsobjectproxy.h"
#include "jsobjectproxy_imp.h"
#include "jsobjecteventproxy.h"
#include "jseventmapper.h"
#include "slotutils.h"

#include "bindings/bindingobject.h"

//#include "kjsembedpart_imp.h"
#include "jsfactory_imp.h"

using namespace KJS;

namespace KJSEmbed {

    typedef Bindings::JSObjectProxyImp JSObjectProxyImp;

    JSObjectProxy::JSObjectProxy( KJSEmbedPart *part, QObject *target, QObject *r, const JSSecurityPolicy *sp )
            : JSProxy( JSProxy::ObjectProxy ),
    jspart( part ), js( part->interpreter() ), obj( target ), root( r ) {
        policy = sp ? sp : JSSecurityPolicy::defaultPolicy();
    }

    JSObjectProxy::JSObjectProxy( KJSEmbedPart *part, QObject *target, QObject *r )
            : JSProxy( JSProxy::ObjectProxy ),
    jspart( part ), js( part->interpreter() ), obj( target ), root( r ) {
        policy = JSSecurityPolicy::defaultPolicy();
    }

    JSObjectProxy::JSObjectProxy( KJSEmbedPart *part, QObject *target )
            : JSProxy( JSProxy::ObjectProxy ),
    jspart( part ), js( part->interpreter() ), obj( target ), root( target ) {
        policy = JSSecurityPolicy::defaultPolicy();
    }
    JSObjectProxy::~JSObjectProxy() {
        if ( owner() == JavaScript && obj->parent() == 0 )
            delete obj;
    }

    void JSObjectProxy::setSecurityPolicy( const JSSecurityPolicy *pol ) {
        policy = pol ? pol : new JSSecurityPolicy();
    }

    bool JSObjectProxy::isAllowed( KJS::Interpreter *js ) const {
        return policy->isInterpreterAllowed( this, js );
    }

    KJS::UString JSObjectProxy::toString( KJS::ExecState *exec ) const {
        if ( !exec ) {
            kdWarning() << "JS toString with null state, ignoring" << endl;
            return KJS::UString();
        }
        if ( !isAllowed( exec->interpreter() ) ) {
            kdWarning() << "JS toString request from unknown interpreter, ignoring" << endl;
            return KJS::UString();
        }

        QString s( "%1 (%2)" );
        s = s.arg( obj ? obj->name() : "Dead Object" );
        s = s.arg( obj ? obj->className() : "" );
        return KJS::UString( s );
    }

    KJS::Value JSObjectProxy::get( KJS::ExecState *exec, const KJS::Identifier &p ) const {
            if ( !isAllowed( exec->interpreter() ) ) {
                kdWarning() << "JS get request from unknown interpreter, ignoring" << endl;
                return KJS::Null();
            }

            if ( !policy->isPropertyAllowed( this, obj, p.ascii() ) )
                return ObjectImp::get( exec, p );

            if ( !obj ) {
                kdDebug( 80001 ) << "JS getting '" << p.ustring().qstring() << "' but qobj has died" << endl;
                return ObjectImp::get( exec, p );
            }
            kdDebug( 80001 ) << "JS getting '" << p.ascii() << endl;

            // Properties
            QString prop = p.ustring().qstring();
            QMetaObject *meta = obj->metaObject();

            if ( meta->findProperty( p.ascii(), true ) != -1 ) {
                QVariant val = obj->property( prop.ascii() );
                kdDebug( 80001 ) << "JS getting '" << p.ascii() << "' ( " << val.typeName() << ")" << endl;

                return convertToValue( exec, val );
            }

            return ObjectImp::get
                       ( exec, p );
        }

    void JSObjectProxy::put( KJS::ExecState *exec,
                             const KJS::Identifier &p, const KJS::Value &v,
                             int attr ) {
        if ( !isAllowed( exec->interpreter() ) ) {
            kdWarning() << "JS put request from unknown interpreter, ignoring" << endl;
            return ;
        }

        if ( !policy->hasCapability( JSSecurityPolicy::CapabilitySetProperties ) ) {
            ObjectImp::put( exec, p, v, attr );
            return ;
        }

        if ( !obj ) {
            kdDebug( 80001 ) << "JS setting '" << p.ascii() << "' but qobj has died" << endl;
            ObjectImp::put( exec, p, v, attr );
            return ;
        }

        // Properties
        QMetaObject *meta = obj->metaObject();
        int propIndex = meta->findProperty( p.ascii(), true );
        if ( propIndex != -1 ) {
            QVariant val = convertToVariant( exec, v );
            if ( meta->property(propIndex, true)->isEnumType() ) {
                obj->setProperty( p.ascii(), val.toUInt() );
            } else if ( val.isValid() ) {
                obj->setProperty( p.ascii(), val );
            } else {
                kdWarning(80001) << "Error setting value." << endl;
            }
        } else {
            ObjectImp::put( exec, p, v, attr );
        }

        if ( jspart->factory() ->eventMapper() ->isEventHandler( p ) ) {
            if ( evproxy.isNull() )
                evproxy = new KJSEmbed::JSObjectEventProxy( this );
            evproxy->addFilter( jspart->factory() ->eventMapper() ->findEventType( p ) );
            kdDebug( 80001 ) << "Adding event handler " << p.ascii() << endl;
        }
    }

    //
    // Implementation of JS Method Bindings
    //

    void JSObjectProxy::addBindings( KJS::ExecState *exec, KJS::Object &object ) {
        kdDebug( 80001 ) << "JSObjectProxy::addBindings() " << ( obj->name() ? obj->name() : "dunno" )
        << ", class " << obj->className() << endl;

        if ( policy->hasCapability( JSSecurityPolicy::CapabilityGetProperties | JSSecurityPolicy::CapabilitySetProperties ) ) {
            object.put( exec, "properties", KJS::Object( new JSObjectProxyImp( exec, JSObjectProxyImp::MethodProps, this ) ) );
        }

        if ( policy->hasCapability( JSSecurityPolicy::CapabilityTree ) ) {
            JSObjectProxyImp::addBindingsTree( exec, object, this );
            JSObjectProxyImp::addBindingsDOM( exec, object, this );
        }

        if ( policy->hasCapability( JSSecurityPolicy::CapabilitySlots ) ) {
            addBindingsSlots( exec, object );
            JSObjectProxyImp::addBindingsConnect( exec, object, this );
        }

        addBindingsClass( exec, object );
    }

    void JSObjectProxy::addBindingsClass( KJS::ExecState *exec, KJS::Object & /*object*/ ) {
        KJS::Identifier clazzid;
        QObject *o = obj;
        Bindings::BindingObject *bo = dynamic_cast<Bindings::BindingObject *>( o );
        if ( bo ) {
            clazzid = KJS::Identifier( bo->jsClassName() ? bo->jsClassName() : obj->className() );
        } else {
            clazzid = KJS::Identifier( obj->className() );
        }

        KJS::Object global = js->globalObject();
        if ( global.hasProperty( exec, clazzid ) ) {
            kdDebug( 80001 ) << "addBindingsClass() " << clazzid.qstring() << " already known" << endl;

            KJS::Object clazz = global.get( exec, clazzid ).toObject( exec );
            Bindings::JSFactoryImp *imp = dynamic_cast<Bindings::JSFactoryImp *>( clazz.imp() );
            if ( !imp ) {
                kdWarning() << "addBindingsClass() Class was not created by normal means" << endl;
                return ;
            }

            kdDebug( 80001 ) << "addBindingsClass() Adding enums" << endl;
            imp->setDefaultValue( js->builtinObject().construct( exec, KJS::List() ) );
            addBindingsEnum( exec, clazz );
        } else {
            kdWarning() << "addBindingsClass() " << clazzid.qstring() << " not known" << endl;
        }
    }

    void JSObjectProxy::addBindingsEnum( KJS::ExecState *exec, KJS::Object &object ) {
        QMetaObject * mo = obj->metaObject();
        QStrList enumList = mo->enumeratorNames( true );

        for ( QStrListIterator iter( enumList ); iter.current(); ++iter ) {

            const QMetaEnum *me = mo->enumerator( iter.current(), true );
            for ( uint i = 0 ; i < me->count ; i++ ) {
                QCString key( ( me->items ) [ i ].key );
                int val = ( me->items ) [ i ].value;
                object.put( exec, key.data(), KJS::Number( val ), KJS::ReadOnly );
            }
        }
    }

    void JSObjectProxy::addBindingsSlots( KJS::ExecState *exec, KJS::Object &object ) {
        // Publish slots with supported signatures as methods.
        QMetaObject * mo = obj->metaObject();
        QStrList slotList( mo->slotNames( true ) );
        for ( QStrListIterator iter( slotList ); iter.current(); ++iter ) {
            addSlotBinding( iter.current(), exec, object );
        }
    }

    void JSObjectProxy::addSlotBinding( const QCString &name, KJS::ExecState *exec, KJS::Object &object ) {
        // Lookup and bind slot
        QMetaObject * mo = obj->metaObject();
        int slotid = mo->findSlot( name.data(), true );
        if ( slotid == -1 )
            return ;

        const QMetaData *md = mo->slot( slotid, true );
        if ( md->access != QMetaData::Public )
            return ;

        // Find signature
        int id = Bindings::JSSlotUtils::findSignature( name );
        //    kdDebug( 80001 )<<"JSObjectProxy::addSlotBinding()::slot:"<<name<<" id:"<<id<<endl;
        if ( id < 0 )
            return ;

        QCString jsname = name;
        jsname.detach();
        jsname.replace( QRegExp( "\\([^\\)]*\\)" ), "" );

        // Find the return type, we only care if it is a pointer type
        const QUMethod *m = md->method;
        const char *retclass = 0;
        QCString ptr( "ptr" );

        if ( m->count && ( m->parameters->inOut == QUParameter::Out )
                && ( ptr == m->parameters->type->desc() ) ) {
            retclass = ( const char * ) m->parameters->typeExtra;
            //  kdDebug(80001) << "Return type is a pointer, type " << retclass << endl;
        }

        // Create the Imp
        JSObjectProxyImp *imp = new JSObjectProxyImp( exec, JSObjectProxyImp::MethodSlot,
                                retclass ? retclass : "", id, name, this );

        if ( !object.hasProperty( exec, KJS::Identifier( jsname ) ) ) {
            // The identifier is unused
            object.put( exec, KJS::Identifier( jsname.data() ), KJS::Object( imp ) );
        } else {
            // The identifier has already been used
            QString s( name );
            QCString cs = QString( "%1%2" ).arg( jsname ).arg( s.contains( ',' ) + 1 ).ascii();
            //kdDebug(80001) << "Method " << jsname << " exists, using " << cs << " for " << s << endl;
            object.put( exec, KJS::Identifier( cs.data() ), KJS::Object( imp ) );
        }
    }

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

