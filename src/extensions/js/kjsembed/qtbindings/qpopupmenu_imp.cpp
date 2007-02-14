


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

#include <qpopupmenu.h>
#include "qpopupmenu_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QPopupMenuImp::QPopupMenuImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QPopupMenuImp::~QPopupMenuImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QPopupMenuImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QPopupMenuImp *meth = new QPopupMenuImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QPopupMenuImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_popup_3, "popup" },
        { Method_updateItem_4, "updateItem" },
        { Method_setCheckable_5, "setCheckable" },
        { Method_isCheckable_6, "isCheckable" },
        { Method_setFont_7, "setFont" },
        { Method_show_8, "show" },
        { Method_hide_9, "hide" },
        { Method_exec_10, "exec" },
        { Method_exec_11, "exec" },
        { Method_setActiveItem_12, "setActiveItem" },
        { Method_sizeHint_13, "sizeHint" },
        { Method_idAt_14, "idAt" },
        { Method_idAt_15, "idAt" },
        { Method_customWhatsThis_16, "customWhatsThis" },
        { Method_insertTearOffHandle_17, "insertTearOffHandle" },
        { Method_activateItemAt_18, "activateItemAt" },
        { Method_itemGeometry_19, "itemGeometry" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QPopupMenuImp *meth = new QPopupMenuImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a QPopupMenu pointer from an Object.
 */
QPopupMenu *QPopupMenuImp::toQPopupMenu( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QPopupMenu *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "QPopupMenu" )
        return 0;

    return op->toNative<QPopupMenu>();
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QPopupMenuImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QPopupMenu_1:
             return QPopupMenu_1( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QPopupMenuCons has no constructor with id '%1'").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QPopupMenuImp::QPopupMenu_1( KJS::ExecState *exec, const KJS::List &args )
{

    // Unsupported parameter QWidget *
    return KJS::Object();

    QWidget * arg0; // Dummy

    const char *arg1 = (args.size() >= 2) ? args[1].toString(exec).ascii() : 0;


    // We should now create an instance of the QPopupMenu object

    QPopupMenu *ret = new QPopupMenu(

          arg0,
          arg1 );


}

KJS::Value QPopupMenuImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QPopupMenuImp::toQPopupMenu( self );

    switch( id ) {

    case Method_popup_3:
        return popup_3( exec, self, args );
        break;

    case Method_updateItem_4:
        return updateItem_4( exec, self, args );
        break;

    case Method_setCheckable_5:
        return setCheckable_5( exec, self, args );
        break;

    case Method_isCheckable_6:
        return isCheckable_6( exec, self, args );
        break;

    case Method_setFont_7:
        return setFont_7( exec, self, args );
        break;

    case Method_show_8:
        return show_8( exec, self, args );
        break;

    case Method_hide_9:
        return hide_9( exec, self, args );
        break;

    case Method_exec_10:
        return exec_10( exec, self, args );
        break;

    case Method_exec_11:
        return exec_11( exec, self, args );
        break;

    case Method_setActiveItem_12:
        return setActiveItem_12( exec, self, args );
        break;

    case Method_sizeHint_13:
        return sizeHint_13( exec, self, args );
        break;

    case Method_idAt_14:
        return idAt_14( exec, self, args );
        break;

    case Method_idAt_15:
        return idAt_15( exec, self, args );
        break;

    case Method_customWhatsThis_16:
        return customWhatsThis_16( exec, self, args );
        break;

    case Method_insertTearOffHandle_17:
        return insertTearOffHandle_17( exec, self, args );
        break;

    case Method_activateItemAt_18:
        return activateItemAt_18( exec, self, args );
        break;

    case Method_itemGeometry_19:
        return itemGeometry_19( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QPopupMenuImp has no method with id '%1'" ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QPopupMenuImp::popup_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPoint arg0 = extractQPoint(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->popup(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::updateItem_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->updateItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::setCheckable_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setCheckable(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::isCheckable_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isCheckable(  );
      return KJS::Boolean( ret );

}

KJS::Value QPopupMenuImp::setFont_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFont arg0 = extractQFont(exec, args, 0);

      instance->setFont(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::show_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->show(  );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::hide_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->hide(  );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::exec_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->exec(  );
      return KJS::Number( ret );

}

KJS::Value QPopupMenuImp::exec_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPoint arg0 = extractQPoint(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      int ret;
      ret = instance->exec(
       arg0,
       arg1 );
      return KJS::Number( ret );

}

KJS::Value QPopupMenuImp::setActiveItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->setActiveItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::sizeHint_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
    QSize ret;
      ret = instance->sizeHint(  );

       return convertToValue( exec, ret );

}

KJS::Value QPopupMenuImp::idAt_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      int ret;
      ret = instance->idAt(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QPopupMenuImp::idAt_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPoint arg0 = extractQPoint(exec, args, 0);

      int ret;
      ret = instance->idAt(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QPopupMenuImp::customWhatsThis_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->customWhatsThis(  );
      return KJS::Boolean( ret );

}

KJS::Value QPopupMenuImp::insertTearOffHandle_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      int ret;
      ret = instance->insertTearOffHandle(
       arg0,
       arg1 );
      return KJS::Number( ret );

}

KJS::Value QPopupMenuImp::activateItemAt_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->activateItemAt(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QPopupMenuImp::itemGeometry_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);
        QRect ret;
      ret = instance->itemGeometry(
       arg0 );

       return convertToValue( exec, ret );

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


