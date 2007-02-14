
  

#ifndef KJSEMBED_QCheckListItem_H
#define KJSEMBED_QCheckListItem_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>
#include <kjsembed/jsbindingbase.h>

class QCheckListItem;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {
namespace Bindings {

    class QCheckListItemLoader : public JSBindingBase
    {
        public:
            KJS::Object createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const;
            void addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy) const;
    };
}
/**
 * Wrapper class for QCheckListItem methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QCheckListItemImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QCheckListItem_1,
       Constructor_QCheckListItem_2,
       Constructor_QCheckListItem_3,
       Constructor_QCheckListItem_4,
       Constructor_QCheckListItem_5,
       Constructor_QCheckListItem_6,
       Constructor_QCheckListItem_7,
       Constructor_QCheckListItem_8,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_paintCell_10,
       Method_paintFocus_11,
       Method_width_12,
       Method_setup_13,
       Method_setOn_14,
       Method_isOn_15,
       Method_type_16,
       Method_text_17,
       Method_text_18,
       Method_setTristate_19,
       Method_isTristate_20,
       Method_state_21,
       Method_setState_22,
       Method_rtti_23,
       Method_Last = -1
   };

   QCheckListItemImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QCheckListItemImp();

   static void addBindings( KJS::ExecState *exec, KJS::Object &object );

   static QCheckListItem *toQCheckListItem( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QCheckListItem_1( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_2( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_3( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_4( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_5( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_6( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_7( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QCheckListItem_8( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value paintCell_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value paintFocus_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value width_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setup_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setOn_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isOn_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value type_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value text_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setTristate_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value isTristate_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value state_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setState_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value rtti_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QCheckListItem *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QCheckListItem_H

// Local Variables:
// c-basic-offset: 4
// End:


