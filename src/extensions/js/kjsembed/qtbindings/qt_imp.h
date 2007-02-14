
  

#ifndef KJSEMBED_Qt_H
#define KJSEMBED_Qt_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class Qt;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for Qt methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QtImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_Last = -1
   };

   QtImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QtImp();

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

   static Qt *toQt( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //


   //
   // Methods implemented by this class.
   //


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
   Qt *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_Qt_H

// Local Variables:
// c-basic-offset: 4
// End:


