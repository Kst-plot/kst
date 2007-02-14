


#include <qcstring.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>

#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qlistview.h>
#include "qchecklistitem_imp.h"
#include "qlistviewitem_imp.h"
/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {

    KJS::Object QCheckListItemLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
    {
        if ( args.size() == 0 ) {
                        // FALL THRU
        } else {
            JSOpaqueProxy * arg0 = JSProxy::toOpaqueProxy( args[ 0 ].imp() );
            QString arg1 = ( args.size() >= 2 ) ? args[ 1 ].toString( exec ).qstring() : QString::null;
            JSOpaqueProxy *prx = 0;

            if ( arg0 ) {
                if ( arg0->typeName() == "QListViewItem" ) {
                    QListViewItem * parent = arg0->toNative<QListViewItem>();
                    prx = new JSOpaqueProxy( new QCheckListItem( parent, arg1 ), "QCheckListItem" );
                } else {
                    return KJS::Object();
                }
            } else {
                JSObjectProxy *arg0 = JSProxy::toObjectProxy( args[ 0 ].imp() );
                if ( arg0 ) {
                    QListView * parent = ( QListView * ) ( arg0->widget() );
                    prx = new JSOpaqueProxy( new QCheckListItem( parent, arg1 ), "QCheckListItem" );
                } else {
                    return KJS::Object();
                }
            }
            prx->setOwner( JSProxy::Native );
            KJS::Object proxyObj( prx );
            addBindings(jspart, exec, proxyObj );
            return proxyObj;
        }
        return KJS::Object();
    }
    void QCheckListItemLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
    {
        QListViewItemImp::addBindings( exec, proxy );
        QCheckListItemImp::addBindings( exec, proxy );
    }

}
QCheckListItemImp::QCheckListItemImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QCheckListItemImp::~QCheckListItemImp()
{
}

void QCheckListItemImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_paintCell_10, "paintCell" },
        { Method_paintFocus_11, "paintFocus" },
        { Method_width_12, "width" },
        { Method_setup_13, "setup" },
        { Method_setOn_14, "setOn" },
        { Method_isOn_15, "isOn" },
        { Method_type_16, "type" },
        { Method_text_17, "text" },
        { Method_text_18, "text" },
        { Method_setTristate_19, "setTristate" },
        { Method_isTristate_20, "isTristate" },
        { Method_state_21, "state" },
        { Method_setState_22, "setState" },
        { Method_rtti_23, "rtti" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].id ) {
        if ( lastName != methods[idx].name ) {
            QCheckListItemImp *meth = new QCheckListItemImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {

        // enum Type
        { "RadioButton", QCheckListItem::RadioButton },
        { "CheckBox", QCheckListItem::CheckBox },
        { "Controller", QCheckListItem::Controller },
        { "RadioButtonController", QCheckListItem::RadioButtonController },
        { "CheckBoxController", QCheckListItem::CheckBoxController },
        // enum ToggleState
        { "Off", QCheckListItem::Off },
        { "NoChange", QCheckListItem::NoChange },
        { "On", QCheckListItem::On },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );

}

QCheckListItem *QCheckListItemImp::toQCheckListItem( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QCheckListItem *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;
    return op->toNative<QCheckListItem>();
}


KJS::Object QCheckListItemImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QCheckListItem_1:
             return QCheckListItem_1( exec, args );
             break;

         case Constructor_QCheckListItem_2:
             return QCheckListItem_2( exec, args );
             break;

         case Constructor_QCheckListItem_3:
             return QCheckListItem_3( exec, args );
             break;

         case Constructor_QCheckListItem_4:
             return QCheckListItem_4( exec, args );
             break;

         case Constructor_QCheckListItem_5:
             return QCheckListItem_5( exec, args );
             break;

         case Constructor_QCheckListItem_6:
             return QCheckListItem_6( exec, args );
             break;

         case Constructor_QCheckListItem_7:
             return QCheckListItem_7( exec, args );
             break;

         case Constructor_QCheckListItem_8:
             return QCheckListItem_8( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QCheckListItemCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QCheckListItemImp::QCheckListItem_1( KJS::ExecState *exec, const KJS::List &args )
{

    QWidget *arg0 = extractQWidget(exec,args,0);

    QWidget *arg1 = extractQWidget(exec,args,1);

    QWidget *arg2 = extractQWidget(exec,args,2);

    return KJS::Object(  );
}

KJS::Object QCheckListItemImp::QCheckListItem_2( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg3 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_3( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_4( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg3 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_5( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_6( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg3 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_7( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Object QCheckListItemImp::QCheckListItem_8( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg1 = extractQWidget(exec,args,0);

    // TODO
    QWidget *arg2 = extractQWidget(exec,args,0);
    return KJS::Object();

}

KJS::Value QCheckListItemImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QCheckListItemImp::toQCheckListItem( self );

    switch( id ) {

    case Method_paintCell_10:
        return paintCell_10( exec, self, args );
        break;

    case Method_paintFocus_11:
        return paintFocus_11( exec, self, args );
        break;

    case Method_width_12:
        return width_12( exec, self, args );
        break;

    case Method_setup_13:
        return setup_13( exec, self, args );
        break;

    case Method_setOn_14:
        return setOn_14( exec, self, args );
        break;

    case Method_isOn_15:
        return isOn_15( exec, self, args );
        break;

    case Method_type_16:
        return type_16( exec, self, args );
        break;

    case Method_text_17:
        return text_17( exec, self, args );
        break;

    case Method_text_18:
        return text_18( exec, self, args );
        break;

    case Method_setTristate_19:
        return setTristate_19( exec, self, args );
        break;

    case Method_isTristate_20:
        return isTristate_20( exec, self, args );
        break;

    case Method_state_21:
        return state_21( exec, self, args );
        break;

    case Method_setState_22:
        return setState_22( exec, self, args );
        break;

    case Method_rtti_23:
        return rtti_23( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QCheckListItemImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QCheckListItemImp::paintCell_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QPainter *
    return KJS::Value();

    QPainter * arg0; // Dummy

    // Unsupported parameter const QColorGroup &
    return KJS::Value();
#if 0
    const QColorGroup & arg1; // Dummy

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

    int arg3 = (args.size() >= 4) ? args[3].toInteger(exec) : -1;

    int arg4 = (args.size() >= 5) ? args[4].toInteger(exec) : -1;

      instance->paintCell(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4 );
      return KJS::Value(); // Returns void
#endif
}

KJS::Value QCheckListItemImp::paintFocus_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QPainter *
    return KJS::Value();
#if 0
    QPainter * arg0; // Dummy

    // Unsupported parameter const QColorGroup &
    return KJS::Value();

    const QColorGroup & arg1; // Dummy

    QRect arg2 = extractQRect(exec, args, 2);

      instance->paintFocus(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void
#endif
}

KJS::Value QCheckListItemImp::width_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QFontMetrics &
    return KJS::Value();
#if 0
    const QFontMetrics & arg0; // Dummy

    // Unsupported parameter const QListView *
    return KJS::Value();

    const QListView * arg1; // Dummy

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

      int ret;
      ret = instance->width(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );
#endif
}

KJS::Value QCheckListItemImp::setup_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->setup(  );
      return KJS::Value(); // Returns void

}

KJS::Value QCheckListItemImp::setOn_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setOn(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QCheckListItemImp::isOn_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isOn(  );
      return KJS::Boolean( ret );

}

KJS::Value QCheckListItemImp::type_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->type(  );
      return KJS::Value(); // Returns 'Type'

}

KJS::Value QCheckListItemImp::text_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->text(  );
      return KJS::String( ret );

}

KJS::Value QCheckListItemImp::text_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      QString ret;
      ret = instance->text(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QCheckListItemImp::setTristate_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setTristate(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QCheckListItemImp::isTristate_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isTristate(  );
      return KJS::Boolean( ret );

}

KJS::Value QCheckListItemImp::state_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->state(  );
      return KJS::Value(); // Returns 'ToggleState'

}

KJS::Value QCheckListItemImp::setState_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter ToggleState
    return KJS::Value();
#if 0
    ToggleState arg0; // Dummy

      instance->setState(
       arg0 );
      return KJS::Value(); // Returns void
#endif
}

KJS::Value QCheckListItemImp::rtti_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->rtti(  );
      return KJS::Number( ret );

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


