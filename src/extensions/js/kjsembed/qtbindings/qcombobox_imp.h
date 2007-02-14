
  

#ifndef KJSEMBED_QComboBox_H
#define KJSEMBED_QComboBox_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QComboBox;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QComboBox methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QComboBoxImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QComboBox_1,
       Constructor_QComboBox_2,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_count_4,
       Method_insertStringList_5,
       Method_insertStrList_6,
       Method_insertStrList_7,
       Method_insertStrList_8,
       Method_insertItem_9,
       Method_insertItem_10,
       Method_insertItem_11,
       Method_removeItem_12,
       Method_currentItem_13,
       Method_setCurrentItem_14,
       Method_currentText_15,
       Method_setCurrentText_16,
       Method_text_17,
       Method_pixmap_18,
       Method_changeItem_19,
       Method_changeItem_20,
       Method_changeItem_21,
       Method_autoResize_22,
       Method_setAutoResize_23,
       Method_sizeHint_24,
       Method_setPalette_25,
       Method_setFont_26,
       Method_setEnabled_27,
       Method_setSizeLimit_28,
       Method_sizeLimit_29,
       Method_setMaxCount_30,
       Method_maxCount_31,
       Method_setInsertionPolicy_32,
       Method_insertionPolicy_33,
       Method_setValidator_34,
       Method_validator_35,
       Method_setListBox_36,
       Method_listBox_37,
       Method_setLineEdit_38,
       Method_lineEdit_39,
       Method_setAutoCompletion_40,
       Method_autoCompletion_41,
       Method_eventFilter_42,
       Method_setDuplicatesEnabled_43,
       Method_duplicatesEnabled_44,
       Method_editable_45,
       Method_setEditable_46,
       Method_popup_47,
       Method_hide_48,
       Method_Last = -1
   };

   QComboBoxImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QComboBoxImp();

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

   static QComboBox *toQComboBox( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QComboBox_1( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QComboBox_2( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value count_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertStringList_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertStrList_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertStrList_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertStrList_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value currentItem_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setCurrentItem_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value currentText_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setCurrentText_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value pixmap_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value autoResize_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setAutoResize_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sizeHint_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setPalette_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setFont_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setEnabled_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setSizeLimit_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sizeLimit_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setMaxCount_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value maxCount_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setInsertionPolicy_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertionPolicy_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setValidator_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value validator_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setListBox_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value listBox_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setLineEdit_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value lineEdit_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setAutoCompletion_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value autoCompletion_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value eventFilter_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDuplicatesEnabled_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value duplicatesEnabled_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value editable_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setEditable_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value popup_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value hide_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QComboBox *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QComboBox_H

// Local Variables:
// c-basic-offset: 4
// End:


