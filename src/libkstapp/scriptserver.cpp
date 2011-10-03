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

#include "scriptserver.h"
#include "dialogscriptinterface.h"
#include "labelscriptinterface.h"
#include "stringscriptinterface.h"
#include "viewitemscriptinterface.h"

#include "sessionmodel.h"
#include "datagui.h"

//viewitems
#include "arrowitem.h"
#include "boxitem.h"
#include "buttonitem.h"
#include "lineedititem.h"
#include "circleitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "layoutboxitem.h"
#include "legenditem.h"
#include "lineitem.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "sharedaxisboxitem.h"
#include "svgitem.h"
#include "viewitemdialog.h"
#include "plotitemdialog.h"
#include "arrowitemdialog.h"
#include "labelitemdialog.h"
#include "legenditemdialog.h"
#include "document.h"

#include "curve.h"
#include "equation.h"
#include "vector.h"
#include "matrix.h"
#include "histogram.h"
#include "psd.h"
#include "eventmonitorentry.h"
#include "image.h"
#include "csd.h"
#include "basicplugin.h"
#include "dialog.h"
#include "editablematrix.h"

#include <updatemanager.h>

#include <QLocalSocket>
#include <iostream>
#include <QFile>

namespace Kst {

ScriptServer::ScriptServer(ObjectStore *obj) : _server(new QLocalServer(this)), _store(obj),_interface(0), _if(0),
    _curMac(0) {

    QString initial="kstScript";
    QStringList args= qApp->arguments();
    for(int i=0;i<args.size();i++) {
        if(args.at(i).startsWith("--serverName=")) {
            initial=args.at(i);
            initial.remove("--serverName=");
        }
    }

    QString connectTo=initial;
    while(1) {
        QLocalSocket socket;
        socket.connectToServer(connectTo);
        socket.waitForConnected(300);
        if(socket.state()!=QLocalSocket::ConnectedState) {
            _server->removeServer(connectTo);
            _server->listen(connectTo);
            break;
        }
        socket.disconnectFromServer();
        connectTo=initial+QString::number(connectTo.remove(initial).toInt()+1);
    }
    connect(_server,SIGNAL(newConnection()),this,SLOT(procConnection()));

    //setup _map={QByteArray->ScriptMemberFn}
    _fnMap.insert("getVectorList()",&ScriptServer::getVectorList);
    _fnMap.insert("newVector()",&ScriptServer::newVector);

    _fnMap.insert("getEditableVectorList()",&ScriptServer::getEditableVectorList);
    _fnMap.insert("newEditableVectorAndGetHandle()",&ScriptServer::newEditableVectorAndGetHandle);

    _fnMap.insert("getScalarList()",&ScriptServer::getScalarList);
    _fnMap.insert("newScalar()",&ScriptServer::newScalar);

    _fnMap.insert("getMatrixList()",&ScriptServer::getMatrixList);
    _fnMap.insert("newMatrix()",&ScriptServer::newMatrix);

    _fnMap.insert("getEditableMatrixList()",&ScriptServer::getEditableMatrixList);
    _fnMap.insert("newEditableMatrixAndGetHandle()",&ScriptServer::newEditableMatrixAndGetHandle);

    _fnMap.insert("getStringList()",&ScriptServer::getStringList);
    _fnMap.insert("newString()",&ScriptServer::newString);
    _fnMap.insert("newStringGen()",&ScriptServer::newStringGen);

    _fnMap.insert("getCurveList()",&ScriptServer::getCurveList);
    _fnMap.insert("newCurve()",&ScriptServer::newCurve);

    _fnMap.insert("getEquationList()",&ScriptServer::getEquationList);
    _fnMap.insert("newEquation()",&ScriptServer::newEquation);

    _fnMap.insert("getHistogramList()",&ScriptServer::getHistogramList);
    _fnMap.insert("newHistogram()",&ScriptServer::newHistogram);

    _fnMap.insert("getPSDList()",&ScriptServer::getPSDList);
    _fnMap.insert("newPSD()",&ScriptServer::newPSD);

    _fnMap.insert("getImageList()",&ScriptServer::getImageList);
    _fnMap.insert("newImage()",&ScriptServer::newImage);

    _fnMap.insert("getCSDList()",&ScriptServer::getCSDList);
    _fnMap.insert("newCSD()",&ScriptServer::newCSD);

    _fnMap.insert("getBasicPluginList()",&ScriptServer::getBasicPluginList);
    _fnMap.insert("getBasicPluginTypeList()",&ScriptServer::getBasicPluginTypeList);
    _fnMap.insert("newBasicPlugin()",&ScriptServer::newBasicPlugin);

    _fnMap.insert("getArrowList()",&ScriptServer::getArrowList);
    _fnMap.insert("newArrow()",&ScriptServer::newArrow);

    _fnMap.insert("getBoxList()",&ScriptServer::getBoxList);
    _fnMap.insert("newBox()",&ScriptServer::newBox);

    _fnMap.insert("getButtonList()",&ScriptServer::getButtonList);
    _fnMap.insert("newButton()",&ScriptServer::newButton);

    _fnMap.insert("getLineEditList()",&ScriptServer::getLineEditList);
    _fnMap.insert("newLineEdit()",&ScriptServer::newLineEdit);

    _fnMap.insert("getCircleList()",&ScriptServer::getCircleList);
    _fnMap.insert("newCircle()",&ScriptServer::newCircle);

    _fnMap.insert("getEllipseList()",&ScriptServer::getEllipseList);
    _fnMap.insert("newEllipse()",&ScriptServer::newEllipse);

    _fnMap.insert("getLabelList()",&ScriptServer::getLabelList);
    _fnMap.insert("newLabel()",&ScriptServer::newLabel);

    _fnMap.insert("getLineList()",&ScriptServer::getLineList);
    _fnMap.insert("newLine()",&ScriptServer::newLine);

    _fnMap.insert("getPictureList()",&ScriptServer::getPictureList);
    _fnMap.insert("newPicture()",&ScriptServer::newPicture);

    _fnMap.insert("getPlotList()",&ScriptServer::getPlotList);
    _fnMap.insert("newPlot()",&ScriptServer::newPlot);
    _fnMap.insert("setPlotXRange()",&ScriptServer::setPlotXRange);
    _fnMap.insert("setPlotYRange()",&ScriptServer::setPlotYRange);
    _fnMap.insert("setPlotXAuto()",&ScriptServer::setPlotXAuto);
    _fnMap.insert("setPlotYAuto()",&ScriptServer::setPlotYAuto);
    _fnMap.insert("setPlotXAutoBorder()",&ScriptServer::setPlotXAutoBorder);
    _fnMap.insert("setPlotYAutoBorder()",&ScriptServer::setPlotYAutoBorder);
    _fnMap.insert("setPlotXNoSpike()",&ScriptServer::setPlotXNoSpike);
    _fnMap.insert("setPlotYNoSpike()",&ScriptServer::setPlotYNoSpike);
    _fnMap.insert("setPlotXAC()",&ScriptServer::setPlotXAC);
    _fnMap.insert("setPlotYAC()",&ScriptServer::setPlotYAC);
    _fnMap.insert("setPlotGlobalFont()",&ScriptServer::setPlotGlobalFont);

    _fnMap.insert("setPlotTopLabel()",&ScriptServer::setPlotTopLabel);
    _fnMap.insert("setPlotBottomLabel()",&ScriptServer::setPlotBottomLabel);
    _fnMap.insert("setPlotLeftLabel()",&ScriptServer::setPlotLeftLabel);
    _fnMap.insert("setPlotRightLabel()",&ScriptServer::setPlotRightLabel);
    _fnMap.insert("setPlotTopLabelAuto()",&ScriptServer::setPlotTopLabelAuto);
    _fnMap.insert("setPlotBottomLabelAuto()",&ScriptServer::setPlotBottomLabelAuto);
    _fnMap.insert("setPlotLeftLabelAuto()",&ScriptServer::setPlotLeftLabelAuto);
    _fnMap.insert("setPlotRightLabelAuto()",&ScriptServer::setPlotRightLabelAuto);

    _fnMap.insert("getSharedAxisBoxList()",&ScriptServer::getSharedAxisBoxList);
    _fnMap.insert("newSharedAxisBox()",&ScriptServer::newSharedAxisBox);

    _fnMap.insert("getSvgItemList()",&ScriptServer::getSvgItemList);
    _fnMap.insert("newSvgItem()",&ScriptServer::newSvgItem);

    _fnMap.insert("beginEdit()",&ScriptServer::beginEdit);
    _fnMap.insert("endEdit()",&ScriptServer::endEdit);

    _fnMap.insert("eliminate()",&ScriptServer::eliminate);

    _fnMap.insert("done()",&ScriptServer::done);
    _fnMap.insert("clear()",&ScriptServer::clear);

    _fnMap.insert("tabCount()",&ScriptServer::tabCount);
    _fnMap.insert("newTab()",&ScriptServer::newTab);
    _fnMap.insert("setTab()",&ScriptServer::setTab);
    _fnMap.insert("screenBack()",&ScriptServer::screenBack);
    _fnMap.insert("screenForward()",&ScriptServer::screenForward);
    _fnMap.insert("countFromEnd()",&ScriptServer::countFromEnd);
    _fnMap.insert("readToEnd()",&ScriptServer::readToEnd);
    _fnMap.insert("setPaused()",&ScriptServer::setPaused);
    _fnMap.insert("unsetPaused()",&ScriptServer::unsetPaused);

    _fnMap.insert("newMacro()",&ScriptServer::newMacro);
    _fnMap.insert("newMacro_()",&ScriptServer::newMacro_);
    _fnMap.insert("delMacro()",&ScriptServer::delMacro);
    _fnMap.insert("endMacro()",&ScriptServer::endMacro);

    _fnMap.insert("if()",&ScriptServer::kstScriptIf);
    _fnMap.insert("fi()",&ScriptServer::kstScriptFi);

    _fnMap.insert("EditableVector::setBinaryArray()",&ScriptServer::editableVectorSetBinaryArray);
    _fnMap.insert("EditableMatrix::setBinaryArray()",&ScriptServer::editableMatrixSetBinaryArray);
    _fnMap.insert("EditableVector::set()",&ScriptServer::editableVectorSet);
    _fnMap.insert("Vector::getBinaryArray()",&ScriptServer::vectorGetBinaryArray);
    _fnMap.insert("Matrix::getBinaryArray()",&ScriptServer::matrixGetBinaryArray);
    _fnMap.insert("String::value()",&ScriptServer::stringValue);
    _fnMap.insert("String::setValue()",&ScriptServer::stringSetValue);
    _fnMap.insert("Scalar::value()",&ScriptServer::scalarValue);
    _fnMap.insert("Scalar::setValue()",&ScriptServer::scalarSetValue);

    _fnMap.insert("commands()",&ScriptServer::commands);

    QFile scriptMacros(":/script/kstScript.txt");
    if(!scriptMacros.open(QFile::ReadOnly|QIODevice::Text)) {
        qDebug()<<"Could not open script macros.";
    }
    QList<QByteArray> x = scriptMacros.read(3000000).split('\n');
    for(int i=0;i<x.size();i++) {
        if(!x[i].contains("command")) {
            exec(x[i],0);
        }
    }
}

ScriptServer::~ScriptServer()
{
    delete _server;
    delete _if;
    delete _curMac;
    delete _interface;

    while(_macroMap.size()) {
        delete _macroMap.take(_macroMap.keys().first());
    }

    while(_varMap.size()) {
        delete _varMap.take(_varMap.keys().first());
    }
}

/** Conv. function which takes a response, and executes if 'if' statement is unexistant or true. */
QByteArray handleResponse(const QByteArray& response, QLocalSocket* s, const int& ifMode, const QByteArray& ifString,
                          IfSI*& ifStat,VarSI* var)
{
    if(ifMode) {
        bool isTrue=0;
        switch(ifMode) {
        case 1:
            isTrue=response==ifString;
            break;
        case 2:
            isTrue=response!=ifString;
            break;
        case 3:
            isTrue=response.contains(ifString);
            break;
        case 4:
            isTrue=!response.contains(ifString);
            break;
        case 5:
            Q_ASSERT(var);
            var->val=response;
            break;
        default:
            qFatal("Shouldn't be able to get here.");
        }

        if(ifMode<5) {  //ifMode==5 is a hack (== check value of variable)
            if(!ifStat||ifStat->on) {
                ifStat=new IfSI(ifStat,isTrue);
            } else if(!ifStat->on) {
                ++ifStat->recurse;
            }
        }
    }

    if(s) {
        if(response.isEmpty()) {
            s->write(" ");
        } else {
            s->write(response);
        }
        s->waitForBytesWritten();
    }

    return response.isEmpty()?" ":response;
}

/** Provides alternative syntax of ifStat. */
QByteArray handleResponse(const QByteArray& response, QLocalSocket* s, const int& ifMode, const QByteArray& ifString,
                          IfSI** ifStat,VarSI* var)
{
    return handleResponse(response,s,ifMode,ifString,*ifStat,var);
}

/** Runs a macro */
QByteArray ScriptServer::procMacro(QByteArray&command,QLocalSocket*s)
{
    //macro
    QByteArray strx=command;
    strx.remove(0,1);   //'#'
    strx.remove(strx.indexOf("("),99999);
    MacroSI* msi=_macroMap.value(strx,0);
    if(!msi) {
        return handleResponse("No such macro.",s,0,"",_if,0);
    }
    command.remove(0,command.indexOf("(")+1);
    command.remove(command.lastIndexOf(")"),9999999);
    QByteArrayList vars=command.split(',');
    while(command.isEmpty()&&vars.size()) {
        vars.removeFirst();
    }
    if(vars.size()!=msi->args.size()) {
        return handleResponse("Invalid arg count.",s,0,"",_if,0);
    }
    for(int i=0;i<msi->args.size();i++) {
        VarSI*x=_varMap.value(msi->args[i],0);
        if(!x) {
            x=new VarSI(msi->args[i],"");
            _varMap.insert(msi->args[i],x);
        }
        x->val=vars.at(i);
    }
    QByteArray ret;
    for(int j=0;j<msi->commands.size();j++) {
        if(msi->commands[j].startsWith("command")) {
            continue;
        }
        ret=exec(msi->commands[j],0);
    }
    return handleResponse(ret,s,0,"",_if,0);
}

/** @sa outputViewItemList() */
template<class T> QByteArray outputObjectList(
    QLocalSocket* s,ObjectStore*_store,const int&ifMode,const QByteArray&ifString,IfSI*& ifStat,VarSI*var) {

    ObjectList<T> vl=_store->getObjects<T>();
    QByteArray a;
    typename ObjectList<T>::ConstIterator it = vl.begin();
    for(; it != vl.end(); ++it) {
        SharedPtr<T> v = (*it);
        v->readLock();
        a+="["%v->Name()%"]";
        v->unlock();
    }
    if(a.size()) {
        return handleResponse(a,s,ifMode,ifString,ifStat,var);
    } else {
        return handleResponse("NO_OBJECTS",s,ifMode,ifString,ifStat,var);
    }
}

/** @sa outputObjectList() */
template<class T> QByteArray outputViewItemList(QLocalSocket* s,const int&ifMode,const QByteArray&ifString,IfSI*& ifStat,VarSI*var) {
    QList<T *> vl=ViewItem::getItems<T>();
    QByteArray a;
    typename QList<T*>::iterator it = vl.begin();
    for(; it != vl.end(); ++it) {
        T* v = (*it);
        a+="["%v->Name()%"]";
    }
    if(a.size()) {
        return handleResponse(a,s,ifMode,ifString,ifStat,var);
    } else {
        return handleResponse("NO_OBJECTS",s,ifMode,ifString,ifStat,var);
    }
}

/** Connects pending connections to readSomething. */
void ScriptServer::procConnection() {
    while(_server->hasPendingConnections()) {
        QLocalSocket* s=_server->nextPendingConnection();
        connect(s,SIGNAL(readyRead()),this,SLOT(readSomething()));
    }
}

/** Processes a socket with data. */
void ScriptServer::readSomething()
{
    QLocalSocket* s=qobject_cast<QLocalSocket*>(sender());
    Q_ASSERT(s);
    QByteArray command=s->read(1000000);
    if(command.startsWith("attachTo(")) {
        QString search=command.remove(0,9).remove(command.lastIndexOf(")"),9999);
        for(int h=0;h<2;h++) {
            for(int i=0;i<vi.size();i++) {
                if(search.contains(vi[i]->shortName().toAscii())) {
                    ButtonItem* bi=dynamic_cast<ButtonItem*>(vi[i]);
                    if(bi) {
                        bi->addSocket(s);
                        disconnect(s,SIGNAL(readyRead()),this,SLOT(readSomething()));
                        return;
                    }
                    LineEditItem* li=dynamic_cast<LineEditItem*>(vi[i]);
                    if(li) {
                        li->addSocket(s);
                        disconnect(s,SIGNAL(readyRead()),this,SLOT(readSomething()));
                        return;
                    }
                }
            }
            vi=ViewItem::getItems<ViewItem>();
        }
        return;
    }
    exec(command,s);
}

/** {"Bob", "Fred} -> "Bob"+r+"Fred" */
inline QByteArray join(const QByteArrayList&n,const char&r) {
    QByteArray ret;
    for(int i=0;i<n.size();i++) {
        ret+=n[i]%r;
    }
    return ret;
}

/** The heart of the script server. This function is what performs all the actions. s may be null. */
QByteArray ScriptServer::exec(QByteArray command, QLocalSocket *s,int ifMode,QByteArray ifEqual)
{
    if(command.isEmpty()) {
        return handleResponse("",s,0,"",_if,0);
    }

    // MACROS
    if(command.startsWith("#")) {
        return procMacro(command,s);
    }

    if(_curMac&&!_curMacComEcho&&!command.startsWith("endMacro")) {
        _curMac->commands.push_back(command);
        return "recorded.";
    }

    // False ifs.
    if(_if&&!_if->on)
    {
        if(!command.contains("if")&&!command.contains("fi")) {
            return handleResponse("if is false, ignoring.",s,0,"",0,0);
        }
    }

    // Variables
    VarSI* var=0;
    if(command.contains('$')&&!command.contains("if(")&&!command.contains("fi()")) {
        QByteArray varx=command;
        varx.remove(0,varx.indexOf("$")+1);
        QByteArray endc=" )=";
        int lasti=9999999;
        int lastidx=0;
        for(int i=0;i<endc.size();i++) {
            if(varx.indexOf(endc[i])<lasti&&varx.indexOf(endc[i])!=-1) {
                lasti=varx.indexOf(endc[i]);
                lastidx=varx.indexOf(endc[i]);
            }
        }
        if(lasti!=9999999) {
            varx.remove(lastidx,9999);
        }
        var=_varMap.value(varx,0);
        if(!var) {
            var=new VarSI(varx,"");
            _varMap.insert(varx,var);
        }

        QByteArray cx=command;
        cx.replace("$"%varx,"");

        if(command.contains("=\"")&&!ifMode) {
            command.remove(0,command.indexOf("=\"")+2);
            if(!command.contains("\"")) {
                return handleResponse("Invalid assignment",s,0,"",_if,0);
            }
            command.remove(command.lastIndexOf("\""),9999);
            var->val=command;
            return handleResponse("Assigned",s,0,"",_if,0);
        } else if(command.contains("=")&&!ifMode) {
            command.remove(0,command.indexOf("=")+1);
            ifMode=5;
        } else {
            command.replace("$"%varx,var->val);
        }

        if(cx.isEmpty()) {
            return handleResponse(var->val,s,ifMode,ifEqual,_if,var);
        }
    }

    // Map
    QByteArray ycommand(command);
    ycommand.remove(ycommand.indexOf("("),9999999);
    ycommand+="()";
    ScriptMemberFn fn=_fnMap.value(ycommand,&ScriptServer::noSuchFn);
    if(fn!=&ScriptServer::noSuchFn) {
        return CALL_MEMBER_FN(*this,fn)(command, s,_store,ifMode,ifEqual,_if,var);
    } else {
        if(command.contains("::")) {
            QByteArray ret=checkPrimatives(command,s);
            if(ret!="") {
                return ret;
            }
        }

        if(_interface) {
            return handleResponse(_interface->doCommand(command).toAscii(),s,ifMode,ifEqual,_if,var); //magic
        } else {
            return handleResponse("Unknown command!",s,ifMode,ifEqual,_if,var);
        }
    }
    return "?";
}

QByteArray ScriptServer::checkPrimatives(QByteArray &command, QLocalSocket *s)
{
    ///
    if(command.startsWith("DataVector::")) {
        command.replace("DataVector::","");
        QByteArray actc=command;
        command.remove(command.indexOf("("),999999);
        actc.remove(0,actc.indexOf("("));
        actc.remove(actc.lastIndexOf(")"),909099);
        QByteArrayList m;
        m.push_back(command);
        m<<actc.split(',');
        if(m.size()<2) {
            return handleResponse("Invalid call to vector",s,0,"",0,0);
        } else {
            QByteArray b=m.takeAt(1);
            ObjectPtr o=_store->retrieveObject(b);
            DataVectorPtr v=kst_cast<DataVector>(o);
            if(v) {
                return handleResponse(v->scriptInterface(m),s,0,"",0,0);
            } else {
                return handleResponse("No such object",s,0,"",0,0);
            }
        }
    } else if(command.startsWith("Vector::")) {
        command.replace("Vector::","");
        QByteArray actc=command;
        command.remove(command.indexOf("("),999999);
        actc.remove(0,actc.indexOf("("));
        actc.remove(actc.lastIndexOf(")"),909099);
        QByteArrayList m;
        m.push_back(command);
        m<<actc.split(',');
        if(m.size()<2) {
            return handleResponse("Invalid call to vector",s,0,"",0,0);
        } else {
            QByteArray b=m.takeAt(1);
            ObjectPtr o=_store->retrieveObject(b);
            VectorPtr v=kst_cast<Vector>(o);
            if(v) {
                return handleResponse(v->scriptInterface(m),s,0,"",0,0);
            } else {
                return handleResponse("No such object",s,0,"",0,0);
            }
        }
    }

    return "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QByteArray ScriptServer::getVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Vector>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newVector(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        QByteArray vn; _interface = DialogLauncherSI::self->showVectorDialog(vn);
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getEditableVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                               const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<EditableVector>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newEditableVectorAndGetHandle(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    EditableVectorPtr objectPtr=_store->createObject<EditableVector>();
    objectPtr->writeLock();
    objectPtr->setDescriptiveName("Script Vector");
    objectPtr->unlock();
    UpdateManager::self()->doUpdates(1);
    return handleResponse("Finished editing "+objectPtr->Name().toAscii(),s,ifMode,ifEqual,_if,var);
}



QByteArray ScriptServer::getScalarList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Scalar>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newScalar(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        QByteArray vn; _interface = DialogLauncherSI::self->showScalarDialog(vn);
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getMatrixList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Matrix>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newMatrix(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        QByteArray vn; _interface = DialogLauncherSI::self->showMatrixDialog(vn);
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getEditableMatrixList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                               const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<EditableMatrix>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newEditableMatrixAndGetHandle(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    EditableMatrixPtr objectPtr=_store->createObject<EditableMatrix>();
    objectPtr->writeLock();
    objectPtr->setDescriptiveName("Script Matrix");
    objectPtr->unlock();
    UpdateManager::self()->doUpdates(1);
    return handleResponse("Finished editing "+objectPtr->Name().toAscii(),s,ifMode,ifEqual,_if,var);
}



QByteArray ScriptServer::getStringList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<String>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newString(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        QByteArray vn; _interface = DialogLauncherSI::self->showStringDialog(vn);
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}

QByteArray ScriptServer::newStringGen(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        QByteArray vn; _interface = DialogLauncherSI::self->showStringGenDialog(vn,0,_store);
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getCurveList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Curve>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newCurve(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        _interface = DialogLauncherSI::self->showCurveDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getEquationList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                         const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Equation>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newEquation(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                     const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        _interface = DialogLauncherSI::self->showEquationDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getHistogramList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                          const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Histogram>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newHistogram(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        _interface = DialogLauncherSI::self->showHistogramDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getPSDList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<PSD>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newPSD(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        _interface = DialogLauncherSI::self->showPowerSpectrumDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getImageList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<Image>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newImage(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else {
        _interface = DialogLauncherSI::self->showImageDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getCSDList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<CSD>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newCSD(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var);}
    else {
        _interface = DialogLauncherSI::self->showCSDDialog();
        return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
    }
}



QByteArray ScriptServer::getBasicPluginList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                            const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputObjectList<BasicPlugin>(s,_store,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newBasicPlugin(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                        const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    command.replace("newBasicPlugin(","");
    if(command.contains(")")) { command.remove(command.indexOf(")"),99999999); }

    bool ok=0;
    for(int i=0;i<DataObject::dataObjectPluginList().size();i++) {
        if(command==DataObject::dataObjectPluginList()[i]) {
            ok=1;
            break;
        }
    }
    if(!ok) { return handleResponse("No such plugin",s,ifMode,ifEqual,_if,var); }
    else if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->showBasicPluginDialog(command); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getBasicPluginTypeList(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    QString a;
    for(int i=0;i<DataObject::dataObjectPluginList().size();i++) {
        a.push_back(DataObject::dataObjectPluginList()[i].toAscii()%"\n");
    }
    return handleResponse(a.toAscii(),s,ifMode,ifEqual,_if,var);
}



QByteArray ScriptServer::getArrowList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<ArrowItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newArrow(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newArrow(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getBoxList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<BoxItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newBox(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newBox(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getButtonList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<ButtonItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newButton(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newButton(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getLineEditList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                         const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<LineEditItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newLineEdit(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                     const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newLineEdit(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getCircleList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<CircleItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newCircle(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newCircle(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getEllipseList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                        const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<EllipseItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newEllipse(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newEllipse(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getLabelList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<LabelItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newLabel(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newLabel(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getLineList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                     const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<LineItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newLine(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                 const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newLine(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getPictureList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                        const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {

    return outputViewItemList<PictureItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newPicture(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    command.replace("newPicture(","");
    command.remove(command.lastIndexOf(")"),1);
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newPicture(command); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}


/***********************************/
/* Plot related scripting commands */
/***********************************/

QByteArray ScriptServer::getPlotList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                     const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<PlotItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newPlot(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                 const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newPlot(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}

QByteArray ScriptServer::setPlotXRange(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    double xmin = vars.at(1).toDouble();
    double xmax = vars.at(2).toDouble();
    if (xmin == xmax) {
      xmin -= 0.1;
      xmax += 0.1;
    }
    if (xmin > xmax) {
      double tmp = xmin;
      xmin = xmax;
      xmax = tmp;
    }

    double ymin = plot->yMin();
    double ymax = plot->yMax();

    QRectF R(QPointF(xmin,ymin), QPointF(xmax,ymax));

    plot->zoomXRange(R);

  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotYRange(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    double ymin = vars.at(1).toDouble();
    double ymax = vars.at(2).toDouble();
    if (ymin == ymax) {
      ymin -= 0.1;
      ymax += 0.1;
    }
    if (ymin > ymax) {
      double tmp = ymin;
      ymin = ymax;
      ymax = tmp;
    }

    double xmin = plot->xMin();
    double xmax = plot->xMax();

    QRectF R(QPointF(xmin,ymin), QPointF(xmax,ymax));

    plot->zoomYRange(R);

  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}

QByteArray ScriptServer::setPlotXAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomXMaximum();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotYAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomYMaximum();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotXAutoBorder(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomXAutoBorder();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotYAutoBorder(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomYAutoBorder();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotXNoSpike(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomXNoSpike();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotYNoSpike(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->zoomYNoSpike();
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotXAC(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    double R = fabs(vars.at(1).toDouble());
    if (R==0) {
      R = 0.2;
    }
    plot->zoomXMeanCentered(R);
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotYAC(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    double R = fabs(vars.at(1).toDouble());
    if (R==0) {
      R = 0.2;
    }
    plot->zoomYMeanCentered(R);
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotGlobalFont(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    QFont font = plot->globalFont();
    QString family = vars.at(1);
    bool bold = ((vars.at(2)=="bold") || (vars.at(2)=="true"));
    bool italic = ((vars.at(3)=="italic") || (vars.at(3)=="true"));
    if (!family.isEmpty()) {
      font.setFamily(family);
    }
    font.setItalic(italic);
    font.setBold(bold);

    plot->setGlobalFont(font);
  }
  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotTopLabel(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->topLabelDetails()->setText(vars.at(1));
    plot->topLabelDetails()->setIsAuto(false);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotBottomLabel(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->bottomLabelDetails()->setText(vars.at(1));
    plot->bottomLabelDetails()->setIsAuto(false);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotLeftLabel(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->leftLabelDetails()->setText(vars.at(1));
    plot->leftLabelDetails()->setIsAuto(false);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotRightLabel(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->rightLabelDetails()->setText(vars.at(1));
    plot->rightLabelDetails()->setIsAuto(false);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}

QByteArray ScriptServer::setPlotTopLabelAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->topLabelDetails()->setIsAuto(true);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotBottomLabelAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->bottomLabelDetails()->setIsAuto(true);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotLeftLabelAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->leftLabelDetails()->setIsAuto(true);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}


QByteArray ScriptServer::setPlotRightLabelAuto(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &ifMode,
                                       const QByteArray &ifString, IfSI *&ifStat, VarSI *var) {
  command.remove(0,command.indexOf('(')+1);
  command.remove(command.lastIndexOf(")"),1);
  QByteArrayList vars=command.split(',');

  PlotItem *plot = ViewItem::retrieveItem<PlotItem>(vars.at(0));

  if (plot) {
    plot->rightLabelDetails()->setIsAuto(true);
  }

  return handleResponse("Done",s,ifMode,ifString,_if,var);
}



/***********************************/

QByteArray ScriptServer::getSharedAxisBoxList(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                              const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<SharedAxisBoxItem>(s,ifMode,ifEqual,_if,var);
}
QByteArray ScriptServer::newSharedAxisBox(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                          const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newSharedAxisBox(); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }
}



QByteArray ScriptServer::getSvgItemList(QByteArray&, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                        const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return outputViewItemList<SvgItem>(s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newSvgItem(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    command.replace("newSvgItem(","");
    command.remove(command.lastIndexOf(")"),1);
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var); }
    else { _interface = DialogLauncherSI::self->newSvgItem(command); return handleResponse("Ok",s,ifMode,ifEqual,_if,var); }

}

QByteArray ScriptServer::beginEdit(QByteArray&command, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) {
        return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var);
    }
    command.replace("beginEdit(","");
    // check if object
    command.remove(command.lastIndexOf(")"),999999);
    ObjectPtr o=_store->retrieveObject(command);
    if(!o) {
        // check if view item
        for(int h=0;h<2;h++) {
            for(int i=0;i<vi.size();i++) {
                if(command.contains(vi[i]->shortName().toAscii())) {
                    _interface=DialogLauncherSI::self->showViewItemDialog(vi[i]);
                    return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
                }
            }
            vi=ViewItem::getItems<ViewItem>();
        }

        return handleResponse("No such object",s,ifMode,ifEqual,_if,var);
    } else {
        if((_interface=DialogLauncherSI::self->showObjectDialog(o))) {
            return handleResponse("Ok",s,ifMode,ifEqual,_if,var);
        } else {
            return handleResponse("Unknown error",s,ifMode,ifEqual,_if,var);
        }
    }
}

QByteArray ScriptServer::eliminate(QByteArray&command, QLocalSocket* s,ObjectStore*_store,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(_interface) {
        return handleResponse("To access this function, first call endEdit()",s,ifMode,ifEqual,_if,var);
    }
    command.replace("beginEdit(","");
    // check if object
    command.remove(command.lastIndexOf(")"),999999);
    ObjectPtr o=_store->retrieveObject(command);
    if(!o) {
        // check if view item
        for(int h=0;h<2;h++) {
            for(int i=0;i<vi.size();i++) {
                if(command.contains(vi[i]->shortName().toAscii())) {
                    vi[i]->hide();  // goodbye, memory.
                    return handleResponse("It died a peaceful death.",s,ifMode,ifEqual,_if,var);
                }
            }
            vi=ViewItem::getItems<ViewItem>();
        }

        return handleResponse("No such object (or it's hiding somewhere in the dark corners of Kst)",s,ifMode,ifEqual,_if,var);
    } else {
        if (RelationPtr relation = kst_cast<Relation>(o)) {
            Data::self()->removeCurveFromPlots(relation);
        }
        _store->removeObject(o);
        kstApp->mainWindow()->document()->session()->triggerReset();
        return handleResponse("It died a peaceful death.",s,ifMode,ifEqual,_if,var);
    }
}

QByteArray ScriptServer::endEdit(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                 const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(!_interface) {
        return handleResponse("No dialog open.",s,ifMode,ifEqual,_if,var);
    }

    if(!_interface->isValid()) {
        delete _interface;
        _interface=0;
        return handleResponse("The interface isn't valid (i.e., no dialog opne).",s,ifMode,ifEqual,_if,var);
    }

    QByteArray x=_interface->getHandle();
    UpdateManager::self()->doUpdates(1);
    delete _interface;
    _interface=0;
    return handleResponse(x,s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::done(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                              const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(!s) {
        return "Invalid... no socket...";
    }
    s->write("Bye.");
    s->flush();
    s->close();
    delete s;
    return "Bye.";
}


QByteArray ScriptServer::clear(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                              const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
  kstApp->mainWindow()->newDoc(true);
  return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}


QByteArray ScriptServer::tabCount(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    return handleResponse(QByteArray::number(kstApp->mainWindow()->tabWidget()->count()),s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::newTab(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->tabWidget()->createView();
    kstApp->mainWindow()->tabWidget()->setCurrentIndex(kstApp->mainWindow()->tabWidget()->count()-1);
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}
QByteArray ScriptServer::setTab(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->tabWidget()->setCurrentIndex(command.replace("setTab(","").replace(")","").toInt());
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::screenBack(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                    const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->_backAct->trigger();
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}
QByteArray ScriptServer::screenForward(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                       const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->_forwardAct->trigger();
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::countFromEnd(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                      const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->_readFromEndAct->trigger();
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}
QByteArray ScriptServer::readToEnd(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    kstApp->mainWindow()->_readToEndAct->trigger();
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}

QByteArray ScriptServer::setPaused(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                   const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(!kstApp->mainWindow()->_pauseAct->isChecked()) {
        kstApp->mainWindow()->_pauseAct->trigger();
    }
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);
}
QByteArray ScriptServer::unsetPaused(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                     const QByteArray&ifEqual,IfSI*& _if,VarSI*var) {
    if(kstApp->mainWindow()->_pauseAct->isChecked()) {
        kstApp->mainWindow()->_pauseAct->trigger();
    }
    return handleResponse("Done",s,ifMode,ifEqual,_if,var);

}

QByteArray ScriptServer::newMacro(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                  const QByteArray&,IfSI*& _if,VarSI*) {
    if(_curMac) {
        return handleResponse("Call endMacro() first",s,0,"",_if,0);
    }
    command.replace("newMacro(","").replace(')',"");
    if(!command.size()) {
        return handleResponse("Invalid call to newMacro(...)",s,0,"",_if,0);
    }
    QByteArrayList x=command.split(',');
    if(!x.size()) {
        return handleResponse("Invalid call to newMacro(...)",s,0,"",_if,0);
    }
    QByteArray c=x[0];
    x.removeFirst();
    for(int i=0;i<x.size();i++) {
        if(!x.at(i).size()) {
            x.takeAt(i--);
        }
    }
    QByteArrayList z;
    for(int i=0;i<x.size();i++) {
        z.push_back(x.at(i));
        if(z.back().contains("=")) {
            z.back().remove(z.back().indexOf("="),99999);
            VarSI*varx=_varMap.value(z.back(),0);
            if(!varx) {
                _varMap.insert(z.back(),varx=new VarSI(z.back(),""));
            }
            varx->val=x[i].remove(0,z.back().size()+1);
        }

    }
    _curMac=new MacroSI(0,c,z,QByteArrayList());
    _curMacComEcho=1;
    return handleResponse("Editing macro.",s,0,"",_if,0);
}

QByteArray ScriptServer::newMacro_(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                   const QByteArray&,IfSI*& _if,VarSI*) {
    if(_curMac) {
        return handleResponse("Call endMacro() first",s,0,"",_if,0);
    }
    command.replace("newMacro_(","").replace(')',"");
    if(!command.size()) {
        return handleResponse("Invalid call to newMacro_(...)",s,0,"",_if,0);
    }
    QByteArrayList x=command.split(',');
    if(!x.size()) {
        return handleResponse("Invalid call to newMacro_(...)",s,0,"",_if,0);
    }
    QByteArray c=x[0];
    x.removeFirst();
    for(int i=0;i<x.size();i++) {
        if(!x.at(i).size()) {
            x.takeAt(i--);
        }
    }
    _curMac=new MacroSI(0,c,x,QByteArrayList());
    _curMacComEcho=0;
    return handleResponse("Editing macro.",s,0,"",_if,0);
}

QByteArray ScriptServer::delMacro(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                  const QByteArray&,IfSI*& _if,VarSI*) {
    if(_curMac) {
        return handleResponse("Call endMacro() first",s,0,"",_if,0);
    }
    command.replace("delMacro(","").replace(')',"");
    for(int i=0;i<_macroMap.size();i++) {
        if(_macroMap.values()[i]->handle==command) {
            delete _macroMap.take(_macroMap.keys()[i]);
            QByteArray str;
            for(int i=0;i<_macroMap.size();i++) {
                str+="newMacro_("+_macroMap.values()[i]->handle;

                for(int j=0;j<_macroMap.values()[i]->args.size();j++) {
                    str+=","%_macroMap.values()[i]->args.at(j);
                }

                str+=")\n";

                for(int j=0;j<_macroMap.values()[i]->commands.size();j++) {
                    str+=_macroMap.values()[i]->commands.at(j)%"\n";
                }

                str+="endMacro()\n";
            }
            return handleResponse("Removed!",s,0,"",_if,0);
        }
    }
    return handleResponse("No such macro",s,0,"",_if,0);
}
QByteArray ScriptServer::endMacro(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                  const QByteArray&,IfSI*& _if,VarSI*) {
    if(_curMac) {
        _macroMap.insert(_curMac->handle,_curMac);

        QByteArray str;
        if(_curMacComEcho) {
            std::cout<<"######### New Macro #########\n";
            str+="newMacro_("+_macroMap.values().back()->handle;

            for(int j=0;j<_macroMap.values().back()->args.size();j++) {
                str+=","%_macroMap.values().back()->args[j];
            }

            str+=")\n";

            for(int j=0;j<_macroMap.values().back()->commands.size();j++) {
                str+=_macroMap.values().back()->commands[j]%"\n";
            }

            str+="endMacro()\n";
            std::cout<<str.data();
        }
        _curMac=0;
        return handleResponse("Ok",s,0,"",_if,0);
    }
    return "No return. (well... except for this)";
}

QByteArray ScriptServer::kstScriptIf(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                     const QByteArray&,IfSI*&,VarSI*) {
    int ifMode=0;
    QByteArray ifEqual="";
    command=command.remove(0,3);
    command.remove(command.lastIndexOf(")"),9999);
    if(command.contains("==")) {
        ifMode=1;
        ifEqual=command;
        command.remove(command.indexOf("=="),99999);
        ifEqual.remove(0,ifEqual.indexOf("==")+2);
    } else if(command.contains("!=")) {
        ifMode=2;
        ifEqual=command;
        command.remove(command.indexOf("!="),99999);
        ifEqual.remove(0,ifEqual.indexOf("!=")+2);
    } else if(command.contains(" CONTAINS ")) {
        ifMode=3;
        ifEqual=command;
        command.remove(command.indexOf(" CONTAINS "),99999);
        ifEqual.remove(0,ifEqual.indexOf(" CONTAINS ")+10);
    } else if(command.contains(" !CONTAINS ")) {
        ifMode=4;
        ifEqual=command;
        command.remove(command.indexOf(" !CONTAINS "),99999);
        ifEqual.remove(0,ifEqual.indexOf(" !CONTAINS ")+11);
    } else {
        ifMode=1;
        ifEqual="true";
    }
    return exec(command,s,ifMode,ifEqual);
}

QByteArray ScriptServer::kstScriptFi(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                     const QByteArray&,IfSI*&,VarSI*) {
    if(_if&&!_if->on) {
        if(--_if->recurse<0) {
            IfSI* ifx=_if->parent;
            delete _if;
            _if=ifx;
            return handleResponse("Scope changed.",s,0,"",_if,0);
        } else if(command.startsWith("if(")) {
            ++_if->recurse;
            return handleResponse("Scope changed.",s,0,"",_if,0);
        } else if(command.contains("command")){
            return handleResponse("fi()",s,0,"",_if,0);
        }
        return " ";
    } else if(_if) {
        if(--_if->recurse<0) {
            IfSI* ifx=_if->parent;
            delete _if;
            _if=ifx;
        }
        return handleResponse("Scope changed.",s,0,"",_if,0);
    } else {
        return "?";
    }
}

QByteArray ScriptServer::commands(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,
                                  const QByteArray&ifEqual,IfSI*&_if,VarSI*var) {
    if(!_interface) {
        QByteArrayList v;
        for(int i=0;i<_macroMap.values().size();i++) {
            v.push_back("#"+_macroMap.values()[i]->handle+((_macroMap.values()[i]->args.size())?QByteArray("("):QByteArray("()")));
            for(int j=0;j<_macroMap.values()[i]->args.size();j++) {
                if(j) v.back()+=",";
                v.back()+=_macroMap.values()[i]->args[j];
            }
            v.back()+=")";
        }

        QByteArray builtIns;
        for(int i=0;i<_fnMap.keys().size();i++) {
            builtIns+="\n"+_fnMap.keys()[i];
        }
        return handleResponse((join(v,'\n')+builtIns+"\n"),s,ifMode,ifEqual,_if,var);
    } else {
        QString a="endEdit()\n"%join(_interface->commands(),'\n');
        return handleResponse(a.toAscii(),s,ifMode,ifEqual,_if,var);
    }
}

QByteArray ScriptServer::editableVectorSetBinaryArray(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                                      const QByteArray&,IfSI*&,VarSI*) {
    command.replace("EditableVector::setBinaryArray(","");
    command.chop(1);
    s->write("Handshake");
    ObjectPtr o=_store->retrieveObject(command);
    EditableVectorPtr v=kst_cast<EditableVector>(o);
    if(!v) {
        s->write("No such object.");
        s->flush();
        return "No such object.";
    }
    s->flush();

    QDataStream ds(s);
    s->waitForReadyRead(200000);
    QByteArray copy;
    QDataStream out(&copy,QIODevice::WriteOnly);
    qint64 count;
    ds>>count;
    out<<count;
    double x;
    int bl=0;
    for(int i=0;i<count;i++) {
        while(!bl--) {
            s->waitForReadyRead(-1);
            bl=s->bytesAvailable()/sizeof(double);
        }
        ds>>x;
        out<<x;
    }

    v->writeLock();
    v->change(copy);
    v->unlock();
    s->write("Done.");
    s->waitForBytesWritten(-1);
    return "Done.";
}

QByteArray ScriptServer::editableMatrixSetBinaryArray(QByteArray &command, QLocalSocket *s, ObjectStore *_store, const int &, const QByteArray &, IfSI *&, VarSI *)
{
    command.replace("EditableMatrix::setBinaryArray(","");
    command.chop(1);
    QByteArrayList params=command.split(',');
    if(params.count()!=7) {
        s->write("Invalid param count. Need 7.");
        s->waitForBytesWritten(-1);
        return "Invalid param count. Need 7.";
    }
    ObjectPtr o=_store->retrieveObject(params[0]);
    EditableMatrixPtr v=kst_cast<EditableMatrix>(o);
    if(!v) {
        s->write("No such object.");
        s->waitForBytesWritten(-1);
        return "No such object.";
    }
    s->write("Handshake");
    s->waitForBytesWritten(-1);

    QDataStream ds(s);
    s->waitForReadyRead(300);
    QByteArray copy;
    QDataStream out(&copy,QIODevice::WriteOnly);
    qint64 _nX=params.at(1).toInt();
    qint64 _nY=params.at(2).toInt();
    double x;
    int bl=0;
    for(int i=0;i<_nX*_nY;i++) {
        while(!bl--) {
            s->waitForReadyRead(-1);
            bl=s->bytesAvailable()/sizeof(double);
        }
        ds>>x;
        out<<x;
    }

    v->writeLock();
    v->change(copy,_nX,_nY,params[3].toDouble(),params[4].toDouble(),params[5].toDouble(),params[6].toDouble());
    v->unlock();
    s->write("Done.");
    s->waitForBytesWritten(-1);
    return "Done.";
}

QByteArray ScriptServer::editableVectorSet(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                           const QByteArray&,IfSI*&,VarSI*) {
    command.remove(0,20);   //editableVectorSet(
    command.chop(1);
    QByteArrayList b=command.split(',');
    if(b.size()<3) {
        s->write("Invalid parameter count.");
        s->waitForBytesWritten(-1);
        return "Invalid parameter count.";
    }
    ObjectPtr o=_store->retrieveObject(b[0]);
    EditableVectorPtr v=kst_cast<EditableVector>(o);
    if(!v) {
        s->write("No such object.");
        s->waitForBytesWritten(-1);
        return "No such object.";
    }
    v->setValue(b[1].toInt(),b[2].toDouble());
    s->write("Done.");
    s->waitForBytesWritten(-1);
    return "Done.";
}
QByteArray ScriptServer::vectorGetBinaryArray(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                              const QByteArray&,IfSI*&,VarSI*) {
    Q_ASSERT(sizeof(double)==sizeof(qint64)&&4096%sizeof(double)==0);

    command.replace("Vector::getBinaryArray(","");
    command.remove(command.indexOf(")"),99999);
    ObjectPtr o=_store->retrieveObject(command);
    VectorPtr v=kst_cast<Vector>(o);
    if(!v) {
        QByteArray ba;
        QDataStream ds(&ba,QIODevice::WriteOnly);
        ds<<(qint64)0;
        s->write(ba,ba.size());
        s->waitForReadyRead(30000);
        s->read(3000000);
        s->waitForBytesWritten(-1);
        return "No object";
    }
    QByteArray x=v->getBinaryArray();
    const char* d=x.data();
    int pos=-8;
    while(pos<x.size()) {
        int thisProc=s->write(d,qMin(40960,x.size()-pos))/sizeof(double);
        while(s->bytesToWrite()) {
            s->waitForBytesWritten(-1);
        }
        d=&d[thisProc*8];
        pos+=thisProc*8;
    }
    return "Data sent via handleResponse(...)";
}

QByteArray ScriptServer::matrixGetBinaryArray(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                              const QByteArray&,IfSI*&,VarSI*) {
    Q_ASSERT(sizeof(double)==sizeof(qint64)&&4096%sizeof(double)==0);

    command.replace("Matrix::getBinaryArray(","");
    command.remove(command.indexOf(")"),99999);
    ObjectPtr o=_store->retrieveObject(command);
    MatrixPtr m=kst_cast<Matrix>(o);
    if(!m) {
        qDebug()<<"No such thing.";
        QByteArray ba;
        QDataStream ds(&ba,QIODevice::WriteOnly);
        ds<<(qint64)0;
        s->write(ba,ba.size());
        s->waitForReadyRead(30000);
        s->read(3000000);
        s->waitForBytesWritten(-1);
        return "No object";
    }
    QByteArray x=m->getBinaryArray();
    const char* d=x.data();
    int pos=-8;
    while(pos<x.size()) {
        int thisProc=s->write(d,qMin(40960,x.size()-pos))/sizeof(double);
        while(s->bytesToWrite()) {
            s->waitForBytesWritten(-1);
        }
        d=&d[thisProc*8];
        pos+=thisProc*8;
    }
    return "Data sent via handleResponse(...)";
}

QByteArray ScriptServer::stringValue(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                     const QByteArray&,IfSI*&,VarSI*) {
    command.replace("String::value(","");
    command.remove(command.lastIndexOf(")"),999999);
    ObjectPtr o=_store->retrieveObject(command);
    StringPtr str=kst_cast<String>(o);
    if(str) {
        return handleResponse(str->value().toAscii(),s,0,"",0,0);
    } else {
        return handleResponse("No such object (variables not supported)",s,0,"",0,0);;
    }
}

QByteArray ScriptServer::stringSetValue(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                        const QByteArray&,IfSI*&,VarSI*) {
    command.replace("String::setValue(","");
    command.remove(command.lastIndexOf(")"),999999);
    QByteArrayList x;
    x.push_back(command);
    x[0].remove(x.at(0).indexOf(","),99999);
    x.push_back(command);
    x[1].remove(0,x.at(1).indexOf(",")+1);
    if(!x.at(0).size()) {
        return handleResponse("Invalid call to setValueOfString(",s,0,"",0,0);
    }
    ObjectPtr o=_store->retrieveObject(x[0]);
    StringPtr str=kst_cast<String>(o);
    if(str) {
        str->writeLock();
        str->setValue(x[1]);
        str->registerChange();
        str->unlock();
        return handleResponse("Okay",s,0,"",0,0);
    } else {
        return handleResponse("No such object (variables not supported)",s,0,"",0,0);;
    }
}

QByteArray ScriptServer::scalarValue(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                     const QByteArray&,IfSI*&,VarSI*) {
    command.replace("Scalar::value(","");
    command.remove(command.lastIndexOf(")"),999999);
    ObjectPtr o=_store->retrieveObject(command);
    ScalarPtr sca=kst_cast<Scalar>(o);
    if(sca) {
        return handleResponse(QByteArray::number(sca->value()),s,0,"",0,0);
    } else {
        return handleResponse("No such object (variables not supported)",s,0,"",0,0);;
    }
}

QByteArray ScriptServer::scalarSetValue(QByteArray&command, QLocalSocket* s,ObjectStore*,const int&,
                                        const QByteArray&,IfSI*&,VarSI*) {
    command.replace("Scalar::setValue(","");
    command.remove(command.lastIndexOf(")"),999999);
    QByteArrayList x;
    x.push_back(command);
    x[0].remove(x.at(0).indexOf(","),99999);
    x.push_back(command);
    x[1].remove(0,x.at(1).indexOf(",")+1);
    if(!x.at(0).size()) {
        return handleResponse("Invalid call to setValueOfScalar(",s,0,"",0,0);
    }
    ObjectPtr o=_store->retrieveObject(x[0]);
    ScalarPtr sca=kst_cast<Scalar>(o);
    if(sca) {
        sca->writeLock();
        sca->setValue(x[1].toDouble());
        sca->registerChange();
        sca->unlock();
        return handleResponse("Okay",s,0,"",0,0);
    } else {
        return handleResponse("No such object (variables not supported)",s,0,"",0,0);;
    }
}

}
