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
#include "labelscriptinterface.h"
#include "viewitemscriptinterface.h"
#include "arrowscriptinterface.h"
#include "plotscriptinterface.h"
#include "legendscriptinterface.h"

#include "stringscriptinterface.h"
#include "scalarscriptinterface.h"
#include "vectorscriptinterface.h"
#include "matrixscriptinterface.h"

#include "relationscriptinterface.h"

#include "dataobjectscriptinterface.h"

#include "sessionmodel.h"
#include "updateserver.h"
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
#include "lineitem.h"
#include "pictureitem.h"
#include "plotitem.h"
#include "svgitem.h"
#include "viewitemdialog.h"
#include "document.h"

#include "curve.h"
#include "equation.h"
#include "editablevector.h"
#include "matrix.h"
#include "histogram.h"
#include "psd.h"
#include "eventmonitorentry.h"
#include "image.h"
#include "csd.h"
#include "basicplugin.h"
#include "dialog.h"
#include "editablematrix.h"

#include "datasourcepluginmanager.h"

#include <updatemanager.h>

#include <QLocalSocket>
#include <iostream>
#include <QFile>
#include <QStringBuilder>

namespace Kst {

ScriptServer::ScriptServer(ObjectStore *obj) : _server(new QLocalServer(this)), _store(obj),_interface(0) {

    QString initial;

    initial = kstApp->userName();

    serverNameSet = false;

    // The command line hasn't been parsed yet, so
    // we can't rely on that to get the server name.
    QStringList args= qApp->arguments();
    for(int i=0;i<args.size();i++) {
        if (args.at(i).startsWith("--serverName=")) {
          initial=args.at(i);
          initial.remove("--serverName=");
          serverNameSet = true;
      }
    }

    serverName=initial;

    int j = 1;
    while(1) {
        QLocalSocket socket;
        socket.connectToServer(serverName);
        socket.waitForConnected(300);
        if(socket.state()!=QLocalSocket::ConnectedState) {
            _server->removeServer(serverName);
            _server->listen(serverName);
            break;
        }
        socket.disconnectFromServer();
        serverName=initial+"-"+QString::number(j++);
    }
    connect(_server,SIGNAL(newConnection()),this,SLOT(procConnection()));

    _fnMap.insert("newDataVector()",&ScriptServer::newDataVector);
    _fnMap.insert("newGeneratedVector()",&ScriptServer::newGeneratedVector);
    _fnMap.insert("newEditableVector()",&ScriptServer::newEditableVector);

    _fnMap.insert("getVectorList()",&ScriptServer::getVectorList);
    _fnMap.insert("getDataVectorList()",&ScriptServer::getDataVectorList);
    _fnMap.insert("getGeneratedVectorList()",&ScriptServer::getGeneratedVectorList);
    _fnMap.insert("getEditableVectorList()",&ScriptServer::getEditableVectorList);

    _fnMap.insert("getMatrixList()",&ScriptServer::getMatrixList);
    _fnMap.insert("newDataMatrix()",&ScriptServer::newDataMatrix);

    _fnMap.insert("getEditableMatrixList()",&ScriptServer::getEditableMatrixList);
    _fnMap.insert("newEditableMatrix()",&ScriptServer::newEditableMatrix);

    _fnMap.insert("getScalarList()",&ScriptServer::getScalarList);
    _fnMap.insert("newGeneratedScalar()",&ScriptServer::newGeneratedScalar);
    _fnMap.insert("newDataScalar()",&ScriptServer::newDataScalar);
    _fnMap.insert("newVectorScalar()",&ScriptServer::newVScalar);

    _fnMap.insert("getStringList()",&ScriptServer::getStringList);
    _fnMap.insert("newGeneratedString()",&ScriptServer::newGeneratedString);
    _fnMap.insert("newDataString()",&ScriptServer::newDataString);

    _fnMap.insert("getCurveList()",&ScriptServer::getCurveList);
    _fnMap.insert("newCurve()",&ScriptServer::newCurve);

    _fnMap.insert("getEquationList()",&ScriptServer::getEquationList);
    _fnMap.insert("newEquation()",&ScriptServer::newEquation);

    _fnMap.insert("getHistogramList()",&ScriptServer::getHistogramList);
    _fnMap.insert("newHistogram()",&ScriptServer::newHistogram);

    _fnMap.insert("getSpectrumList()",&ScriptServer::getSpectrumList);
    _fnMap.insert("newSpectrum()",&ScriptServer::newSpectrum);

    _fnMap.insert("getPluginList()", &ScriptServer::getPluginList);
    _fnMap.insert("newPlugin()",&ScriptServer::newPlugin);

    _fnMap.insert("getImageList()",&ScriptServer::getImageList);
    _fnMap.insert("newImage()",&ScriptServer::newImage);

    _fnMap.insert("getCSDList()",&ScriptServer::getCSDList);
//    _fnMap.insert("newCSD()",&ScriptServer::newCSD);

    _fnMap.insert("getBasicPluginList()",&ScriptServer::getBasicPluginList);
    _fnMap.insert("getBasicPluginTypeList()",&ScriptServer::getBasicPluginTypeList);
//    _fnMap.insert("newBasicPlugin()",&ScriptServer::newBasicPlugin);

    _fnMap.insert("getArrowList()",&ScriptServer::getArrowList);
    _fnMap.insert("newArrow()",&ScriptServer::newArrow);

    _fnMap.insert("getBoxList()",&ScriptServer::getBoxList);
    _fnMap.insert("newBox()",&ScriptServer::newBox);

    _fnMap.insert("getLegendList()",&ScriptServer::getLegendList);
    _fnMap.insert("newLegend()",&ScriptServer::newLegend);

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

#ifndef KST_NO_SVG
    _fnMap.insert("getSvgItemList()",&ScriptServer::getSvgItemList);
    _fnMap.insert("newSvgItem()",&ScriptServer::newSvgItem);
#endif

    _fnMap.insert("beginEdit()",&ScriptServer::beginEdit);
    _fnMap.insert("endEdit()",&ScriptServer::endEdit);

    _fnMap.insert("eliminate()",&ScriptServer::eliminate);

    _fnMap.insert("done()",&ScriptServer::done);
    _fnMap.insert("clear()",&ScriptServer::clear);

    _fnMap.insert("tabCount()",&ScriptServer::tabCount);
    _fnMap.insert("newTab()",&ScriptServer::newTab);
    _fnMap.insert("setTab()",&ScriptServer::setTab);
    _fnMap.insert("renameTab()",&ScriptServer::renameTab);
    _fnMap.insert("screenBack()",&ScriptServer::screenBack);
    _fnMap.insert("screenForward()",&ScriptServer::screenForward);
    _fnMap.insert("countFromEnd()",&ScriptServer::countFromEnd);
    _fnMap.insert("readToEnd()",&ScriptServer::readToEnd);
    _fnMap.insert("setPaused()",&ScriptServer::setPaused);
    _fnMap.insert("unsetPaused()",&ScriptServer::unsetPaused);
    _fnMap.insert("hide()", &ScriptServer::hide);
    _fnMap.insert("minimize()", &ScriptServer::minimize);
    _fnMap.insert("show()", &ScriptServer::show_window);
    _fnMap.insert("maximize()", &ScriptServer::maximize);
    _fnMap.insert("quit()", &ScriptServer::quit_kst);

    _fnMap.insert("fileOpen()", &ScriptServer::fileOpen);
    _fnMap.insert("fileSave()", &ScriptServer::fileSave);
    _fnMap.insert("exportGraphics()", &ScriptServer::exportGraphics);

    _fnMap.insert("setDatasourceBoolConfig()", &ScriptServer::setDatasourceBoolConfig);
    _fnMap.insert("setDatasourceIntConfig()", &ScriptServer::setDatasourceIntConfig);
    _fnMap.insert("setDatasourceStringConfig()", &ScriptServer::setDatasourceStringConfig);

    _fnMap.insert("cleanupLayout()", &ScriptServer::cleanupLayout);

    _fnMap.insert("testCommand()", &ScriptServer::testCommand);

#if 0

    _fnMap.insert("EditableVector::setBinaryArray()",&ScriptServer::editableVectorSetBinaryArray);
    _fnMap.insert("EditableMatrix::setBinaryArray()",&ScriptServer::editableMatrixSetBinaryArray);
    _fnMap.insert("EditableVector::set()",&ScriptServer::editableVectorSet);
    _fnMap.insert("Vector::getBinaryArray()",&ScriptServer::vectorGetBinaryArray);
    _fnMap.insert("Matrix::getBinaryArray()",&ScriptServer::matrixGetBinaryArray);
    _fnMap.insert("String::value()",&ScriptServer::stringValue);
    _fnMap.insert("String::setValue()",&ScriptServer::stringSetValue);
    _fnMap.insert("Scalar::value()",&ScriptServer::scalarValue);
    _fnMap.insert("Scalar::setValue()",&ScriptServer::scalarSetValue);
#endif

}

ScriptServer::~ScriptServer()
{
    delete _server;
  delete _interface;
}

void ScriptServer::setScriptServerName(QString initial)
{
  int j = 1;

  _server->close();
  serverName = initial;
  while(1) {
      QLocalSocket socket;
      socket.connectToServer(serverName);
      socket.waitForConnected(300);
      if(socket.state()!=QLocalSocket::ConnectedState) {
          _server->removeServer(serverName);
          _server->listen(serverName);
          break;
      }
      socket.disconnectFromServer();
      serverName=initial+"-"+QString::number(j++);
  }
  serverNameSet = true;
  //connect(_server,SIGNAL(newConnection()),this,SLOT(procConnection()));
}

/** Conv. function which takes a response, and executes if 'if' statement is unexistant or true. */
QByteArray handleResponse(const QByteArray& response, QLocalSocket* s)
{
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

/** @sa outputViewItemList() */
template<class T> QByteArray outputObjectList(
    QLocalSocket* s,ObjectStore*_store) {

    ObjectList<T> vl=_store->getObjects<T>();
    QByteArray a;
    typename ObjectList<T>::ConstIterator it = vl.constBegin();
    bool first = true;
    for(; it != vl.constEnd(); ++it) {
        SharedPtr<T> v = (*it);
        v->readLock();
        if (!first) {
          a += '|';
        }
        first = false;
        a+=v->Name();
        v->unlock();
    }
    if(a.size()) {
        return handleResponse(a,s);
    } else {
        return handleResponse("NO_OBJECTS",s);
    }
}

/** @sa outputObjectList() */
template<class T> QByteArray outputViewItemList(QLocalSocket* s) {
    QList<T *> vl=ViewItem::getItems<T>();
    QByteArray a;
    typename QList<T*>::iterator it = vl.begin();
    for(; it != vl.end(); ++it) {
        T* v = (*it);
        a+='['%v->Name()%']';
    }
    if(a.size()) {
        return handleResponse(a,s);
    } else {
        return handleResponse("NO_OBJECTS",s);
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
                if(search.contains(vi[i]->shortName().toLatin1())) {
                    ButtonItem* bi=qobject_cast<ButtonItem*>(vi[i]);
                    if(bi) {
                        bi->addSocket(s);
                        disconnect(s,SIGNAL(readyRead()),this,SLOT(readSomething()));
                        return;
                    }
                    LineEditItem* li=qobject_cast<LineEditItem*>(vi[i]);
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
QByteArray ScriptServer::exec(QByteArray command, QLocalSocket *s)
{
  if(command.isEmpty()) {
        return handleResponse("",s);
    }

    // Map
    QByteArray ycommand(command);
    ycommand.remove(ycommand.indexOf("("),9999999);
    ycommand+="()";
    ScriptMemberFn fn=_fnMap.value(ycommand,&ScriptServer::noSuchFn);
    if(fn!=&ScriptServer::noSuchFn) {
        return CALL_MEMBER_FN(*this,fn)(command, s,_store);
    } else {
        if(_interface) {
            return handleResponse(_interface->doCommand(command).toLatin1(),s); //magic
        } else {
            return handleResponse("Unknown command!",s);
        }
    }

    return "?";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QByteArray ScriptServer::getVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Vector>(s,_store);
}

QByteArray ScriptServer::getDataVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<DataVector>(s,_store);
}

QByteArray ScriptServer::getGeneratedVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<GeneratedVector>(s,_store);
}


QByteArray ScriptServer::getEditableVectorList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<EditableVector>(s,_store);
}

QByteArray ScriptServer::newEditableVector(QByteArray&, QLocalSocket* s,ObjectStore*) {
  if(_interface) {
    return handleResponse("To access this function, first call endEdit()",s);
  } else {
    _interface = EditableVectorSI::newVector(_store); return handleResponse("Ok",s);
  }
}


QByteArray ScriptServer::newDataVector(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = DataVectorSI::newVector(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::newGeneratedVector(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = GeneratedVectorSI::newVector(_store); return handleResponse("Ok",s);
    }
}



QByteArray ScriptServer::getScalarList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Scalar>(s,_store);
}


QByteArray ScriptServer::newGeneratedScalar(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = ScalarGenSI::newScalar(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::newDataScalar(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = ScalarDataSI::newScalar(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::newVScalar(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = ScalarVectorSI::newScalar(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::getMatrixList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Matrix>(s,_store);
}


QByteArray ScriptServer::newDataMatrix(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = DataMatrixSI::newMatrix(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::getEditableMatrixList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<EditableMatrix>(s,_store);
}

QByteArray ScriptServer::newEditableMatrix(QByteArray&, QLocalSocket* s,ObjectStore*) {
  if(_interface) {
    return handleResponse("To access this function, first call endEdit()",s);
  } else {
    _interface = EditableMatrixSI::newMatrix(_store); return handleResponse("Ok",s);
  }
}



QByteArray ScriptServer::getStringList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<String>(s,_store);
}

QByteArray ScriptServer::newGeneratedString(QByteArray&, QLocalSocket* s,ObjectStore*) {
    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = StringGenSI::newString(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::newDataString(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = StringDataSI::newString(_store); return handleResponse("Ok",s);
    }
}



QByteArray ScriptServer::getCurveList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Curve>(s,_store);
}


QByteArray ScriptServer::newCurve(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = CurveSI::newCurve(_store); return handleResponse("Ok",s);
    }
}



QByteArray ScriptServer::getEquationList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Equation>(s,_store);
}

QByteArray ScriptServer::newEquation(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = EquationSI::newEquation(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::getHistogramList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Histogram>(s,_store);
}


QByteArray ScriptServer::newHistogram(QByteArray&, QLocalSocket* s,ObjectStore*) {
  if (_interface) {
    return handleResponse("To access this function, first call endEdit()",s);
  } else {
    _interface = HistogramSI::newHistogram(_store); return handleResponse("Ok",s);
  }
}


QByteArray ScriptServer::getSpectrumList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<PSD>(s,_store);
}


QByteArray ScriptServer::newSpectrum(QByteArray&, QLocalSocket* s,ObjectStore*) {

  if(_interface) {
    return handleResponse("To access this function, first call endEdit()",s);
  } else {
    _interface = SpectrumSI::newSpectrum(_store); return handleResponse("Ok",s);
  }
}


QByteArray ScriptServer::getPluginList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<BasicPlugin>(s,_store);
}


QByteArray ScriptServer::newPlugin(QByteArray& plugin, QLocalSocket* s,ObjectStore* store) {

  if(_interface) {
    return handleResponse("To access this function, first call endEdit()",s);
  } else {
    plugin.replace("newPlugin(","");
    plugin.remove(plugin.lastIndexOf(")"),1);
    _interface = PluginSI::newPlugin(store, plugin);
    return handleResponse("Ok",s);
  }
}


QByteArray ScriptServer::getImageList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<Image>(s,_store);
}

QByteArray ScriptServer::newImage(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) {
      return handleResponse("To access this function, first call endEdit()",s);
    } else {
      _interface = ImageSI::newImage(_store); return handleResponse("Ok",s);
    }
}


QByteArray ScriptServer::getCSDList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<CSD>(s,_store);
}

/*
QByteArray ScriptServer::newCSD(QByteArray&, QLocalSocket* s,ObjectStore*,const int&ifMode,

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s);}
    else {
        _interface = DialogLauncherSI::self->showCSDDialog();
        return handleResponse("Ok",s);
    }
}
*/


QByteArray ScriptServer::getBasicPluginList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputObjectList<BasicPlugin>(s,_store);
}

QByteArray ScriptServer::getBasicPluginTypeList(QByteArray&, QLocalSocket* s,ObjectStore*) {

    QString a;
    for(int i=0;i<DataObject::dataObjectPluginList().size();i++) {
        a.push_back(DataObject::dataObjectPluginList()[i].toLatin1()+'\n');
    }
    return handleResponse(a.toLatin1(),s);
}



QByteArray ScriptServer::getArrowList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<ArrowItem>(s);
}

QByteArray ScriptServer::newArrow(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ArrowSI::newArrow(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getBoxList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<BoxItem>(s);
}

QByteArray ScriptServer::newBox(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newBox(); return handleResponse("Ok",s); }
}

QByteArray ScriptServer::getLegendList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<LegendItem>(s);
}

QByteArray ScriptServer::newLegend(QByteArray&command, QLocalSocket* s,ObjectStore*) {

  if(_interface) {
    return handleResponse("To access this function, first call endEdit()",s); }
  else {
    _interface = LegendSI::newLegend(command.replace("newLegend(","").replace(")",""));
    return handleResponse("Ok",s);
  }
}


QByteArray ScriptServer::getButtonList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<ButtonItem>(s);
}

QByteArray ScriptServer::newButton(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newButton(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getLineEditList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<LineEditItem>(s);
}

QByteArray ScriptServer::newLineEdit(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newLineEdit(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getCircleList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<CircleItem>(s);
}

QByteArray ScriptServer::newCircle(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newCircle(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getEllipseList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<EllipseItem>(s);
}

QByteArray ScriptServer::newEllipse(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newEllipse(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getLabelList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<LabelItem>(s);
}

QByteArray ScriptServer::newLabel(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = LabelSI::newLabel(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getLineList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<LineItem>(s);
}

QByteArray ScriptServer::newLine(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newLine(); return handleResponse("Ok",s); }
}



QByteArray ScriptServer::getPictureList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {


    return outputViewItemList<PictureItem>(s);
}

QByteArray ScriptServer::newPicture(QByteArray&command, QLocalSocket* s,ObjectStore*) {

    command.replace("newPicture(","");
    command.remove(command.lastIndexOf(")"),1);
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newPicture(command); return handleResponse("Ok",s); }
}


/***********************************/
/* Plot related scripting commands */
/***********************************/

QByteArray ScriptServer::getPlotList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<PlotItem>(s);
}

QByteArray ScriptServer::newPlot(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = PlotSI::newPlot(); return handleResponse("Ok",s); }
}


#ifndef KST_NO_SVG
QByteArray ScriptServer::getSvgItemList(QByteArray&, QLocalSocket* s,ObjectStore*_store) {

    return outputViewItemList<SvgItem>(s);
}

QByteArray ScriptServer::newSvgItem(QByteArray&command, QLocalSocket* s,ObjectStore*) {

    command.replace("newSvgItem(","");
    command.remove(command.lastIndexOf(")"),1);
    if(_interface) { return handleResponse("To access this function, first call endEdit()",s); }
    else { _interface = ViewItemSI::newSvgItem(command); return handleResponse("Ok",s); }

}
#endif


QByteArray ScriptServer::beginEdit(QByteArray&command, QLocalSocket* s,ObjectStore*_store) {

    if(_interface) {
        return handleResponse("To access this function, first call endEdit()",s);
    }
    command.replace("beginEdit(","");

    command.remove(command.lastIndexOf(")"),999999);

    ViewItem *view_item = ViewItem::retrieveItem<ViewItem>(command);
    if (view_item) {
      _interface = view_item->scriptInterface();
      return handleResponse("Ok",s);
    } else {      
      ObjectPtr o=_store->retrieveObject(command);
      if (o) {
        _interface = o->scriptInterface();
        if (_interface) {
          return handleResponse("Ok",s);
        } else {
          return handleResponse("Not supported",s);
        }
      }
      return handleResponse("Unknown error",s);
    }
}

QByteArray ScriptServer::eliminate(QByteArray&command, QLocalSocket* s,ObjectStore*_store) {

    if(_interface) {
        return handleResponse("To access this function, first call endEdit()",s);
    }
    command.replace("beginEdit(","");
    // check if object
    command.remove(command.lastIndexOf(")"),999999);
    ObjectPtr o=_store->retrieveObject(command);
    if(!o) {
        // check if view item
        for(int h=0;h<2;h++) {
            for(int i=0;i<vi.size();i++) {
                if(command.contains(vi[i]->shortName().toLatin1())) {
                    vi[i]->hide();  // goodbye, memory.
                    return handleResponse("Done",s);
                }
            }
            vi=ViewItem::getItems<ViewItem>();
        }

        return handleResponse("No such object",s);
    } else {
        if (RelationPtr relation = kst_cast<Relation>(o)) {
            Data::self()->removeCurveFromPlots(relation);
        }
        _store->removeObject(o);
        UpdateServer::self()->requestUpdateSignal();

        return handleResponse("Done",s);
    }
}

QByteArray ScriptServer::endEdit(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(!_interface) {
        return handleResponse("No interface open.",s);
    }

    if(!_interface->isValid()) {
        _interface=0;
        return handleResponse("The interface isn't valid.",s);
    }

    QByteArray x=_interface->endEditUpdate();
    _interface=0;
    return handleResponse(x,s);
}

QByteArray ScriptServer::done(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(!s) {
        return "Invalid... no socket...";
    }
    s->write("Bye.");
    s->flush();
    s->close();
    delete s;
    return "Bye.";
}


QByteArray ScriptServer::clear(QByteArray&, QLocalSocket* s,ObjectStore*) {

  kstApp->mainWindow()->newDoc(true);
  return handleResponse("Done",s);
}


QByteArray ScriptServer::tabCount(QByteArray&, QLocalSocket* s,ObjectStore*) {

    return handleResponse(QByteArray::number(kstApp->mainWindow()->tabWidget()->count()),s);
}

QByteArray ScriptServer::newTab(QByteArray&, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->tabWidget()->createView();
    kstApp->mainWindow()->tabWidget()->setCurrentIndex(kstApp->mainWindow()->tabWidget()->count()-1);
    return handleResponse("Done",s);
}

QByteArray ScriptServer::setTab(QByteArray&command, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->tabWidget()->setCurrentIndex(command.replace("setTab(","").replace(")","").toInt());
    return handleResponse("Done",s);
}

QByteArray ScriptServer::renameTab(QByteArray&command, QLocalSocket* s,ObjectStore*) {

    int index = kstApp->mainWindow()->tabWidget()->currentIndex();
    QString new_name = ScriptInterface::getArg(command);
    kstApp->mainWindow()->tabWidget()->setTabText(index, new_name);
    return handleResponse("Done",s);
}

QByteArray ScriptServer::screenBack(QByteArray&, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->_backAct->trigger();
    return handleResponse("Done",s);
}

QByteArray ScriptServer::screenForward(QByteArray&, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->_forwardAct->trigger();
    return handleResponse("Done",s);
}

QByteArray ScriptServer::countFromEnd(QByteArray&, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->_readFromEndAct->trigger();
    return handleResponse("Done",s);
}

QByteArray ScriptServer::readToEnd(QByteArray&, QLocalSocket* s,ObjectStore*) {

    kstApp->mainWindow()->_readToEndAct->trigger();
    return handleResponse("Done",s);
}

QByteArray ScriptServer::setPaused(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(!kstApp->mainWindow()->_pauseAct->isChecked()) {
        kstApp->mainWindow()->_pauseAct->trigger();
    }
    return handleResponse("Done",s);
}

QByteArray ScriptServer::unsetPaused(QByteArray&, QLocalSocket* s,ObjectStore*) {

    if(kstApp->mainWindow()->_pauseAct->isChecked()) {
        kstApp->mainWindow()->_pauseAct->trigger();
    }
    return handleResponse("Done",s);

}

QByteArray ScriptServer::hide(QByteArray&, QLocalSocket* s,ObjectStore*) {
    kstApp->mainWindow()->hide();

    return handleResponse("Done",s);
}

QByteArray ScriptServer::minimize(QByteArray&, QLocalSocket* s,ObjectStore*) {
    kstApp->mainWindow()->showMinimized();

    return handleResponse("Done",s);
}

QByteArray ScriptServer::maximize(QByteArray&, QLocalSocket* s,ObjectStore*) {
    kstApp->mainWindow()->showMaximized();

    return handleResponse("Done",s);
}

QByteArray ScriptServer::show_window(QByteArray&, QLocalSocket* s,ObjectStore*) {
    kstApp->mainWindow()->showNormal();

    return handleResponse("Done",s);
}


QByteArray ScriptServer::quit_kst(QByteArray&, QLocalSocket* s,ObjectStore*) {
    kstApp->quit();

    return handleResponse("Done",s);
}


QByteArray ScriptServer::fileOpen(QByteArray&command, QLocalSocket* s, ObjectStore*) {
  command.replace("fileOpen(", "");
  command.chop(1);

  kstApp->mainWindow()->openFile(command);

  return handleResponse("Done",s);
}

QByteArray ScriptServer::fileSave(QByteArray&command, QLocalSocket* s, ObjectStore*) {
  command.replace("fileSave(", "");
  command.chop(1);

  kstApp->mainWindow()->document()->save(command);

  return handleResponse("Done",s);
}

QByteArray ScriptServer::exportGraphics(QByteArray&command, QLocalSocket* s, ObjectStore*) {
  QStringList args = ScriptInterface::getArgs(command);

  if (args.length() == 7) {
    QString filename = args[0];
    QString format = args[1];
    int width = args[2].toInt();
    int height = args[3].toInt();
    int display = args[4].toInt();
    bool allTabs = (args[5].trimmed().toLower() == "true");
    int autosave_period = args[6].toInt();

    kstApp->mainWindow()->exportGraphicsFile(filename, format, width, height, display, allTabs, autosave_period*1000);
  }
  return handleResponse("Done",s);
}

QByteArray ScriptServer::testCommand(QByteArray&command, QLocalSocket* s,ObjectStore*) {
  static int i=0;

  qDebug() << "testCommand: " << i++;
  return handleResponse("Done",s);
}


QByteArray ScriptServer::cleanupLayout(QByteArray&command, QLocalSocket* s,ObjectStore*) {

    QString param = command.replace("cleanupLayout(","").replace(")","");
    bool isNum = true;
    int n_cols = param.toInt(&isNum);

    if (isNum) { // columns
      kstApp->mainWindow()->tabWidget()->currentView()->createLayout(false, n_cols);
    } else if (param.toLower() == "protect") {
      kstApp->mainWindow()->tabWidget()->currentView()->createLayout();
    } else {
      kstApp->mainWindow()->tabWidget()->currentView()->createUnprotectedLayout();
    }

    kstApp->mainWindow()->tabWidget()->setCurrentIndex(command.replace("setTab(","").replace(")","").toInt());
    return handleResponse("Done",s);
}

/*** Interface for changing datasource configuration settings ***/
QByteArray ScriptServer::setDatasourceBoolConfig(QByteArray& command, QLocalSocket* s, ObjectStore*) {
  QStringList args = ScriptInterface::getArgs(command);
  if (args.length()==4) {
    QString ds_type = args[0]; // eg "Ascii File"
    QString filename = args[1];
    QString setting = args[2];
    bool value = (args[3].trimmed().toLower() == "true");
    bool set_default = (filename == "$DEFAULT");

    DataSourcePluginManager::settingsObject().beginGroup(ds_type);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().beginGroup(filename);
    }
    DataSourcePluginManager::settingsObject().setValue(setting, value);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().endGroup();
    }
    DataSourcePluginManager::settingsObject().endGroup();
  }
  return handleResponse("Done",s);

}

QByteArray ScriptServer::setDatasourceIntConfig(QByteArray& command, QLocalSocket* s, ObjectStore*) {
  QStringList args = ScriptInterface::getArgs(command);
  if (args.length()==4) {
    QString ds_type = args[0]; // eg "Ascii File"
    QString filename = args[1];
    QString setting = args[2];
    int value = args[3].toInt();
    bool set_default = (filename == "$DEFAULT");

    DataSourcePluginManager::settingsObject().beginGroup(ds_type);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().beginGroup(filename);
    }
    DataSourcePluginManager::settingsObject().setValue(setting, value);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().endGroup();
    }
    DataSourcePluginManager::settingsObject().endGroup();
  }
  return handleResponse("Done",s);

}

QByteArray ScriptServer::setDatasourceStringConfig(QByteArray& command, QLocalSocket* s, ObjectStore*) {
  QStringList args = ScriptInterface::getArgs(command);
  if (args.length()==4) {
    QString ds_type = args[0]; // eg "Ascii File"
    QString filename = args[1];
    QString setting = args[2];
    QString value = args[3].replace('`', ','); // , is replaced with `.  switch back.
    bool set_default = (filename == "$DEFAULT");

    DataSourcePluginManager::settingsObject().beginGroup(ds_type);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().beginGroup(filename);
    }
    DataSourcePluginManager::settingsObject().setValue(setting, value);
    if (!set_default) {
      DataSourcePluginManager::settingsObject().endGroup();
    }
    DataSourcePluginManager::settingsObject().endGroup();
  }
  return handleResponse("Done",s);

}

}
