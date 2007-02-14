


#include <qcstring.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>
#include <qiconset.h>

#include <kjs/object.h>

#include <kjsembed/global.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>

#include <qlistview.h>
#include "qlistview_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QListViewImp::QListViewImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QListViewImp::~QListViewImp()
{
}

void QListViewImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_treeStepSize_3, "treeStepSize" },
        { Method_setTreeStepSize_4, "setTreeStepSize" },
        { Method_insertItem_5, "insertItem" },
        { Method_takeItem_6, "takeItem" },
        { Method_removeItem_7, "removeItem" },
        { Method_header_8, "header" },
        { Method_addColumn_9, "addColumn" },
        { Method_addColumn_10, "addColumn" },
        { Method_removeColumn_11, "removeColumn" },
        { Method_setColumnText_12, "setColumnText" },
        { Method_setColumnText_13, "setColumnText" },
        { Method_columnText_14, "columnText" },
        { Method_setColumnWidth_15, "setColumnWidth" },
        { Method_columnWidth_16, "columnWidth" },
        { Method_setColumnWidthMode_17, "setColumnWidthMode" },
        { Method_columnWidthMode_18, "columnWidthMode" },
        { Method_columns_19, "columns" },
        { Method_setColumnAlignment_20, "setColumnAlignment" },
        { Method_columnAlignment_21, "columnAlignment" },
        { Method_show_22, "show" },
        { Method_itemAt_23, "itemAt" },
        { Method_itemRect_24, "itemRect" },
        { Method_itemPos_25, "itemPos" },
        { Method_ensureItemVisible_26, "ensureItemVisible" },
        { Method_repaintItem_27, "repaintItem" },
        { Method_setMultiSelection_28, "setMultiSelection" },
        { Method_isMultiSelection_29, "isMultiSelection" },
        { Method_setSelectionMode_30, "setSelectionMode" },
        { Method_selectionMode_31, "selectionMode" },
        { Method_clearSelection_32, "clearSelection" },
        { Method_setSelected_33, "setSelected" },
        { Method_setSelectionAnchor_34, "setSelectionAnchor" },
        { Method_isSelected_35, "isSelected" },
        { Method_selectedItem_36, "selectedItem" },
        { Method_setOpen_37, "setOpen" },
        { Method_isOpen_38, "isOpen" },
        { Method_setCurrentItem_39, "setCurrentItem" },
        { Method_currentItem_40, "currentItem" },
        { Method_firstChild_41, "firstChild" },
        { Method_lastItem_42, "lastItem" },
        { Method_childCount_43, "childCount" },
        { Method_setAllColumnsShowFocus_44, "setAllColumnsShowFocus" },
        { Method_allColumnsShowFocus_45, "allColumnsShowFocus" },
        { Method_setItemMargin_46, "setItemMargin" },
        { Method_itemMargin_47, "itemMargin" },
        { Method_setRootIsDecorated_48, "setRootIsDecorated" },
        { Method_rootIsDecorated_49, "rootIsDecorated" },
        { Method_setSorting_50, "setSorting" },
        { Method_sortColumn_51, "sortColumn" },
        { Method_setSortColumn_52, "setSortColumn" },
        { Method_sortOrder_53, "sortOrder" },
        { Method_setSortOrder_54, "setSortOrder" },
        { Method_sort_55, "sort" },
        { Method_setFont_56, "setFont" },
        { Method_setPalette_57, "setPalette" },
        { Method_eventFilter_58, "eventFilter" },
        { Method_sizeHint_59, "sizeHint" },
        { Method_minimumSizeHint_60, "minimumSizeHint" },
        { Method_setShowSortIndicator_61, "setShowSortIndicator" },
        { Method_showSortIndicator_62, "showSortIndicator" },
        { Method_setShowToolTips_63, "setShowToolTips" },
        { Method_showToolTips_64, "showToolTips" },
        { Method_setResizeMode_65, "setResizeMode" },
        { Method_resizeMode_66, "resizeMode" },
        { Method_findItem_67, "findItem" },
        { Method_setDefaultRenameAction_68, "setDefaultRenameAction" },
        { Method_defaultRenameAction_69, "defaultRenameAction" },
        { Method_isRenaming_70, "isRenaming" },
        { Method_hideColumn_71, "hideColumn" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    while( methods[idx].id ) {
        if ( lastName != methods[idx].name ) {
            QListViewImp *meth = new QListViewImp( exec, methods[idx].id );
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

        // enum WidthMode
        { "Manual", QListView::Manual },
        { "Maximum", QListView::Maximum },
        // enum SelectionMode
        { "Single", QListView::Single },
        { "Multi", QListView::Multi },
        { "Extended", QListView::Extended },
        { "NoSelection", QListView::NoSelection },
        // enum ResizeMode
        { "NoColumn", QListView::NoColumn },
        { "AllColumns", QListView::AllColumns },
        { "LastColumn", QListView::LastColumn },
        // enum RenameAction
        { "Accept", QListView::Accept },
        { "Reject", QListView::Reject },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );

}

QListView *QListViewImp::toQListView( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QListView *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "QListView" )
        return 0;

    return op->toNative<QListView>();
}


KJS::Object QListViewImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QListView_1:
             return QListView_1( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QListViewCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QListViewImp::QListView_1( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = 0L;

    // TODO
    QWidget *arg1 = 0L;

    // TODO
    QWidget *arg2 = 0L;

}

KJS::Value QListViewImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QListViewImp::toQListView( self );

    switch( id ) {

    case Method_treeStepSize_3:
        return treeStepSize_3( exec, self, args );
        break;

    case Method_setTreeStepSize_4:
        return setTreeStepSize_4( exec, self, args );
        break;

    case Method_insertItem_5:
        return insertItem_5( exec, self, args );
        break;

    case Method_takeItem_6:
        return takeItem_6( exec, self, args );
        break;

    case Method_removeItem_7:
        return removeItem_7( exec, self, args );
        break;

    case Method_header_8:
        return header_8( exec, self, args );
        break;

    case Method_addColumn_9:
        return addColumn_9( exec, self, args );
        break;

    case Method_addColumn_10:
        return addColumn_10( exec, self, args );
        break;

    case Method_removeColumn_11:
        return removeColumn_11( exec, self, args );
        break;

    case Method_setColumnText_12:
        return setColumnText_12( exec, self, args );
        break;

    case Method_setColumnText_13:
        return setColumnText_13( exec, self, args );
        break;

    case Method_columnText_14:
        return columnText_14( exec, self, args );
        break;

    case Method_setColumnWidth_15:
        return setColumnWidth_15( exec, self, args );
        break;

    case Method_columnWidth_16:
        return columnWidth_16( exec, self, args );
        break;

    case Method_setColumnWidthMode_17:
        return setColumnWidthMode_17( exec, self, args );
        break;

    case Method_columnWidthMode_18:
        return columnWidthMode_18( exec, self, args );
        break;

    case Method_columns_19:
        return columns_19( exec, self, args );
        break;

    case Method_setColumnAlignment_20:
        return setColumnAlignment_20( exec, self, args );
        break;

    case Method_columnAlignment_21:
        return columnAlignment_21( exec, self, args );
        break;

    case Method_show_22:
        return show_22( exec, self, args );
        break;

    case Method_itemAt_23:
        return itemAt_23( exec, self, args );
        break;

    case Method_itemRect_24:
        return itemRect_24( exec, self, args );
        break;

    case Method_itemPos_25:
        return itemPos_25( exec, self, args );
        break;

    case Method_ensureItemVisible_26:
        return ensureItemVisible_26( exec, self, args );
        break;

    case Method_repaintItem_27:
        return repaintItem_27( exec, self, args );
        break;

    case Method_setMultiSelection_28:
        return setMultiSelection_28( exec, self, args );
        break;

    case Method_isMultiSelection_29:
        return isMultiSelection_29( exec, self, args );
        break;

    case Method_setSelectionMode_30:
        return setSelectionMode_30( exec, self, args );
        break;

    case Method_selectionMode_31:
        return selectionMode_31( exec, self, args );
        break;

    case Method_clearSelection_32:
        return clearSelection_32( exec, self, args );
        break;

    case Method_setSelected_33:
        return setSelected_33( exec, self, args );
        break;

    case Method_setSelectionAnchor_34:
        return setSelectionAnchor_34( exec, self, args );
        break;

    case Method_isSelected_35:
        return isSelected_35( exec, self, args );
        break;

    case Method_selectedItem_36:
        return selectedItem_36( exec, self, args );
        break;

    case Method_setOpen_37:
        return setOpen_37( exec, self, args );
        break;

    case Method_isOpen_38:
        return isOpen_38( exec, self, args );
        break;

    case Method_setCurrentItem_39:
        return setCurrentItem_39( exec, self, args );
        break;

    case Method_currentItem_40:
        return currentItem_40( exec, self, args );
        break;

    case Method_firstChild_41:
        return firstChild_41( exec, self, args );
        break;

    case Method_lastItem_42:
        return lastItem_42( exec, self, args );
        break;

    case Method_childCount_43:
        return childCount_43( exec, self, args );
        break;

    case Method_setAllColumnsShowFocus_44:
        return setAllColumnsShowFocus_44( exec, self, args );
        break;

    case Method_allColumnsShowFocus_45:
        return allColumnsShowFocus_45( exec, self, args );
        break;

    case Method_setItemMargin_46:
        return setItemMargin_46( exec, self, args );
        break;

    case Method_itemMargin_47:
        return itemMargin_47( exec, self, args );
        break;

    case Method_setRootIsDecorated_48:
        return setRootIsDecorated_48( exec, self, args );
        break;

    case Method_rootIsDecorated_49:
        return rootIsDecorated_49( exec, self, args );
        break;

    case Method_setSorting_50:
        return setSorting_50( exec, self, args );
        break;

    case Method_sortColumn_51:
        return sortColumn_51( exec, self, args );
        break;

    case Method_setSortColumn_52:
        return setSortColumn_52( exec, self, args );
        break;

    case Method_sortOrder_53:
        return sortOrder_53( exec, self, args );
        break;

    case Method_setSortOrder_54:
        return setSortOrder_54( exec, self, args );
        break;

    case Method_sort_55:
        return sort_55( exec, self, args );
        break;

    case Method_setFont_56:
        return setFont_56( exec, self, args );
        break;

    case Method_setPalette_57:
        return setPalette_57( exec, self, args );
        break;

    case Method_eventFilter_58:
        return eventFilter_58( exec, self, args );
        break;

    case Method_sizeHint_59:
        return sizeHint_59( exec, self, args );
        break;

    case Method_minimumSizeHint_60:
        return minimumSizeHint_60( exec, self, args );
        break;

    case Method_setShowSortIndicator_61:
        return setShowSortIndicator_61( exec, self, args );
        break;

    case Method_showSortIndicator_62:
        return showSortIndicator_62( exec, self, args );
        break;

    case Method_setShowToolTips_63:
        return setShowToolTips_63( exec, self, args );
        break;

    case Method_showToolTips_64:
        return showToolTips_64( exec, self, args );
        break;

    case Method_setResizeMode_65:
        return setResizeMode_65( exec, self, args );
        break;

    case Method_resizeMode_66:
        return resizeMode_66( exec, self, args );
        break;

    case Method_findItem_67:
        return findItem_67( exec, self, args );
        break;

    case Method_setDefaultRenameAction_68:
        return setDefaultRenameAction_68( exec, self, args );
        break;

    case Method_defaultRenameAction_69:
        return defaultRenameAction_69( exec, self, args );
        break;

    case Method_isRenaming_70:
        return isRenaming_70( exec, self, args );
        break;

    case Method_hideColumn_71:
        return hideColumn_71( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QListViewImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Value QListViewImp::treeStepSize_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->treeStepSize(  );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setTreeStepSize_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->setTreeStepSize(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::insertItem_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->insertItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::takeItem_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->takeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::removeItem_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->removeItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::header_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->header(  );
      return KJS::Value(); // Returns 'QHeader *'

}

KJS::Value QListViewImp::addColumn_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      int ret;
      ret = instance->addColumn(
       arg0,
       arg1 );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::addColumn_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet  arg0; // Dummy

    QString arg1 = (args.size() >= 2) ? args[1].toString(exec).qstring() : QString::null;

    int arg2 = (args.size() >= 3) ? args[2].toInteger(exec) : -1;

      int ret;
      ret = instance->addColumn(
       arg0,
       arg1,
       arg2 );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::removeColumn_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->removeColumn(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setColumnText_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    QString arg1 = (args.size() >= 2) ? args[1].toString(exec).qstring() : QString::null;

      instance->setColumnText(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setColumnText_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    // Unsupported parameter const QIconSet
    return KJS::Value();

    const QIconSet  arg1; // Dummy

    QString arg2 = (args.size() >= 3) ? args[2].toString(exec).qstring() : QString::null;

      instance->setColumnText(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::columnText_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      QString ret;
      ret = instance->columnText(
       arg0 );
      return KJS::String( ret );

}

KJS::Value QListViewImp::setColumnWidth_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      instance->setColumnWidth(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::columnWidth_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      int ret;
      ret = instance->columnWidth(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setColumnWidthMode_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    // Unsupported parameter WidthMode
    return KJS::Value();

    WidthMode arg1; // Dummy

      instance->setColumnWidthMode(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::columnWidthMode_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->columnWidthMode(
       arg0 );
      return KJS::Value(); // Returns 'WidthMode'

}

KJS::Value QListViewImp::columns_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->columns(  );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setColumnAlignment_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      instance->setColumnAlignment(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::columnAlignment_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      int ret;
      ret = instance->columnAlignment(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::show_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->show(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::itemAt_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QPoint &
    return KJS::Value();

    const QPoint & arg0; // Dummy

      instance->itemAt(
       arg0 );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::itemRect_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      instance->itemRect(
       arg0 );
      return KJS::Value(); // Returns 'QRect'

}

KJS::Value QListViewImp::itemPos_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      int ret;
      ret = instance->itemPos(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::ensureItemVisible_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      instance->ensureItemVisible(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::repaintItem_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      instance->repaintItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setMultiSelection_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setMultiSelection(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::isMultiSelection_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isMultiSelection(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setSelectionMode_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter SelectionMode
    return KJS::Value();

    SelectionMode arg0; // Dummy

      instance->setSelectionMode(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::selectionMode_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->selectionMode(  );
      return KJS::Value(); // Returns 'SelectionMode'

}

KJS::Value QListViewImp::clearSelection_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->clearSelection(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setSelected_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      instance->setSelected(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setSelectionAnchor_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->setSelectionAnchor(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::isSelected_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      bool ret;
      ret = instance->isSelected(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::selectedItem_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->selectedItem(  );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::setOpen_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      instance->setOpen(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::isOpen_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter const QListViewItem *
    return KJS::Value();

    const QListViewItem * arg0; // Dummy

      bool ret;
      ret = instance->isOpen(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setCurrentItem_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter QListViewItem *
    return KJS::Value();

    QListViewItem * arg0; // Dummy

      instance->setCurrentItem(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::currentItem_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->currentItem(  );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::firstChild_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->firstChild(  );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::lastItem_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->lastItem(  );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::childCount_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->childCount(  );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setAllColumnsShowFocus_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setAllColumnsShowFocus(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::allColumnsShowFocus_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->allColumnsShowFocus(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setItemMargin_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->setItemMargin(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::itemMargin_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->itemMargin(  );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setRootIsDecorated_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setRootIsDecorated(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::rootIsDecorated_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->rootIsDecorated(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setSorting_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    bool arg1 = (args.size() >= 2) ? args[1].toBoolean(exec) : false;

      instance->setSorting(
       arg0,
       arg1 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::sortColumn_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->sortColumn(  );
      return KJS::Number( ret );

}

KJS::Value QListViewImp::setSortColumn_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->setSortColumn(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::sortOrder_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->sortOrder(  );
      return KJS::Value(); // Returns 'SortOrder'

}

KJS::Value QListViewImp::setSortOrder_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter SortOrder
    return KJS::Value();

    SortOrder arg0; // Dummy

      instance->setSortOrder(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::sort_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->sort(  );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setFont_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFont arg0; // TODO

      instance->setFont(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::setPalette_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QPalette arg0; // TODO

      instance->setPalette(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::eventFilter_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
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

KJS::Value QListViewImp::sizeHint_59( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->sizeHint(  );
      return KJS::Value(); // Returns 'QSize'

}

KJS::Value QListViewImp::minimumSizeHint_60( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->minimumSizeHint(  );
      return KJS::Value(); // Returns 'QSize'

}

KJS::Value QListViewImp::setShowSortIndicator_61( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setShowSortIndicator(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::showSortIndicator_62( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->showSortIndicator(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setShowToolTips_63( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    bool arg0 = (args.size() >= 1) ? args[0].toBoolean(exec) : false;

      instance->setShowToolTips(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::showToolTips_64( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->showToolTips(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::setResizeMode_65( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter ResizeMode
    return KJS::Value();

    ResizeMode arg0; // Dummy

      instance->setResizeMode(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::resizeMode_66( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->resizeMode(  );
      return KJS::Value(); // Returns 'ResizeMode'

}

KJS::Value QListViewImp::findItem_67( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

    // Unsupported parameter ComparisonFlags
    return KJS::Value();

    ComparisonFlags arg2; // Dummy

      instance->findItem(
       arg0,
       arg1,
       arg2 );
      return KJS::Value(); // Returns 'QListViewItem *'

}

KJS::Value QListViewImp::setDefaultRenameAction_68( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter RenameAction
    return KJS::Value();

    RenameAction arg0; // Dummy

      instance->setDefaultRenameAction(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QListViewImp::defaultRenameAction_69( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->defaultRenameAction(  );
      return KJS::Value(); // Returns 'RenameAction'

}

KJS::Value QListViewImp::isRenaming_70( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->isRenaming(  );
      return KJS::Boolean( ret );

}

KJS::Value QListViewImp::hideColumn_71( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      instance->hideColumn(
       arg0 );
      return KJS::Value(); // Returns void

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


