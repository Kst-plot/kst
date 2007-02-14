


#include <qcstring.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>
#include <qkeysequence.h>
#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qmenudata.h>
#include "qmenudata_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QMenuDataImp::QMenuDataImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QMenuDataImp::~QMenuDataImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QMenuDataImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QMenuDataImp *meth = new QMenuDataImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QMenuDataImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_count_3, "count" },
        { Method_insertItem_4, "insertItem" },
        { Method_insertItem_5, "insertItem" },
        { Method_insertItem_6, "insertItem" },
        { Method_insertItem_7, "insertItem" },
        { Method_insertItem_8, "insertItem" },
        { Method_insertItem_9, "insertItem" },
        { Method_insertItem_10, "insertItem" },
        { Method_insertItem_11, "insertItem" },
        { Method_insertItem_12, "insertItem" },
        { Method_insertItem_13, "insertItem" },
        { Method_insertItem_14, "insertItem" },
        { Method_insertItem_15, "insertItem" },
        { Method_insertItem_16, "insertItem" },
        { Method_insertItem_17, "insertItem" },
        { Method_insertItem_18, "insertItem" },
        { Method_insertSeparator_19, "insertSeparator" },
        { Method_removeItem_20, "removeItem" },
        { Method_removeItemAt_21, "removeItemAt" },
        { Method_clear_22, "clear" },
        { Method_accel_23, "accel" },
        { Method_setAccel_24, "setAccel" },
        { Method_iconSet_25, "iconSet" },
        { Method_text_26, "text" },
        { Method_pixmap_27, "pixmap" },
        { Method_setWhatsThis_28, "setWhatsThis" },
        { Method_whatsThis_29, "whatsThis" },
        { Method_changeItem_30, "changeItem" },
        { Method_changeItem_31, "changeItem" },
        { Method_changeItem_32, "changeItem" },
        { Method_changeItem_33, "changeItem" },
        { Method_changeItem_34, "changeItem" },
        { Method_changeItem_35, "changeItem" },
        { Method_changeItem_36, "changeItem" },
        { Method_isItemActive_37, "isItemActive" },
        { Method_isItemEnabled_38, "isItemEnabled" },
        { Method_setItemEnabled_39, "setItemEnabled" },
        { Method_isItemChecked_40, "isItemChecked" },
        { Method_setItemChecked_41, "setItemChecked" },
        { Method_isItemVisible_42, "isItemVisible" },
        { Method_setItemVisible_43, "setItemVisible" },
        { Method_updateItem_44, "updateItem" },
        { Method_indexOf_45, "indexOf" },
        { Method_idAt_46, "idAt" },
        { Method_setId_47, "setId" },
        { Method_connectItem_48, "connectItem" },
        { Method_disconnectItem_49, "disconnectItem" },
        { Method_setItemParameter_50, "setItemParameter" },
        { Method_itemParameter_51, "itemParameter" },
        { Method_findItem_52, "findItem" },
        { Method_findItem_53, "findItem" },
        { Method_findPopup_54, "findPopup" },
        { Method_activateItemAt_55, "activateItemAt" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QMenuDataImp *meth = new QMenuDataImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a QMenuData pointer from an Object.
 */
QMenuData *QMenuDataImp::toQMenuData( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QMenuData *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "QMenuData" )
        return 0;

    return op->toNative<QMenuData>();
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QMenuDataImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QMenuData_1:
             return QMenuData_1( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QMenuDataCons has no constructor with id '%1'").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QMenuDataImp::QMenuData_1( KJS::ExecState *exec, const KJS::List &args )
{


    // We should now create an instance of the QMenuData object

    QMenuData *ret = new QMenuData(
        );

    return KJS::Object();
}

KJS::Value QMenuDataImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QMenuDataImp::toQMenuData( self );

    switch( id ) {

    case Method_count_3:
        return count_3( exec, self, args );
        break;

    case Method_insertItem_4:
        return insertItem_4( exec, self, args );
        break;

    case Method_insertItem_5:
        return insertItem_5( exec, self, args );
        break;

    case Method_insertItem_6:
        return insertItem_6( exec, self, args );
        break;

    case Method_insertItem_7:
        return insertItem_7( exec, self, args );
        break;

    case Method_insertItem_8:
        return insertItem_8( exec, self, args );
        break;

    case Method_insertItem_9:
        return insertItem_9( exec, self, args );
        break;

    case Method_insertItem_10:
        return insertItem_10( exec, self, args );
        break;

    case Method_insertItem_11:
        return insertItem_11( exec, self, args );
        break;

    case Method_insertItem_12:
        return insertItem_12( exec, self, args );
        break;

    case Method_insertItem_13:
        return insertItem_13( exec, self, args );
        break;

    case Method_insertItem_14:
        return insertItem_14( exec, self, args );
        break;

    case Method_insertItem_15:
        return insertItem_15( exec, self, args );
        break;

    case Method_insertItem_16:
        return insertItem_16( exec, self, args );
        break;

    case Method_insertItem_17:
        return insertItem_17( exec, self, args );
        break;

    case Method_insertItem_18:
        return insertItem_18( exec, self, args );
        break;

    case Method_insertSeparator_19:
        return insertSeparator_19( exec, self, args );
        break;

    case Method_removeItem_20:
        return removeItem_20( exec, self, args );
        break;

    case Method_removeItemAt_21:
        return removeItemAt_21( exec, self, args );
        break;

    case Method_clear_22:
        return clear_22( exec, self, args );
        break;

    case Method_accel_23:
        return accel_23( exec, self, args );
        break;

    case Method_setAccel_24:
        return setAccel_24( exec, self, args );
        break;

    case Method_iconSet_25:
        return iconSet_25( exec, self, args );
        break;

    case Method_text_26:
        return text_26( exec, self, args );
        break;

    case Method_pixmap_27:
        return pixmap_27( exec, self, args );
        break;

    case Method_setWhatsThis_28:
        return setWhatsThis_28( exec, self, args );
        break;

    case Method_whatsThis_29:
        return whatsThis_29( exec, self, args );
        break;

    case Method_changeItem_30:
        return changeItem_30( exec, self, args );
        break;

    case Method_changeItem_31:
        return changeItem_31( exec, self, args );
        break;

    case Method_changeItem_32:
        return changeItem_32( exec, self, args );
        break;

    case Method_changeItem_33:
        return changeItem_33( exec, self, args );
        break;

    case Method_changeItem_34:
        return changeItem_34( exec, self, args );
        break;

    case Method_changeItem_35:
        return changeItem_35( exec, self, args );
        break;

    case Method_changeItem_36:
        return changeItem_36( exec, self, args );
        break;

    case Method_isItemActive_37:
        return isItemActive_37( exec, self, args );
        break;

    case Method_isItemEnabled_38:
        return isItemEnabled_38( exec, self, args );
        break;

    case Method_setItemEnabled_39:
        return setItemEnabled_39( exec, self, args );
        break;

    case Method_isItemChecked_40:
        return isItemChecked_40( exec, self, args );
        break;

    case Method_setItemChecked_41:
        return setItemChecked_41( exec, self, args );
        break;

    case Method_isItemVisible_42:
        return isItemVisible_42( exec, self, args );
        break;

    case Method_setItemVisible_43:
        return setItemVisible_43( exec, self, args );
        break;

    case Method_updateItem_44:
        return updateItem_44( exec, self, args );
        break;

    case Method_indexOf_45:
        return indexOf_45( exec, self, args );
        break;

    case Method_idAt_46:
        return idAt_46( exec, self, args );
        break;

    case Method_setId_47:
        return setId_47( exec, self, args );
        break;

    case Method_connectItem_48:
        return connectItem_48( exec, self, args );
        break;

    case Method_disconnectItem_49:
        return disconnectItem_49( exec, self, args );
        break;

    case Method_setItemParameter_50:
        return setItemParameter_50( exec, self, args );
        break;

    case Method_itemParameter_51:
        return itemParameter_51( exec, self, args );
        break;

    case Method_findItem_52:
        return findItem_52( exec, self, args );
        break;

    case Method_findItem_53:
        return findItem_53( exec, self, args );
        break;

    case Method_findPopup_54:
        return findPopup_54( exec, self, args );
        break;

    case Method_activateItemAt_55:
        return activateItemAt_55( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QMenuDataImp has no method with id '%1'" ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QMenuDataImp::count_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      uint ret;
      ret = instance->count(  );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg1; // Dummy

    const char *arg2 = (args.size() >= 3) ? args[2].toString(exec).ascii() : 0;

    // Unsupported parameter const QKeySequence
    return KJS::Value();

    const QKeySequence  arg3; // Dummy

    int arg4 = extractInt(exec, args, 4);

    int arg5 = extractInt(exec, args, 5);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4,
       arg5 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg2; // Dummy

    const char *arg3 = (args.size() >= 4) ? args[3].toString(exec).ascii() : 0;

    // Unsupported parameter const QKeySequence
    return KJS::Value();

    const QKeySequence arg4; // Dummy

    int arg5 = extractInt(exec, args, 5);

    int arg6 = extractInt(exec, args, 6);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4,
       arg5,
       arg6 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg1; // Dummy

    const char *arg2 = (args.size() >= 3) ? args[2].toString(exec).ascii() : 0;

    // Unsupported parameter const QKeySequence
    return KJS::Value();

    const QKeySequence  arg3; // Dummy

    int arg4 = extractInt(exec, args, 4);

    int arg5 = extractInt(exec, args, 5);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4,
       arg5 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QPixmap arg1 = extractQPixmap(exec, args, 1);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg2; // Dummy

    const char *arg3 = (args.size() >= 4) ? args[3].toString(exec).ascii() : 0;

    // Unsupported parameter const QKeySequence
    return KJS::Value();

    const QKeySequence arg4; // Dummy

    int arg5 = extractInt(exec, args, 5);

    int arg6 = extractInt(exec, args, 6);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4,
       arg5,
       arg6 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    // Unsupported parameter QPopupMenu *
    return KJS::Value();

    QPopupMenu * arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    // Unsupported parameter QPopupMenu *
    return KJS::Value();

    QPopupMenu * arg2; // Dummy

    int arg3 = extractInt(exec, args, 3);

    int arg4 = extractInt(exec, args, 4);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QPixmap arg1 = extractQPixmap(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    // Unsupported parameter QPopupMenu *
    return KJS::Value();

    QPopupMenu * arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QPixmap arg1 = extractQPixmap(exec, args, 1);

    // Unsupported parameter QPopupMenu *
    return KJS::Value();

    QPopupMenu * arg2; // Dummy

    int arg3 = extractInt(exec, args, 3);

    int arg4 = extractInt(exec, args, 4);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QWidget *
    return KJS::Value();

    QWidget * arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    // Unsupported parameter QCustomMenuItem *
    return KJS::Value();

    QCustomMenuItem * arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2,
       arg3 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertItem_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QCustomMenuItem *
    return KJS::Value();

    QCustomMenuItem * arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      int ret;
      ret = instance->insertItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::insertSeparator_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      int ret;
      ret = instance->insertSeparator(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::removeItem_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->removeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::removeItemAt_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->removeItemAt(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::clear_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->clear(  );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::accel_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->accel(
       arg0 );
      return KJS::Value(); // Returns 'QKeySequence'

}

KJS::Value QMenuDataImp::setAccel_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QKeySequence
    return KJS::Value();

    const QKeySequence  arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

      instance->setAccel(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::iconSet_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->iconSet(
       arg0 );
      return KJS::Value(); // Returns 'QIconSet *'

}

KJS::Value QMenuDataImp::text_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      QString ret;
      ret = instance->text(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QMenuDataImp::pixmap_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->pixmap(
       arg0 );
      return KJS::Value(); // Returns 'QPixmap *'

}

KJS::Value QMenuDataImp::setWhatsThis_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    QString arg1 = extractQString(exec, args, 1);

      instance->setWhatsThis(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::whatsThis_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      QString ret;
      ret = instance->whatsThis(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QMenuDataImp::changeItem_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    QString arg1 = extractQString(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    QPixmap arg1 = extractQPixmap(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet  arg1; // Dummy

    QString arg2 = extractQString(exec, args, 2);

      instance->changeItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet  arg1; // Dummy

    QPixmap arg2 = extractQPixmap(exec, args, 2);

      instance->changeItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::changeItem_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      instance->changeItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::isItemActive_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      bool ret;
      ret = instance->isItemActive(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::isItemEnabled_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      bool ret;
      ret = instance->isItemEnabled(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::setItemEnabled_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      instance->setItemEnabled(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::isItemChecked_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      bool ret;
      ret = instance->isItemChecked(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::setItemChecked_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      instance->setItemChecked(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::isItemVisible_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      bool ret;
      ret = instance->isItemVisible(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::setItemVisible_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      instance->setItemVisible(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::updateItem_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->updateItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::indexOf_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      int ret;
      ret = instance->indexOf(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::idAt_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      int ret;
      ret = instance->idAt(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::setId_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->setId(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QMenuDataImp::connectItem_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg1; // Dummy

    const char *arg2 = (args.size() >= 3) ? args[2].toString(exec).ascii() : 0;

      bool ret;
      ret = instance->connectItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::disconnectItem_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    // Unsupported parameter const QObject *
    return KJS::Value();

    const QObject * arg1; // Dummy

    const char *arg2 = (args.size() >= 3) ? args[2].toString(exec).ascii() : 0;

      bool ret;
      ret = instance->disconnectItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::setItemParameter_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      bool ret;
      ret = instance->setItemParameter(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QMenuDataImp::itemParameter_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      int ret;
      ret = instance->itemParameter(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QMenuDataImp::findItem_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->findItem(
       arg0 );
      return KJS::Value(); // Returns 'QMenuItem *'

}

KJS::Value QMenuDataImp::findItem_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    // Unsupported parameter QMenuData **
    return KJS::Value();

    QMenuData ** arg1; // Dummy

      instance->findItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'QMenuItem *'

}

KJS::Value QMenuDataImp::findPopup_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QPopupMenu *
    return KJS::Value();

    QPopupMenu * arg0; // Dummy

    // Unsupported parameter int *
    return KJS::Value();

    int * arg1; // Dummy

      instance->findPopup(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'QMenuItem *'

}

KJS::Value QMenuDataImp::activateItemAt_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->activateItemAt(
       arg0 );
      return KJS::Value(); // Returns void

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


