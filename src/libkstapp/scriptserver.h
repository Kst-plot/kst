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

#ifndef SCRIPTSERVER_H
#define SCRIPTSERVER_H

#ifndef CALL_MEMBER_FN
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
#endif

#include "objectstore.h"
#include "scriptinterface.h"
#include <QLocalServer>
#include <QMap>

namespace Kst {

class ViewItem;

class ScriptServer;

typedef QByteArray (ScriptServer::*ScriptMemberFn)(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                            const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

class ScriptServer : public QObject
{
    Q_OBJECT
    QLocalServer* _server;
    ObjectStore* _store;
    ScriptInterface* _interface;
    IfSI* _if;
    MacroSI* _curMac;
    bool _curMacComEcho;
    QList<ViewItem*> vi;    // cache
    QMap<QByteArray,ScriptMemberFn> _fnMap;
    QMap<QByteArray,MacroSI*> _macroMap;
    QMap<QByteArray,VarSI*> _varMap;
public:
    ScriptServer(ObjectStore*obj);
    ~ScriptServer();
    QByteArray checkPrimatives(QByteArray&command,QLocalSocket* s);
public slots:
    void procConnection();
    void readSomething();
    QByteArray procMacro(QByteArray&command,QLocalSocket*s);
    QByteArray exec(QByteArray command,QLocalSocket* s,int ifMode=0, QByteArray ifEqual="");

protected:
    QByteArray noSuchFn(QByteArray& , QLocalSocket*,ObjectStore*,const int&, const QByteArray&,IfSI*& ,VarSI*) {return ""; }

    //
    // these commands are processed directly in ScriptServer and not by an interface.
    //

    // ObjectStore
    QByteArray getVectorList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newVector(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getEditableVectorList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newEditableVectorAndGetHandle(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getScalarList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newScalar(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getMatrixList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newMatrix(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getEditableMatrixList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newEditableMatrixAndGetHandle(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getStringList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newString(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newStringGen(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getCurveList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newCurve(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getEquationList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newEquation(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getHistogramList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newHistogram(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getPSDList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newPSD(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getImageList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newImage(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getCSDList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newCSD(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getBasicPluginList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray getBasicPluginTypeList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newBasicPlugin(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getArrowList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newArrow(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getBoxList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newBox(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getButtonList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newButton(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getLineEditList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newLineEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getCircleList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newCircle(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getEllipseList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newEllipse(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getLabelList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newLabel(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getLineList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newLine(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getPictureList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newPicture(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getPlotList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newPlot(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getSharedAxisBoxList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newSharedAxisBox(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray getSvgItemList(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newSvgItem(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Access to interfaces
    QByteArray beginEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray endEdit(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Quit:
    QByteArray done(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Destruction is much easier than construction.
    QByteArray eliminate(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // General
    QByteArray tabCount(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newTab(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray setTab(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    QByteArray screenBack(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray screenForward(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray countFromEnd(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray readToEnd(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray setPaused(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray unsetPaused(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Macros
    QByteArray newMacro(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray newMacro_(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray delMacro(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray endMacro(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // If
    QByteArray kstScriptIf(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    QByteArray kstScriptFi(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Commands
    QByteArray commands(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);

    // Hacks
    QByteArray editableVectorSetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //EditableVector::setBinaryArray(

    QByteArray editableMatrixSetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //EditableMatrix::setBinaryArray(

    QByteArray editableVectorSet(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //EditableVector::set(

    QByteArray vectorGetBinaryArray(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //Vector::getBinaryArray(

    QByteArray matrixGetBinaryArray(QByteArray& command, QLocalSocket*s,ObjectStore*_store,const int&ifMode,const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //Matrix::getBinaryArray(

    QByteArray stringValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //String::value(

    QByteArray stringSetValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //String::setValue(

    QByteArray scalarValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //Scalar::value(

    QByteArray scalarSetValue(QByteArray& command, QLocalSocket* s,ObjectStore*_store,const int&ifMode, const QByteArray&ifString,IfSI*& ifStat,VarSI*var);
    //Scalar::setValue(

};


}

#endif // SCRIPTSERVER_H
