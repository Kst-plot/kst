
  

#ifndef KJSEMBED_QMenuData_H
#define KJSEMBED_QMenuData_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QMenuData;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QMenuData methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QMenuDataImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QMenuData_1,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_count_3,
       Method_insertItem_4,
       Method_insertItem_5,
       Method_insertItem_6,
       Method_insertItem_7,
       Method_insertItem_8,
       Method_insertItem_9,
       Method_insertItem_10,
       Method_insertItem_11,
       Method_insertItem_12,
       Method_insertItem_13,
       Method_insertItem_14,
       Method_insertItem_15,
       Method_insertItem_16,
       Method_insertItem_17,
       Method_insertItem_18,
       Method_insertSeparator_19,
       Method_removeItem_20,
       Method_removeItemAt_21,
       Method_clear_22,
       Method_accel_23,
       Method_setAccel_24,
       Method_iconSet_25,
       Method_text_26,
       Method_pixmap_27,
       Method_setWhatsThis_28,
       Method_whatsThis_29,
       Method_changeItem_30,
       Method_changeItem_31,
       Method_changeItem_32,
       Method_changeItem_33,
       Method_changeItem_34,
       Method_changeItem_35,
       Method_changeItem_36,
       Method_isItemActive_37,
       Method_isItemEnabled_38,
       Method_setItemEnabled_39,
       Method_isItemChecked_40,
       Method_setItemChecked_41,
       Method_isItemVisible_42,
       Method_setItemVisible_43,
       Method_updateItem_44,
       Method_indexOf_45,
       Method_idAt_46,
       Method_setId_47,
       Method_connectItem_48,
       Method_disconnectItem_49,
       Method_setItemParameter_50,
       Method_itemParameter_51,
       Method_findItem_52,
       Method_findItem_53,
       Method_findPopup_54,
       Method_activateItemAt_55,
       Method_Last = -1
   };

   QMenuDataImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QMenuDataImp();

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

   static QMenuData *toQMenuData( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QMenuData_1( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value count_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertItem_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertSeparator_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeItem_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value removeItemAt_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value clear_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value accel_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setAccel_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value iconSet_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value pixmap_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setWhatsThis_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value whatsThis_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_33( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_34( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_35( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value changeItem_36( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isItemActive_37( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isItemEnabled_38( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setItemEnabled_39( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isItemChecked_40( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setItemChecked_41( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isItemVisible_42( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setItemVisible_43( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value updateItem_44( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value indexOf_45( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value idAt_46( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setId_47( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value connectItem_48( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value disconnectItem_49( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setItemParameter_50( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemParameter_51( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value findItem_52( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value findItem_53( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value findPopup_54( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value activateItemAt_55( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QMenuData *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QMenuData_H

// Local Variables:
// c-basic-offset: 4
// End:


