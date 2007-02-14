// -*- c++ -*-

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

#ifndef KJSEMBEDCUSTOMOBJECTIMP_H
#define KJSEMBEDCUSTOMOBJECTIMP_H

#include <kjs/object.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/slotutils.h>

#include <kjsembed/jsobjectproxy_imp.h>

class QObject;

namespace KJSEmbed {
namespace Bindings {

/**
 * Implements the JS methods of custom object bindings.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */
class KJSEMBED_EXPORT CustomObjectImp : public JSProxyImp
{
public:
    /** Identifiers for the methods provided by this class. */
    enum MethodId {
	WidgetGrabWidget,
	WidgetDrawLine,
	WidgetDrawText,

	ListBoxInsertItem,
  ListBoxRemoveItem,

	ListViewAddColumn,
	ListViewInsertItem,
	ListViewSelectedItem,
	ListViewHideColumn,
  ListViewTakeItem,
  ListViewFirstChild,
  ListViewCurrentItem,


	MainWindowSetCentralWidget,
	MainWindowCreateGUI,
	MainWindowMenuBar,
	MainWindowStatusBar,
	MainWindowSetStandardToolBarMenuEnabled,

	TabWidgetAddTab,

	ScrollViewViewport,
	ScrollViewAddChild,

	ApplicationExec,

	TimerIsActive,
	TimerStart,
	TimerStop,

	BoxLayoutAddWidget,
	BoxLayoutAddSpacing,
	BoxLayoutAddStretch,
	BoxLayoutAddLayout,

	URLRequesterSetMode,
	URLRequesterSetFilter,
	URLRequesterUrl,
	URLRequesterSetLocalProtocol,
	URLRequesterLocalProtocol,

	KXMLGUIClientActionCollection,

	KReadOnlyPartOpenStream,
	KReadOnlyPartWriteStream,
	KReadOnlyPartCloseStream,

	HBoxSpacing,

	KHTMLPartSelectAll,
	KHTMLPartHasSelection,
	KHTMLPartSelectedText,

	KSystemTrayContextMenu,
	KSystemTrayActionCollection,
	KSystemTrayLoadIcon,

	QCanvasViewCanvas,
	QCanvasViewSetCanvas,

	QSplashScreenSetPixmap,
	QSplashScreenPixmap,
	QSplashScreenFinish,
	QSplashScreenRepaint,

	QToolBoxAddItem,
	QToolBoxInsertItem,
	QToolBoxRemoveItem,
	QToolBoxItemLabel,
	QToolBoxSetItemLabel,
	QToolBoxItemIconSet,
	QToolBoxSetItemIconSet,
	QToolBoxItem,

	QMenuDataInsertItem,
	QMenuDataRemoveItem,
	QMenuDataInsertSeparator
    };

    static void addBindings( KJS::ExecState *exec, KJS::Object &object );

    CustomObjectImp( KJS::ExecState *exec, int id, JSObjectProxy *parent );
    virtual ~CustomObjectImp() {}

    KJS::Value widgetGrabWidget(KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void widgetDrawLine( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void widgetDrawText( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void listBoxInsertItem( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void listViewAddColumn( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void listViewInsertItem( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value listViewSelectedItem( KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    void listViewHideColumn( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void mainWinSetCentralWidget( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void mainWinCreateGUI( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value mainWinMenuBar( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value mainWinStatusBar( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void mainWinSetStandardToolBarMenuEnabled( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void tabWidgetAddTab( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );

    KJS::Value scrollViewViewport( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );
    void scrollViewAddChild( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );

    KJS::Value applicationExec( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    KJS::Value timerIsActive( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value timerStart( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value timerStop( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void boxLayoutAddWidget( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void boxLayoutAddSpacing( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void boxLayoutAddStretch( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    void boxLayoutAddLayout( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    KJS::Value xmlguiClientActionCollection( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    KJS::Value kroPartOpenStream( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value kroPartWriteStream( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );
    KJS::Value kroPartCloseStream( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void hboxSpacing( KJS::ExecState *exec, KJS::Object &, const KJS::List &args );

    void khtmlPartSelectAll( KJS::ExecState *, KJS::Object &, const KJS::List & );
    KJS::Value khtmlPartHasSelection( KJS::ExecState *, KJS::Object &, const KJS::List & );
    KJS::Value khtmlPartSelectedText( KJS::ExecState *, KJS::Object &, const KJS::List &args );

    KJS::Value ksystemtrayContextMenu( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );
    KJS::Value ksystemtrayActionCollection( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );
    KJS::Value ksystemtrayLoadIcon( KJS::ExecState *exec, KJS::Object&, const KJS::List &args );

    KJS::Value qsplashScreenSetPixmap(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qsplashScreenPixmap(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qsplashScreenFinish(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qsplashScreenRepaint(KJS::ExecState *exec, KJS::Object &, const KJS::List & );

    KJS::Value qlistViewTakeItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qlistViewFirstChild(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qlistViewCurrentItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );

    KJS::Value qtoolBoxAddItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qtoolBoxInsertItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    void qtoolBoxRemoveItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qtoolBoxItemLabel(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    void qtoolBoxSetItemLabel(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qtoolBoxItemIconSet(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    void qtoolBoxSetItemIconSet(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qtoolBoxItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );

    KJS::Value qmenuDataInsertItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    void qmenuDataRemoveItem(KJS::ExecState *exec, KJS::Object &, const KJS::List & );
    KJS::Value qmenuDataInsertSeparator(KJS::ExecState *exec, KJS::Object &, const KJS::List & );

    virtual bool implementsCall() const { return true; }
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    int id;
    JSObjectProxy *proxy;
    class CustomObjectImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDCUSTOMOBJECTIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
