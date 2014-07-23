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
#include "editablematrix.h"

namespace Kst {

/*******************/
class KSTCORE_EXPORT MatrixCommonSI : public ScriptInterface
{
    Q_OBJECT

  public:
    QString value(QString& command);
    QString length(QString&);
    QString min(QString&);
    QString max(QString&);
    QString mean(QString&);
    QString width(QString&);
    QString height(QString&);
    QString dX(QString&);
    QString dY(QString&);
    QString minX(QString&);
    QString minY(QString&);
    QString store(QString &command);

  protected:
    MatrixPtr _matrix;
    QString noSuchFn(QString&) {return ""; }

};


/*******************/
class DataMatrixSI;
typedef QString (DataMatrixSI::*DataMatrixInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT DataMatrixSI : public MatrixCommonSI
{
    Q_OBJECT
public:
    explicit DataMatrixSI(DataMatrixPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newMatrix(ObjectStore *store);

    QString change(QString &);
    QString field(QString &);
    QString filename(QString &);
    QString startX(QString &);
    QString startY(QString &);

private:
    QMap<QString,DataMatrixInterfaceMemberFn> _fnMap;
    DataMatrixPtr _datamatrix;
};

/*******************/
class EditableMatrixSI;
typedef QString (EditableMatrixSI::*EditableMatrixInterfaceMemberFn)(QString& command);

class KSTCORE_EXPORT EditableMatrixSI : public MatrixCommonSI
{
    Q_OBJECT
public:
    explicit EditableMatrixSI(EditableMatrixPtr it);
    QString doCommand(QString);
    bool isValid();
    QByteArray endEditUpdate();

    static ScriptInterface* newMatrix(ObjectStore *store);

    QString load(QString &);

private:
    QMap<QString,EditableMatrixInterfaceMemberFn> _fnMap;
    EditableMatrixPtr _editablematrix;
};

}
#endif // MATRIXSCRIPTINTERFACE_H
