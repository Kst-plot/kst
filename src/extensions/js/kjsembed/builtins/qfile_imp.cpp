

#include <qcstring.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qfont.h>

#ifndef QT_ONLY
#include <klocale.h>
#include <kdebug.h>
#endif

#include <kjs/object.h>

#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/global.h>

#include <qfile.h>
#include "qfile_imp.h"

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

QFileImp::QFileImp( KJS::ExecState *exec, int mid, bool constructor )
   : JSProxyImp(exec), id(mid), cons(constructor)
{
}

QFileImp::~QFileImp()
{
}

void QFileImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSProxy::MethodTable methods[] = {

        { Method_name_4, "name" },
        { Method_setName_5, "setName" },
        { Method_exists_6, "exists" },
        { Method_remove_7, "remove" },
        { Method_open_8, "open" },
        { Method_open_9, "open" },
        { Method_open_10, "open" },
        { Method_close_11, "close" },
        { Method_flush_12, "flush" },
        { Method_size_13, "size" },
        { Method_at_14, "at" },
        { Method_at_15, "at" },
        { Method_atEnd_16, "atEnd" },
        { Method_readBlock_17, "readBlock" },
        { Method_writeBlock_18, "writeBlock" },
        { Method_writeBlock_19, "writeBlock" },
        { Method_readLine_20, "readLine" },
        { Method_readLine_21, "readLine" },
        { Method_getch_22, "getch" },
        { Method_putch_23, "putch" },
        { Method_ungetch_24, "ungetch" },
        { Method_handle_25, "handle" },
        { Method_errorString_26, "errorString" },
        { Method_encodeName_27, "encodeName" },
        { Method_decodeName_28, "decodeName" },
        { Method_setEncodingFunction_29, "setEncodingFunction" },
        { Method_setDecodingFunction_30, "setDecodingFunction" },
        { Method_exists_31, "exists" },
        { Method_remove_32, "remove" },
	{ 0, 0 }
    };

    int idx = 0;
    QCString lastName;

    do {
        if ( lastName != methods[idx].name ) {
            QFileImp *meth = new QFileImp( exec, methods[idx].id );
            object.put( exec , methods[idx].name, KJS::Object(meth) );
            lastName = methods[idx].name;
        }
        ++idx;
    } while( methods[idx].id );


}

QFile *QFileImp::toQFile( KJS::Object &self )
{
    JSObjectProxy *ob = JSProxy::toObjectProxy( self.imp() );
    if ( ob ) {
        QObject *obj = ob->object();
	if ( obj )
           return dynamic_cast<QFile *>( obj );
    }

    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op )
        return 0;

    if ( op->typeName() != "QFile" )
        return 0;

    return  op->toNative<QFile>();
}


KJS::Object QFileImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
   switch( id ) {

         case Constructor_QFile_1:
             return QFile_1( exec, args );
             break;

         case Constructor_QFile_2:
             return QFile_2( exec, args );
             break;

         default:
             break;
    }

    QString msg = i18n("QFileCons has no constructor with id '%1'.").arg(id);
    return throwError(exec, msg,KJS::ReferenceError);
}


KJS::Object QFileImp::QFile_1( KJS::ExecState *exec, const KJS::List &args )
{
	return KJS::Object();
}

KJS::Object QFileImp::QFile_2( KJS::ExecState *exec, const KJS::List &args )
{

    // TODO
    QWidget *arg0 = 0L;
    return KJS::Object();

}

KJS::Value QFileImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    instance = QFileImp::toQFile( self );

    switch( id ) {

    case Method_name_4:
        return name_4( exec, self, args );
        break;

    case Method_setName_5:
        return setName_5( exec, self, args );
        break;

    case Method_exists_6:
        return exists_6( exec, self, args );
        break;

    case Method_remove_7:
        return remove_7( exec, self, args );
        break;

    case Method_open_8:
        return open_8( exec, self, args );
        break;

    case Method_open_9:
        return open_9( exec, self, args );
        break;

    case Method_open_10:
        return open_10( exec, self, args );
        break;

    case Method_close_11:
        return close_11( exec, self, args );
        break;

    case Method_flush_12:
        return flush_12( exec, self, args );
        break;

    case Method_size_13:
        return size_13( exec, self, args );
        break;

    case Method_at_14:
        return at_14( exec, self, args );
        break;

    case Method_at_15:
        return at_15( exec, self, args );
        break;

    case Method_atEnd_16:
        return atEnd_16( exec, self, args );
        break;

    case Method_readBlock_17:
        return readBlock_17( exec, self, args );
        break;

    case Method_writeBlock_18:
        return writeBlock_18( exec, self, args );
        break;

    case Method_writeBlock_19:
        return writeBlock_19( exec, self, args );
        break;

    case Method_readLine_20:
        return readLine_20( exec, self, args );
        break;

    case Method_readLine_21:
        return readLine_21( exec, self, args );
        break;

    case Method_getch_22:
        return getch_22( exec, self, args );
        break;

    case Method_putch_23:
        return putch_23( exec, self, args );
        break;

    case Method_ungetch_24:
        return ungetch_24( exec, self, args );
        break;

    case Method_handle_25:
        return handle_25( exec, self, args );
        break;

    case Method_errorString_26:
        return errorString_26( exec, self, args );
        break;

    case Method_encodeName_27:
        return encodeName_27( exec, self, args );
        break;

    case Method_decodeName_28:
        return decodeName_28( exec, self, args );
        break;

    case Method_setEncodingFunction_29:
        return setEncodingFunction_29( exec, self, args );
        break;

    case Method_setDecodingFunction_30:
        return setDecodingFunction_30( exec, self, args );
        break;

    case Method_exists_31:
        return exists_31( exec, self, args );
        break;

    case Method_remove_32:
        return remove_32( exec, self, args );
        break;

    default:
        break;
    }

    QString msg = i18n( "QFileImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg, KJS::ReferenceError );
}


KJS::Value QFileImp::name_4( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->name(  );
      return KJS::String( ret );

}

KJS::Value QFileImp::setName_5( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      instance->setName(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QFileImp::exists_6( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->exists(  );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::remove_7( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->remove(  );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::open_8( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      bool ret;
      ret = instance->open(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::open_9( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    // Unsupported parameter FILE *
    return KJS::Value();

    FILE * arg1; // Dummy

      bool ret;
      ret = instance->open(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::open_10( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

    int arg1 = (args.size() >= 2) ? args[1].toInteger(exec) : -1;

      bool ret;
      ret = instance->open(
       arg0,
       arg1 );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::close_11( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->close(  );
      return KJS::Value(); // Returns void

}

KJS::Value QFileImp::flush_12( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->flush(  );
      return KJS::Value(); // Returns void

}

KJS::Value QFileImp::size_13( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->size(  );
      return KJS::Value(); // Returns 'Offset'

}

KJS::Value QFileImp::at_14( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      instance->at(  );
      return KJS::Value(); // Returns 'Offset'

}

KJS::Value QFileImp::at_15( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFile::Offset arg0 = 0; // TODO (hack for qfile)

      bool ret;
      ret = instance->at(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::atEnd_16( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      bool ret;
      ret = instance->atEnd(  );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::readBlock_17( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter char *
    return KJS::Value();

    char * arg0; // Dummy

    // Unsupported parameter Q_ULONG
    return KJS::Value();

    Q_ULONG arg1; // Dummy

      instance->readBlock(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'Q_LONG'

}

KJS::Value QFileImp::writeBlock_18( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    const char *arg0 = (args.size() >= 1) ? args[0].toString(exec).ascii() : 0;

    // Unsupported parameter Q_ULONG
    return KJS::Value();

    Q_ULONG arg1; // Dummy

      instance->writeBlock(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'Q_LONG'

}

KJS::Value QFileImp::writeBlock_19( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QByteArray arg0; // TODO (hack for qfile)

      instance->writeBlock(
       arg0 );
      return KJS::Value(); // Returns 'Q_LONG'

}

KJS::Value QFileImp::readLine_20( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    // Unsupported parameter char *
    return KJS::Value();

    char * arg0; // Dummy

    // Unsupported parameter Q_ULONG
    return KJS::Value();

    Q_ULONG arg1; // Dummy

      instance->readLine(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'Q_LONG'

}

KJS::Value QFileImp::readLine_21( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

    // Unsupported parameter Q_ULONG
    return KJS::Value();

    Q_ULONG arg1; // Dummy

      instance->readLine(
       arg0,
       arg1 );
      return KJS::Value(); // Returns 'Q_LONG'

}

KJS::Value QFileImp::getch_22( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->getch(  );
      return KJS::Number( ret );

}

KJS::Value QFileImp::putch_23( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      int ret;
      ret = instance->putch(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QFileImp::ungetch_24( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    int arg0 = (args.size() >= 1) ? args[0].toInteger(exec) : -1;

      int ret;
      ret = instance->ungetch(
       arg0 );
      return KJS::Number( ret );

}

KJS::Value QFileImp::handle_25( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      int ret;
      ret = instance->handle(  );
      return KJS::Number( ret );

}

KJS::Value QFileImp::errorString_26( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

      QString ret;
      ret = instance->errorString(  );
      return KJS::String( ret );

}

KJS::Value QFileImp::encodeName_27( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      instance->encodeName(
       arg0 );
      return KJS::Value(); // Returns 'QCString'

}

KJS::Value QFileImp::decodeName_28( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    const QCString arg0 = (args.size() >= 1) ? args[0].toString(exec).ascii() : 0;

      QString ret;
      ret = instance->decodeName(
       arg0 );
      return KJS::String( ret );

}

static QCString dummy_encoder( const QString &fileName )
{
    return fileName.local8Bit();
}

static QString dummy_decoder( const QCString &fileName )
{
    return QString(fileName);
}

KJS::Value QFileImp::setEncodingFunction_29( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFile::EncoderFn arg0 = dummy_encoder; // TODO (hack for qfile)

      instance->setEncodingFunction(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QFileImp::setDecodingFunction_30( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QFile::DecoderFn arg0 = dummy_decoder; // TODO (hack for qfile)

      instance->setDecodingFunction(
       arg0 );
      return KJS::Value(); // Returns void

}

KJS::Value QFileImp::exists_31( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->exists(
       arg0 );
      return KJS::Boolean( ret );

}

KJS::Value QFileImp::remove_32( KJS::ExecState *exec, KJS::Object &obj, const KJS::List &args )
{

    QString arg0 = (args.size() >= 1) ? args[0].toString(exec).qstring() : QString::null;

      bool ret;
      ret = instance->remove(
       arg0 );
      return KJS::Boolean( ret );

}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:


