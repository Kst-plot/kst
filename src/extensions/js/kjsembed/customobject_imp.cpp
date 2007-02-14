/*
 *  Copyright (C) 2001-2004, Richard J. Moore <rich@kde.org>
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
#include "global.h"
#include <qapplication.h>
#include <qobject.h>
#include <qobjectlist.h>
#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qdialog.h>
#include <qhbox.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmetaobject.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qsignal.h>
#include <qstrlist.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qsplashscreen.h>
#include <qtoolbox.h>
#include <qmenudata.h>

#include <private/qucom_p.h>
#include <private/qucomextra_p.h>

#ifndef QT_ONLY

#include <khtml_part.h>
#include <kaction.h>

#include <klistview.h>
#include <kurl.h>
#include <kmainwindow.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ksystemtray.h>
#include <kpopupmenu.h>

#endif // QT_ONLY

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jssecuritypolicy.h"
#include "jsfactory.h"
#include "jsbinding.h"
#include "slotproxy.h"
#include "jsopaqueproxy.h"

#include "customobject_imp.h"

#include "qtbindings/qcombobox_imp.h"
//#include "qtbindings/qlistviewitem_imp.h"
//#include "qtbindings/qlistview_imp.h"
//#include "qtbindings/qchecklistitem_imp.h"
#include "qtbindings/qpopupmenu_imp.h"
#include "qtbindings/qframe_imp.h"
#include "qtbindings/qmenudata_imp.h"
#include "qtbindings/qmenuitem_imp.h"



#define ADDBINDING(TYPE,PROXY) \
TYPE *ptr ## TYPE = dynamic_cast< TYPE *> ( PROXY );\
if ( ptr ## TYPE ) \
{ \
	TYPE ## Imp::addBindings( exec, object ); \
}

namespace KJSEmbed {
namespace Bindings {

void CustomObjectImp::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    kdDebug() << "CustomObjectImp::addBindings for " << object.className().qstring() << endl;
    CustomObjectImp *obj = 0;

    JSObjectProxy *proxy = JSProxy::toObjectProxy( object.imp() );
    if ( !proxy )
	return;
    kdDebug() << "CustomObjectImp::addBindings is a proxy" << endl;
    QObject *qobj = proxy->object();
    QWidget *widget = proxy->widget();

#ifndef QT_ONLY
    if ( qobj ) {
        KXMLGUIClient *gc = dynamic_cast<KXMLGUIClient *>( qobj );
        if ( gc ) {
            obj = new CustomObjectImp( exec, KXMLGUIClientActionCollection, proxy );
            object.put( exec, "actionCollection", KJS::Object(obj) );
        }
    }

    KActionCollection *collection = dynamic_cast<KActionCollection*>( qobj );
    if( collection ) proxy->setOwner(JSProxy::Native);
#endif // QT_ONLY

    if ( widget ) {
	obj = new CustomObjectImp( exec, WidgetGrabWidget, proxy );
	object.put(exec, "grabWidget", KJS::Object(obj));
	obj = new CustomObjectImp( exec, WidgetDrawLine, proxy );
	object.put( exec, "drawLine", KJS::Object(obj) );
	obj = new CustomObjectImp( exec, WidgetDrawText, proxy );
	object.put( exec, "drawText", KJS::Object(obj) );

	QListBox *listbox = dynamic_cast<QListBox *>( widget );
	if ( listbox ) {
	    obj = new CustomObjectImp( exec, ListBoxInsertItem, proxy );
	    object.put( exec, "insertItem", KJS::Object(obj) );
	}

	QListView *listview = dynamic_cast<QListView *>( widget );
	if ( listview ) {
	    obj = new CustomObjectImp( exec, ListViewAddColumn, proxy );
	    object.put( exec, "addColumn", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, ListViewInsertItem, proxy );
	    object.put( exec, "insertItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, ListViewSelectedItem, proxy );
	    object.put( exec, "selectedItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, ListViewHideColumn, proxy );
	    object.put( exec, "hideColumn", KJS::Object(obj) );
      obj = new CustomObjectImp( exec, ListViewTakeItem, proxy );
      object.put( exec, "takeItem", KJS::Object(obj) );
      obj = new CustomObjectImp( exec, ListViewFirstChild, proxy );
      object.put( exec, "firstChild", KJS::Object(obj) );
      obj = new CustomObjectImp( exec, ListViewCurrentItem, proxy );
      object.put( exec, "currentItem", KJS::Object(obj) );
	}
	QSplashScreen *splashscreen = dynamic_cast<QSplashScreen *>(widget);
	if( splashscreen )
	{
	     obj = new CustomObjectImp(exec, QSplashScreenSetPixmap, proxy );
	     object.put(exec, "setPixmap", KJS::Object(obj) );
	     obj = new CustomObjectImp(exec, QSplashScreenPixmap, proxy );
	     object.put(exec, "pixmap", KJS::Object(obj) );
	     obj = new CustomObjectImp(exec, QSplashScreenFinish, proxy );
	     object.put(exec, "finish", KJS::Object(obj) );
	     obj = new CustomObjectImp(exec, QSplashScreenRepaint, proxy );
	     object.put(exec, "repaint", KJS::Object(obj) );
	}

#ifndef QT_ONLY
	KMainWindow *mw = dynamic_cast<KMainWindow *>( widget );
	if ( mw ) {
	    obj = new CustomObjectImp( exec, MainWindowSetCentralWidget, proxy );
	    object.put( exec, "setCentralWidget", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, MainWindowCreateGUI, proxy );
	    object.put( exec, "createGUI", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, MainWindowMenuBar, proxy );
	    object.put( exec, "menuBar", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, MainWindowStatusBar, proxy );
	    object.put( exec, "statusBar", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, MainWindowSetStandardToolBarMenuEnabled, proxy );
	    object.put( exec, "setStandardToolBarMenuEnabled", KJS::Object(obj) );
	}

	KSystemTray *systray = dynamic_cast<KSystemTray *>( qobj );
	if ( systray ) {
	    obj = new CustomObjectImp( exec, KSystemTrayContextMenu, proxy );
	    object.put( exec, "contextMenu", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KSystemTrayActionCollection, proxy );
	    object.put( exec, "actionCollection", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KSystemTrayLoadIcon, proxy );
	    object.put( exec, "loadIcon", KJS::Object(obj) );
	}

#endif // QT_ONLY

	QTabWidget *tw = dynamic_cast<QTabWidget *>( widget );
	if ( tw ) {
	    obj = new CustomObjectImp( exec, TabWidgetAddTab, proxy );
	    object.put( exec, "addTab", KJS::Object(obj) );
	}

	QScrollView *sv = dynamic_cast<QScrollView *>( widget );
	if ( sv ) {
	    obj = new CustomObjectImp( exec, ScrollViewViewport, proxy );
	    object.put( exec, "viewport", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, ScrollViewAddChild, proxy );
	    object.put( exec, "addChild", KJS::Object(obj) );
	}

	//ADDBINDING( QMenuItem, widget)
	//ADDBINDING( QMenuData, widget)

	ADDBINDING( QPopupMenu, widget )
	ADDBINDING( QComboBox, widget )
	//ADDBINDING( QFrame, widget )
	/*ADDBINDING( QListView, widget )*/

	/*
	QMenuData *md = dynamic_cast<QMenuData *>( widget );
	if ( md ) {
		QMenuDataImp::addBindings( exec, object );
	}

	QMenuItem *mitem = dynamic_cast<QMenuItem *>( widget );
	if ( mitem ) {
		QMenuItemImp::addBindings( exec, object );
	}
	 */
	QMenuData *menuData = dynamic_cast<QMenuData*>(widget);
	if( menuData )
	{
            obj = new CustomObjectImp( exec, QMenuDataInsertItem, proxy );
	    object.put( exec, "insertItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QMenuDataRemoveItem, proxy );
	    object.put( exec, "removeItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QMenuDataRemoveItem, proxy );
	    object.put( exec, "insertSeparator", KJS::Object(obj) );
	}

	QHBox *hbox = dynamic_cast<QHBox *>( widget );
	if ( hbox ) {
	    obj = new CustomObjectImp( exec, HBoxSpacing, proxy );
	    object.put( exec, "setSpacing", KJS::Object(obj) );
	}
	QToolBox *tb = dynamic_cast<QToolBox *>( qobj );
	if ( tb ) {
	    obj = new CustomObjectImp( exec, QToolBoxAddItem, proxy );
	    object.put( exec, "addItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxInsertItem, proxy );
	    object.put( exec, "insertItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxRemoveItem, proxy );
	    object.put( exec, "removeItem", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxItemLabel, proxy );
	    object.put( exec, "itemLabel", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxSetItemLabel, proxy );
	    object.put( exec, "setItemLabel", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxItemIconSet, proxy );
	    object.put( exec, "itemIconSet", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxSetItemIconSet, proxy );
	    object.put( exec, "setItemIconSet", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, QToolBoxItem, proxy );
	    object.put( exec, "item", KJS::Object(obj) );
    	}
	return;
    }

    if ( qobj ) {
	QApplication *app = dynamic_cast<QApplication *>( qobj );
	if ( app ) {
	    obj = new CustomObjectImp( exec, ApplicationExec, proxy );
	    object.put( exec, "exec", KJS::Object(obj) );
	}

#ifndef QT_ONLY

	KParts::ReadOnlyPart *ropart = dynamic_cast<KParts::ReadOnlyPart *>( qobj );
	if ( ropart ) {
	    obj = new CustomObjectImp( exec, KReadOnlyPartOpenStream, proxy );
	    object.put( exec, "openStream", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KReadOnlyPartWriteStream, proxy );
	    object.put( exec, "writeStream", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KReadOnlyPartCloseStream, proxy );
	    object.put( exec, "closeStream", KJS::Object(obj) );
	}

	KHTMLPart *htmlpart = dynamic_cast<KHTMLPart *>( qobj );
	if ( htmlpart ) {
	    obj = new CustomObjectImp( exec, KHTMLPartSelectAll, proxy );
	    object.put( exec, "selectAll", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KHTMLPartHasSelection, proxy );
	    object.put( exec, "hasSelection", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, KHTMLPartSelectedText, proxy );
	    object.put( exec, "selectedText", KJS::Object(obj) );
	}

#endif // QT_ONLY

	QTimer *timer = dynamic_cast<QTimer *>( qobj );
	if ( timer ) {
	    obj = new CustomObjectImp( exec, TimerIsActive, proxy );
	    object.put( exec, "isActive", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, TimerStart, proxy );
	    object.put( exec, "start", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, TimerStop, proxy );
	    object.put( exec, "stop", KJS::Object(obj) );
	}

	QBoxLayout *box = dynamic_cast<QBoxLayout *>( qobj );
	if ( box ) {
	    obj = new CustomObjectImp( exec, BoxLayoutAddWidget, proxy );
	    object.put( exec, "addWidget", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, BoxLayoutAddSpacing, proxy );
	    object.put( exec, "addSpacing", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, BoxLayoutAddStretch, proxy );
	    object.put( exec, "addStretch", KJS::Object(obj) );
	    obj = new CustomObjectImp( exec, BoxLayoutAddLayout, proxy );
	    object.put( exec, "addLayout", KJS::Object(obj) );
    	}
    }
}

CustomObjectImp::CustomObjectImp( KJS::ExecState *exec, int mid, JSObjectProxy *parent )
    : JSProxyImp(exec), id(mid), proxy(parent)
{
}

KJS::Value CustomObjectImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    switch( id ) {
	case WidgetGrabWidget:
	    return widgetGrabWidget( exec, self, args );
	    break;
	case WidgetDrawLine:
	    widgetDrawLine( exec, self, args );
	    return KJS::Value();
	    break;
	case WidgetDrawText:
	    widgetDrawText( exec, self, args );
	    return KJS::Value();
	    break;
	case ListBoxInsertItem:
	    listBoxInsertItem( exec, self, args );
	    return KJS::Value();
	    break;
	case ListViewAddColumn:
	    listViewAddColumn( exec, self, args );
	    return KJS::Value();
	    break;
	case ListViewInsertItem:
	    listViewInsertItem( exec, self, args );
	    return KJS::Value();
	    break;
	case ListViewSelectedItem:
	    return listViewSelectedItem( exec, self, args );
	    break;
	case ListViewHideColumn:
	    listViewHideColumn( exec, self, args );
	    return KJS::Value();
	    break;
	case MainWindowSetCentralWidget:
	    mainWinSetCentralWidget( exec, self, args );
	    return KJS::Value();
	    break;
	case MainWindowCreateGUI:
	    mainWinCreateGUI( exec, self, args );
	    return KJS::Value();
	    break;
	case MainWindowMenuBar:
	    return mainWinMenuBar( exec, self, args );
	    break;
	case MainWindowStatusBar:
	    return mainWinStatusBar( exec, self, args );
	    break;
	case MainWindowSetStandardToolBarMenuEnabled:
	    mainWinSetStandardToolBarMenuEnabled( exec, self, args );
	    return KJS::Value();
	    break;
	case TabWidgetAddTab:
	    tabWidgetAddTab( exec, self, args );
	    return KJS::Value();
	    break;
	case ScrollViewViewport:
	    return scrollViewViewport( exec, self, args );
	    break;
	case ScrollViewAddChild:
	    scrollViewAddChild( exec, self, args );
	    return KJS::Value();
	    break;
	case ApplicationExec:
	    return applicationExec( exec, self, args );
	    break;
	case TimerIsActive:
	    return timerIsActive( exec, self, args );
	    break;
	case TimerStart:
	    return timerStart( exec, self, args );
	    break;
	case TimerStop:
	    return timerStop( exec, self, args );
	    break;
	case BoxLayoutAddWidget:
	    boxLayoutAddWidget( exec, self, args );
	    return KJS::Value();
	    break;
	case BoxLayoutAddSpacing:
	    boxLayoutAddSpacing( exec, self, args );
	    return KJS::Value();
	    break;
	case BoxLayoutAddStretch:
	    boxLayoutAddStretch( exec, self, args );
	    return KJS::Value();
	    break;
	case BoxLayoutAddLayout:
	    boxLayoutAddLayout( exec, self, args );
	    return KJS::Value();
	    break;
	case URLRequesterSetMode:
	case URLRequesterSetFilter:
	case URLRequesterUrl:
	case URLRequesterSetLocalProtocol:
	case URLRequesterLocalProtocol:
	    kdWarning() << "CustomObjectImp unsupported method call " << id << endl;
	    break;
	case KXMLGUIClientActionCollection:
	    return xmlguiClientActionCollection( exec, self, args );
	    break;
	case KReadOnlyPartOpenStream:
	    return kroPartOpenStream( exec, self, args );
	    break;
	case KReadOnlyPartWriteStream:
	    return kroPartWriteStream( exec, self, args );
	    break;
	case KReadOnlyPartCloseStream:
	    return kroPartCloseStream( exec, self, args );
	    break;
	case HBoxSpacing:
	    hboxSpacing( exec, self, args );
	    return KJS::Value();
	    break;
	case KHTMLPartSelectAll:
	    khtmlPartSelectAll( exec, self, args );
	    return KJS::Value();
	    break;
	case KHTMLPartHasSelection:
	    return khtmlPartHasSelection( exec, self, args );
	    break;
	case KHTMLPartSelectedText:
	    return khtmlPartSelectedText( exec, self, args );
	    break;
	case KSystemTrayContextMenu:
	    return ksystemtrayContextMenu( exec, self, args );
	    break;
	case KSystemTrayActionCollection:
	    return ksystemtrayActionCollection( exec, self, args );
	    break;
	case KSystemTrayLoadIcon:
	    return ksystemtrayLoadIcon( exec, self, args );
	    break;
	case QSplashScreenSetPixmap:
	    return qsplashScreenSetPixmap(exec, self, args );
	    break;
	case QSplashScreenPixmap:
	    return qsplashScreenPixmap(exec, self, args );
	    break;
	case QSplashScreenFinish:
	    return qsplashScreenFinish(exec, self, args );
	    break;
	case QSplashScreenRepaint:
	    return qsplashScreenRepaint(exec, self, args );
	    break;
	case QToolBoxAddItem:
		return qtoolBoxAddItem(exec, self, args );
		break;
	case QToolBoxInsertItem:
		return qtoolBoxInsertItem(exec, self, args );
		break;
	case QToolBoxRemoveItem:
		qtoolBoxRemoveItem(exec, self, args );
		return KJS::Boolean(true);
		break;
	case QToolBoxItemLabel:
		return qtoolBoxItemLabel(exec, self, args );
		break;
	case QToolBoxSetItemLabel:
		qtoolBoxSetItemLabel(exec, self, args );
		return KJS::Boolean(true);
		break;
	case QToolBoxItemIconSet:
		return qtoolBoxItemIconSet(exec, self, args );
		break;
	case QToolBoxSetItemIconSet:
		qtoolBoxSetItemIconSet(exec, self, args );
		return KJS::Boolean(true);
		break;
	case QToolBoxItem:
		return qtoolBoxItem(exec, self, args );
		break;
	case QMenuDataInsertItem:
		return qmenuDataInsertItem(exec, self, args );
		break;
	case QMenuDataRemoveItem:
		qmenuDataRemoveItem(exec, self, args );
		return KJS::Boolean(true);
		break;
	case QMenuDataInsertSeparator:
		qmenuDataInsertSeparator(exec, self, args );
		return KJS::Boolean(true);
		break;

  case ListViewTakeItem:
      return qlistViewTakeItem(exec, self, args );
      break;
  case ListViewFirstChild:
      return qlistViewFirstChild(exec, self, args );
      break;
  case ListViewCurrentItem:
      return qlistViewFirstChild(exec, self, args );
      break;
	default:
	    break;
    }

    return KJS::ObjectImp::call( exec, self, args );
}

KJS::Value CustomObjectImp::qsplashScreenSetPixmap(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
	QSplashScreen *ss = dynamic_cast<QSplashScreen*>(proxy->widget());
    	if ( ss )
	{
		QPixmap pix = extractQPixmap(exec, args, 0 );
		ss->setPixmap(pix);
	}
	return KJS::Value();
}

KJS::Value CustomObjectImp::qsplashScreenPixmap(KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
	QSplashScreen *ss = dynamic_cast<QSplashScreen*>(proxy->widget());
    	if ( ss )
	{
		QPixmap pix = *(ss->pixmap());
		return convertToValue(exec, pix);
	}
	return KJS::Value();
}

KJS::Value CustomObjectImp::qsplashScreenFinish(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
	QSplashScreen *ss = dynamic_cast<QSplashScreen*>(proxy->widget());
    	if ( ss )
	{
		QWidget *w = extractQWidget(exec, args, 0 );
		ss->finish(w);
	}
	return KJS::Value();
}

KJS::Value CustomObjectImp::qsplashScreenRepaint(KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
	QSplashScreen *ss = dynamic_cast<QSplashScreen*>(proxy->widget());
    	if ( ss )
	{
		ss->repaint();
	}
	return KJS::Value();
}


KJS::Value CustomObjectImp::widgetGrabWidget(KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
    QWidget *w = proxy->widget();
    if ( !w )
	return KJS::Value();
    QVariant pix(QPixmap::grabWidget(w) );
    return convertToValue( exec, pix );

}
void CustomObjectImp::widgetDrawLine( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 4 )
	return;

    QWidget *w = proxy->widget();
    if ( !w )
	return;

    int x = extractInt( exec, args, 0 );
		int y = extractInt( exec, args, 1 );
		int x1 = extractInt( exec, args, 2 );
		int y1 = extractInt( exec, args, 3 );

    QPainter p(w);
    p.drawLine(x,y,x1,y1);
}

void CustomObjectImp::widgetDrawText( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 3 )
	return;

    QWidget *w = proxy->widget();
    if ( !w )
	return;

		int x = extractInt( exec, args, 0 );
		int y = extractInt( exec, args, 1 );
		QString s = extractQString( exec, args, 2 );
    w->drawText( x, y, s );
}

void CustomObjectImp::listBoxInsertItem( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 1 )
	return;

    QListBox *lb = dynamic_cast<QListBox *>( proxy->object() );
    if ( !lb )
	return;

		QString s = extractQString( exec, args, 0 );
    lb->insertItem( s );
}

void CustomObjectImp::listViewAddColumn( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 1 )
	return;

    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if ( !lv )
	return;

		QString s = extractQString( exec, args, 0 );
    lv->addColumn( s );
}

void CustomObjectImp::listViewInsertItem( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( !args.size() )
	return;

#ifndef QT_ONLY

    KListView *klv = dynamic_cast<KListView *>( proxy->object() );
    if ( klv ) {
	KListViewItem *lvm = new KListViewItem(klv);

	for( int idx = 0; idx < args.size(); ++idx){
		QVariant arg = convertToVariant(exec, args[idx]);
		if( arg.canCast(QVariant::String) )
			lvm->setText(idx, arg.toString());
		else if ( arg.canCast(QVariant::Pixmap) )
			lvm->setPixmap(idx, arg.toPixmap());
	}
	return;
    }

#endif // QT_ONLY

    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if ( lv ) {
	QListViewItem *lvm = new QListViewItem(lv);

	for( int idx = 0; idx < args.size(); ++idx){
		QVariant arg = convertToVariant(exec, args[idx]);
		if( arg.canCast(QVariant::String) )
			lvm->setText(idx, arg.toString());
		else if ( arg.canCast(QVariant::Pixmap) )
			lvm->setPixmap(idx, arg.toPixmap());
	}

    }
}

KJS::Value CustomObjectImp::listViewSelectedItem( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() )
	return KJS::Value();

    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if ( !lv )
	return KJS::Value();

    QListViewItem *lvi = lv->selectedItem();
    if ( !lvi )
 	return KJS::Null();

    QCheckListItem *cli = dynamic_cast<QCheckListItem *>( lvi );
    if ( cli ) {
	JSOpaqueProxy *prx = new JSOpaqueProxy( lvi, "QCheckListItem" );
	KJS::Object proxyObj(prx);
        proxy->part()->factory()->extendOpaqueProxy(exec, proxyObj);
	return proxyObj;
    }

    JSOpaqueProxy *prx = new JSOpaqueProxy( lvi, "QListViewItem" );
    KJS::Object proxyObj(prx);
    proxy->part()->factory()->extendOpaqueProxy(exec, proxyObj);
    return proxyObj;

}

void CustomObjectImp::listViewHideColumn( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 1 )
	return;

    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if ( !lv )
	return;

		int i = extractInt( exec, args, 0 );
    lv->hideColumn( i );
}

void CustomObjectImp::mainWinSetCentralWidget( KJS::ExecState *exec,
						KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY

    if ( args.size() != 1 )
	return;

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *cwproxy = JSProxy::toObjectProxy( jsobj.imp() );
    QWidget *cw = cwproxy ? cwproxy->widget() : 0;
    if ( !cw )
	return;
    KMainWindow *mw = dynamic_cast<KMainWindow *>( proxy->object() );
    if ( !mw )
	return;

    mw->setCentralWidget( cw );

#else // QT_ONLY
    Q_UNUSED( exec );
    Q_UNUSED( args );
    return;
#endif // QT_ONLY
}

void CustomObjectImp::mainWinSetStandardToolBarMenuEnabled( KJS::ExecState *exec,
							    KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY

    if ( args.size() != 1 )
	return;

    KMainWindow *mw = dynamic_cast<KMainWindow *>( proxy->object() );
    if ( !mw )
	return;

    mw->setStandardToolBarMenuEnabled( args[0].toBoolean(exec) );

#else // QT_ONLY
    Q_UNUSED( exec );
    Q_UNUSED( args );
    return;
#endif // QT_ONLY
}

void CustomObjectImp::mainWinCreateGUI( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY
    kdDebug(80001) << "mainWinCreateGUI() called" << endl;
    KMainWindow *mw = dynamic_cast<KMainWindow *>( proxy->object() );
    if ( !mw ) {
	kdWarning() << "mainWinCreateGUI() called on non-KMainWindow" << endl;
	return;
    }

		mw->createGUI( extractQString( exec, args, 0 ) );
#else // QT_ONLY
    Q_UNUSED( exec );
    Q_UNUSED( args );
    return;
#endif // QT_ONLY
}

KJS::Value CustomObjectImp::mainWinMenuBar( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
#ifndef QT_ONLY
    kdDebug(80001) << "mainWinMenuBar() called" << endl;
    KMainWindow *mw = dynamic_cast<KMainWindow *>( proxy->object() );
    if ( !mw ) {
	kdWarning() << "mainWinMenuBar() called on non-KMainWindow" << endl;
	return KJS::Value();
    }

    QObject *obj = mw->menuBar();
    return proxy->part()->factory()->createProxy( exec, obj, proxy );

#else // QT_ONLY
    Q_UNUSED( exec );
    return KJS::Value();
#endif // QT_ONLY
}

KJS::Value CustomObjectImp::mainWinStatusBar( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
#ifndef QT_ONLY
    kdDebug(80001) << "mainWinStatusBar() called" << endl;
    KMainWindow *mw = dynamic_cast<KMainWindow *>( proxy->object() );
    if ( !mw ) {
	kdWarning() << "mainWinStatusBar() called on non-KMainWindow" << endl;
	return KJS::Value();
    }

    QObject *obj = mw->statusBar();
    return proxy->part()->factory()->createProxy( exec, obj, proxy );

#else // QT_ONLY
    Q_UNUSED( exec );
    return KJS::Value();
#endif // QT_ONLY
}

KJS::Value CustomObjectImp::xmlguiClientActionCollection( KJS::ExecState *exec, KJS::Object&, const KJS::List& )
{
#ifndef QT_ONLY
    KXMLGUIClient *gc = dynamic_cast<KXMLGUIClient *>( proxy->object() );
    if ( !gc ) {
	kdDebug(80001) << "Unable to cast to XMLGUIClient" << endl;
	return KJS::Value();
    }

    KActionCollection *ac = gc->actionCollection();
    if (!ac) {
	kdDebug(80001) << "XMLGUIClient action collection is null" << endl;
	return KJS::Null();
    }
/*    JSObjectProxy *prxy = new JSObjectProxy( proxy->part(), ac );
    KJS::Object proxyObject( prxy );
    prxy->setOwner(JSProxy::Native);
    prxy->addBindings( exec, proxyObject );
    return proxyObject;
  */
   return proxy->part()->factory()->createProxy( exec, ac, proxy );
#else // QT_ONLY
    Q_UNUSED( exec );
    return KJS::Value();
#endif // QT_ONLY
}

void CustomObjectImp::tabWidgetAddTab( KJS::ExecState *exec, KJS::Object&, const KJS::List &args )
{
    if ( args.size() < 2 )
	return;

    QTabWidget *tw = dynamic_cast<QTabWidget *>( proxy->object() );
    if ( !tw )
	return;

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *tproxy = JSProxy::toObjectProxy( jsobj.imp() );
    QWidget *w = tproxy ? tproxy->widget() : 0;
    if ( !w )
	return;

		tw->addTab( w, extractQString( exec, args, 0 ) );
}

KJS::Value CustomObjectImp::scrollViewViewport( KJS::ExecState *exec, KJS::Object&, const KJS::List &args )
{
    QScrollView *sv = dynamic_cast<QScrollView *>( proxy->object() );
    if ( !sv )
	return KJS::Value();

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *sproxy = JSProxy::toObjectProxy( jsobj.imp() );
    QWidget *w = sproxy ? sproxy->widget() : 0;
    if ( !w )
	return KJS::Value();

    return proxy->part()->factory()->createProxy( exec, w, proxy );
}

void CustomObjectImp::scrollViewAddChild( KJS::ExecState *exec, KJS::Object&, const KJS::List &args )
{
    if ( args.size() < 1 )
	return;

    QScrollView *sv = dynamic_cast<QScrollView *>( proxy->object() );
    if ( !sv )
	return;

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *sproxy = JSProxy::toObjectProxy( jsobj.imp() );
    QWidget *w = sproxy ? sproxy->widget() : 0;
    if ( !w )
	return;

		int x = args.size() >= 2 ? extractInt( exec, args, 1 ) : 0;
		int y = args.size() >= 3 ? extractInt( exec, args, 2 ) : 0;

    sv->addChild( w, x, y );
}

KJS::Value CustomObjectImp::applicationExec( KJS::ExecState *, KJS::Object&, const KJS::List& )
{
    QApplication *app = dynamic_cast<QApplication *>( proxy->object() );
    if ( !app )
	return KJS::Value();

    return KJS::Number( app->exec() );
}

KJS::Value CustomObjectImp::timerIsActive( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
    QTimer *timer = dynamic_cast<QTimer *>( proxy->object() );
    if ( !timer )
	return KJS::Value();

    return KJS::Boolean( timer->isActive() );
}

KJS::Value CustomObjectImp::timerStart( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QTimer *timer = dynamic_cast<QTimer *>( proxy->object() );
    if ( !timer )
	return KJS::Value();

    int timerid;
    if ( args.size() == 1 )
			timerid = timer->start( extractInt( exec, args, 0 ) );
    else if ( args.size() == 2 )
			timerid = timer->start( extractInt( exec, args, 0 ), extractBool( exec, args, 0 ) );
    else
	return KJS::Value();

    return KJS::Number( timerid );
}

KJS::Value CustomObjectImp::timerStop( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
    QTimer *timer = dynamic_cast<QTimer *>( proxy->object() );
    if ( !timer )
	return KJS::Value();

    timer->stop();
    return KJS::Value();
}

void CustomObjectImp::boxLayoutAddWidget( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( (args.size() < 1) || (args.size() > 3) )
	return;

    QBoxLayout *box = dynamic_cast<QBoxLayout *>( proxy->object() );
    if ( !box )
	return;

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *proxy = JSProxy::toObjectProxy( jsobj.imp() );
    QWidget *w = proxy ? proxy->widget() : 0;
    if ( !w )
	return;

    if ( args.size() == 1 )
	box->addWidget( w );
    else if ( args.size() == 2 )
			box->addWidget( w, extractInt( exec, args, 1 ) );
    else if ( args.size() == 3 )
			box->addWidget( w, extractInt( exec, args, 1 ), extractInt( exec, args, 2 ) );
}

void CustomObjectImp::boxLayoutAddSpacing( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 1 )
	return;

    QBoxLayout *box = dynamic_cast<QBoxLayout *>( proxy->object() );
    if ( !box )
	return;

		box->addSpacing( extractInt( exec, args, 0 ) );
}

void CustomObjectImp::boxLayoutAddStretch( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() > 1 )
	return;

    QBoxLayout *box = dynamic_cast<QBoxLayout *>( proxy->object() );
    if ( !box )
	return;

    if ( args.size() == 0 )
	box->addStretch();
    else
			box->addStretch( extractInt( exec, args, 0 ) );
}

void CustomObjectImp::boxLayoutAddLayout( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( (args.size() < 1) || (args.size() > 2) )
	return;

    QBoxLayout *box = dynamic_cast<QBoxLayout *>( proxy->object() );
    if ( !box )
	return;

    KJS::Object jsobj = args[0].toObject(exec);
    JSObjectProxy *proxy = JSProxy::toObjectProxy( jsobj.imp() );
    QLayout *l = proxy ? (dynamic_cast<QLayout *>(proxy->object())) : 0;
    if ( !l )
	return;

    if ( args.size() == 1 )
	box->addLayout( l );
    else if ( args.size() == 2 )
			box->addLayout( l, extractInt( exec, args, 1 ) );
}

KJS::Value CustomObjectImp::kroPartOpenStream( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY

    if ( args.size() != 2 )
	return KJS::Boolean( false );

    KParts::ReadOnlyPart *ro = dynamic_cast<KParts::ReadOnlyPart *>( proxy->object() );
    if ( !ro )
	return KJS::Boolean( false );

		bool ok = ro->openStream( extractQString( exec, args, 0 )
				, KURL(extractQString( exec, args, 1 )) );
    return KJS::Boolean( ok );

#else // QT_ONLY
    Q_UNUSED( exec );
    Q_UNUSED( args );
    return KJS::Boolean( false );
#endif // QT_ONLY
}

KJS::Value CustomObjectImp::kroPartWriteStream( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY

    if ( args.size() != 1 )
	return KJS::Boolean( false );

    KParts::ReadOnlyPart *ro = dynamic_cast<KParts::ReadOnlyPart *>( proxy->object() );
    if ( !ro )
	return KJS::Boolean( false );

		QCString s = extractQString( exec, args, 0 ).latin1();
    bool ok = ro->writeStream( s );
    return KJS::Boolean( ok );

#else // QT_ONLY
    Q_UNUSED( exec );
    Q_UNUSED( args );
    return KJS::Boolean( false );
#endif // QT_ONLY
}

KJS::Value CustomObjectImp::kroPartCloseStream( KJS::ExecState *, KJS::Object &, const KJS::List &args )
{
#ifndef QT_ONLY

    if ( args.size() != 0 )
	return KJS::Boolean( false );

    KParts::ReadOnlyPart *ro = dynamic_cast<KParts::ReadOnlyPart *>( proxy->object() );
    if ( !ro )
	return KJS::Boolean( false );

    bool ok = ro->closeStream();
    return KJS::Boolean( ok );

#else // QT_ONLY
    Q_UNUSED( args );
    return KJS::Boolean( false );
#endif // QT_ONLY
}

void CustomObjectImp::hboxSpacing( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 1 )
	return;

    QHBox *box = dynamic_cast<QHBox *>( proxy->object() );
    if ( !box )
	return;

		box->setSpacing( extractInt( exec, args, 0 ) );
}

void CustomObjectImp::khtmlPartSelectAll( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
#ifndef QT_ONLY

    KHTMLPart *htmlpart = dynamic_cast<KHTMLPart *>( proxy->object() );
    if ( htmlpart )
	htmlpart->selectAll();

#endif // QT_ONLY
}

KJS::Value CustomObjectImp::khtmlPartHasSelection( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
#ifndef QT_ONLY

    KHTMLPart *htmlpart = dynamic_cast<KHTMLPart *>( proxy->object() );
    if ( !htmlpart )
	return KJS::Boolean();

    return KJS::Boolean( htmlpart->hasSelection() );
#endif // QT_ONLY
	return KJS::Null();
}

KJS::Value CustomObjectImp::khtmlPartSelectedText( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
#ifndef QT_ONLY

    KHTMLPart *htmlpart = dynamic_cast<KHTMLPart *>( proxy->object() );
    if ( !htmlpart )
	return KJS::String();

    return KJS::String( htmlpart->selectedText() );
#endif // QT_ONLY
return KJS::Null();
}

KJS::Value CustomObjectImp::ksystemtrayContextMenu( KJS::ExecState *exec, KJS::Object&, const KJS::List & )
{
#ifndef QT_ONLY
    KSystemTray *st = dynamic_cast<KSystemTray *>( proxy->widget() );
    if ( !st )
    {
	kdWarning() << "Not a KSystemTray" << endl;
	return KJS::Boolean(false);
    }
    kdDebug() << "Valid system tray?" << endl;
    KPopupMenu *pop = st->contextMenu();
    return proxy->part()->factory()->createProxy( exec, pop, proxy );
#endif // QT_ONLY
return KJS::Null();
}

KJS::Value CustomObjectImp::ksystemtrayActionCollection( KJS::ExecState *exec, KJS::Object&, const KJS::List & )
{
#ifndef QT_ONLY
    KSystemTray *st = dynamic_cast<KSystemTray *>( proxy->object() );
    if ( !st )
	return KJS::Value();

    KActionCollection *ac = st->actionCollection();
    return proxy->part()->factory()->createProxy( exec, ac, proxy );
#endif // QT_ONLY
return KJS::Null();
}

KJS::Value CustomObjectImp::ksystemtrayLoadIcon( KJS::ExecState *exec, KJS::Object&, const KJS::List &args )
{
#ifndef QT_ONLY
    // TODO: Support the 2 arg form of KSystemTray::loadIcon() when we support KInstance args
    if ( args.size() != 1 )
	return KJS::Value();

    QVariant pix( KSystemTray::loadIcon( extractQString( exec, args, 0 ) ) );
    return convertToValue( exec, pix );
#endif // QT_ONLY
return KJS::Null();
}

KJS::Value Bindings::CustomObjectImp::qlistViewTakeItem( KJS::ExecState * exec, KJS::Object &, const KJS::List &args )
{
    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if( !lv )
        return KJS::Boolean(false);
    KJS::Object lviObj = args[0].toObject(exec);
    JSOpaqueProxy *prxy = JSProxy::toOpaqueProxy(lviObj.imp() );
    QListViewItem *itm = prxy->toNative<QListViewItem>();
    if( !itm )
    {
        // Trow an error...
        return KJS::Boolean(false);
    }
    lv->takeItem(itm);
    return KJS::Boolean(true);
}

KJS::Value Bindings::CustomObjectImp::qlistViewFirstChild( KJS::ExecState * exec, KJS::Object &, const KJS::List &args )
{
    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if( !lv )
        return KJS::Boolean(false);
    QListViewItem *itm = lv->firstChild();
    KJS::Object jsObject( new JSOpaqueProxy( itm, "QListViewItem") );
    proxy->part()->factory()->extendOpaqueProxy(exec, jsObject);
    return jsObject;
}

KJS::Value Bindings::CustomObjectImp::qlistViewCurrentItem( KJS::ExecState * exec, KJS::Object &, const KJS::List &args )
{
    QListView *lv = dynamic_cast<QListView *>( proxy->object() );
    if( !lv )
        return KJS::Boolean(false);
    QListViewItem *itm = lv->currentItem();
    KJS::Object jsObject( new JSOpaqueProxy( itm, "QListViewItem") );
    proxy->part()->factory()->extendOpaqueProxy(exec, jsObject);
    return jsObject;
}

KJS::Value Bindings::CustomObjectImp::qtoolBoxAddItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args)
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return KJS::Boolean(false);
    if( args.size() == 2)
    {
        QWidget *w = extractQWidget( exec, args, 0 );
        QString lab = extractQString( exec, args, 1 );
        return KJS::Number( tb->addItem(w,lab) );
    }
    if( args.size() == 3)
    {
        QWidget *w = extractQWidget( exec, args, 0 );
        QIconSet ico = extractQPixmap( exec, args, 1 );
        QString lab = extractQString( exec, args, 2 );
        return KJS::Number( tb->addItem(w,ico,lab) );
    }
    return KJS::Boolean(false);
}

KJS::Value Bindings::CustomObjectImp::qtoolBoxInsertItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return KJS::Boolean(false);
    if( args.size() == 3)
    {
        int idx = extractInt(exec, args, 0);
        QWidget *w = extractQWidget( exec, args, 1 );
        QString lab = extractQString( exec, args, 2 );
        return KJS::Number( tb->insertItem(idx,w,lab) );
    }
    if( args.size() == 4)
    {
        int idx = extractInt(exec, args, 0);
        QWidget *w = extractQWidget( exec, args, 1 );
        QIconSet ico = extractQPixmap( exec, args, 2 );
        QString lab = extractQString( exec, args, 3 );
        return KJS::Number( tb->insertItem(idx,w,ico,lab) );
    }
    return KJS::Boolean(false);
}

void Bindings::CustomObjectImp::qtoolBoxRemoveItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return;
    QWidget *w = extractQWidget(exec, args, 0);
    tb->removeItem(w);
}

KJS::Value Bindings::CustomObjectImp::qtoolBoxItemLabel(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return KJS::Boolean(false);
    int idx = extractInt(exec, args, 0);
    return KJS::String(tb->itemLabel(idx).latin1());
}

void Bindings::CustomObjectImp::qtoolBoxSetItemLabel(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return;

    int idx = extractInt(exec, args, 0);
    QString lab = extractQString( exec, args, 1 );
    tb->setItemLabel(idx,lab);
}

KJS::Value Bindings::CustomObjectImp::qtoolBoxItemIconSet(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return KJS::Boolean(false);
    int idx = extractInt(exec, args, 0);
    QPixmap pix = tb->itemIconSet(idx).pixmap();
    return convertToValue( exec, pix );

}

void Bindings::CustomObjectImp::qtoolBoxSetItemIconSet(KJS::ExecState *exec, KJS::Object &, const KJS::List &args)
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return;
    int idx = extractInt(exec, args, 0);
    QIconSet ico = extractQPixmap( exec, args, 1);
    tb->setItemIconSet(idx,ico);
}

KJS::Value Bindings::CustomObjectImp::qtoolBoxItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QToolBox *tb = dynamic_cast<QToolBox *>( proxy->object() );
    if( !tb )
        return KJS::Boolean(false);
    int idx = extractInt(exec, args, 0);
    return proxy->part()->factory()->createProxy( exec, tb->item(idx), proxy );
}
QPopupMenu *getMenu(const KJS::Object &obj)
{
    JSObjectProxy *jsproxy = JSProxy::toObjectProxy( obj.imp() );
    return dynamic_cast<QPopupMenu *>( jsproxy->object() );
}

KJS::Value Bindings::CustomObjectImp::qmenuDataInsertItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QMenuData *menu = dynamic_cast<QMenuData *>( proxy->object() );
    if( !menu )
        return KJS::Boolean(false);
    int offset = 0;

    QString text = extractQString(exec,args, offset);
    QPixmap pix = extractQPixmap(exec,args, offset);
    int id = -1;
    int idx = -1;
    int newIndex = 0;
    QPopupMenu  *parentMenu = 0;

    if( !pix.isNull() )
    {
        ++offset;
        if( args.size() > offset)
            text = extractQString(exec,args,offset);
    }
    kdDebug() << "text " << text << " " << offset << endl;

    ++offset;
    if( args.size() > offset)
        parentMenu = getMenu(args[offset].toObject(exec));

    if( parentMenu )
        ++offset;
    kdDebug() << "text " << text << " " << offset << endl;

    if( args.size() > offset )
        id = extractInt(exec, args, offset);
    ++offset;
    kdDebug() << "text " << text << " " << offset << endl;

    if( args.size() > offset )
        idx = extractInt(exec, args, offset);

    kdDebug() << "text " << text << " " << offset << endl;

    if( parentMenu )
    {
        if( pix.isNull() )
        {
            newIndex = menu->insertItem(text, parentMenu, id, idx); // text, parent, id, idx
        }
        else
        {
           if( text.isEmpty() )
                newIndex = menu->insertItem(pix, parentMenu, id, idx); // pixmap, parent, id, idx
           else
                newIndex = menu->insertItem(pix, text, parentMenu, id, idx); // pixmap, text, parent, id, idx
        }
    }
    else
    {
        if( pix.isNull() )
        {
            newIndex = menu->insertItem(text, id, idx); // text, id, idx
        }
        else
        {
            if( text.isEmpty() )
                newIndex = menu->insertItem(pix, id, idx); // pixmap, id, idx
            else
                newIndex = menu->insertItem(pix, text, id, idx); // pixmap, text, id, idx
        }
    }

    return KJS::Number(newIndex);
}

void Bindings::CustomObjectImp::qmenuDataRemoveItem(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QMenuData *menu = dynamic_cast<QMenuData *>( proxy->object() );
    if( !menu )
        return;
    int idx = extractInt(exec, args, 0);
    menu->removeItem(idx);
}

KJS::Value Bindings::CustomObjectImp::qmenuDataInsertSeparator(KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    QMenuData *menu = dynamic_cast<QMenuData *>( proxy->object() );
    if( !menu )
        return KJS::Boolean(false);
    int idx = extractInt(exec, args, 0);
    return KJS::Number( menu->insertSeparator(idx) );
}

} // namespace KJSEmbed::Bindings
}// namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

