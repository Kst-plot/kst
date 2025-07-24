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

#include "matrixscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QStringBuilder>


namespace Kst {

/***************************/
/* common matrix commands  */
/***************************/

QString MatrixCommonSI::value(QString &command) {
  QStringList vars = getArgs(command);

  return QString::number(_matrix->value(vars[0].toDouble(), vars[1].toDouble()));
}

QString MatrixCommonSI::length(QString &) {
  return QString::number(_matrix->sampleCount());
}

QString MatrixCommonSI::min(QString &) {
  return QString::number(_matrix->minValue());
}

QString MatrixCommonSI::max(QString &) {
  return QString::number(_matrix->maxValue());
}

QString MatrixCommonSI::mean(QString &) {
  return QString::number(_matrix->meanValue());
}

QString MatrixCommonSI::width(QString &) {
  return QString::number(_matrix->xNumSteps());
}

QString MatrixCommonSI::height(QString &) {
  return QString::number(_matrix->yNumSteps());
}

QString MatrixCommonSI::dX(QString &) {
  return QString::number(_matrix->xStepSize());
}

QString MatrixCommonSI::dY(QString &) {
  return QString::number(_matrix->yStepSize());
}

QString MatrixCommonSI::minX(QString &) {
  return QString::number(_matrix->minX());
}

QString MatrixCommonSI::minY(QString &) {
  return QString::number(_matrix->minY());
}

QString MatrixCommonSI::store(QString & command) {
  QString arg = getArg(command);
  QFile tmpfile(arg);

  bool ok = tmpfile.open(QIODevice::WriteOnly);
  ok |= _matrix->saveToTmpFile(tmpfile);
  tmpfile.close();

  if (ok) {
    return QString("%1 %2").arg(_matrix->xNumSteps()).arg(_matrix->yNumSteps());
  } else {
    return "Error writing tmp file";
  }
}


/******************************************************/
/* Data Matrix                                        */
/******************************************************/
DataMatrixSI::DataMatrixSI(DataMatrixPtr it) : _datamatrix(it) {
    //_datamatrix = it;
    _matrix = it;

    _fnMap.insert("change",&DataMatrixSI::change);
    _fnMap.insert("field",&DataMatrixSI::field);
    _fnMap.insert("filename",&DataMatrixSI::filename);
    _fnMap.insert("startX",&DataMatrixSI::startX);
    _fnMap.insert("startY",&DataMatrixSI::startY);

    // Matrix Common Commands
    _fnMap.insert("value",&DataMatrixSI::value);
    _fnMap.insert("length",&DataMatrixSI::length);
    _fnMap.insert("min",&DataMatrixSI::min);
    _fnMap.insert("max",&DataMatrixSI::max);
    _fnMap.insert("mean",&DataMatrixSI::mean);
    _fnMap.insert("width",&DataMatrixSI::width);
    _fnMap.insert("height",&DataMatrixSI::height);
    _fnMap.insert("dX",&DataMatrixSI::dX);
    _fnMap.insert("dY",&DataMatrixSI::dY);
    _fnMap.insert("minX",&DataMatrixSI::minX);
    _fnMap.insert("minY",&DataMatrixSI::minY);
    _fnMap.insert("store",&DataMatrixSI::store);
}

QString DataMatrixSI::doCommand(QString command_in) {

  if (!_datamatrix) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  DataMatrixInterfaceMemberFn fn=_fnMap.value(command,&DataMatrixSI::noSuchFn);

  if(fn!=&DataMatrixSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doObjectCommand(command_in, _datamatrix);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool DataMatrixSI::isValid() {
  return _datamatrix.isPtrValid();
}

ScriptInterface* DataMatrixSI::newMatrix(ObjectStore *store) {
  DataMatrixPtr matrix;
  matrix = store->createObject<DataMatrix>();
  return new DataMatrixSI(matrix);
}

QByteArray DataMatrixSI::endEditUpdate() {
  _datamatrix->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_datamatrix->Name()).toLatin1();
}

/***************************/
/*   data matrix commands  */
/***************************/

QString DataMatrixSI::change(QString& command) {
  QStringList vars = getArgs(command);

  DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                       _datamatrix->store(), vars.at(0));
  _datamatrix->writeLock();
  _datamatrix->change(ds,
                 vars.at(1),         // field
                 vars.at(2).toInt(), // x start
                 vars.at(3).toInt(), // y start
                 vars.at(4).toInt(), // num x steps
                 vars.at(5).toInt(), // num y steps

                 false, false, 0, 0, // FIXME: image streams!
                 false,
                 vars.at(6).toDouble(), // min x
                 vars.at(7).toDouble(), // min y
                 vars.at(8).toDouble(), // step x
                 vars.at(9).toDouble() // step y
                 );
  _datamatrix->unlock();
  return "Done";
}

QString DataMatrixSI::field(QString& command) {
  QString arg = getArg(command);
  return _datamatrix->field();
}

QString DataMatrixSI::filename(QString& command) {
  QString arg = getArg(command);
  return _datamatrix->filename();
}

QString DataMatrixSI::startX(QString& command) {
  QString arg = getArg(command);
  return QString::number(_datamatrix->reqXStart());
}

QString DataMatrixSI::startY(QString& command) {
  QString arg = getArg(command);
  return QString::number(_datamatrix->reqYStart());
}


/******************************************************/
/* Editable Matrix                                        */
/******************************************************/
EditableMatrixSI::EditableMatrixSI(EditableMatrixPtr it) : _editablematrix(it) {
    //_editablematrix = it;
    _matrix = it;

    _fnMap.insert("load", &EditableMatrixSI::load);

    // Matrix Common Commands
    _fnMap.insert("value",&EditableMatrixSI::value);
    _fnMap.insert("length",&EditableMatrixSI::length);
    _fnMap.insert("min",&EditableMatrixSI::min);
    _fnMap.insert("max",&EditableMatrixSI::max);
    _fnMap.insert("mean",&EditableMatrixSI::mean);
    _fnMap.insert("width",&EditableMatrixSI::width);
    _fnMap.insert("height",&EditableMatrixSI::height);
    _fnMap.insert("dX",&EditableMatrixSI::dX);
    _fnMap.insert("dY",&EditableMatrixSI::dY);
    _fnMap.insert("minX",&EditableMatrixSI::minX);
    _fnMap.insert("minY",&EditableMatrixSI::minY);
    _fnMap.insert("store",&EditableMatrixSI::store);
}

QString EditableMatrixSI::doCommand(QString command_in) {

  if (!_editablematrix) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  EditableMatrixInterfaceMemberFn fn=_fnMap.value(command,&EditableMatrixSI::noSuchFn);

  if(fn!=&EditableMatrixSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doObjectCommand(command_in, _editablematrix);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool EditableMatrixSI::isValid() {
  return _editablematrix.isPtrValid();
}

ScriptInterface* EditableMatrixSI::newMatrix(ObjectStore *store) {
  EditableMatrixPtr matrix;
  matrix = store->createObject<EditableMatrix>();
  return new EditableMatrixSI(matrix);
}

QByteArray EditableMatrixSI::endEditUpdate() {
  _editablematrix->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_editablematrix->Name()).toLatin1();
}

QString EditableMatrixSI::load(QString& command) {
  QStringList vars = getArgs(command);

  QFile tmpfile(vars[0]);
  tmpfile.open(QIODevice::ReadOnly);

  _editablematrix->loadFromTmpFile(tmpfile, vars[1].toInt(), vars[2].toInt());
  return "done";
}

}
