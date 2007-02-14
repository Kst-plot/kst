
  

#ifndef KJSEMBED_QFile_H
#define KJSEMBED_QFile_H

#include <kjs/interpreter.h>
#include <kjs/object.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QFile;

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

/**
 * Wrapper class for QFile methods.
 *
 * @author Richard Moore, rich@kde.org
 */
class QFileImp : public JSProxyImp
{
public:
   /** Enumerates the constructors supported by this class. */
   enum Constructors {
       Constructor_QFile_1,
       Constructor_QFile_2,

       LastConstuctor = -1
   };

   /** Enumerates the methods supported by this class. */
   enum Methods {

       Method_name_4,
       Method_setName_5,
       Method_exists_6,
       Method_remove_7,
       Method_open_8,
       Method_open_9,
       Method_open_10,
       Method_close_11,
       Method_flush_12,
       Method_size_13,
       Method_at_14,
       Method_at_15,
       Method_atEnd_16,
       Method_readBlock_17,
       Method_writeBlock_18,
       Method_writeBlock_19,
       Method_readLine_20,
       Method_readLine_21,
       Method_getch_22,
       Method_putch_23,
       Method_ungetch_24,
       Method_handle_25,
       Method_errorString_26,
       Method_encodeName_27,
       Method_decodeName_28,
       Method_setEncodingFunction_29,
       Method_setDecodingFunction_30,
       Method_exists_31,
       Method_remove_32,
       Method_Last = -1
   };

   QFileImp( KJS::ExecState *exec, int id, bool constructor=false );
   ~QFileImp();

   static void addBindings( KJS::ExecState *exec, KJS::Object &object );

   static QFile *toQFile( KJS::Object &object );

   //
   // Constructors implemented by this class.
   //

   KJS::Object QFile_1( KJS::ExecState *exec, const KJS::List &args );
   KJS::Object QFile_2( KJS::ExecState *exec, const KJS::List &args );

   //
   // Methods implemented by this class.
   //

   KJS::Value name_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setName_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exists_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value remove_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value open_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value open_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value open_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value close_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value flush_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value size_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value at_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value at_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value atEnd_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value readBlock_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value writeBlock_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value writeBlock_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value readLine_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value readLine_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value getch_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value putch_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value ungetch_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value handle_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value errorString_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value encodeName_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value decodeName_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setEncodingFunction_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value setDecodingFunction_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value exists_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );
   KJS::Value remove_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args );

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
   QFile *instance; // Temp
   int id;
   bool cons;
};

} // namespace KJSEmbed

#endif // KJSEMBED_QFile_H

// Local Variables:
// c-basic-offset: 4
// End:


