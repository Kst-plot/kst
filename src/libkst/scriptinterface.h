/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 Joshua Netterfield                               *
 *                   joshua.netterfield@gmail.com                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#endif

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>

#include "kstcore_export.h"
#include "object.h"

typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class NamedObject;

/** A script interface represents an object exposed through the scripting interface.
  * (ex., dialog, primitive, etc.)
  */
class KSTCORE_EXPORT ScriptInterface : public QObject
{
    Q_OBJECT
public:
    virtual QString doCommand(QString)=0;
    virtual bool isValid()=0;
    virtual QByteArray endEditUpdate()=0;
    static QString doNamedObjectCommand(QString command, NamedObject *n);
    static QString doObjectCommand(QString command, ObjectPtr ob);

    static QStringList getArgs(const QString &command);
    static QString getArg(const QString &command);

};

}

#endif // SCRIPTINTERFACE_H
