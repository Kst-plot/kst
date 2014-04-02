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

#ifndef SCALARSCRIPTINTERFACE_H
#define SCALARSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "datascalar.h"
#include "vscalar.h"

namespace Kst {

class KSTCORE_EXPORT ScalarGenSI : public ScriptInterface
{
    Q_OBJECT
    ScalarPtr scalar;
public:
    explicit ScalarGenSI(ScalarPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newScalar(ObjectStore *store);

};


class KSTCORE_EXPORT ScalarDataSI : public ScriptInterface
{
    Q_OBJECT
    DataScalarPtr scalar;
public:
    explicit ScalarDataSI(DataScalarPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newScalar(ObjectStore *store);

};


class KSTCORE_EXPORT ScalarVectorSI : public ScriptInterface
{
    Q_OBJECT
    VScalarPtr scalar;
public:
    explicit ScalarVectorSI(VScalarPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newScalar(ObjectStore *store);

};

}
#endif // SCALARSCRIPTINTERFACE_H
