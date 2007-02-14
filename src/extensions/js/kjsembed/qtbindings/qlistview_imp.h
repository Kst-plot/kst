
  

#ifndef KJSEMBED_QListView_H
#define KJSEMBED_QListView_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QListView;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QListView methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QListViewImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QListView_1,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_treeStepSize_3,
       Method_setTreeStepSize_4,
       Method_insertItem_5,
       Method_takeItem_6,
       Method_removeItem_7,
       Method_header_8,
       Method_addColumn_9,
       Method_addColumn_10,
       Method_removeColumn_11,
       Method_setColumnText_12,
       Method_setColumnText_13,
       Method_columnText_14,
       Method_setColumnWidth_15,
       Method_columnWidth_16,
       Method_setColumnWidthMode_17,
       Method_columnWidthMode_18,
       Method_columns_19,
       Method_setColumnAlignment_20,
       Method_columnAlignment_21,
       Method_show_22,
       Method_itemAt_23,
       Method_itemRect_24,
       Method_itemPos_25,
       Method_ensureItemVisible_26,
       Method_repaintItem_27,
       Method_setMultiSelection_28,
       Method_isMultiSelection_29,
       Method_setSelectionMode_30,
       Method_selectionMode_31,
       Method_clearSelection_32,
       Method_setSelected_33,
       Method_setSelectionAnchor_34,
       Method_isSelected_35,
       Method_selectedItem_36,
       Method_setOpen_37,
       Method_isOpen_38,
       Method_setCurrentItem_39,
       Method_currentItem_40,
       Method_firstChild_41,
       Method_lastItem_42,
       Method_childCount_43,
       Method_setAllColumnsShowFocus_44,
       Method_allColumnsShowFocus_45,
       Method_setItemMargin_46,
       Method_itemMargin_47,
       Method_setRootIsDecorated_48,
       Method_rootIsDecorated_49,
       Method_setSorting_50,
       Method_sortColumn_51,
       Method_setSortColumn_52,
       Method_sortOrder_53,
       Method_setSortOrder_54,
       Method_sort_55,
       Method_setFont_56,
       Method_setPalette_57,
       Method_eventFilter_58,
       Method_sizeHint_59,
       Method_minimumSizeHint_60,
       Method_setShowSortIndicator_61,
       Method_showSortIndicator_62,
       Method_setShowToolTips_63,
       Method_showToolTips_64,
       Method_setResizeMode_65,
       Method_resizeMode_66,
       Method_findItem_67,
       Method_setDefaultRenameAction_68,
       Method_defaultRenameAction_69,
       Method_isRenaming_70,
       Method_hideColumn_71,
       Method_Last = -1
   };

   QListViewImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QListViewImp();

   static void addBindings( KJS::ExecState *exec, KJS::Object &object );

   static QListView *toQListView( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QListView_1( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value treeStepSize_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setTreeStepSize_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value takeItem_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeItem_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value header_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value addColumn_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value addColumn_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeColumn_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setColumnText_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setColumnText_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value columnText_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setColumnWidth_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value columnWidth_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setColumnWidthMode_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value columnWidthMode_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value columns_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setColumnAlignment_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value columnAlignment_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value show_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemAt_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemRect_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemPos_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value ensureItemVisible_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value repaintItem_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setMultiSelection_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isMultiSelection_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSelectionMode_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value selectionMode_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value clearSelection_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSelected_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSelectionAnchor_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isSelected_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value selectedItem_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setOpen_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isOpen_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setCurrentItem_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value currentItem_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value firstChild_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value lastItem_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value childCount_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setAllColumnsShowFocus_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value allColumnsShowFocus_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setItemMargin_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemMargin_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setRootIsDecorated_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rootIsDecorated_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSorting_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sortColumn_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSortColumn_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sortOrder_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSortOrder_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sort_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setFont_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setPalette_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value eventFilter_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sizeHint_59( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value minimumSizeHint_60( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setShowSortIndicator_61( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value showSortIndicator_62( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setShowToolTips_63( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value showToolTips_64( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setResizeMode_65( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value resizeMode_66( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value findItem_67( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDefaultRenameAction_68( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value defaultRenameAction_69( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isRenaming_70( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value hideColumn_71( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

   //
   // JS binding code.
   //

   /** Returns true iff this object implements the construct function. */
   virtual bool implementsConstruct() const { return cons; }

   /** Invokes the construct function. */
   virtual KJS::Object construct( KJS::ExecState *exec, const KJS::List &args );

   /** Returns true iff this object implements the call function. */
   virtual bool implementsCall() const { return !cons; }

   /** Invokes the call function. */
   virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
   QListView *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QListView_H

// Local Variables:
// c-basic-offset: 4
// End:


