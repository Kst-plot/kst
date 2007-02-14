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

#include "kjsembedpart.h"
#include "jsfactory.h"
#include "jsobjectproxy.h"
#include "jsopaqueproxy.h"
#include "jsbinding.h"

#include "jseventutils.h"

namespace KJSEmbed {

JSFactory *JSEventUtils::factory( const JSObjectProxy *prx )
{
    return prx->part()->factory();
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QMouseEvent *ev, const JSObjectProxy *context )
{
    KJS::Object mev = convertEvent( exec, (QEvent *)ev, context);

    mev.put( exec, "pos", convertToValue( exec, QVariant(ev->pos()) ) );
    mev.put( exec, "x", KJS::Number(ev->x()) );
    mev.put( exec, "y", KJS::Number(ev->y()) );

    mev.put( exec, "globalPos", convertToValue( exec, QVariant(ev->globalPos()) ) );
    mev.put( exec, "globalX", KJS::Number(ev->globalX()) );
    mev.put( exec, "globalY", KJS::Number(ev->globalY()) );

    mev.put( exec, "button", KJS::Number(ev->button()) );
    mev.put( exec, "state", KJS::Number(ev->state()) );
    mev.put( exec, "stateAfter", KJS::Number(ev->stateAfter()) );

    return mev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QPaintEvent *ev, const JSObjectProxy *context )
{
    KJS::Object pev = convertEvent( exec, (QEvent *)ev, context);

    pev.put( exec, "rect", convertToValue( exec, QVariant(ev->rect()) ) );
    pev.put( exec, "erased", convertToValue( exec, QVariant(ev->erased()) ) );
    return pev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QKeyEvent *ev, const JSObjectProxy *context )
{
    KJS::Object kev = convertEvent( exec, (QEvent *)ev, context);

    kev.put( exec, "key", KJS::Number(ev->key()) );
    kev.put( exec, "ascii", KJS::Number(ev->ascii()) );
    kev.put( exec, "state", KJS::Number(ev->state()) );
    kev.put( exec, "stateAfter", KJS::Number(ev->stateAfter()) );
    kev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
    kev.put( exec, "text", KJS::String(ev->text()));
    kev.put( exec, "isAutoRepeat", KJS::Boolean(ev->isAutoRepeat()) );
    kev.put( exec, "count", KJS::Number( ev->count()) );
    //kev.put( exec, "accept", KJS::Void( exec, QVariant(ev->accept()), context ) );
    //kev.put( exec, "ignore", KJS::Void( exec, QVariant(ev->ignore()), context ) );
    return kev;
}
KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QIMEvent *ev, const JSObjectProxy *context )
{
    KJS::Object qev = convertEvent( exec, (QEvent *)ev, context);

    qev.put( exec, "text", KJS::String(ev->text()) );
    qev.put( exec, "cursorPos", KJS::Number(ev->cursorPos()) );
    qev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
    //qev.put( exec, "accept", KJS::Void( exec, QVariant(ev->accept()), context ) );
    //qev.put( exec, "ignore", KJS::Void( exec, QVariant(ev->ignore()), context ) );
    return qev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QResizeEvent *ev, const JSObjectProxy *context )
{
    KJS::Object rev = convertEvent( exec, (QEvent *)ev, context);

    rev.put( exec, "size", convertToValue( exec, QVariant(ev->size()) ));
    rev.put( exec, "oldSize", convertToValue( exec, QVariant(ev->oldSize())  ));
    return rev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QFocusEvent *ev, const JSObjectProxy *context )
{
    KJS::Object fev = convertEvent( exec, (QEvent *)ev, context);

    fev.put( exec, "gotFocus", KJS::Boolean(ev->gotFocus()) );
    fev.put( exec, "lostFocus", KJS::Boolean(ev->lostFocus()) );
    return fev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QCloseEvent *ev, const JSObjectProxy *context )
{
    KJS::Object cev = convertEvent( exec, (QEvent *)ev, context);

    cev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
    //cev.put( exec, "accept", KJS::Void( exec, QVariant(ev->accept()), context ) );
    //cev.put( exec, "ignore", KJS::Void( exec, QVariant(ev->ignore()), context ) );
    return cev;
}

#ifdef ENABLE_CHILDEVENTS
KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QChildEvent *ev, const JSObjectProxy *context )
{
    KJS::Object cev = convertEvent( exec, (QEvent *)ev, context);

    cev.put( exec, "inserted", KJS::Boolean(ev->inserted()) );
    cev.put( exec, "removed", KJS::Boolean(ev->removed()) );
    cev.put( exec, "child", factory(context)->createProxy( exec, ev->child(), context ) );

    return cev;
}
#endif

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QMoveEvent *ev, const JSObjectProxy *context )
{
    KJS::Object mev = convertEvent( exec, (QEvent *)ev, context);

    mev.put( exec, "pos", convertToValue( exec, QVariant(ev->pos()) ));
    mev.put( exec, "oldPos", convertToValue( exec, QVariant(ev->oldPos()) ));
    return mev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QWheelEvent *ev, const JSObjectProxy *context )
{
    KJS::Object wev = convertEvent( exec, (QEvent *)ev, context);

    wev.put( exec, "delta", KJS::Number(ev->delta()) );
    wev.put( exec, "pos", convertToValue( exec, QVariant(ev->pos()) ) );
    wev.put( exec, "globalPos", convertToValue( exec, QVariant(ev->globalPos()) ) );
    wev.put( exec, "x", KJS::Number(ev->x()) );
    wev.put( exec, "y", KJS::Number(ev->y()) );
    wev.put( exec, "globalX", KJS::Number(ev->globalX()) );
    wev.put( exec, "globalY", KJS::Number(ev->globalY()) );

    wev.put( exec, "state", KJS::Number(ev->state()) );
    wev.put( exec, "orientation", KJS::Number(ev->orientation()) );
    wev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
    //wev.put( exec, "accept", KJS::Void( exec, QVariant(ev->accept()), context ) );
    //wev.put( exec, "ignore", KJS::Void( exec, QVariant(ev->ignore()), context ) );
    return wev;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
					const QEvent *ev, const JSObjectProxy * /*context*/)
{
    KJS::Object evnt( new JSOpaqueProxy(ev) );
    evnt.put( exec, "type", KJS::Number( (int) ev->type()) );
    evnt.put( exec, "spontaneous", KJS::Boolean(ev->spontaneous()) );
    return evnt;
}

KJS::Object JSEventUtils::convertEvent( KJS::ExecState * exec, const QDropEvent * ev, const JSObjectProxy * context )
{
	KJS::Object dev = convertEvent( exec, (QEvent *)ev, context);
	dev.put( exec, "pos", convertToValue( exec, QVariant(ev->pos()) ) );
	dev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
	dev.put( exec, "isActionAccepted", KJS::Boolean(ev->isActionAccepted()) );
	dev.put( exec, "source", factory(context)->createProxy(exec, ev->source(), context) );
	dev.put( exec, "action", KJS::Number( (int)(ev->action()) ) );
	return dev;
} 

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
				const QDragMoveEvent *ev, const JSObjectProxy *context )
{
	KJS::Object mev = convertEvent(exec, (QDropEvent *)ev, context);
	mev.put( exec, "answerRect", convertToValue( exec, QVariant(ev->answerRect()) ) );
	return mev;
} 

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
				const QTimerEvent *ev, const JSObjectProxy *context )
{
	KJS::Object dev = convertEvent( exec, (QEvent *)ev, context);
	dev.put( exec, "timerId", KJS::Number(ev->timerId()) );
	return dev;
} 

KJS::Object JSEventUtils::convertEvent( KJS::ExecState *exec,
				const QContextMenuEvent *ev, const JSObjectProxy *context )
{
	KJS::Object cxev = convertEvent( exec, (QEvent *)ev, context);
	cxev.put( exec, "x", KJS::Number(ev->x()) );
	cxev.put( exec, "y", KJS::Number(ev->y()) );
	cxev.put( exec, "globalX", KJS::Number(ev->globalX()) );
	cxev.put( exec, "globalY", KJS::Number(ev->globalY()) );
	cxev.put( exec, "pos",convertToValue( exec, QVariant(ev->pos()) ) );
	cxev.put( exec, "globalPos", convertToValue( exec, QVariant(ev->globalPos()) ) );
	cxev.put( exec, "state", KJS::Number( (int) ev->state()) );
	cxev.put( exec, "isAccepted", KJS::Boolean(ev->isAccepted()) );
	cxev.put( exec, "isConsumed", KJS::Boolean(ev->isConsumed()) );
	cxev.put( exec, "reason", KJS::Number((int)ev->reason()) );
	return cxev;	
} 



}// namespace KJSEmbed
