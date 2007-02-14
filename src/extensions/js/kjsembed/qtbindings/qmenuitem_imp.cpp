


#include <qcstring.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>

#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qmenudata.h>
#include "qmenuitem_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QMenuItemImp::QMenuItemImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QMenuItemImp::~QMenuItemImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QMenuItemImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QMenuItemImp *meth = new QMenuItemImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QMenuItemImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {
    { Method_id_3, "id" },
    { Method_iconSet_4, "iconSet" },
    { Method_text_5, "text" },
    { Method_whatsThis_6, "whatsThis" },
    { Method_pixmap_7, "pixmap" },
    { Method_popup_8, "popup" },
    { Method_widget_9, "widget" },
    { Method_custom_10, "custom" },
    { Method_key_11, "key" },
    { Method_signal_12, "signal" },
    { Method_isSeparator_13, "isSeparator" },
    { Method_isEnabled_14, "isEnabled" },
    { Method_isChecked_15, "isChecked" },
    { Method_isDirty_16, "isDirty" },
    { Method_isVisible_17, "isVisible" },
    { Method_isEnabledAndVisible_18, "isEnabledAndVisible" },
    { Method_setText_19, "setText" },
    { Method_setDirty_20, "setDirty" },
    { Method_setVisible_21, "setVisible" },
    { Method_setWhatsThis_22, "setWhatsThis" },
    { 0, 0 }
    };
    
    JSProxy::addMethods<QMenuItemImp>(exec, methods, object );

}

/**
 * Extract a QMenuItem pointer from an Object.
 */
QMenuItem *QMenuItemImp::toQMenuItem( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
    if ( obj )
           return dynamic_cast<QMenuItem *>( obj );
    }

    if( !JSProxy::checkType(self, JSProxy::ObjectProxy, "QMenuItem") ) {
        QObject *obj = JSProxy::toObjectProxy( self.imp() )->object();
        return dynamic_cast<QMenuItem *>( obj );
    }
    
    if( !JSProxy::checkType(self, JSProxy::OpaqueProxy, "QMenuItem") ) {
        return JSProxy::toOpaqueProxy( self.imp() )->toNative<QMenuItem>();
    }
    return 0;
    
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QMenuItemImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QMenuItem_1:
             return QMenuItem_1( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QMenuItemCons has no constructor with id '%1'").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QMenuItemImp::QMenuItem_1( KJS::ExecState *exec, const KJS::List &args )
{


    // We should now create an instance of the QMenuItem object

    QMenuItem *ret = new QMenuItem(
        );


    return KJS::Object();
}

KJS::Value QMenuItemImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QMenuItemImp::toQMenuItem( self );

    switch( id ) {

    case Method_id_3:
        return id_3( exec, self, args );
        break;

    case Method_iconSet_4:
        return iconSet_4( exec, self, args );
        break;

    case Method_text_5:
        return text_5( exec, self, args );
        break;

    case Method_whatsThis_6:
        return whatsThis_6( exec, self, args );
        break;

    case Method_pixmap_7:
        return pixmap_7( exec, self, args );
        break;

    case Method_popup_8:
        return popup_8( exec, self, args );
        break;

    case Method_widget_9:
        return widget_9( exec, self, args );
        break;

    case Method_custom_10:
        return custom_10( exec, self, args );
        break;

    case Method_key_11:
        return key_11( exec, self, args );
        break;

    case Method_signal_12:
        return signal_12( exec, self, args );
        break;

    case Method_isSeparator_13:
        return isSeparator_13( exec, self, args );
        break;

    case Method_isEnabled_14:
        return isEnabled_14( exec, self, args );
        break;

    case Method_isChecked_15:
        return isChecked_15( exec, self, args );
        break;

    case Method_isDirty_16:
        return isDirty_16( exec, self, args );
        break;

    case Method_isVisible_17:
        return isVisible_17( exec, self, args );
        break;

    case Method_isEnabledAndVisible_18:
        return isEnabledAndVisible_18( exec, self, args );
        break;

    case Method_setText_19:
        return setText_19( exec, self, args );
        break;

    case Method_setDirty_20:
        return setDirty_20( exec, self, args );
        break;

    case Method_setVisible_21:
        return setVisible_21( exec, self, args );
        break;

    case Method_setWhatsThis_22:
        return setWhatsThis_22( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QMenuItemImp has no method with id '%1'" ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QMenuItemImp::id_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->id(  );
      return KJS::Number( ret );

}

KJS::Value QMenuItemImp::iconSet_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->iconSet(  );
      return KJS::Value(); // Returns 'QIconSet *'

}

KJS::Value QMenuItemImp::text_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->text(  );
      return KJS::String( ret );

}

KJS::Value QMenuItemImp::whatsThis_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->whatsThis(  );
      return KJS::String( ret );

}

KJS::Value QMenuItemImp::pixmap_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    return convertToValue(exec, *instance->pixmap( ));

}

KJS::Value QMenuItemImp::popup_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->popup(  );
      return KJS::Value(); // Returns 'QPopupMenu *'

}

KJS::Value QMenuItemImp::widget_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->widget(  );
      return KJS::Value(); // Returns 'QWidget *'

}

KJS::Value QMenuItemImp::custom_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->custom(  );
      return KJS::Value(); // Returns 'QCustomMenuItem *'

}

KJS::Value QMenuItemImp::key_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    return convertToValue( exec, instance->key(  ));

}

KJS::Value QMenuItemImp::signal_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->signal(  );
      return KJS::Value(); // Returns 'QSignal *'

}

KJS::Value QMenuItemImp::isSeparator_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isSeparator(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::isEnabled_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isEnabled(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::isChecked_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isChecked(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::isDirty_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isDirty(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::isVisible_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isVisible(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::isEnabledAndVisible_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isEnabledAndVisible(  );
      return KJS::Boolean( ret );

}

KJS::Value QMenuItemImp::setText_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

      instance->setText(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuItemImp::setDirty_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setDirty(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuItemImp::setVisible_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setVisible(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuItemImp::setWhatsThis_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

      instance->setWhatsThis(
       arg0 );
      return KJS::Value(); // Returns void

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


