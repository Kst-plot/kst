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

#include "global.h"
#include "jseventmapper.h"
#include <qevent.h>
namespace KJSEmbed {

/** Used internally for the event handler table. */
struct EventType
{
    EventType( KJS::Identifier _id, QEvent::Type _type ) :
	id(_id), type(_type) {;}

    const KJS::Identifier id;
    const QEvent::Type type;
};

static EventType events[] = {
	EventType(KJS::Identifier("timerEvent"), QEvent::Timer),

#ifdef ENABLE_CHILDEVENTS
    EventType( KJS::Identifier("childInsertEvent"), QEvent::ChildInserted ),
    EventType( KJS::Identifier("childRemoveEvent"), QEvent::ChildRemoved ),
#endif

    EventType( KJS::Identifier("mouseReleaseEvent"), QEvent::MouseButtonRelease ),
    EventType( KJS::Identifier("mouseMoveEvent"), QEvent::MouseMove ),
    EventType( KJS::Identifier("mouseDoubleClickEvent"), QEvent::MouseButtonDblClick ),
    EventType( KJS::Identifier("mousePressEvent"), QEvent::MouseButtonPress ),

    EventType( KJS::Identifier("keyPressEvent"), QEvent::KeyPress ),
    EventType( KJS::Identifier("keyReleaseEvent"), QEvent::KeyRelease ),

    EventType( KJS::Identifier("paintEvent"), QEvent::Paint ),

    EventType( KJS::Identifier("moveEvent"), QEvent::Move ),
    EventType( KJS::Identifier("resizeEvent"), QEvent::Resize ),

    EventType( KJS::Identifier("closeEvent"), QEvent::Close ),

    EventType( KJS::Identifier("showEvent"), QEvent::Show ),
    EventType( KJS::Identifier("hideEvent"), QEvent::Hide ),

    EventType( KJS::Identifier("dragEnterEvent"), QEvent::DragEnter ),
    EventType( KJS::Identifier("dragMoveEvent"), QEvent::DragMove ),
    EventType( KJS::Identifier("dragLeaveEvent"), QEvent::DragLeave ),
    EventType( KJS::Identifier("dragResponseEvent"), QEvent::DragResponse ),
    EventType( KJS::Identifier("dropEvent"), QEvent::Drop ),

    EventType( KJS::Identifier(), QEvent::None )
};

JSEventMapper::JSEventMapper()
{
    int i = 0;
    do {
	addEvent( events[i].id, events[i].type );
	i++;
    } while( events[i].type != QEvent::None );
}

JSEventMapper::~JSEventMapper()
{
}

void JSEventMapper::addEvent( const KJS::Identifier &name, QEvent::Type t )
{
    handlerToEvent.insert( name.qstring(), (const uint *) t );
    eventToHandler.insert( (long) t, &name );
}

QEvent::Type JSEventMapper::findEventType( const KJS::Identifier &name ) const
{
    uint *evtp = handlerToEvent[ name.qstring() ];
    uint evt;
    if (evtp) {
	evt = (*evtp);
    } else {
	evt = 0;
    }
    return static_cast<QEvent::Type>( evt );
}

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
