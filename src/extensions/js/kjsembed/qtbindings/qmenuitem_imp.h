
  

#ifndef KJSEMBED_QMenuItem_H
#define KJSEMBED_QMenuItem_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QMenuItem;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QMenuItem methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QMenuItemImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QMenuItem_1,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_id_3,
       Method_iconSet_4,
       Method_text_5,
       Method_whatsThis_6,
       Method_pixmap_7,
       Method_popup_8,
       Method_widget_9,
       Method_custom_10,
       Method_key_11,
       Method_signal_12,
       Method_isSeparator_13,
       Method_isEnabled_14,
       Method_isChecked_15,
       Method_isDirty_16,
       Method_isVisible_17,
       Method_isEnabledAndVisible_18,
       Method_setText_19,
       Method_setDirty_20,
       Method_setVisible_21,
       Method_setWhatsThis_22,
       Method_Last = -1
   };

   QMenuItemImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QMenuItemImp();

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

   static QMenuItem *toQMenuItem( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QMenuItem_1( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value id_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value iconSet_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value whatsThis_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value pixmap_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value popup_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value widget_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value custom_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value key_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value signal_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isSeparator_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isEnabled_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isChecked_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isDirty_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isVisible_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isEnabledAndVisible_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setText_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDirty_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setVisible_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setWhatsThis_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QMenuItem *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QMenuItem_H

// Local Variables:
// c-basic-offset: 4
// End:


