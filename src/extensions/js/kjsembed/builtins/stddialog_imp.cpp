/*
 *  Copyright (C) 2003, Richard J. Moore <rich@kde.org>
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

#ifndef QT_ONLY

#include <kcolordialog.h>
#include <kfiledialog.h>
#include <kpropertiesdialog.h>

#else

#include <qfiledialog.h>
#define KFileDialog QFileDialog

#endif // QT_ONLY

#include <kjsembed/jsbinding.h>
#include <kjsembed/global.h>

#include "stddialog_imp.h"

namespace KJSEmbed {
namespace BuiltIns {

StdDialogImp::StdDialogImp( KJS::ExecState *exec, int mid )
    : JSProxyImp(exec), id(mid)
{
}

StdDialogImp::~StdDialogImp() {}

void StdDialogImp::addBindings( KJS::ExecState *exec, KJS::Object &parent )
{
    StdDialogImp *sdi;

    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetOpenFileName );
    parent.put( exec, "getOpenFileName", KJS::Object( sdi ) );
    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetSaveFileName );
    parent.put( exec, "getSaveFileName", KJS::Object( sdi ) );

    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetOpenURL );
    parent.put( exec, "getOpenURL", KJS::Object( sdi ) );
    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetSaveURL );
    parent.put( exec, "getSaveURL", KJS::Object( sdi ) );

    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetExistingDirectory );
    parent.put( exec, "getExistingDirectory", KJS::Object( sdi ) );
    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetExistingURL );
    parent.put( exec, "getExistingURL", KJS::Object( sdi ) );

    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetImageOpenURL );
    parent.put( exec, "getImageOpenURL", KJS::Object( sdi ) );

    sdi = new StdDialogImp( exec, StdDialogImp::MethodGetColor );
    parent.put( exec, "getColor", KJS::Object( sdi ) );

    sdi = new StdDialogImp( exec, StdDialogImp::MethodShowProperties );
    parent.put( exec, "showProperties", KJS::Object( sdi ) );
}

KJS::Value StdDialogImp::call( KJS::ExecState *exec, KJS::Object &/*self*/, const KJS::List &args )
{
    QString arg0 = (args.size() > 0) ? args[0].toString(exec).qstring() : QString::null;
    QString arg1 = (args.size() > 1) ? args[1].toString(exec).qstring() : QString::null;

    switch( id ) {
	case MethodGetOpenFileName:
	{
	    QString s = KFileDialog::getOpenFileName( arg0, arg1 );
	    return KJS::String( s );
	}
	case MethodGetSaveFileName:
	{
	    QString s = KFileDialog::getSaveFileName( arg0, arg1 );
	    return KJS::String( s );
	}
	case MethodGetExistingDirectory:
	{
	    QString s = KFileDialog::getExistingDirectory( arg0 );
	    return KJS::String( s );
	}
#ifndef QT_ONLY
        case MethodGetColor:
	{
	    QColor col;
	    int res = KColorDialog::getColor( col );
	    if ( res == QDialog::Accepted )
		return convertToValue( exec, QVariant(col) );
	}
	case MethodGetOpenURL:
	{
	    KURL u = KFileDialog::getOpenURL( arg0, arg1 );
	    return KJS::String( u.url() );
	}
	case MethodGetSaveURL:
	{
	    KURL u = KFileDialog::getSaveURL( arg0, arg1 );
	    return KJS::String( u.url() );
	}
	case MethodGetExistingURL:
	{
	    KURL u = KFileDialog::getExistingURL( arg0 );
	    return KJS::String( u.url() );
	}
	case MethodGetImageOpenURL:
	{
	    KURL u = KFileDialog::getImageOpenURL( arg0 );
	    return KJS::String( u.url() );
	}
        case MethodShowProperties:
	{
	    // The default args mean this dialog auto-shows
	    /*KPropertiesDialog *dlg =*/ new KPropertiesDialog( KURL(arg0) );
	    return KJS::Null();
	}
#endif // QT_ONLY
	default:
	    kdWarning() << "StdDialogImp has no method " << id << endl;
    }

    kdWarning() << "StdDialogImp returning null" << endl;

    return KJS::Null();
}

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
