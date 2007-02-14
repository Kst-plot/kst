// -*- c++ -*-

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

#ifndef KJSEMBED_SAXHANDLER_H
#define KJSEMBED_SAXHANDLER_H

#include <qxml.h>
#include <kjs/object.h>

namespace KJSEmbed {
namespace BuiltIns {

/**
 * SAX handler that calls JS methods according to the SAX API.
 *
 * @author Richard Moore, rich@kde.org
 */
class SaxHandler : public QXmlDefaultHandler
{
public:
    /** Defines the errors that are specific to the KJS SAX support. */
    enum SaxHandlerError {
	ErrorNone,
	ErrorNoHandler,
	ErrorNotCallable
    };

    /** Creates a SaxHandler. */
    SaxHandler( KJS::ExecState *exec );
    /** Cleans up. */
    virtual ~SaxHandler();

    /**
     * Sets the handler object that defines the callbacks to be invoked
     * according to the SAX api.
     */
    void setHandler( const KJS::Object &handler );

    virtual bool startDocument();
    virtual bool endDocument();

    virtual bool startElement( const QString &ns, const QString &ln, const QString &qn,
			       const QXmlAttributes &attrs );
    virtual bool endElement( const QString &ns, const QString &ln, const QString &qn );
    virtual bool characters( const QString &chars );

    virtual QString errorString();

private:
    KJS::ExecState *exec;
    KJS::Object jshandler;
    int error;

    class SaxHandlerPrivate *d;
};

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

#endif // KJSEMBED_SAXHANDLER_H


