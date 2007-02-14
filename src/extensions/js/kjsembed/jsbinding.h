// -*- c++ -*-

/*
 *  Copyright (C) 2001-2002, Richard J. Moore <rich@kde.org>
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

#ifndef KJSEMBEDJSBINDING_H
#define KJSEMBEDJSBINDING_H
#include "global.h"
#include <qstring.h>
#include <qvariant.h>
#include <qwidget.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>
#include <kjs/types.h>

/**
 * Implement QString-KJS::UString conversion methods. These methods are declared
 * by KJS, but libkjs doesn't actually contain their implementations.
 *
 * @version $Id$
 */
namespace KJSEmbed {
    class  JSObjectProxy;
    class JSOpaqueProxy;

	enum JavaScriptArrayType { None, List, Map };

    KJSEMBED_EXPORT JavaScriptArrayType checkArray( KJS::ExecState *exec, const KJS::Value &val );

    /** Converts a QVariant to the closest possible KJS::Value. */
    KJSEMBED_EXPORT KJS::Value convertToValue( KJS::ExecState *exec, const QVariant &val);

    /** Converts a KJS::Value to the closest possible QVariant. */
    KJSEMBED_EXPORT QVariant convertToVariant( KJS::ExecState *exec, const KJS::Value &v );

    /** Returns HTML that documents the specified object. */
    KJSEMBED_EXPORT QString dumpObject( KJS::ExecState *exec, KJS::Object &obj );

    /** Returns HTML that documents the QObject facilities of the specified object. */
    KJSEMBED_EXPORT QString dumpQObject( KJS::ExecState *exec, KJS::Object &obj );

    /** Returns a QStringList that contains all possible properties,
    * methods and slots for use with a completion object
    */
    KJSEMBED_EXPORT QStringList dumpCompletion( KJS::ExecState *exec, KJS::Object &obj);

    KJSEMBED_EXPORT QPen extractQPen(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QFont extractQFont(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QBrush extractQBrush(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QPalette extractQPalette(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QPixmap extractQPixmap( KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QImage extractQImage( KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QString extractQString( KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT int extractInt( KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QColor extractQColor(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QSize extractQSize(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT bool extractBool(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT double extractDouble(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT uint extractUInt(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QStringList extractQStringList(KJS::ExecState *exec, const KJS::List &args, int idx);

    KJSEMBED_EXPORT QDateTime extractQDateTime(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QDate extractQDate(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QTime extractQTime(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QRect extractQRect(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QPoint extractQPoint(KJS::ExecState *exec, const KJS::List &args, int idx);

    KJSEMBED_EXPORT QFont extractQFont(KJS::ExecState *exec, const KJS::List &args, int idx);
    KJSEMBED_EXPORT QStrList extractQStrList(KJS::ExecState *exec, const KJS::List &args, int idx);

    KJSEMBED_EXPORT QObject *extractQObject( KJS::ExecState *exec, const KJS::List &args, int idx );
    KJSEMBED_EXPORT QWidget *extractQWidget( KJS::ExecState *exec, const KJS::List &args, int idx );

    KJSEMBED_EXPORT JSOpaqueProxy *extractOpaqueProxy( KJS::ExecState *exec, const KJS::List &args, int idx );

    // Helpers for Variant conversions....
    KJSEMBED_EXPORT QStrList convertArrayToStrList( KJS::ExecState *exec, const KJS::Value &value);
    KJSEMBED_EXPORT QStringList convertArrayToStringList( KJS::ExecState *exec, const KJS::Value &value);
    KJSEMBED_EXPORT QDateTime convertDateToDateTime(KJS::ExecState *exec, const KJS::Value &value);
    KJSEMBED_EXPORT QValueList<QVariant> convertArrayToList( KJS::ExecState *exec, const KJS::Value &value );
    KJSEMBED_EXPORT QMap<QString, QVariant> convertArrayToMap( KJS::ExecState *exec, const KJS::Value &value );

    /**
     * This will throw an exception with the error string.
     * @returns An error object.
     */
    KJS::Object throwError( KJS::ExecState *exec, const QString &error, KJS::ErrorType type = KJS::GeneralError );
}

#endif // KJSEMBEDJSBINDING_H

// Local Variables:
// c-basic-offset: 4
// End:
