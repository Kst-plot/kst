#include <qmetaobject.h>
#include <private/qucom_p.h>

#include <qpen.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qsize.h>

#include <kjs/interpreter.h>
#include <kjs/ustring.h>
#include <kjs/types.h>
#include <kjs/value.h>

#include "global.h"
#include "jsbinding.h"
#include "jsobjectproxy.h"
#include "jsfactory.h"
#include "jsvalueproxy.h"
#include "jsopaqueproxy.h"
#include "jsbuiltinproxy.h"
#include "kjsembedpart.h"

#include "bindings/pen_imp.h"
#include "bindings/image_imp.h"
#include "bindings/pixmap_imp.h"
#include "bindings/painter_imp.h"
#include "bindings/brush_imp.h"
#include "bindings/size_imp.h"
#include "bindings/point_imp.h"
#include "bindings/rect_imp.h"

#include <stdlib.h>

namespace KJS {

    UString::UString( const QString &d )
    {
	uint len = d.length();
	UChar *dat = new UChar[len];
	memcpy( dat, d.unicode(), len * sizeof(UChar) );
	rep = UString::Rep::create(dat, len);
    }

    QString UString::qstring() const
    {
	return QString((QChar*) data(), size());
    }

    QConstString UString::qconststring() const
    {
	return QConstString((QChar*) data(), size());
    }

    QString Identifier::qstring() const
    {
	return QString((QChar*) data(), size());
    }

}

namespace KJSEmbed {

    //
    //
    // Supported
    // =========
    //
    // Bool, Double, CString, String, StringList, Int, UInt, Date, Time, DateTime,
    // Rect, Size, Point, Pixmap, Image, Brush, Pen, Size, Rect, Point, QValueList<QVariant>
    //
    // Opqaue
    // ======
    //
    // Invalid, Map, List, Palette, ColorGroup,
    // IconSet, PointArray, Region, Bitmap, Cursor, SizePolicy,
    // ByteArray, BitArray
    //

    KJS::Value convertToValue( KJS::ExecState *exec, const QVariant &val ) {
        if ( !val.isValid() )
            return KJS::Undefined();

        kdDebug( 80001 ) << "Convert a " << val.typeName() << " to a value" << endl;
        KJS::List items;
        uint type = val.type();

        switch( type )
        {
            case QVariant::String:
            case QVariant::CString:
                return KJS::String( val.toString() );
                break;
            case QVariant::Int:
                return KJS::Number( val.toInt() );
                break;
            case QVariant::UInt:
                return KJS::Number( val.toUInt() );
                break;
            case QVariant::Double:
                return KJS::Number( val.toDouble() );
                break;
            case QVariant::Bool:
                return KJS::Boolean( val.toBool() );
                break;
            case QVariant::Rect:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::Rect::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            case QVariant::Date:
            case QVariant::DateTime:
            case QVariant::Time:
            {
                QDateTime dt = QDateTime::currentDateTime();
                if ( type == QVariant::Date )
                    dt.setDate( val.toDate() );
                else if ( type == QVariant::Time )
                    dt.setTime( val.toTime() );
                else
                    dt = val.toDateTime();
                items.append( KJS::Number( dt.date().year() ) );
                items.append( KJS::Number( dt.date().month() - 1 ) );
                items.append( KJS::Number( dt.date().day() ) );
                items.append( KJS::Number( dt.time().hour() ) );
                items.append( KJS::Number( dt.time().minute() ) );
                items.append( KJS::Number( dt.time().second() ) );
                items.append( KJS::Number( dt.time().msec() ) );
                return KJS::Object( exec->interpreter() ->builtinDate().construct( exec, items ) );
                break;
            }
            case QVariant::Point:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::Point::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            case QVariant::StringList:
            {
                QStringList lst = val.toStringList();
                QStringList::Iterator idx = lst.begin();
                for ( ; idx != lst.end(); ++idx )
                    items.append( KJS::String( ( *idx ) ) );
                return KJS::Object( exec->interpreter() ->builtinArray().construct( exec, items ) );
                break;
            }
            case QVariant::List:
//            case QVariant::StringList:  
// This will cause an extra conversion step for a QStrinList so use the above case.
            {
                QValueList<QVariant> lst = val.toList();
                QValueList<QVariant>::Iterator idx = lst.begin();
                for ( ; idx != lst.end(); ++idx )
                    items.append( convertToValue( exec, ( *idx ) ) );
                return KJS::Object( exec->interpreter() ->builtinArray().construct( exec, items ) );
                break;
            }
            case QVariant::Map:
            {
                QMap<QString,QVariant> map = val.toMap();
                QMap<QString,QVariant>::Iterator idx = map.begin();
                KJS::Object array( exec->interpreter() ->builtinArray().construct( exec, 0 ) );
                for ( ; idx != map.end(); ++idx )
                    array.put(exec,  KJS::Identifier(idx.key().latin1()), convertToValue( exec,  idx.data() ) );
                return array;
                break;
            }
            case QVariant::Pixmap:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::Pixmap::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            case QVariant::Brush:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::BrushImp::addBindings( exec, proxyObj );
                return proxyObj;
            }
            case QVariant::Image:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::ImageImp::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            case QVariant::Pen:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::Pen::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            case QVariant::Size:
            {
                JSValueProxy * prx = new JSValueProxy( );
                prx->setValue( val );
                KJS::Object proxyObj( prx );
                Bindings::Size::addBindings( exec, proxyObj );
                return proxyObj;
                break;
            }
            default:
            {
                if ( val.canCast( QVariant::String ) )
                    return KJS::String( val.toString() );
                else
                {
                    kdDebug( 80001 ) << "convertToValue() Creating value, type wrapper for " << val.typeName() << endl;

                    JSValueProxy *valproxy = new JSValueProxy();
                    KJS::Object obj = KJS::Object( valproxy );
                    valproxy->addBindings( exec, obj );
                    valproxy->setValue( val );

                    return obj;
                }
            }
        }
    }

    QVariant convertToVariant( KJS::ExecState *exec, const KJS::Value &v ) {
        QVariant val;
        KJS::Type type = v.type();
        kdDebug( 80001 ) << "Convert a value of type " << type << " to a variant" << endl;
        switch( type )
        {
            case KJS::StringType:
                val = v.toString( exec ).qstring();
                break;
            case  KJS::NumberType:
                val = v.toNumber( exec );
                break;
            case KJS::BooleanType:
                val = v.toBoolean( exec );
                break;
            case KJS::ObjectType:
            {
                KJS::Object obj = v.toObject( exec );
                kdDebug( 80001 ) << "convertToVariant() got an " << obj.className().qstring() << endl;
                if ( obj.className().qstring() == "Array" ) {
                    if ( checkArray( exec, v ) == List )
                       val = convertArrayToList( exec, v );
                    else
                       val = convertArrayToMap(exec, v);
                } else if ( obj.className().qstring() == "Date" ) {
                    val = convertDateToDateTime( exec, v );
		} else if ( obj.className().qstring() == "Function" ) {
		  /*  val = obj.className().qstring();*/
                } else {
                    //JSObjectProxy *prx = JSProxy::toObjectProxy( obj.imp() );
		    JSValueProxy *vprx = JSProxy::toValueProxy(obj.imp() );
		    //JSOpaqueProxy *oprx = JSProxy::toOpaqueProxy( obj.imp() );
		    JSProxy *prxy = dynamic_cast<JSProxy*>(obj.imp());
		    /*
		    kdDebug(80001) << "Type " << obj.imp()->dispatchType() << endl;
    
		    kdDebug(80001) << "Object proxy: " << prx << " Value Proxy: " << vprx << " Opaque Proxy " << oprx << " Proxy " << prxy << endl;
		    kdDebug(80001) << "Object prototype valid " << obj.prototype().isValid() << endl;
		    kdDebug(80001) << "Prototype name: " << obj.prototype().toString(exec).ascii() << endl;
		    kdDebug(80001) << "Proxy imp " << dynamic_cast<JSProxyImp*>(obj.imp()) << endl;
		    kdDebug(80001) << "Builtin " << dynamic_cast<JSBuiltinProxy*>(obj.imp()) << endl;
		    */
                    /*if ( prx ) {
                    
		          QObject * o = prx->object();
                        QCString cs( o ? o->className() : "" );
                        if ( obj.implementsConstruct() )
                            val = QString( "new %1();" ).arg( obj.className().qstring() );
                        else if ( obj.implementsCall() )
                            val = QString( "%1(...);" ).arg( obj.className().qstring() );
                        kdDebug( 80001 ) << "convertToVariant() got a JSObjectProxy" << endl;         
                    } else */if ( vprx ) {
                        kdDebug( 80001 ) << "convertToVariant() got a variant JSValueProxy" << endl;
                        val = vprx->toVariant();
                    } else if ( prxy ) {
                    /*    kdDebug( 80001 ) << "convertToVariant() got a JSProxy" << endl;
                        val = prxy->typeName();
		      */
		    } else {  // Last resort map the object to a QMap<QString,QVariant>
		      kdDebug( 80001 ) << "convertToVariant() map to struct." << endl;
		      val = convertArrayToMap(exec, v);
                    }
                }
                break;
            }
            default:
            {
                QString msg = i18n( "Unsupported KJS::Value, type %1" ).arg( ( int ) v.type() );
                throwError(exec, msg);
                val = msg;
                break;
            }
        }
        kdDebug(80001) << "Returning a " << val.typeName() << endl;
        return val;
    }

    QStringList dumpCompletion( KJS::ExecState *exec, KJS::Object &obj ) {
        QStringList returnList;
        if ( !obj.isValid() ) {
            return returnList;
        }

        KJS::ReferenceList l = obj.propList( exec );
        KJS::ReferenceListIterator propIt = l.begin();
        while ( propIt != l.end() ) {

            KJS::Identifier name = propIt->getPropertyName( exec );
            if ( obj.hasProperty( exec, name ) ) {
                KJS::Value v = obj.get( exec, name );
                KJS::Object vobj = v.toObject( exec );
                QString nm( name.ustring().qstring() );
                QString vl;

                if ( vobj.implementsConstruct() ) {
                    if ( vobj.prototype().toString( exec ).qstring() == "[function]" )
                        returnList << QString( "%1(...)" ).arg( nm );
                } else if ( vobj.implementsCall() ) {
                    returnList << QString( "%1(...)" ).arg( nm );
                } else {
                    QVariant val = convertToVariant( exec, v );
                    vl = val.toString();
                    returnList << nm;
                }
            }

            propIt++;
        }

        JSObjectProxy *proxy = JSProxy::toObjectProxy( obj.imp() );
        QObject *qo = proxy ? proxy->object() : 0;
        QMetaObject *mo = qo ? qo->metaObject() : 0;

        // QObject
        if ( proxy ) {

            // QProperties
            QStrList propList( mo->propertyNames( true ) );
            for ( QStrListIterator iter1( propList ); iter1.current(); ++iter1 ) {

                QCString name = iter1.current();
                int propid = mo->findProperty( name.data(), true );
                if ( propid == -1 )
                    continue;
                returnList << name;
            }

            // Slots
            KJS::List items;
            QStrList slotList( mo->slotNames( true ) );

            for ( QStrListIterator iter2( slotList ); iter2.current(); ++iter2 ) {
                QCString name = iter2.current();
                QString nm( name );

                int slotid = mo->findSlot( name.data(), true );
                if ( slotid != -1 ) {
                    returnList << nm;
                }
            }
        }
        return returnList;
    }

    QString dumpObject( KJS::ExecState *exec, KJS::Object &obj ) {
        if ( !obj.isValid() )
            return QString( "Invalid object\n" );

        QStringList properties;
        QStringList methods;

        KJS::ReferenceList l = obj.propList( exec, true );
        KJS::ReferenceListIterator propIt = l.begin();

        while ( propIt != l.end() ) {

            KJS::Identifier name = propIt->getPropertyName( exec );

            if ( obj.hasProperty( exec, name ) ) {

                KJS::Value v = obj.get( exec, name );
                KJS::Object vobj = v.toObject( exec );
                QString nm( name.ustring().qstring() );

                kdDebug( 80001 ) << "Getting: " << name.ustring().qstring() << " "
                << vobj.prototype().toString( exec ).qstring() << endl;

                if ( vobj.implementsConstruct() ) {
                    // Do nothing
                    kdDebug( 80001 ) << "implementsConstruct: " << nm << endl;
                    if ( vobj.prototype().toString( exec ).qstring() == "[function]" )
                        methods += nm;
                } else if ( vobj.implementsCall() ) {
                    kdDebug( 80001 ) << "implementsCall: " << nm << endl;
                    methods += nm;
                } else {
                    properties += nm;
                }
            }

            propIt++;
        }

        QString s;

        // JS properties
        s += QString( "<h2>Properties</h2>\n" );

        if ( !properties.isEmpty() ) {
            s += "<table width=\"90%\">\n";
            s += "<tr><th>Type</th><th>Name</th><th>Value</th></tr>\n";

            properties.sort();
            for ( QStringList::Iterator iter = properties.begin(); iter != properties.end(); ++iter ) {
		KJS::Value v = obj.get( exec, KJS::Identifier( KJS::UString( *iter ) ) );
                QVariant val = convertToVariant( exec, v );

                s += QString( "<tr><td align=\"center\">%1</td><td><b>%2</b></td><td align=\"center\">%3</td></tr>\n" )
                     .arg( val.typeName() ).arg( *iter ).arg( val.toString() );
            }

            s += "</table>\n";
        } else
            s += "<i>None</i>\n";

        // Methods
        s += QString( "<h2>Methods</h2>\n" );

        if ( !methods.isEmpty() ) {
            s += "<table width=\"90%\">";
            methods.sort();
            for ( QStringList::Iterator iter = methods.begin(); iter != methods.end(); ++iter ) {
                s += "<tr><td><b>";
                s += *iter;
                s += "(...)</b></td></tr>";
            }
            s += "</table>";
        } else
            s += "<i>None</i>\n";

        s += dumpQObject( exec, obj );
        return s;
    }

    QString dumpQObject( KJS::ExecState * /*exec*/, KJS::Object &obj ) {
        if ( !obj.isValid() )
            return QString( "Invalid object\n" );

        QString s( "" );
        JSObjectProxy *proxy = JSProxy::toObjectProxy( obj.imp() );
        QObject *qo = proxy ? proxy->object() : 0;
        QMetaObject *mo = qo ? qo->metaObject() : 0;

        // QObject
        if ( proxy ) {

            // QProperties
            s += QString( "<h2>Qt Properties</h2>\n" );

            QStrList propList( mo->propertyNames( true ) );

            if ( !propList.isEmpty() ) {

                s += "<table width=\"90%\">\n";
                s += "<tr><th>Type</th><th>Name</th><th>Value</th></tr>\n";

                propList.sort();
                for ( QStrListIterator iter( propList ); iter.current(); ++iter ) {

                    QCString name = iter.current();
                    int propid = mo->findProperty( name.data(), true );
                    if ( propid == -1 )
                        continue;

                    QVariant vl = qo->property( name.data() );
                    s += QString( "<tr><td align=\"center\">%1</td><td><b>%2</b></td><td align=\"center\">%3</td></tr>\n" )
                         .arg( mo->property( propid, true ) ->type() ).arg( name ).arg( vl.toString() );
                }

                s += "</table>\n";
            } else {
                s += "<i>None</i>";
            }

            // Signals
            s += QString( "<h2>Qt Signals</h2>\n" );
            s += "<table width=\"90%\">";

            QStrList signalList( mo->signalNames( true ) );
            signalList.sort();

            for ( QStrListIterator iter1( signalList ); iter1.current(); ++iter1 ) {

                QCString name = iter1.current();
                QString nm( name );

                int signalid = mo->findSignal( name.data(), true );
                if ( signalid != -1 )
                    s += QString( "<tr><td><b>%1</b></td></tr>\n" ).arg( nm );
            }

            s += "</table>";
            if ( signalList.isEmpty() )
                s += "<i>None</i>";

            // Slots
            s += QString( "<h2>Qt Slots</h2>\n" );
            s += "<table width=\"90%\">";

            KJS::List items;
            QStrList slotList( mo->slotNames( true ) );
            slotList.sort();

            for ( QStrListIterator iter2( slotList ); iter2.current(); ++iter2 ) {

                QCString name = iter2.current();
                QString nm( name );

                int slotid = mo->findSlot( name.data(), true );
                if ( slotid != -1 ) {
                    const QMetaData * md = mo->slot( slotid, true );
                    const QUMethod *method = md->method;
                    if ( method->count && ( method->parameters->inOut == QUParameter::Out ) ) {
                        QCString typenm( method->parameters->type->desc() );
                        if ( typenm == "ptr" ) {
                            s += QString( "<tr><td>%1</td><td><b>%2</b></td></tr>\n" )
                                 .arg( ( const char * ) method->parameters->typeExtra ).arg( nm );
                        } else {
                            s += QString( "<tr><td>%1</td><td><b>%2</b></td></tr>\n" )
                                 .arg( typenm.data() ).arg( nm );
                        }
                    } else
                        s += QString( "<tr><td>void</td><td><b>%1</b></td></tr>\n" ).arg( nm );
                }
            }

            s += "</table>";
            if ( slotList.isEmpty() )
                s += "<i>None</i>";
        }

        return s;
    }



    QPen extractQPen( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toPen() : QPen();
    }

    QBrush extractQBrush( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toBrush() : QBrush();
    }

    QFont extractQFont( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toFont() : QFont();
    }

    QPixmap extractQPixmap( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toPixmap() : QPixmap();
    }

    QImage extractQImage( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toImage() : QImage();
    }

    QPalette extractQPalette( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toPalette() : QPalette();
    }

    QString extractQString( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? args[ idx ].toString( exec ).qstring() : QString::null;
    }

    int extractInt( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? args[ idx ].toInteger( exec ) : 0;
    }

    QColor extractQColor( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toColor() : QColor();
    }

    QSize extractQSize( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toSize() : QSize();
    }

    bool extractBool( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? args[ idx ].toBoolean( exec ) : false;
    }

    double extractDouble( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? args[ idx ].toNumber( exec ) : 0.0;
    }

    uint extractUInt( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? args[ idx ].toUInt32( exec ) : 0;
    }

    QStringList extractQStringList( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertArrayToStringList( exec, args[ idx ] ) : QStringList();
    }

    /**
    * This will return if the value is a list, associative array or neither
    * @return 
    */

    JavaScriptArrayType checkArray( KJS::ExecState *exec, const KJS::Value &val ) {
        KJS::Object obj = val.toObject( exec );
        kdDebug( 80001 ) << "Object type: " << obj.className().qstring() << endl;
        if ( obj.className().qstring() == "Array" ) {
            //KJS::ReferenceList lst = obj.propList( exec, true );
            KJS::Value len = obj.get(exec, KJS::Identifier("length"));
            kdDebug(80001) << "Array length: " << len.toNumber(exec) << endl;
            kdDebug(80001) << "Object type: " << len.type() << endl;
	    char buff[4];
            if( !obj.hasProperty(exec, KJS::Identifier("length")) )
                return Map;
            else if( !obj.hasProperty(exec, KJS::Identifier( itoa((len.toNumber(exec) - 1), buff, 10 ) ) ) )
                return Map;
            /*
            if( obj.get(exec, KJS::Identifier("length")).toInteger(exec) == 0 )
                return List;
            int offset = 0;
            KJS::ReferenceListIterator idx = lst.begin();
            for ( ; idx != lst.end(); idx++ ) {
                if ( idx->getPropertyName( exec ).qstring().toInt() != offset )
                    return Map;
                offset++;
                kdDebug( 80001 ) << "Property Name: " << idx->getPropertyName( exec ).qstring() << endl;
                kdDebug( 80001 ) << "Property Value: " << idx->getValue( exec ).toString( exec ).qstring() << endl;
            }
            */
            else
                return List;
        } else
            return None;
    }

    QMap<QString, QVariant> convertArrayToMap( KJS::ExecState *exec, const KJS::Value &value ) {
        QMap<QString, QVariant> returnMap;
          KJS::Object obj = value.toObject(exec);
//          if( obj.className().qstring() == "Array" ) {
            KJS::ReferenceList lst = obj.propList(exec,false);
            KJS::ReferenceListIterator idx = lst.begin();
            for( ; idx != lst.end(); idx++ )
            {
              KJS::Identifier id = idx->getPropertyName(exec);
              KJS::Value val = idx->getValue(exec);
              returnMap.insert(id.qstring(),convertToVariant(exec,val));
            }
//          }
        return returnMap;
    }

    QValueList<QVariant> convertArrayToList( KJS::ExecState *exec, const KJS::Value &value ) {
        QValueList<QVariant> returnList;
        KJS::Object obj = value.toObject( exec );
        if ( obj.className().qstring() == "Array" ) {
            int length = obj.get( exec, KJS::Identifier( "length" ) ).toInteger( exec );
            for ( int index = 0; index < length; ++index ) {
                char buff[4];
                KJS::Value val = obj.get(exec, KJS::Identifier( itoa( index, buff, 10 ) ) );
                if( val.isValid() )
                    returnList += convertToVariant(exec, val );
                else
                    returnList += "";
            }
        }
        return returnList;
    }

    QStringList convertArrayToStringList( KJS::ExecState *exec, const KJS::Value &value ) {
        QStringList returnList;
        KJS::Object obj = value.toObject( exec );
        if ( obj.className().qstring() == "Array" ) {
            int length = obj.get( exec, KJS::Identifier( "length" ) ).toInteger( exec );
            for ( int index = 0; index < length; ++index ) {
                char buff[4];
                KJS::Value val = obj.get(exec, KJS::Identifier( itoa( index, buff, 10 ) ) );

                if( val.isValid() )
                    returnList +=  val.toString(exec).qstring();
                else
                    returnList += "";
                }
        }
        return returnList;
    }
/*
    QStringList convertArrayToStringList( KJS::ExecState *exec, const KJS::Value &value ) {
        QStringList returnList;
        KJS::Object obj = value.toObject( exec );

        if ( obj.className().qstring() == "Array" ) {
            int length = obj.get( exec, KJS::Identifier( "length" ) ).toInteger( exec );
            for ( int index = 0; index < length; ++index ) {
                returnList << obj.get( exec, KJS::Identifier( KJS::UString::from( index ) ) ).toString( exec ).qstring();
            }
        }

        return returnList;
    }
*/
    QDateTime convertDateToDateTime( KJS::ExecState *exec, const KJS::Value &value ) {
        KJS::List args;
        QDateTime returnDateTime;
        KJS::Object obj = value.toObject( exec );

        if ( obj.className().qstring() == "Date" ) {
            int seconds = obj.get( exec, KJS::Identifier( "getSeconds" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );
            int minutes = obj.get( exec, KJS::Identifier( "getMinutes" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );
            int hours = obj.get( exec, KJS::Identifier( "getHours" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );
            int month = obj.get( exec, KJS::Identifier( "getMonth" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );
            int day = obj.get( exec, KJS::Identifier( "getDate" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );
            int year = obj.get( exec, KJS::Identifier( "getFullYear" ) ).toObject( exec ).call( exec, obj, args ).toInteger( exec );

            returnDateTime.setDate( QDate( year, month + 1, day ) );
            returnDateTime.setTime( QTime( hours, minutes, seconds ) );
        } else {
            kdWarning() << "convertDateToDateTime() received a " << obj.className().qstring() << " instead of a Date" << endl;
        }

        return returnDateTime;
    }

    QDateTime extractQDateTime( KJS::ExecState * exec, const KJS::List & args, int idx ) {
        return ( args.size() > idx ) ? convertDateToDateTime( exec, args[ idx ] ) : QDateTime();
    }

    QDate extractQDate( KJS::ExecState * exec, const KJS::List & args, int idx ) {
        return ( args.size() > idx ) ? convertDateToDateTime( exec, args[ idx ] ).date() : QDate();
    }

    QTime extractQTime( KJS::ExecState * exec, const KJS::List & args, int idx ) {
        return ( args.size() > idx ) ? convertDateToDateTime( exec, args[ idx ] ).time() : QTime();
    }

    QRect extractQRect( KJS::ExecState * exec, const KJS::List & args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toRect() : QRect();
    }

    QPoint extractQPoint( KJS::ExecState * exec, const KJS::List & args, int idx ) {
        return ( args.size() > idx ) ? convertToVariant( exec, args[ idx ] ).toPoint() : QPoint();
    }

    QStrList extractQStrList( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        return ( args.size() > idx ) ? convertArrayToStrList( exec, args[ idx ] ) : QStrList();
    }

    QStrList convertArrayToStrList( KJS::ExecState *exec, const KJS::Value &value ) {
        QStrList returnList;
        KJS::Object obj = value.toObject( exec );

        if ( obj.className().qstring() == "Array" ) {
            int length = obj.get( exec, KJS::Identifier( "length" ) ).toInteger( exec );
            for ( int index = 0; index < length; ++index ) {
                returnList.append( obj.get( exec, KJS::Identifier( KJS::UString::from( index ) ) ).toString( exec ).qstring().latin1() );
            }
        }

        return returnList;
    }

    QObject *extractQObject( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        KJS::Object obj = args[ idx ].toObject( exec );
        JSObjectProxy *proxy = JSProxy::toObjectProxy( obj.imp() );
        if ( !proxy )
            return 0L;
        return proxy->object();

    }

    QWidget *extractQWidget( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        KJS::Object obj = args[ idx ].toObject( exec );
        JSObjectProxy *proxy = JSProxy::toObjectProxy( obj.imp() );
        if ( !proxy )
            return 0L;
        return proxy->widget();
    }

    JSOpaqueProxy *extractOpaqueProxy( KJS::ExecState *exec, const KJS::List &args, int idx ) {
        KJS::Object obj = args[ idx ].toObject( exec );
        return JSProxy::toOpaqueProxy( obj.imp() );
    }

    KJS::Object throwError( KJS::ExecState *exec, const QString &error, KJS::ErrorType type )
    {
        int sourceId = exec->context().sourceId();
        int startLine = exec->context().curStmtFirstLine();
        
        KJS::Object retValue = KJS::Error::create( exec, type, error.utf8(), startLine, sourceId );
        kdWarning(80001) << error << endl;
        exec->setException( retValue );
        return retValue;
    }
} // namespace KJSEmbed

