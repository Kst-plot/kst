


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
#include "qlistviewitem_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {

    KJS::Object QListViewItemLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
    {
        if ( args.size() == 0 ) {
                        // FALL THRU
        } else {
            JSOpaqueProxy * arg0 = JSProxy::toOpaqueProxy( args[ 0 ].imp() );
            JSOpaqueProxy *prx = 0;

            if ( arg0 ) {
                if ( arg0->typeName() == "QListViewItem" ) {
                    QListViewItem * parent = arg0->toNative<QListViewItem>();
                    prx = new JSOpaqueProxy( new QListViewItem( parent ), "QListViewItem" );
                } else {
                    return KJS::Object();
                }
            } else {
                JSObjectProxy *arg0 = JSProxy::toObjectProxy( args[ 0 ].imp() );
                if ( arg0 ) {
                    QListView * parent = ( QListView * ) ( arg0->widget() );
                    prx = new JSOpaqueProxy( new QListViewItem( parent ), "QListViewItem" );
                } else {
                    return KJS::Object();
                }
            }
            prx->setOwner( JSProxy::Native );
            KJS::Object proxyObj( prx );
            addBindings(jspart,exec,proxyObj);
            return proxyObj;
        }
        return KJS::Object();
    }
    
    void QListViewItemLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
    {
        QListViewItemImp::addBindings( exec, proxy );
    }
}

QListViewItemImp::QListViewItemImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QListViewItemImp::~QListViewItemImp()
{
}

/**
 * Adds bindings for static methods and enum constants to the specified Object.
 */
void QListViewItemImp::addStaticBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QListViewItemImp *meth = new QListViewItemImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }


}

/**
 * Adds bindings for instance methods to the specified Object.
 */
void QListViewItemImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_insertItem_10, "insertItem" },
        { Method_takeItem_11, "takeItem" },
        { Method_removeItem_12, "removeItem" },
        { Method_height_13, "height" },
        { Method_invalidateHeight_14, "invalidateHeight" },
        { Method_totalHeight_15, "totalHeight" },
        { Method_width_16, "width" },
        { Method_widthChanged_17, "widthChanged" },
        { Method_depth_18, "depth" },
        { Method_setText_19, "setText" },
        { Method_text_20, "text" },
        { Method_setPixmap_21, "setPixmap" },
        { Method_pixmap_22, "pixmap" },
        { Method_key_23, "key" },
        { Method_compare_24, "compare" },
        { Method_sortChildItems_25, "sortChildItems" },
        { Method_childCount_26, "childCount" },
        { Method_isOpen_27, "isOpen" },
        { Method_setOpen_28, "setOpen" },
        { Method_setup_29, "setup" },
        { Method_setSelected_30, "setSelected" },
        { Method_isSelected_31, "isSelected" },
        { Method_paintCell_32, "paintCell" },
        { Method_paintBranches_33, "paintBranches" },
        { Method_paintFocus_34, "paintFocus" },
        { Method_firstChild_35, "firstChild" },
        { Method_nextSibling_36, "nextSibling" },
        { Method_parent_37, "parent" },
        { Method_itemAbove_38, "itemAbove" },
        { Method_itemBelow_39, "itemBelow" },
        { Method_itemPos_40, "itemPos" },
        { Method_listView_41, "listView" },
        { Method_setSelectable_42, "setSelectable" },
        { Method_isSelectable_43, "isSelectable" },
        { Method_setExpandable_44, "setExpandable" },
        { Method_isExpandable_45, "isExpandable" },
        { Method_repaint_46, "repaint" },
        { Method_sort_47, "sort" },
        { Method_moveItem_48, "moveItem" },
        { Method_setDragEnabled_49, "setDragEnabled" },
        { Method_setDropEnabled_50, "setDropEnabled" },
        { Method_dragEnabled_51, "dragEnabled" },
        { Method_dropEnabled_52, "dropEnabled" },
        { Method_acceptDrop_53, "acceptDrop" },
        { Method_setVisible_54, "setVisible" },
        { Method_isVisible_55, "isVisible" },
        { Method_setRenameEnabled_56, "setRenameEnabled" },
        { Method_renameEnabled_57, "renameEnabled" },
        { Method_startRename_58, "startRename" },
        { Method_setEnabled_59, "setEnabled" },
        { Method_isEnabled_60, "isEnabled" },
        { Method_rtti_61, "rtti" },
        { Method_setMultiLinesEnabled_62, "setMultiLinesEnabled" },
        { Method_multiLinesEnabled_63, "multiLinesEnabled" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].name ) {
        if ( lastName != methods[idx].name ) {
            QListViewItemImp *meth = new QListViewItemImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    }
}

/**
 * Extract a QListViewItem pointer from an Object.
 */
QListViewItem *QListViewItemImp::toQListViewItem( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QListViewItem *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;
    return op->toNative<QListViewItem>();
}

/**
 * Select and invoke the correct constructor.
 */
KJS::Object QListViewItemImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QListViewItem_1:
             return QListViewItem_1( exec, args );
             break;

         case Constructor_QListViewItem_2:
             return QListViewItem_2( exec, args );
             break;

         case Constructor_QListViewItem_3:
             return QListViewItem_3( exec, args );
             break;

         case Constructor_QListViewItem_4:
             return QListViewItem_4( exec, args );
             break;

         case Constructor_QListViewItem_5:
             return QListViewItem_5( exec, args );
             break;

         case Constructor_QListViewItem_6:
             return QListViewItem_6( exec, args );
             break;

         case Constructor_QListViewItem_7:
             return QListViewItem_7( exec, args );
             break;

         case Constructor_QListViewItem_8:
             return QListViewItem_8( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QListViewItemCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QListViewItemImp::QListViewItem_1( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    // Unsupported parameter QListView *
    return KJS::Value();

    QListView * arg0; // Dummy


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0 );
#endif
return KJS::Object();

}

KJS::Object QListViewItemImp::QListViewItem_2( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0 );
#endif
return KJS::Object();
}

KJS::Object QListViewItemImp::QListViewItem_3( KJS::ExecState *exec, const KJS::List &args )
{
#if 0

    // Unsupported parameter QListView *
    return KJS::Value();

    QListView * arg0; // Dummy

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg1; // Dummy


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1 );
#endif
return KJS::Object();
}

KJS::Object QListViewItemImp::QListViewItem_4( KJS::ExecState *exec, const KJS::List &args )
{
#if 0

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg1; // Dummy


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1 );
#endif
return KJS::Object();

}

KJS::Object QListViewItemImp::QListViewItem_5( KJS::ExecState *exec, const KJS::List &args )
{
#if 0

    // Unsupported parameter QListView *
    return KJS::Value();

    QListView * arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    QString arg2 = extractQString(exec, args, 2);

    QString arg3 = extractQString(exec, args, 3);

    QString arg4 = extractQString(exec, args, 4);

    QString arg5 = extractQString(exec, args, 5);

    QString arg6 = extractQString(exec, args, 6);

    QString arg7 = extractQString(exec, args, 7);

    QString arg8 = extractQString(exec, args, 8);


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1,
          arg2,
          arg3,
          arg4,
          arg5,
          arg6,
          arg7,
          arg8 );
#endif
return KJS::Object();

}

KJS::Object QListViewItemImp::QListViewItem_6( KJS::ExecState *exec, const KJS::List &args )
{
#if 0

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    QString arg1 = extractQString(exec, args, 1);

    QString arg2 = extractQString(exec, args, 2);

    QString arg3 = extractQString(exec, args, 3);

    QString arg4 = extractQString(exec, args, 4);

    QString arg5 = extractQString(exec, args, 5);

    QString arg6 = extractQString(exec, args, 6);

    QString arg7 = extractQString(exec, args, 7);

    QString arg8 = extractQString(exec, args, 8);


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1,
          arg2,
          arg3,
          arg4,
          arg5,
          arg6,
          arg7,
          arg8 );

#endif
return KJS::Object();

}

KJS::Object QListViewItemImp::QListViewItem_7( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    // Unsupported parameter QListView *
    return KJS::Value();

    QListView * arg0; // Dummy

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg1; // Dummy

    QString arg2 = extractQString(exec, args, 2);

    QString arg3 = extractQString(exec, args, 3);

    QString arg4 = extractQString(exec, args, 4);

    QString arg5 = extractQString(exec, args, 5);

    QString arg6 = extractQString(exec, args, 6);

    QString arg7 = extractQString(exec, args, 7);

    QString arg8 = extractQString(exec, args, 8);

    QString arg9 = extractQString(exec, args, 9);


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1,
          arg2,
          arg3,
          arg4,
          arg5,
          arg6,
          arg7,
          arg8,
          arg9 );
#endif
return KJS::Object();

}

KJS::Object QListViewItemImp::QListViewItem_8( KJS::ExecState *exec, const KJS::List &args )
{
#if 0
    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg1; // Dummy

    QString arg2 = extractQString(exec, args, 2);

    QString arg3 = extractQString(exec, args, 3);

    QString arg4 = extractQString(exec, args, 4);

    QString arg5 = extractQString(exec, args, 5);

    QString arg6 = extractQString(exec, args, 6);

    QString arg7 = extractQString(exec, args, 7);

    QString arg8 = extractQString(exec, args, 8);

    QString arg9 = extractQString(exec, args, 9);


    // We should now create an object of type QListViewItemQListViewItem *ret = new QListViewItem(

          arg0,
          arg1,
          arg2,
          arg3,
          arg4,
          arg5,
          arg6,
          arg7,
          arg8,
          arg9 );
#endif
return KJS::Object();

}

KJS::Value QListViewItemImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QListViewItemImp::toQListViewItem( self );

    switch( id ) {

    case Method_insertItem_10:
        return insertItem_10( exec, self, args );
        break;

    case Method_takeItem_11:
        return takeItem_11( exec, self, args );
        break;

    case Method_removeItem_12:
        return removeItem_12( exec, self, args );
        break;

    case Method_height_13:
        return height_13( exec, self, args );
        break;

    case Method_invalidateHeight_14:
        return invalidateHeight_14( exec, self, args );
        break;

    case Method_totalHeight_15:
        return totalHeight_15( exec, self, args );
        break;

    case Method_width_16:
        return width_16( exec, self, args );
        break;

    case Method_widthChanged_17:
        return widthChanged_17( exec, self, args );
        break;

    case Method_depth_18:
        return depth_18( exec, self, args );
        break;

    case Method_setText_19:
        return setText_19( exec, self, args );
        break;

    case Method_text_20:
        return text_20( exec, self, args );
        break;

    case Method_setPixmap_21:
        return setPixmap_21( exec, self, args );
        break;

    case Method_pixmap_22:
        return pixmap_22( exec, self, args );
        break;

    case Method_key_23:
        return key_23( exec, self, args );
        break;

    case Method_compare_24:
        return compare_24( exec, self, args );
        break;

    case Method_sortChildItems_25:
        return sortChildItems_25( exec, self, args );
        break;

    case Method_childCount_26:
        return childCount_26( exec, self, args );
        break;

    case Method_isOpen_27:
        return isOpen_27( exec, self, args );
        break;

    case Method_setOpen_28:
        return setOpen_28( exec, self, args );
        break;

    case Method_setup_29:
        return setup_29( exec, self, args );
        break;

    case Method_setSelected_30:
        return setSelected_30( exec, self, args );
        break;

    case Method_isSelected_31:
        return isSelected_31( exec, self, args );
        break;

    case Method_paintCell_32:
        return paintCell_32( exec, self, args );
        break;

    case Method_paintBranches_33:
        return paintBranches_33( exec, self, args );
        break;

    case Method_paintFocus_34:
        return paintFocus_34( exec, self, args );
        break;

    case Method_firstChild_35:
        return firstChild_35( exec, self, args );
        break;

    case Method_nextSibling_36:
        return nextSibling_36( exec, self, args );
        break;

    case Method_parent_37:
        return parent_37( exec, self, args );
        break;

    case Method_itemAbove_38:
        return itemAbove_38( exec, self, args );
        break;

    case Method_itemBelow_39:
        return itemBelow_39( exec, self, args );
        break;

    case Method_itemPos_40:
        return itemPos_40( exec, self, args );
        break;

    case Method_listView_41:
        return listView_41( exec, self, args );
        break;

    case Method_setSelectable_42:
        return setSelectable_42( exec, self, args );
        break;

    case Method_isSelectable_43:
        return isSelectable_43( exec, self, args );
        break;

    case Method_setExpandable_44:
        return setExpandable_44( exec, self, args );
        break;

    case Method_isExpandable_45:
        return isExpandable_45( exec, self, args );
        break;

    case Method_repaint_46:
        return repaint_46( exec, self, args );
        break;

    case Method_sort_47:
        return sort_47( exec, self, args );
        break;

    case Method_moveItem_48:
        return moveItem_48( exec, self, args );
        break;

    case Method_setDragEnabled_49:
        return setDragEnabled_49( exec, self, args );
        break;

    case Method_setDropEnabled_50:
        return setDropEnabled_50( exec, self, args );
        break;

    case Method_dragEnabled_51:
        return dragEnabled_51( exec, self, args );
        break;

    case Method_dropEnabled_52:
        return dropEnabled_52( exec, self, args );
        break;

    case Method_acceptDrop_53:
        return acceptDrop_53( exec, self, args );
        break;

    case Method_setVisible_54:
        return setVisible_54( exec, self, args );
        break;

    case Method_isVisible_55:
        return isVisible_55( exec, self, args );
        break;

    case Method_setRenameEnabled_56:
        return setRenameEnabled_56( exec, self, args );
        break;

    case Method_renameEnabled_57:
        return renameEnabled_57( exec, self, args );
        break;

    case Method_startRename_58:
        return startRename_58( exec, self, args );
        break;

    case Method_setEnabled_59:
        return setEnabled_59( exec, self, args );
        break;

    case Method_isEnabled_60:
        return isEnabled_60( exec, self, args );
        break;

    case Method_rtti_61:
        return rtti_61( exec, self, args );
        break;

    case Method_setMultiLinesEnabled_62:
        return setMultiLinesEnabled_62( exec, self, args );
        break;

    case Method_multiLinesEnabled_63:
        return multiLinesEnabled_63( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QListViewItemImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QListViewItemImp::insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->insertItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::takeItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->takeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::removeItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->removeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::height_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->height(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::invalidateHeight_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->invalidateHeight(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::totalHeight_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->totalHeight(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::width_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
#if 0
    // Unsupported parameter const QFontMetrics &
    return KJS::Value();

    const QFontMetrics & arg0; // Dummy

    // Unsupported parameter const QListView *
    return KJS::Value();

    const QListView * arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

      int ret;
      ret = instance->width(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );
#endif
return KJS::Object();

}

KJS::Value QListViewItemImp::widthChanged_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->widthChanged(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::depth_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->depth(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::setText_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    QString arg1 = extractQString(exec, args, 1);

      instance->setText(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::text_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      QString ret;
      ret = instance->text(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QListViewItemImp::setPixmap_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    QPixmap arg1 = extractQPixmap(exec, args, 1);

      instance->setPixmap(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::pixmap_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->pixmap(
       arg0 );
      return KJS::Value(); // Returns 'const QPixmap *'

}

KJS::Value QListViewItemImp::key_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      QString ret;
      ret = instance->key(
       arg0,
       arg1 );
      return KJS::String( ret );

}

KJS::Value QListViewItemImp::compare_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    int arg1 = extractInt(exec, args, 1);

    bool arg2 = extractBool(exec, args, 2);

      int ret;
      ret = instance->compare(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::sortChildItems_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      instance->sortChildItems(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::childCount_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->childCount(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::isOpen_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isOpen(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::setOpen_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setOpen(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::setup_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->setup(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::setSelected_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setSelected(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::isSelected_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isSelected(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::paintCell_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
#if 0
    // Unsupported parameter QPainter *
    return KJS::Value();

    QPainter * arg0; // Dummy

    // Unsupported parameter const QColorGroup &
    return KJS::Value();

    const QColorGroup & arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

    int arg4 = extractInt(exec, args, 4);

      instance->paintCell(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4 );
      return KJS::Value(); // Returns void
#endif

return KJS::Object();
}

KJS::Value QListViewItemImp::paintBranches_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
#if 0
    // Unsupported parameter QPainter *
    return KJS::Value();

    QPainter * arg0; // Dummy

    // Unsupported parameter const QColorGroup &
    return KJS::Value();

    const QColorGroup & arg1; // Dummy

    int arg2 = extractInt(exec, args, 2);

    int arg3 = extractInt(exec, args, 3);

    int arg4 = extractInt(exec, args, 4);

      instance->paintBranches(
       arg0,
       arg1,
       arg2,
       arg3,
       arg4 );
      return KJS::Value(); // Returns void
#endif
return KJS::Object();

}

KJS::Value QListViewItemImp::paintFocus_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{
#if 0
    // Unsupported parameter QPainter *
    return KJS::Value();

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
return KJS::Object();

}

KJS::Value QListViewItemImp::firstChild_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QListViewItem *item = instance->firstChild(  );
      if( item )
      {
          JSOpaqueProxy *prx = new JSOpaqueProxy( item, "QListViewItem" );
          prx->setOwner( JSProxy::Native );
          KJS::Object proxyObj( prx );
          addBindings(exec,proxyObj);
          return proxyObj;
      }
      else
          return KJS::Null();

}

KJS::Value QListViewItemImp::nextSibling_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QListViewItem *item = instance->nextSibling(  );
      if( item )
      {
          JSOpaqueProxy *prx = new JSOpaqueProxy( item, "QListViewItem" );
          prx->setOwner( JSProxy::Native );
          KJS::Object proxyObj( prx );
          addBindings(exec,proxyObj);
          return proxyObj;
      }
      else
          return KJS::Null();

}

KJS::Value QListViewItemImp::parent_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QListViewItem *item = instance->parent(  );
      if( item )
      {
          JSOpaqueProxy *prx = new JSOpaqueProxy( item, "QListViewItem" );
          prx->setOwner( JSProxy::Native );
          KJS::Object proxyObj( prx );
          addBindings(exec,proxyObj);
          return proxyObj;
      }
      else
          return KJS::Null();

}

KJS::Value QListViewItemImp::itemAbove_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QListViewItem *item = instance->itemAbove(  );
      if( item )
      {
          JSOpaqueProxy *prx = new JSOpaqueProxy( item, "QListViewItem" );
          prx->setOwner( JSProxy::Native );
          KJS::Object proxyObj( prx );
          addBindings(exec,proxyObj);
          return proxyObj;
      }
      else
          return KJS::Null();

}

KJS::Value QListViewItemImp::itemBelow_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QListViewItem *item = instance->itemBelow(  );
      if( item )
      {
          JSOpaqueProxy *prx = new JSOpaqueProxy( item, "QListViewItem" );
          prx->setOwner( JSProxy::Native );
          KJS::Object proxyObj( prx );
          addBindings(exec,proxyObj);
          return proxyObj;
      }
      else
          return KJS::Null();

}

KJS::Value QListViewItemImp::itemPos_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->itemPos(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::listView_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->listView(  );
      return KJS::Value(); // Returns 'QListView *'

}

KJS::Value QListViewItemImp::setSelectable_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setSelectable(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::isSelectable_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isSelectable(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::setExpandable_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setExpandable(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::isExpandable_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isExpandable(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::repaint_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->repaint(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::sort_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->sort(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::moveItem_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->moveItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::setDragEnabled_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setDragEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::setDropEnabled_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setDropEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::dragEnabled_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->dragEnabled(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::dropEnabled_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->dropEnabled(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::acceptDrop_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QMimeSource *
    return KJS::Value();

    const QMimeSource * arg0; // Dummy

      bool ret;
      ret = instance->acceptDrop(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::setVisible_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setVisible(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::isVisible_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isVisible(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::setRenameEnabled_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

    bool arg1 = extractBool(exec, args, 1);

      instance->setRenameEnabled(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::renameEnabled_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      bool ret;
      ret = instance->renameEnabled(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::startRename_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = extractInt(exec, args, 0);

      instance->startRename(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::setEnabled_59( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::isEnabled_60( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isEnabled(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewItemImp::rtti_61( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->rtti(  );
      return KJS::Number( ret );

}

KJS::Value QListViewItemImp::setMultiLinesEnabled_62( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = extractBool(exec, args, 0);

      instance->setMultiLinesEnabled(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewItemImp::multiLinesEnabled_63( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->multiLinesEnabled(  );
      return KJS::Boolean( ret );

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


