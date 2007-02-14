
  

#ifndef KJSEMBED_QPopupMenu_H
#define KJSEMBED_QPopupMenu_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QPopupMenu;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QPopupMenu methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QPopupMenuImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QPopupMenu_1,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_popup_3,
       Method_updateItem_4,
       Method_setCheckable_5,
       Method_isCheckable_6,
       Method_setFont_7,
       Method_show_8,
       Method_hide_9,
       Method_exec_10,
       Method_exec_11,
       Method_setActiveItem_12,
       Method_sizeHint_13,
       Method_idAt_14,
       Method_idAt_15,
       Method_customWhatsThis_16,
       Method_insertTearOffHandle_17,
       Method_activateItemAt_18,
       Method_itemGeometry_19,
       Method_Last = -1
   };

   QPopupMenuImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QPopupMenuImp();

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

   static QPopupMenu *toQPopupMenu( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QPopupMenu_1( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value popup_3( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value updateItem_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setCheckable_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isCheckable_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setFont_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value show_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value hide_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exec_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exec_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setActiveItem_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value sizeHint_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value idAt_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value idAt_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value customWhatsThis_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value insertTearOffHandle_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value activateItemAt_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value itemGeometry_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QPopupMenu *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QPopupMenu_H

// Local Variables:
// c-basic-offset: 4
// End:


