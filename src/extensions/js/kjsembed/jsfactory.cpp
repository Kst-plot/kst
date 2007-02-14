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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <qobject.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <qimage.h>
#include <qlistview.h>
#include <qmainwindow.h>
//#include <qpainter.h>
#include <qprogressdialog.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qwidgetfactory.h>
#include <qsplitter.h>
#include <qscrollview.h>
#include <qlayout.h>
#include <qsplashscreen.h>

#ifndef QT_ONLY

#include <kapplication.h>
#include <dcopref.h>
#include <dcopclient.h>
#include <kaction.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kservice.h>
#include <ksystemtray.h>
#include <ktrader.h>

#include <kparts/part.h>
#include <kparts/mainwindow.h>
#include <kparts/componentfactory.h>

#endif // QT_ONLY

#include <kjs/interpreter.h>
#include <kjs/identifier.h>
#include <kjs/types.h>

#include "jsbinding.h"
#include "jsbindingbase.h"
#include "jsobjectproxy.h"
#include "jsopaqueproxy.h"
#include "jsvalueproxy.h"
#include "jsconsolewidget.h"
#include "jseventmapper.h"
#include "jseventutils.h"
#include "kjsembedpart.h"
#include "customobject_imp.h"

#include "builtins/stddialog_imp.h"
#include "builtins/textstream_imp.h"
#include "builtins/qdir_imp.h"

#include "qtbindings/qcombobox_imp.h"
#include "qtbindings/qpopupmenu_imp.h"
#include "qtbindings/qlistviewitem_imp.h"
#include "qtbindings/qchecklistitem_imp.h"

#ifndef QT_ONLY

#include "builtins/stdaction_imp.h"

#include "bindings/dcop_imp.h"
#include "bindings/netaccess_imp.h"
#endif // QT_ONLY

#include "bindings/image_imp.h"
#include "bindings/pen_imp.h"
#include "bindings/pixmap_imp.h"
#include "bindings/painter_imp.h"
#include "bindings/movie_imp.h"
#include "bindings/sql_imp.h"
#include "bindings/kconfig_imp.h"
#include "bindings/brush_imp.h"
#include "bindings/size_imp.h"
#include "bindings/rect_imp.h"
#include "bindings/point_imp.h"


#include "global.h"
#include "jsbindingplugin.h"
#include "jsfactory_imp.h"
#include "jsfactory.h"

//
// KPart Implementation
//
namespace KJSEmbed {

    typedef Bindings::JSFactoryImp JSFactoryImp;
    typedef Bindings::CustomObjectImp CustomObjectImp;
    typedef BuiltIns::StdDialogImp StdDialogImp;
    typedef BuiltIns::TextStreamImp TextStreamImp;

    class JSFactoryPrivate {
    public:
        QDict<KJSEmbed::Bindings::JSBindingPlugin> plugins;
        QDict<KJSEmbed::Bindings::JSBindingBase> opaqueTypes;
        QDict<KJSEmbed::Bindings::JSBindingBase> objectTypes;
    };


    JSFactory::JSFactory( KJSEmbedPart *part )
            : jspart( part ) {
        evmapper = new JSEventMapper();
        d = new JSFactoryPrivate;
        registerOpaqueType("QDir", new Bindings::QDirLoader());
        registerOpaqueType("QCheckListItem", new Bindings::QCheckListItemLoader());
        registerOpaqueType("QListViewItem", new Bindings::QListViewItemLoader());
        registerOpaqueType("Painter", new Bindings::PainterLoader());

#ifndef QT_ONLY
        registerOpaqueType("DCOPClient", new Bindings::JSDCOPClientLoader());
        registerObjectType("DCOPInterface", new Bindings::JSDCOPInterfacerLoader());
        registerOpaqueType("DCOPRef", new Bindings::JSDCOPRefLoader());
#endif

    }

    JSFactory::~JSFactory() {
        delete evmapper;
	delete d;
    }

    void JSFactory::addBindingPluginTypes( KJS::ExecState *exec, KJS::Object &parent ) {
#ifndef QT_ONLY

        // Get list of valid plugin types
        KTrader::OfferList offers = KTrader::self() ->query( "JSBindingPlugin/Binding" );
        if ( !offers.count() )
            return ;

        KTrader::OfferListIterator itr = offers.begin();
        while ( itr != offers.end() ) {
            QString classname = ( *itr ) ->name();
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, classname );
            parent.put( exec, KJS::Identifier( cons->parameter() ), KJS::Object( cons ) );
            addType( classname, TypePlugin );
            ++itr;
        }
#else
        Q_UNUSED( exec );
        Q_UNUSED( parent );
#endif // QT_ONLY

    }

    bool JSFactory::isBindingPlugin( const QString &classname ) const {
        if ( !isSupported( classname ) )
            return false;
        return ( objtypes[ classname ] & TypePlugin );
    }

    KJS::Object JSFactory::createBindingPlugin( KJS::ExecState *exec, const QString &classname, const KJS::List &args ) {
#ifndef QT_ONLY

        Bindings::JSBindingPlugin * plugin = d->plugins[ classname ];
        if ( plugin == 0L ) {
            QString query = "JSBindingPlugin/Binding";
            QString constraint = "[Name] == '" + classname + "'";
            plugin = KParts::ComponentFactory::createInstanceFromQuery<Bindings::JSBindingPlugin>( query, constraint );
            if ( plugin != 0L )
                d->plugins.insert( classname, plugin );
        }

        if ( plugin != 0L )
            return plugin->createBinding( jspart, exec, args );

#else

        Q_UNUSED( exec );
        Q_UNUSED( args );
#endif // QT_ONLY

        kdWarning( 80001 ) << "Unable to load binding " << classname << "." << endl;

        return KJS::Object();
    }

    KJS::Object JSFactory::create( KJS::ExecState *exec, const QString &classname, const KJS::List &args ) {
        KJS::Object returnObject;
        if ( !isSupported( classname ) ) {
            QString msg = i18n( "Object of type '%1' is not supported." ).arg( classname );
            return throwError(exec, msg,KJS::TypeError);
        }
        if ( isQObject( classname ) ) {
            // create via widget factory...
            Bindings::JSBindingBase *factory =  d->objectTypes.find(classname);
            if( factory ) {
                returnObject = factory->createBinding( jspart, exec, args );
                JSProxy::toProxy( returnObject.imp() ) ->setOwner( JSProxy::JavaScript );
            } else {
                QObject *parent = extractQObject(exec, args, 0);
                QString name = extractQString(exec, args, 1);
    
                QObject *obj = create( classname, parent, name.latin1() );
                if ( obj ) {
                    returnObject = createProxy( exec, obj );
                    JSProxy::toProxy( returnObject.imp() ) ->setOwner( JSProxy::JavaScript );
                } else {
                    QString msg = i18n( "QObject of type '%1' could not be created." ).arg( classname );
                    returnObject = throwError(exec, msg,KJS::TypeError);
                }
            }
        } else if ( isOpaque( classname ) ) {
            // Try to create from the registry
            Bindings::JSBindingBase *factory =  d->opaqueTypes.find(classname);
            if( factory )
                returnObject = factory->createBinding( jspart, exec, args );
            else
                returnObject = createOpaque( exec, classname, args );

            if ( !returnObject.isValid() ) {
                QString msg = i18n( "Opaque object of type '%1' could not be created." ).arg( classname );
                returnObject = throwError(exec, msg,KJS::TypeError);
            }
        } else if ( isValue( classname ) ) {
            returnObject = createValue( exec, classname, args );
            if ( !returnObject.isValid() ) {
                QString msg = i18n( "Value object of type '%1' could not be created." ).arg( classname );
                returnObject = throwError(exec, msg,KJS::TypeError);
            }
        } else if ( isBindingPlugin( classname ) ) {
            returnObject = createBindingPlugin( exec, classname, args );
            if ( !returnObject.isValid() ) {
                QString msg = i18n( "Plugin object of type '%1' could not be created." ).arg( classname );
                returnObject = throwError(exec, msg,KJS::TypeError);
            } else
                JSProxy::toProxy( returnObject.imp() ) ->setOwner( JSProxy::JavaScript );
        } else {
            QString msg = i18n( "Could not create object of type '%1'." ).arg( classname );
            returnObject = throwError(exec, msg,KJS::TypeError);
        }
        return returnObject;
    }

    KJS::Object JSFactory::createProxy( KJS::ExecState *exec, QObject *target,
                                        const JSObjectProxy *ctx ) const {
        kdDebug( 80001 ) << "JSFactory::createProxy: Target '" << target->name()
        << "' type " << target->className() << endl;

        JSObjectProxy *prx;
        if ( ctx )
            prx = new JSObjectProxy( jspart, target, ctx->rootObject(), ctx->securityPolicy() );
        else {
            prx = new JSObjectProxy( jspart, target );

        }
        kdDebug( 80001 ) << "Proxy created" << endl;

        KJS::Object proxyObj( prx );
        prx->addBindings( exec, proxyObj );
        extendProxy( exec, proxyObj );
        prx->setOwner(JSProxy::Native);
        
        kdDebug( 80001 ) << "Returning object" << endl;

        return proxyObj;
    }

    KJS::Object JSFactory::createProxy( KJS::ExecState *exec,
                                        QTextStream *target, const JSObjectProxy *context ) const {
        Q_UNUSED( context )
        kdDebug( 80001 ) << "TextStream proxy created" << endl;

        JSOpaqueProxy *prx = new JSOpaqueProxy( target );
        KJS::Object proxyObj( prx );
        prx->addBindings( exec, proxyObj );
        TextStreamImp::addBindings( exec, proxyObj );

        return proxyObj;
    }

    KJS::Object JSFactory::createProxy( KJS::ExecState *exec,
                                        QEvent *target, const JSObjectProxy *context ) const {
        switch ( target->type() ) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::MouseButtonDblClick:
                return JSEventUtils::convertEvent( exec, ( QMouseEvent * ) target, context );
                break;
            case QEvent::KeyPress:
            case QEvent::KeyRelease:
            case QEvent::Accel:
            case QEvent::AccelOverride:
                return JSEventUtils::convertEvent( exec, ( QKeyEvent * ) target, context );
                break;
            case QEvent::IMStart:
            case QEvent::IMCompose:
            case QEvent::IMEnd:
                return JSEventUtils::convertEvent( exec, ( QIMEvent * ) target, context );
                break;
            case QEvent::Paint:
                return JSEventUtils::convertEvent( exec, ( QPaintEvent * ) target, context );
                break;
            case QEvent::Resize:
                return JSEventUtils::convertEvent( exec, ( QResizeEvent * ) target, context );
                break;
            case QEvent::FocusIn:
            case QEvent::FocusOut:
                return JSEventUtils::convertEvent( exec, ( QFocusEvent * ) target, context );
                break;
            case QEvent::Close:
                return JSEventUtils::convertEvent( exec, ( QCloseEvent * ) target, context );
                break;
            case QEvent::ChildInserted:
            case QEvent::ChildRemoved:
                return JSEventUtils::convertEvent( exec, ( QChildEvent * ) target, context );
                break;
            case QEvent::Move:
                return JSEventUtils::convertEvent( exec, ( QMoveEvent * ) target, context );
                break;
            case QEvent::Wheel:
                return JSEventUtils::convertEvent( exec, ( QWheelEvent * ) target, context );
                break;
            case QEvent::Timer:
                return JSEventUtils::convertEvent( exec, ( QTimerEvent * ) target, context );
                break;
            case QEvent::ContextMenu:
                return JSEventUtils::convertEvent( exec, ( QContextMenuEvent * ) target, context );
                break;
            case QEvent::DragMove:
            case QEvent::DragEnter:
                return JSEventUtils::convertEvent( exec, ( QDragMoveEvent * ) target, context );
                break;
            case QEvent::Drop:
                return JSEventUtils::convertEvent( exec, ( QDropEvent * ) target, context );
                break;
            case QEvent::Enter:
            case QEvent::Leave:
            case QEvent::Clipboard:
            case QEvent::DragLeave:
            case QEvent::Show:
            case QEvent::Hide:
                return JSEventUtils::convertEvent( exec, ( QEvent * ) target, context );
                break;
            default:
                break;
        }

        return JSEventUtils::convertEvent( exec, ( QEvent * ) target, context );
    }

    KJS::Object JSFactory::extendProxy( KJS::ExecState *exec, KJS::Object &target ) const {
        CustomObjectImp::addBindings( exec, target );
        addBindingsPlugin( exec, target );
        return target;
    }

    void JSFactory::addBindingsPlugin( KJS::ExecState *exec, KJS::Object &target ) const {
        kdDebug( 800001 ) << "JSFactory::addBindingsPlugin" << endl;
        JSObjectProxy *proxy = JSProxy::toObjectProxy( target.imp() );
        if ( !proxy )
            return ;
        if ( !isBindingPlugin( proxy->object() ->className() ) )
            return ;

#ifndef QT_ONLY

        Bindings::JSBindingPlugin *plugin = d->plugins[ proxy->object() ->className() ];
        if ( plugin == 0L ) {
            QString query = "JSBindingPlugin/Binding";
            QString constraint = "[Name] == '" + QString::fromUtf8( proxy->object() ->className() ) + "'";
            plugin = KParts::ComponentFactory::createInstanceFromQuery<Bindings::JSBindingPlugin>( query, constraint );
            if ( plugin != 0L )
                d->plugins.insert( proxy->object() ->className(), plugin );
        }

        if ( plugin != 0L ) {
            kdDebug( 800001 ) << "JSFactory::addBindingsPlugin: calling plugin function" << endl;
            plugin->addBindings( exec, target );
            return ;
        }
#else
        Q_UNUSED( exec );
#endif // QT_ONLY

        kdWarning( 80001 ) << "Unable to add bindings to " << proxy->object() ->className() << "." << endl;
        return ;

    }


    QObject *JSFactory::createBinding( const QString &cname, QObject *parent, const char *name ) {
#ifndef QT_ONLY

        // Bindings
        /*if ( cname == "DCOPInterface" )
            return new Bindings::JSDCOPInterface( jspart->interpreter(), parent, name );
        else */if ( cname == "NetAccess" )
            return new Bindings::NetAccess( parent, name );
        else
#endif // QT_ONLY
            if ( cname == "Movie" )
                return new Bindings::Movie( parent, name );
            else if ( cname == "SqlDatabase" )
                return new Bindings::SqlDatabase( parent, name );
            else if ( cname == "SqlQuery" )
                return new Bindings::SqlQuery( parent, name );
            else if ( cname == "Config" )
                return new Bindings::Config( parent, name );

        return 0;
    }

    QObject *JSFactory::createObject( const QString &cname, QObject *parent, const char *name ) {

        // QObjects defined by Qt
        if ( cname == "QObject" )
            return new QObject( parent, name );
        else if ( cname == "QTimer" )
            return new QTimer( parent, name );

#ifndef QT_ONLY
        // QObjects defined by KDE
        else if ( cname == "KAction" )
            return new KAction( parent, name );
        else if ( cname == "KToggleAction" )
            return new KToggleAction( parent, name );
#endif // QT_ONLY

        QWidget *w = dynamic_cast<QWidget *>( parent );
        if ( !w )
            return 0;

        if ( cname == "QHBoxLayout" )
            return new QHBoxLayout( w, 0, -1, name );
        else if ( cname == "QVBoxLayout" )
            return new QVBoxLayout( w, 0, -1, name );
        return 0;
    }

    KJS::Object JSFactory::createOpaque( KJS::ExecState *exec, const QString &cname, const KJS::List &args ) {
        return KJS::Object();
    }

    KJS::Object JSFactory::createValue( KJS::ExecState *exec, const QString &cname, const KJS::List & args ) {
        if ( cname == "Image" ) {
            QImage img = QImage();
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( img );
            KJS::Object proxyObj( prx );
            Bindings::ImageImp::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Pixmap" ) {
            QPixmap pix = QPixmap();
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( pix );
            KJS::Object proxyObj( prx );
            Bindings::Pixmap::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Brush" ) {
            QBrush brsh;
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( brsh );
            KJS::Object proxyObj( prx );
            Bindings::BrushImp::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Pen" ) {
            QPen pen;
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( pen );
            KJS::Object proxyObj( prx );
            Bindings::Pen::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Rect" ) {
            QRect rect;
            if ( args.size() == 4 ) {
                rect.setX( extractInt( exec, args, 0 ) );
                rect.setY( extractInt( exec, args, 1 ) );
                rect.setWidth( extractInt( exec, args, 2 ) );
                rect.setHeight( extractInt( exec, args, 3 ) );
            }
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( rect );
            KJS::Object proxyObj( prx );
            Bindings::Rect::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Point" ) {
            QPoint point;
            if ( args.size() == 2 ) {
                point.setX( extractInt( exec, args, 0 ) );
                point.setY( extractInt( exec, args, 1 ) );
            }
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( point );
            KJS::Object proxyObj( prx );
            Bindings::Point::addBindings( exec, proxyObj );
            return proxyObj;
        }
        if ( cname == "Size" ) {
            QSize size;
            if ( args.size() == 2 ) {
                size.setWidth( extractInt( exec, args, 0 ) );
                size.setHeight( extractInt( exec, args, 1 ) );
            }
            JSValueProxy *prx = new JSValueProxy( );
            prx->setValue( size );
            KJS::Object proxyObj( prx );
            Bindings::Size::addBindings( exec, proxyObj );
            return proxyObj;
        }


        return KJS::Object();
    }

    QWidget *JSFactory::createWidget( const QString &cname, QWidget *pw, const char *name ) {
        if ( cname == "QSplitter" )
            return new QSplitter( pw, name );
        else if ( cname == "QMainWindow" )
            return new QMainWindow( pw, name );
        else if ( cname == "QProgressDialog" )
            return new QProgressDialog( pw, name );
        else if ( cname == "QScrollView" )
            return new QScrollView( pw, name );
        else if ( cname == "QSplashScreen" ) {
            QPixmap pix( 16, 16 );
            pix.fill();
            return new QSplashScreen( pix );
        }
#ifndef QT_ONLY
        else if ( cname == "KMainWindow" )
            return new KMainWindow( pw, name );
        else if ( cname == "KParts_MainWindow" )
            return new KParts::MainWindow( pw, name );
        else if ( cname == "KSystemTray" )
            return new KSystemTray( pw, name );
#endif // QT_ONLY

        return 0;
    }

    QObject *JSFactory::create( const QString &cname, QObject *parent, const char *name ) {
        kdDebug( 80001 ) << "KJSEmbedPart::create() name " << name << " class " << cname << endl;

        // Factory widgets
        QWidgetFactory wf;
        QWidget *pw = ( parent && parent->isWidgetType() ) ? static_cast<QWidget *>( parent ) : 0;
        QWidget *w = wf.createWidget( cname, pw, name );
        if ( w ) {
            kdDebug( 80001 ) << "Created from factory" << endl;
            return w;
        }
        // Custom widgets
        QObject *obj;
        obj = createWidget( cname, pw, name );
        if ( obj ) {
            if ( !isQObject( obj->className() ) )
                addType( obj->className() );
            kdDebug( 80001 ) << "Created from createWidget" << endl;
            return obj;
        }

        // Custom objects
        obj = JSFactory::createObject( cname, parent, name );
        if ( obj ) {
            if ( !isQObject( obj->className() ) )
                addType( obj->className() );
            kdDebug( 80001 ) << "Created from createObject" << endl;
            return obj;
        }

        // Binding objects
        obj = JSFactory::createBinding( cname, parent, name );
        if ( obj ) {
            if ( !isQObject( obj->className() ) )
                addType( obj->className() );
            kdDebug( 80001 ) << "Created from bindings" << endl;
            return obj;
        }

        kdDebug( 80001 ) << "Found nothing in :" << cname << ":" << endl;
        return 0;
    }

    KParts::ReadOnlyPart *JSFactory::createROPart( const QString &svc, QObject *parent, const char *name ) {
        kdDebug( 80001 ) << "JSFactory::createROPart svc " << svc << " parent " << ( ulong ) parent << endl;
        return createROPart( svc, "'KParts/ReadOnlyPart' in ServiceTypes", parent, name );
    }

    KParts::ReadOnlyPart *JSFactory::createROPart( const QString &svc, const QString &con,
            QObject *parent, const char *name ) {
        kdDebug( 80001 ) << "JSFactory::createROPart svc " << svc << " constraint " << con
        << " parent " << ( ulong ) parent << endl;

        return createROPart( svc, con, parent, name, QStringList() );
    }

    KParts::ReadOnlyPart *JSFactory::createROPart( const QString &svc, const QString &con,
            QObject *parent, const char *name,
            const QStringList &args ) {
#ifndef QT_ONLY
        kdDebug( 80001 ) << "JSFactory::createROPart svc " << svc << " constraint " << con
        << " parent " << ( ulong ) parent
        << " args: " << args << endl;

        KTrader::OfferList offers = KTrader::self() ->query( svc, con );
        if ( !offers.count() )
            return 0;

        KService::Ptr ptr = offers.first();
        KLibFactory *fact = KLibLoader::self() ->factory( ptr->library().ascii() );
        if ( !fact ) {
            kdDebug( 80001 ) << "Unable to find a matching part" << endl;
            return 0;
        }

        QObject *obj = fact->create( parent, name, "KParts::ReadOnlyPart", args );
        addType( obj->className() );
        return static_cast<KParts::ReadOnlyPart *>( obj );
#else // QT_ONLY

        Q_UNUSED( svc );
        Q_UNUSED( con );
        Q_UNUSED( parent );
        Q_UNUSED( name );
        Q_UNUSED( args );
        return 0;
#endif // QT_ONLY

    }

    KParts::ReadWritePart *JSFactory::createRWPart( const QString &svc, QObject *parent, const char *name ) {
        kdDebug( 80001 ) << "JSFactory::createRWPart svc " << svc << " parent " << ( ulong ) parent << endl;
        return createRWPart( svc, "'KParts/ReadWritePart' in ServiceTypes", parent, name );
    }

    KParts::ReadWritePart *JSFactory::createRWPart( const QString &svc, const QString &con,
            QObject *parent, const char *name ) {
        kdDebug( 80001 ) << "JSFactory::createRWPart svc " << svc << " constraint " << con
        << " parent " << ( ulong ) parent << endl;

        return createRWPart( svc, con, parent, name, QStringList() );
    }

    KParts::ReadWritePart *JSFactory::createRWPart( const QString &svc, const QString &con,
            QObject *parent, const char *name,
            const QStringList &args ) {
#ifndef QT_ONLY
        kdDebug( 80001 ) << "JSFactory::createRWPart svc " << svc << " constraint " << con
        << " parent " << ( ulong ) parent
        << " args: " << args << endl;

        KTrader::OfferList offers = KTrader::self() ->query( svc, con );
        if ( !offers.count() )
            return 0;

        KService::Ptr ptr = offers.first();
        KLibFactory *fact = KLibLoader::self() ->factory( ptr->library().ascii() );
        if ( !fact ) {
            kdDebug( 80001 ) << "Unable to find a matching part" << endl;
            return 0;
        }

        QObject *obj = fact->create( parent, name, "KParts::ReadWritePart", args );
        addType( obj->className() );
        return static_cast<KParts::ReadWritePart *>( obj );
#else // QT_ONLY

        Q_UNUSED( svc );
        Q_UNUSED( con );
        Q_UNUSED( parent );
        Q_UNUSED( name );
        Q_UNUSED( args );
        return 0;
#endif // QT_ONLY

    }

    QWidget *JSFactory::loadUI( const QString &uiFile, QObject *connector, QWidget *parent, const char *name ) {
        return QWidgetFactory::create( uiFile, connector, parent, name );
    }

    QStringList JSFactory::listBindingPlugins( KJS::ExecState *exec, KJS::Object &self ) {
        Q_UNUSED( exec )
        Q_UNUSED( self )
        QStringList pluginList;
        QStringList allTypes = objtypes.keys();
        for ( uint idx = 0; idx < allTypes.count(); ++idx ) {
            if ( objtypes[ allTypes[ idx ] ] & TypePlugin )
                pluginList.append( allTypes[ idx ] );
        }
        return pluginList;
    }

    bool JSFactory::isQObject( const QString &clazz ) const {
        if ( !isSupported( clazz ) )
            return false;

        return ( objtypes[ clazz ] & TypeQObject );
    }

    bool JSFactory::isValue( const QString &clazz ) const {
        if ( !isSupported( clazz ) )
            return false;

        return objtypes[ clazz ] == TypeValue;
    }

    bool JSFactory::isOpaque( const QString &clazz ) const {
        if ( !isSupported( clazz ) )
            return false;
        return objtypes[ clazz ] == TypeOpaque;
    }

    void JSFactory::addQObjectPlugin( const QString &classname, KJSEmbed::Bindings::JSBindingPlugin* plugin ) {
        if ( plugin ) {
            addType( classname, TypeQObjectPlugin );
            d->plugins.insert( classname, plugin );
        }
    }

    void JSFactory::addType( const QString &clazz, uint prxtype ) {
        if ( prxtype == TypeInvalid ) {
            objtypes.remove( clazz );
            return ;
        }
        kdDebug() << "Add type " << clazz << endl;
        objtypes[ clazz ] = prxtype;
    }

    bool JSFactory::isSupported( const QString &clazz ) const {
        kdDebug() << "Checking " << clazz << endl;
        return objtypes.contains( clazz );
    }

    uint JSFactory::proxyType( const QString &clazz ) const {
        if ( !isSupported( clazz ) )
            return TypeInvalid;
        return objtypes[ clazz ];
    }

    void JSFactory::addTypes( KJS::ExecState *exec, KJS::Object &parent ) {
        addWidgetFactoryTypes( exec, parent );
        addCustomTypes( exec, parent );
        addBindingTypes( exec, parent );
        addObjectTypes( exec, parent );
        addOpaqueTypes( exec, parent );
        addValueTypes( exec, parent );
        addBindingPluginTypes( exec, parent );
    }

    QStringList JSFactory::types() const {
        return objtypes.keys();
    }

    void JSFactory::addWidgetFactoryTypes( KJS::ExecState *exec, KJS::Object &parent ) {
        QStringList sl = QWidgetFactory::widgets();

        for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it ) {
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, *it );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( *it );
        }
    }

    void JSFactory::addCustomTypes( KJS::ExecState *exec, KJS::Object &parent ) {
        const char * classes[] = {

                                     "QSplitter", "QScrollView",
                                     "QObject", "QTimer", "QSplashScreen", "QProgressDialog",
                                     "QLayout", "QBoxLayout", "QHBoxLayout", "QVBoxLayout",
#ifndef QT_ONLY
                                     "KMainWindow", "KXMLGUIClient", "KSystemTray",
                                     "KAction", "KToggleAction",
                                     "KParts_MainWindow",
#endif // QT_ONLY
                                     0
                                 };

        for ( int i = 0 ; classes[ i ] ; i++ ) {
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, classes[ i ] );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( classes[ i ] );
        }
    }

    void JSFactory::addBindingTypes( KJS::ExecState *exec, KJS::Object &parent ) {


        const char * bindings[] = {
                                      "Pixmap", "KJSEmbed::Bindings::Pixmap",
                                      "SqlDatabase", "KJSEmbed::Bindings::SqlDatabase",
                                      "Movie", "KJSEmbed::Bindings::Movie",
                                      "SqlQuery", "KJSEmbed::Bindings::SqlQuery",
#ifndef QT_ONLY
                                      "NetAccess", "KJSEmbed::Bindings::NetAccess",
                                      /*"DCOPInterface", "KJSEmbed::Bindings::JSDCOPInterface",*/
#endif
                                      "Config", "KJSEmbed::Bindings::Config",
                                      0, 0
                                  };

        for ( int i = 0 ; bindings[ i ] ; i += 2 ) {
            JSFactoryImp * cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, bindings[ i ] );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( bindings[ i ] );
            addType( bindings[ i + 1 ] );
        }

       

    }

    void JSFactory::addOpaqueTypes( KJS::ExecState *exec, KJS::Object &parent ) {
        const char * classes[] = {
                                     "QTextStream",
                                     "TextStream",
                                     0
                                 };

        for ( int i = 0 ; classes[ i ] ; i++ ) {
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, classes[ i ] );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( classes[ i ], TypeOpaque );
        }

        // Create the custom added types.
        QDictIterator<KJSEmbed::Bindings::JSBindingBase> idx( d->opaqueTypes );
        for( ; idx.current(); ++idx){
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, idx.currentKey() );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( idx.currentKey(), TypeOpaque );
        }

    }
    void JSFactory::addValueTypes( KJS::ExecState *exec, KJS::Object &parent ) {
        const char * classes[] = {
                                     "Image",
                                     "Brush",
                                     "Pixmap",
                                     "Pen",
                                     "Rect",
                                     "Size",
                                     "Point",
                                     0
                                 };

        for ( int i = 0 ; classes[ i ] ; i++ ) {
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, classes[ i ] );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( classes[ i ], TypeValue );
        }
    }


    void JSFactory::addObjectTypes( KJS::ExecState *exec, KJS::Object & parent ) {
        const char * classes[] = {
                                     "QAccel",
                                     "QAccessibleObject",
                                     "QAction",
                                     "QActionGroup",
                                     "QApplication",
                                     "QAquaStyle",
                                     "QAssistantClient",
                                     "QAxObject",
                                     "QAxWidget",
                                     "QButton",
                                     "QCDEStyle",
                                     "QClipboard",
                                     "QColorDialog",
                                     "QColorDrag",
                                     "QComboBox",
                                     "QCommonStyle",
                                     "QCopChannel",
                                     "QDataPump",
                                     "QDateTimeEditBase",
                                     "QDesktopWidget",
                                     "QDns",
                                     "QDockArea",
                                     "QDockWindow",
                                     "QDoubleValidator",
                                     "QDragObject",
                                     "QEditorFactory",
                                     "QErrorMessage",
                                     "QEventLoop",
                                     "QFileDialog",
                                     "QFileIconProvider",
                                     "QFontDialog",
                                     "QFtp",
                                     "QGLWidget",
                                     "QGridLayout",
                                     "QGridView",
                                     "QHButtonGroup",
                                     "QHGroupBox",
                                     "QHeader",
                                     "QHttp",
                                     "QIconDrag",
                                     "QImageDrag",
                                     "QInputDialog",
                                     "QIntValidator",
                                     "QLocalFs",
                                     "QMacStyle",
                                     "QMenuBar",
                                     "QMessageBox",
                                     "QMotif",
                                     "QMotifDialog",
                                     "QMotifPlusStyle",
                                     "QMotifStyle",
                                     "QMotifWidget",
                                     "QNPInstance",
                                     "QNPWidget",
                                     "QNetworkOperation",
                                     "QNetworkProtocol",
                                     "QObjectCleanupHandler",
                                     "QPlatinumStyle",
                                     "QProcess",
                                     "QPopupMenu",
                                     "QProgressDialog",
                                     "QRegExpValidator",
                                     "QSGIStyle",
                                     "QServerSocket",
                                     "QSessionManager",
                                     "QSignal",
                                     "QSignalMapper",
                                     "QSizeGrip",
                                     "QSocket",
                                     "QSocketNotifier",
                                     "QSound",
                                     "QSqlDatabase",
                                     "QSqlDriver",
                                     "QSqlEditorFactory",
                                     "QSqlForm",
                                     "QStatusBar",
                                     "QStoredDrag",
                                     "QStyle",
                                     "QStyleSheet",
                                     "QTabBar",
                                     "QTabDialog",
                                     "QTextDrag",
                                     "QToolBar",
                                     "QToolTipGroup",
                                     "QTranslator",
                                     "QUriDrag",
                                     "QUrlOperator",
                                     "QVButtonGroup",
                                     "QVGroupBox",
                                     "QValidator",
                                     "QWSKeyboardHandler",
                                     "QWindowsStyle",
                                     "QWindowsXPStyle",
                                     "QWorkspace",
                                     "QXtWidget",

                                     0
                                 };

        for ( int i = 0 ; classes[ i ] ; i++ ) {
            if ( !isSupported( classes[ i ] ) )
                addType( classes[ i ] );
        }
                // Create the custom added types.
        QDictIterator<KJSEmbed::Bindings::JSBindingBase> idx( d->objectTypes );
        for( ; idx.current(); ++idx){
            JSFactoryImp *cons = new JSFactoryImp( exec, this, JSFactoryImp::NewInstance, idx.currentKey() );
            parent.put( exec, KJS::Identifier( KJS::UString( cons->parameter() ) ), KJS::Object( cons ) );
            addType( idx.currentKey(), TypeQObject );
        }
    }

    void JSFactory::registerOpaqueType( const QString & className, KJSEmbed::Bindings::JSBindingBase * bindingFactory )
    {
        d->opaqueTypes.insert(className,bindingFactory);
    } 

    void JSFactory::unregisterOpaqueType( const QString & className )
    {
        d->opaqueTypes.remove(className);
    }

    void JSFactory::extendOpaqueProxy( KJS::ExecState * exec, KJS::Object &proxy ) const
    {
	JSOpaqueProxy *prx = JSProxy::toOpaqueProxy( proxy.imp() );
	if( prx )
	{
	   kdDebug() << "Looking for " << prx->typeName() << endl;
           Bindings::JSBindingBase *bindingFactory = d->opaqueTypes.find( prx->typeName() );
           if( bindingFactory )
	   {
               kdDebug() << "Extending proxy" << endl;
               bindingFactory->addBindings(jspart, exec, proxy );
	   }
	}
    }
    void JSFactory::registerObjectType( const QString & className, KJSEmbed::Bindings::JSBindingBase * bindingFactory )
    {
        d->objectTypes.insert(className,bindingFactory);
    } 

    void JSFactory::unregisterObjectType( const QString & className )
    {
        d->objectTypes.remove(className);
    }

    void JSFactory::extendObjectProxy( KJS::ExecState * exec, KJS::Object &proxy ) const
    {
	JSObjectProxy *prx = JSProxy::toObjectProxy( proxy.imp() );
	if( prx )
	{
	   kdDebug() << "Looking for " << prx->typeName() << endl;
           Bindings::JSBindingBase *bindingFactory = d->objectTypes.find( prx->typeName() );
           if( bindingFactory )
	   {
               kdDebug() << "Extending proxy" << endl;
               bindingFactory->addBindings(jspart, exec, proxy );
	   }
	}
    }
    
}// namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
