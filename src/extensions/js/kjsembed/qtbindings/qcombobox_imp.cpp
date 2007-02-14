


#include <qcstring.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>

#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qcombobox.h>
#include "qcombobox_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QComboBoxImp::QComboBoxImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QComboBoxImp::~QComboBoxImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QComboBoxImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QComboBoxImp *meth = new QComboBoxImp( exec, methods[idx].id );
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

        // enum Policy
        { "NoInsertion", QComboBox::NoInsertion },
        { "AtTop", QComboBox::AtTop },
        { "AtCurrent", QComboBox::AtCurrent },
        { "AtBottom", QComboBox::AtBottom },
        { "AfterCurrent", QComboBox::AfterCurrent },
        { "BeforeCurrent", QComboBox::BeforeCurrent },
	{ 0, 0 }
    };

    int enumidx = 0;
    while( enums[enumidx].id ) {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    }

}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QComboBoxImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_count_4, "count" },
        { Method_insertStringList_5, "insertStringList" },
        { Method_insertStrList_6, "insertStrList" },
        { Method_insertStrList_7, "insertStrList" },
        { Method_insertStrList_8, "insertStrList" },
        { Method_insertItem_9, "insertItem" },
        { Method_insertItem_10, "insertItem" },
        { Method_insertItem_11, "insertItem" },
        { Method_removeItem_12, "removeItem" },
        { Method_currentItem_13, "currentItem" },
        { Method_setCurrentItem_14, "setCurrentItem" },
        { Method_currentText_15, "currentText" },
        { Method_setCurrentText_16, "setCurrentText" },
        { Method_text_17, "text" },
        { Method_pixmap_18, "pixmap" },
        { Method_changeItem_19, "changeItem" },
        { Method_changeItem_20, "changeItem" },
        { Method_changeItem_21, "changeItem" },
        { Method_autoResize_22, "autoResize" },
        { Method_setAutoResize_23, "setAutoResize" },
        { Method_sizeHint_24, "sizeHint" },
        { Method_setPalette_25, "setPalette" },
        { Method_setFont_26, "setFont" },
        { Method_setEnabled_27, "setEnabled" },
        { Method_setSizeLimit_28, "setSizeLimit" },
        { Method_sizeLimit_29, "sizeLimit" },
        { Method_setMaxCount_30, "setMaxCount" },
        { Method_maxCount_31, "maxCount" },
        { Method_setInsertionPolicy_32, "setInsertionPolicy" },
        { Method_insertionPolicy_33, "insertionPolicy" },
        { Method_setValidator_34, "setValidator" },
        { Method_validator_35, "validator" },
        { Method_setListBox_36, "setListBox" },
        { Method_listBox_37, "listBox" },
        { Method_setLineEdit_38, "setLineEdit" },
        { Method_lineEdit_39, "lineEdit" },
        { Method_setAutoCompletion_40, "setAutoCompletion" },
        { Method_autoCompletion_41, "autoCompletion" },
        { Method_eventFilter_42, "eventFilter" },
        { Method_setDuplicatesEnabled_43, "setDuplicatesEnabled" },
        { Method_duplicatesEnabled_44, "duplicatesEnabled" },
        { Method_editable_45, "editable" },
        { Method_setEditable_46, "setEditable" },
        { Method_popup_47, "popup" },
        { Method_hide_48, "hide" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QComboBoxImp *meth = new QComboBoxImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a QComboBox pointer from an Object.
 */
QComboBox *QComboBoxImp::toQComboBox( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QComboBox *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "QComboBox" )
        return 0;

    return op->toNative<QComboBox>();
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QComboBoxImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QComboBox_1:
             return QComboBox_1( exec, args );
             break;

         case Constructor_QComboBox_2:
             return QComboBox_2( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QComboBoxCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QComboBoxImp::QComboBox_1( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    // Unsupported parameter QWidget *
    return KJS::Value();

    QWidget * arg0; // Dummy

    const char *arg1 = (args.size() >= 2) ? args[1].toString(exec).ascii() : 0;


    // We should now create an object of type QComboBoxQComboBox *ret = new QComboBox(

          arg0,
          arg1 );
#endif
return KJS::Object();
}

KJS::Object QComboBoxImp::QComboBox_2( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    bool arg0 = extractBool(exec, args, 0);

    // Unsupported parameter QWidget *
    return KJS::Value();

    QWidget * arg1; // Dummy

    const char *arg2 = (args.size() >= 3) ? args[2].toString(exec).ascii() : 0;


    // We should now create an object of type QComboBoxQComboBox *ret = new QComboBox(

          arg0,
          arg1,
          arg2 );
#endif
return KJS::Object();

}

KJS::Value QComboBoxImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QComboBoxImp::toQComboBox( self );

    switch( id ) {

    case Method_count_4:
        return count_4( exec, self, args );
        break;

    case Method_insertStringList_5:
        return insertStringList_5( exec, self, args );
        break;

    case Method_insertStrList_6:
        return insertStrList_6( exec, self, args );
        break;

    case Method_insertStrList_7:
        return insertStrList_7( exec, self, args );
        break;

    case Method_insertStrList_8:
        return insertStrList_8( exec, self, args );
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

    case Method_removeItem_12:
        return removeItem_12( exec, self, args );
        break;

    case Method_currentItem_13:
        return currentItem_13( exec, self, args );
        break;

    case Method_setCurrentItem_14:
        return setCurrentItem_14( exec, self, args );
        break;

    case Method_currentText_15:
        return currentText_15( exec, self, args );
        break;

    case Method_setCurrentText_16:
        return setCurrentText_16( exec, self, args );
        break;

    case Method_text_17:
        return text_17( exec, self, args );
        break;

    case Method_pixmap_18:
        return pixmap_18( exec, self, args );
        break;

    case Method_changeItem_19:
        return changeItem_19( exec, self, args );
        break;

    case Method_changeItem_20:
        return changeItem_20( exec, self, args );
        break;

    case Method_changeItem_21:
        return changeItem_21( exec, self, args );
        break;

    case Method_autoResize_22:
        return autoResize_22( exec, self, args );
        break;

    case Method_setAutoResize_23:
        return setAutoResize_23( exec, self, args );
        break;

    case Method_sizeHint_24:
        return sizeHint_24( exec, self, args );
        break;

    case Method_setPalette_25:
        return setPalette_25( exec, self, args );
        break;

    case Method_setFont_26:
        return setFont_26( exec, self, args );
        break;

    case Method_setEnabled_27:
        return setEnabled_27( exec, self, args );
        break;

    case Method_setSizeLimit_28:
        return setSizeLimit_28( exec, self, args );
        break;

    case Method_sizeLimit_29:
        return sizeLimit_29( exec, self, args );
        break;

    case Method_setMaxCount_30:
        return setMaxCount_30( exec, self, args );
        break;

    case Method_maxCount_31:
        return maxCount_31( exec, self, args );
        break;

    case Method_setInsertionPolicy_32:
        return setInsertionPolicy_32( exec, self, args );
        break;

    case Method_insertionPolicy_33:
        return insertionPolicy_33( exec, self, args );
        break;

    case Method_setValidator_34:
        return setValidator_34( exec, self, args );
        break;

    case Method_validator_35:
        return validator_35( exec, self, args );
        break;

    case Method_setListBox_36:
        return setListBox_36( exec, self, args );
        break;

    case Method_listBox_37:
        return listBox_37( exec, self, args );
        break;

    case Method_setLineEdit_38:
        return setLineEdit_38( exec, self, args );
        break;

    case Method_lineEdit_39:
        return lineEdit_39( exec, self, args );
        break;

    case Method_setAutoCompletion_40:
        return setAutoCompletion_40( exec, self, args );
        break;

    case Method_autoCompletion_41:
        return autoCompletion_41( exec, self, args );
        break;

    case Method_eventFilter_42:
        return eventFilter_42( exec, self, args );
        break;

    case Method_setDuplicatesEnabled_43:
        return setDuplicatesEnabled_43( exec, self, args );
        break;

    case Method_duplicatesEnabled_44:
        return duplicatesEnabled_44( exec, self, args );
        break;

    case Method_editable_45:
        return editable_45( exec, self, args );
        break;

    case Method_setEditable_46:
        return setEditable_46( exec, self, args );
        break;

    case Method_popup_47:
        return popup_47( exec, self, args );
        break;

    case Method_hide_48:
        return hide_48( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QComboBoxImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QComboBoxImp::count_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->count(  );
      return KJS::Number( ret );

}

KJS::Value QComboBoxImp::insertStringList_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QStringList arg0 = extractQStringList(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->insertStringList(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertStrList_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QStrList arg0 = extractQStrList(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->insertStrList(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertStrList_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QStrList *
    return KJS::Value();

    const QStrList * arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

      instance->insertStrList(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertStrList_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const char **
    return KJS::Value();

    const char ** arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      instance->insertStrList(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertItem_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->insertItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->insertItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    QString arg1 = extractQString(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      instance->insertItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::removeItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->removeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::currentItem_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->currentItem(  );
      return KJS::Number( ret );

}

KJS::Value QComboBoxImp::setCurrentItem_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->setCurrentItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::currentText_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->currentText(  );
      return KJS::String( ret );

}

KJS::Value QComboBoxImp::setCurrentText_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

      instance->setCurrentText(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::text_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      QString ret;
      ret = instance->text(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QComboBoxImp::pixmap_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->pixmap(
       arg0 );
      return KJS::Value(); // Returns 'const QPixmap *'

}

KJS::Value QComboBoxImp::changeItem_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = extractQString(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::changeItem_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    int arg1 = extractInt(exec, args, 1);

      instance->changeItem(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::changeItem_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPixmap arg0 = extractQPixmap(exec, args, 0);

    QString arg1 = extractQString(exec, args, 1);

    int arg2 = extractInt(exec, args, 2);

      instance->changeItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::autoResize_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->autoResize(  );
      return KJS::Boolean( ret );

}

KJS::Value QComboBoxImp::setAutoResize_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setAutoResize(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::sizeHint_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
    QSize ret;
      ret = instance->sizeHint(  );

       return convertToValue( exec, ret );

}

KJS::Value QComboBoxImp::setPalette_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPalette arg0 = extractQPalette(exec, args, 0);

      instance->setPalette(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::setFont_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFont arg0 = extractQFont(exec, args, 0);

      instance->setFont(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::setEnabled_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::setSizeLimit_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->setSizeLimit(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::sizeLimit_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->sizeLimit(  );
      return KJS::Number( ret );

}

KJS::Value QComboBoxImp::setMaxCount_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->setMaxCount(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::maxCount_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->maxCount(  );
      return KJS::Number( ret );

}

KJS::Value QComboBoxImp::setInsertionPolicy_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QComboBox::Policy arg0 = QComboBox::AtBottom; // TODO (hack for combo box)

      instance->setInsertionPolicy(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::insertionPolicy_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->insertionPolicy(  );
      return KJS::Value(); // Returns 'Policy'

}

KJS::Value QComboBoxImp::setValidator_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QValidator *
    return KJS::Value();

    const QValidator * arg0; // Dummy

      instance->setValidator(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::validator_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->validator(  );
      return KJS::Value(); // Returns 'const QValidator *'

}

KJS::Value QComboBoxImp::setListBox_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListBox *
    return KJS::Value();

    QListBox * arg0; // Dummy

      instance->setListBox(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::listBox_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->listBox(  );
      return KJS::Value(); // Returns 'QListBox *'

}

KJS::Value QComboBoxImp::setLineEdit_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QLineEdit *
    return KJS::Value();

    QLineEdit * arg0; // Dummy

      instance->setLineEdit(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::lineEdit_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->lineEdit(  );
      return KJS::Value(); // Returns 'QLineEdit *'

}

KJS::Value QComboBoxImp::setAutoCompletion_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setAutoCompletion(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::autoCompletion_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->autoCompletion(  );
      return KJS::Boolean( ret );

}

KJS::Value QComboBoxImp::eventFilter_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QObject *
    return KJS::Value();

    QObject * arg0; // Dummy

    // Unsupported parameter QEvent *
    return KJS::Value();

    QEvent * arg1; // Dummy

      bool ret;
      ret = instance->eventFilter(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QComboBoxImp::setDuplicatesEnabled_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setDuplicatesEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::duplicatesEnabled_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->duplicatesEnabled(  );
      return KJS::Boolean( ret );

}

KJS::Value QComboBoxImp::editable_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->editable(  );
      return KJS::Boolean( ret );

}

KJS::Value QComboBoxImp::setEditable_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setEditable(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::popup_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->popup(  );
      return KJS::Value(); // Returns void

}

KJS::Value QComboBoxImp::hide_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->hide(  );
      return KJS::Value(); // Returns void

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


