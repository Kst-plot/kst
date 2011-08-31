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

#ifndef STRINGSCRIPTINTERFACE_H
#define STRINGSCRIPTINTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QObject>

#include "scriptinterface.h"
#include "string_kst.h"
typedef QList<QByteArray> QByteArrayList;

namespace Kst {

class StringGenSI : public ScriptInterface
{
    Q_OBJECT
    StringPtr str;
public:
    StringGenSI(StringPtr it);
    QByteArrayList commands();
    QString doCommand(QString);
    bool isValid();
    QByteArray getHandle();
};

}

#endif // STRINGSCRIPTINTERFACE_H
