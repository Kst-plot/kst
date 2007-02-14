
  

#ifndef KJSEMBED_QListViewItem_H
#define KJSEMBED_QListViewItem_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>
#include <kjsembed/jsbindingbase.h>

class QListViewItem;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {

    class QListViewItemLoader : public JSBindingBase
    {
        public:
            KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
            void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
    };
}
/**
 * Wrapper class for QListViewItem methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QListViewItemImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QListViewItem_1,
       Constructor_QListViewItem_2,
       Constructor_QListViewItem_3,
       Constructor_QListViewItem_4,
       Constructor_QListViewItem_5,
       Constructor_QListViewItem_6,
       Constructor_QListViewItem_7,
       Constructor_QListViewItem_8,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_insertItem_10,
       Method_takeItem_11,
       Method_removeItem_12,
       Method_height_13,
       Method_invalidateHeight_14,
       Method_totalHeight_15,
       Method_width_16,
       Method_widthChanged_17,
       Method_depth_18,
       Method_setText_19,
       Method_text_20,
       Method_setPixmap_21,
       Method_pixmap_22,
       Method_key_23,
       Method_compare_24,
       Method_sortChildItems_25,
       Method_childCount_26,
       Method_isOpen_27,
       Method_setOpen_28,
       Method_setup_29,
       Method_setSelected_30,
       Method_isSelected_31,
       Method_paintCell_32,
       Method_paintBranches_33,
       Method_paintFocus_34,
       Method_firstChild_35,
       Method_nextSibling_36,
       Method_parent_37,
       Method_itemAbove_38,
       Method_itemBelow_39,
       Method_itemPos_40,
       Method_listView_41,
       Method_setSelectable_42,
       Method_isSelectable_43,
       Method_setExpandable_44,
       Method_isExpandable_45,
       Method_repaint_46,
       Method_sort_47,
       Method_moveItem_48,
       Method_setDragEnabled_49,
       Method_setDropEnabled_50,
       Method_dragEnabled_51,
       Method_dropEnabled_52,
       Method_acceptDrop_53,
       Method_setVisible_54,
       Method_isVisible_55,
       Method_setRenameEnabled_56,
       Method_renameEnabled_57,
       Method_startRename_58,
       Method_setEnabled_59,
       Method_isEnabled_60,
       Method_rtti_61,
       Method_setMultiLinesEnabled_62,
       Method_multiLinesEnabled_63,
       Method_Last = -1
   };

   QListViewItemImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QListViewItemImp();

   /**
    * Adds the static bindings for this class to the specified Object. The
    * static bindings are the static methods and the enums of this class.
    */
   static void addStaticBindings( KJS::ExecState *exec, KJS::Object &object );

   /**
    * Adds the bindings for this class to the specified Object. The bindings
    * added are the instance methods of this class.
    */
   static void addBindings( KJS::ExecState *exec, KJS::Object &object );

   static QListViewItem *toQListViewItem( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QListViewItem_1( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_2( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_3( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_4( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_5( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_6( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_7( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QListViewItem_8( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value takeItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value height_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value invalidateHeight_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value totalHeight_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value width_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value widthChanged_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value depth_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setText_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setPixmap_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value pixmap_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value key_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value compare_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sortChildItems_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value childCount_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isOpen_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setOpen_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setup_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSelected_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isSelected_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value paintCell_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value paintBranches_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value paintFocus_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value firstChild_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value nextSibling_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value parent_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemAbove_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemBelow_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemPos_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value listView_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSelectable_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isSelectable_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setExpandable_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isExpandable_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value repaint_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sort_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value moveItem_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDragEnabled_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDropEnabled_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value dragEnabled_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value dropEnabled_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value acceptDrop_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setVisible_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isVisible_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setRenameEnabled_56( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value renameEnabled_57( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value startRename_58( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setEnabled_59( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isEnabled_60( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rtti_61( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setMultiLinesEnabled_62( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value multiLinesEnabled_63( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QListViewItem *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QListViewItem_H

// Local Variables:
// c-basic-offset: 4
// End:


