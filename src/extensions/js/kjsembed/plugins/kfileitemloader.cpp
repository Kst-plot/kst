/***************************************************************************
 *   Copyright (C) 2004 by ian reinhart geiser                             *
 *   geiseri@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "kfileitemloader.h"

#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/jsfactory.h>
#include <kjsembed/jsfactory_imp.h>
#include <kjsembed/kjsembedpart.h>

#include <kfileitem.h>
#include <kdebug.h>

namespace KJSEmbed {
namespace Bindings {

KFileItemLoader::KFileItemLoader(QObject *parent, const char *name, const QStringList &args)
 : JSBindingPlugin(parent, name, args)
{
}


KFileItemLoader::~KFileItemLoader()
{
}

KJS::Object KFileItemLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
{
        kdDebug() << "Loading a KFileItem object" << endl;
        JSOpaqueProxy *prx = new JSOpaqueProxy(  (void *) 0, "KFileItem" );

        KJS::Object proxyObj(prx);
        KFileItemImp::addBindings( exec, proxyObj );
        return proxyObj;
}

} // Bindings
} // KJSEmbed

KJSEmbed::Bindings::KFileItemImp::KFileItemImp( KJS::ExecState * exec, int id )
: JSProxyImp(exec), mid(id)
{

}

KJSEmbed::Bindings::KFileItemImp::~ KFileItemImp( )
{

}

void KJSEmbed::Bindings::KFileItemImp::addBindings( KJS::ExecState * exec, KJS::Object & object )
{

	JSOpaqueProxy *op = JSProxy::toOpaqueProxy( object.imp() );
	if ( !op ) 
	{
		kdWarning() << "KFileItemImp::addBindings() failed, not a JSOpaqueProxy" << endl;
		return;
	}
	
	if ( op->typeName() != "KFileItem" ) 
	{
		kdWarning() << "KFileItemImp::addBindings() failed, type is " << op->typeName() <<
	endl;
		return;
	}
	
	JSProxy::MethodTable methods[] = {
		{ Methodrefresh, "refresh"},
		{ MethodrefreshMimeType, "refreshMimeType"},
		{ Methodurl, "url"},
		{ MethodsetUrl, "setUrl"},
		{ MethodsetName, "setName"},
		{ MethodpermissionsString, "permissionsString"},
		{ Methoduser, "user"},
		{ Methodgroup, "group"},
		{ MethodisLink, "isLink"},
		{ MethodisDir, "isDir"},
		{ MethodisFile, "isFile"},
		{ MethodisReadable, "isReadable"},
		{ MethodlinkDest, "linkDest"},
		{ MethodtimeString, "timeString"}, 
		{ MethodisLocalFile, "isLocalFile"},
		{ Methodtext, "text"},
		{ Methodname, "name"}, 
		{ MethodmimeType, "mimeType"},
		{ MethodisMimeTypeKnown, "isMimeTypeKnown"},
		{ MethodmimeComment, "mimeComment"},
		{ MethodiconName, "iconName"},
		{ Methodpixmap, "pixmap"},
		{ Methodoverlays, "overlays"},
		{ MethodgetStatusBarInfo, "getStatusBarInfo"},
		{ MethodgetToolTipText, "getToolTipText"}, 
		{ Methodrun, "run"},
		{ 0, 0 }
	};
	
	int idx = 0;
	do {
		KFileItemImp *meth = new KFileItemImp( exec, methods[idx].id );
		object.put( exec , methods[idx].name, KJS::Object(meth) );
		++idx;
	} while( methods[idx].id );

}

KJS::Value KJSEmbed::Bindings::KFileItemImp::call( KJS::ExecState * exec, KJS::Object & self, const KJS::List & args )
{
	kdDebug() << "KFileItemImp::call() " << mid << endl;
	JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
	if ( !op ) {
		kdWarning() << "KFileItemImp::call() failed, not a JSOpaqueProxy" << endl;
		return KJS::Value();
	}
	
	if ( op->typeName() != "KFileItem" ) {
		kdWarning() << "KFileItemImp::call() failed, type is " << op->typeName() << endl;
		return KJS::Value();
	}
	
	KFileItem *obj = op->toNative<KFileItem >();
	
	KJS::Value retValue = KJS::Value();
	switch ( mid ) {
	case Methodrefresh:
		obj->refresh();
		break;
	case MethodrefreshMimeType:
		obj->refreshMimeType(); 
		break;
	case Methodurl:
	{
		QString url = obj->url().url();
		retValue = KJS::String(url);
		break;
	}
	case MethodsetUrl:
	{
		QString url = extractQString(exec, args, 0);
		obj->setURL(url);
		break;
	}
	case MethodsetName: 
	case MethodpermissionsString: 
	case Methoduser: 
	case Methodgroup: 
	case MethodisLink: 
	case MethodisDir: 
	case MethodisFile: 
	case MethodisReadable: 
	case MethodlinkDest: 
	case MethodtimeString: 
	case MethodisLocalFile: 
	case Methodtext:
	{
		retValue = convertToValue(exec, obj->text() );
		break;
	} 
	case Methodname:  
	case MethodmimeType: 
	case MethodisMimeTypeKnown: 
	case MethodmimeComment: 
	case MethodiconName: 
	{
		retValue = convertToValue( exec, obj->iconName() );
		break;
	}
	case Methodpixmap:
	{
		int size = extractInt(exec, args, 0);
		int state = extractInt(exec, args, 1);
		retValue = convertToValue(exec, obj->pixmap(size, state));
		break;
	}
	case Methodoverlays: 
	{
		retValue = convertToValue(exec, obj->overlays());
		break;
	}
	case MethodgetStatusBarInfo: 
	{
		retValue = KJS::String( obj->getStatusBarInfo() );
		break;
	}
	case MethodgetToolTipText:
	{
		int maxcount = extractInt(exec, args, 0);
		retValue = KJS::String(obj->getToolTipText(maxcount));
		break;
	}
	case Methodrun:
		obj->run();
		break;
	default:
		kdWarning() << "KFileItemImp has no method " << mid << endl;
		break;
	}
	
	op->setValue((void*) obj, "KFileItem");
	return retValue;

} 

#include <kgenericfactory.h>
typedef KGenericFactory<KJSEmbed::Bindings::KFileItemLoader> KFileItemLoaderFactory;
K_EXPORT_COMPONENT_FACTORY( libfileitemplugin, KFileItemLoaderFactory( "KFileItemLoader" ) )
