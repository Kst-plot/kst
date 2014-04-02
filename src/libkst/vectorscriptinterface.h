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

#ifndef VECTORSCRIPTINTERFACE_H
#define VECTORSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "datavector.h"
#include "generatedvector.h"

namespace Kst {

class VectorDataSI : public ScriptInterface
{
    Q_OBJECT
    DataVectorPtr vector;
public:
    explicit VectorDataSI(DataVectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *store);
};

class VectorGenSI : public ScriptInterface
{
    Q_OBJECT
    GeneratedVectorPtr vector;
public:
    explicit VectorGenSI(GeneratedVectorPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newVector(ObjectStore *store);
};

}
#endif // VECTORSCRIPTINTERFACE_H
