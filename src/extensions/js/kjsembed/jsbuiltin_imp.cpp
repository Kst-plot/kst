/*
 *  Copyright (C) 2001-2003, Richard J. Moore <rich@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

// #define HAVE_KSPY 1
#include <stdio.h>

#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>

#ifndef QT_ONLY

#include "global.h"
#include <klocale.h>
#include <kinputdialog.h>
#include <kmessagebox.h>
#include <kstddirs.h>
#include "ksimpleprocess.h"

#else

#include <qinputdialog.h>
#include <qmessagebox.h>

#endif // QT_ONLY

#include <kjs/object.h>

#ifdef HAVE_KSPY
#include <kspy.h>
#endif

#include "global.h"
#include "jsfactory.h"
#include "jsobjectproxy.h"
#include "kjsembedpart.h"

#include "builtins/saxhandler.h"

#include "jsbuiltin_imp.h"

//
// KJSEmbedPart Bindings
//
namespace KJSEmbed {
namespace Bindings {

JSBuiltInImp::JSBuiltInImp( JSBuiltIn *builtin, int mid, const QString &p )
    : JSProxyImp(builtin->part()->globalExec()), part(builtin->part()), id(mid), param(p)
{
    setName( KJS::Identifier( KJS::UString(param) ) );
}

JSBuiltInImp::~JSBuiltInImp() {}

KJS::Value JSBuiltInImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    QString arg0 = (args.size() > 0) ? args[0].toString(exec).qstring() : QString::null;
    QString arg1 = (args.size() > 1) ? args[1].toString(exec).qstring() : QString::null;

    if ( id == MethodLoadScript ) {

	if ( args.size() == 1 ) {
	    bool ok = part->runFile( arg0 );
	    return KJS::Boolean( ok );
	}
	else if ( args.size() == 2 ) {
	    bool ok = part->runFile( arg0, args[1] );
	    return KJS::Boolean( ok );
	}
	else {
	    QString msg = i18n( "Method requires 1 or 2 arguments, received %1." ).arg( args.size() );
      return throwError(exec, msg);
	}
    }
    else if ( id == MethodPrint ) {

	(*conout()) << arg0;

	return KJS::Boolean( true );
    }
    else if ( id == MethodPrintLn ) {

	(*conout()) << arg0 << endl;

	return KJS::Boolean( true );
    }
    else if ( id == MethodWarn ) {

	(*conerr()) << arg0 << endl;

	return KJS::Boolean( true );
    }
    else if ( id == MethodReadLine ) {

	QString line = conin()->readLine();
	if ( !line.isNull() )
	    return KJS::String( line );

    }
    else if ( id == MethodOpenFile ) {
// #warning "This is leaked!!!!!"
// 	QFile *f=new QFile( arg0 );
// 	int mode = 0;
// 	if ( arg1 == QString("ro") )
// 	    mode = IO_ReadOnly;
// 	else if ( arg1 == QString("wo") )
// 	    mode = IO_WriteOnly;

// 	if ( !f->open( mode ) )
// 	    return KJS::Boolean(false);

// 	Bindings::TextStream *ts = new Bindings::TextStream( part, "file", new QTextStream(f) );

// 	return KJS::Object( ts );
	return KJS::Null();
    }
    else if ( id == MethodReadFile ) {

	QFile f( arg0 );
	if ( !f.open( IO_ReadOnly ) ) {
	    QString msg = i18n( "Could not open file '%1'." ).arg(arg0);
      return throwError(exec, msg);
	}

	QString s = f.readAll();
	return KJS::String( s );
    }
    else if ( id == MethodWriteFile ) {
	QFile f( arg0 );
	if ( !f.open( IO_WriteOnly ) )
	    return KJS::Boolean( false );

	QTextStream ts( &f );
	ts << arg1;

	return KJS::Boolean( true );
    }
    else if ( id == MethodDumpObject ) {
	if ( args.size() == 1 ) {
	    KJS::Object ob = args[0].toObject(exec);
	    return KJS::String( dumpObject( exec, ob ) );
	}
	return KJS::String( dumpObject( exec, self ) );
    }
    else if ( id == MethodRunKSpy ) {
#ifdef HAVE_KSPY
	KSpy::invoke();
#endif
	return KJS::Undefined();
    }
    else if ( id == MethodExit ) {
	int status = args.size() ? args[0].toInteger( exec ) : 0;
	::exit( status );
    }
    else if ( id == MethodSaxLoadFile ) {
	if ( args.size() != 2 ) {
	    QString msg = i18n( "Method requires 2 arguments, received %1." ).arg( args.size() );
      return throwError(exec, msg);
	}

	BuiltIns::SaxHandler sax( exec );
	sax.setHandler( args[0].toObject(exec) );

	QFile f( arg1 );
	QXmlInputSource src( &f );
	QXmlSimpleReader reader;
	reader.setContentHandler( &sax );

	bool ok = reader.parse( src );
	if ( !ok ) {
      return throwError(exec, sax.errorString().utf8());
	}

	return KJS::Boolean( true );
    }
    else if ( id == MethodDumpCompletion ) {
	KJS::Object obj = obj.isValid() ? args[0].toObject(exec) : KJS::Object();
	if ( !obj.isValid() ) {
	    QString msg = i18n( "Method requires an object." );
      return throwError(exec, msg,KJS::TypeError);
	}

	QVariant v = dumpCompletion( exec, obj );
	return convertToValue( exec, v ); // FIXME: This will crash if the type is wrong
    }
    else if ( id == MethodAlert ) {

#ifndef QT_ONLY
    	KMessageBox::information( 0L, arg0 );
#else // QT_ONLY
    	QMessageBox::information( 0L, i18n("Information"), arg0 );
#endif // QT_ONLY

	return KJS::Undefined();
    }
    else if ( id == MethodConfirm ) {

	int answer;
#ifndef QT_ONLY
    	answer = KMessageBox::questionYesNo( 0L, arg0 );
#else // QT_ONLY
    	answer = QMessageBox::question(0L, i18n("Question"), arg0 );
#endif // QT_ONLY

	return KJS::Number( answer );
    }
    else if ( id == MethodPrompt ) {

	QString text;
#ifndef QT_ONLY
	text = KInputDialog::getText( arg0, arg0, arg1 );
#else // QT_ONLY
	text = QInputDialog::getText( arg0, arg0, QLineEdit::Normal, arg1 );
#endif // QT_ONLY

	return KJS::String( text );
    }
    else if ( id == MethodI18n ) {
    	QString retString(i18n(arg0.latin1()));
	return KJS::String( retString );
    }
    else if ( id == MethodImport ) {

#ifndef QT_ONLY
	//  Scan $KDEDIRS/share/apps/$APPNAME/
	//  Scan $KDEDIRS/share/apps/kjsembed/
	KGlobal::dirs()->addResourceType("kjsembed", KStandardDirs::kde_default("data") +"/kjsembed/");
	QString fname = KGlobal::dirs ()->findResource ("kjsembed", QString(arg0));

	kdDebug(80001) << "Loading: " << fname  << endl;

	//QString code = part->loadFile(fname);

	return KJS::Boolean( part->runFile(fname, part->globalObject()));
#else // QT_ONLY
	return KJS::Boolean( false );
#endif // QT_ONLY

    }
    else if ( id == MethodShell ) {

#ifndef QT_ONLY
	// return default data
	kdDebug( 80001 ) << "Run: " << arg0 << endl;
	return KJS::String(KSimpleProcess::exec( arg0 ));
#else // QT_ONLY
	return KJS::String();
#endif // QT_ONLY

    }
    else {
	kdWarning() << "JSBuiltInImp has no method " << id << endl;
    }

    QString msg = i18n( "JSBuiltInImp has no method with id '%1'." ).arg( id );
    return throwError(exec, msg,KJS::ReferenceError);
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
