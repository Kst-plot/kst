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

#ifndef MATRIXSCRIPTINTERFACE_H
#define MATRIXSCRIPTINTERFACE_H

#include <QString>

#include "scriptinterface.h"
#include "datamatrix.h"

namespace Kst {

class KSTCORE_EXPORT MatrixDataSI : public ScriptInterface
{
    Q_OBJECT
    DataMatrixPtr matrix;
public:
    explicit MatrixDataSI(DataMatrixPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newMatrix(ObjectStore *store);
};

}
#endif // MATRIXSCRIPTINTERFACE_H
