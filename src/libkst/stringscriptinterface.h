/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2014 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STRINGSCRIPTINTERFACE_H
#define STRINGSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "datastring.h"

namespace Kst {

class KSTCORE_EXPORT StringGenSI : public ScriptInterface
{
    Q_OBJECT
    StringPtr str;
public:
    explicit StringGenSI(StringPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newString(ObjectStore *store);

};

class KSTCORE_EXPORT StringDataSI : public ScriptInterface
{
    Q_OBJECT
    DataStringPtr str;
public:
    explicit StringDataSI(DataStringPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newString(ObjectStore *store);

};

}

#endif // STRINGSCRIPTINTERFACE_H
