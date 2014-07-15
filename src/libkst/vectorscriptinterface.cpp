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

#include "vectorscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QStringBuilder>


namespace Kst {

/***************************/
/* common vector commands  */
/***************************/

QString VectorCommonSI::value(QString & command) {
  QString arg = getArg(command);
  return QString::number(_vector->value(arg.toInt()));
}

QString VectorCommonSI::length(QString &) {
  return QString::number(_vector->length());
}

QString VectorCommonSI::min(QString &) {
  return QString::number(_vector->min());
}

QString VectorCommonSI::max(QString &) {
  return QString::number(_vector->max());
}

QString VectorCommonSI::mean(QString &) {
  return QString::number(_vector->mean());
}

QString VectorCommonSI::descriptionTip(QString &) {
  return _vector->descriptionTip();
}

QString VectorCommonSI::store(QString & command) {
  QString arg = getArg(command);
  QFile tmpfile(arg);

  bool ok = tmpfile.open(QIODevice::WriteOnly);
  ok |= _vector->saveToTmpFile(tmpfile);
  tmpfile.close();

  if (ok) {
    return "Done";
  } else {
    return "Error writing tmp file";
  }
}


/******************************************************/
/* Plain (base) Vectors                               */
/******************************************************/
VectorSI::VectorSI(VectorPtr it) {
  if (it) {
    _vector = it;
  } else {
    _vector = 0;
  }

  _fnMap.insert("value",&VectorSI::value);
  _fnMap.insert("length",&VectorSI::length);
  _fnMap.insert("min",&VectorSI::min);
  _fnMap.insert("max",&VectorSI::max);
  _fnMap.insert("mean",&VectorSI::mean);
  _fnMap.insert("descriptionTip",&VectorSI::descriptionTip);
  _fnMap.insert("store",&VectorSI::store);
}

QString VectorSI::doCommand(QString command_in) {

  if (!_vector) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  VectorInterfaceMemberFn fn=_fnMap.value(command,&VectorSI::noSuchFn);

  if(fn!=&VectorSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doNamedObjectCommand(command_in, _vector);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool VectorSI::isValid() {
  return _vector.isPtrValid();
}

ScriptInterface* VectorSI::newVector(ObjectStore *) {
  return 0L;
}

QByteArray VectorSI::endEditUpdate() {
  _vector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_vector->Name()).toLatin1();
}


/******************************************************/
/* Data Vectors                                       */
/******************************************************/
DataVectorSI::DataVectorSI(DataVectorPtr it) {
  if (it) {
    _datavector = it;
    _vector = it;
  } else {
    _datavector = 0;
    _vector = 0;
  }

  _fnMap.insert("change",&DataVectorSI::change);
  _fnMap.insert("field",&DataVectorSI::field);
  _fnMap.insert("filename",&DataVectorSI::filename);
  _fnMap.insert("start",&DataVectorSI::start);
  _fnMap.insert("NFrames",&DataVectorSI::NFrames);
  _fnMap.insert("skip",&DataVectorSI::skip);
  _fnMap.insert("boxcarFirst",&DataVectorSI::boxcarFirst);

  _fnMap.insert("value",&DataVectorSI::value);
  _fnMap.insert("length",&DataVectorSI::length);
  _fnMap.insert("min",&DataVectorSI::min);
  _fnMap.insert("max",&DataVectorSI::max);
  _fnMap.insert("mean",&DataVectorSI::mean);
  _fnMap.insert("descriptionTip",&DataVectorSI::descriptionTip);
  _fnMap.insert("store",&DataVectorSI::store);

}

QString DataVectorSI::doCommand(QString command_in) {

  if (!_vector) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  DataVectorInterfaceMemberFn fn=_fnMap.value(command,&DataVectorSI::noSuchFn);

  if(fn!=&DataVectorSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doNamedObjectCommand(command_in, _vector);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool DataVectorSI::isValid() {
  return _datavector.isPtrValid();
}

ScriptInterface* DataVectorSI::newVector(ObjectStore *store) {
  DataVectorPtr vector;
  vector = store->createObject<DataVector>();
  return new DataVectorSI(vector);
}

QByteArray DataVectorSI::endEditUpdate() {
  _datavector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_datavector->Name()).toLatin1();
}

/***************************/
/*   data vector commands  */
/***************************/

QString DataVectorSI::change(QString& command) {
  QStringList vars = getArgs(command);

  DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                       _datavector->store(), vars.at(0));
  _datavector->writeLock();
  _datavector->change(ds,
                      vars.at(1),         // field
                      vars.at(2).toInt(), // f0
                      vars.at(3).toInt(), // n
                      vars.at(4).toInt(), // skip
                      vars.at(4).toInt() > 0, // do skip
                      vars.at(5) == "True" // do average
                      );
  _datavector->unlock();
  return "Done";
}

QString DataVectorSI::field(QString& command) {
  QString arg = getArg(command);
  return _datavector->field();
}

QString DataVectorSI::filename(QString& command) {
  QString arg = getArg(command);
  return _datavector->filename();
}

QString DataVectorSI::start(QString& command) {
  QString arg = getArg(command);
  return QString::number(_datavector->startFrame());
}

QString DataVectorSI::NFrames(QString& command) {
  QString arg = getArg(command);
  return QString::number(_datavector->numFrames());
}

QString DataVectorSI::skip(QString& command) {
  QString arg = getArg(command);
  return QString::number(_datavector->skip());
}

QString DataVectorSI::boxcarFirst(QString& command) {
  QString arg = getArg(command);
  return _datavector->doAve()?"True":"False";
}

/******************************************************/
/* Generated  Vectors                                 */
/******************************************************/
GeneratedVectorSI::GeneratedVectorSI(GeneratedVectorPtr it) {
    if (it) {
      _generatedvector = it;
      _vector = it;
    } else {
      _generatedvector = 0;
      _vector = 0;
    }

    _fnMap.insert("change",&GeneratedVectorSI::change);

    _fnMap.insert("value",&GeneratedVectorSI::value);
    _fnMap.insert("length",&GeneratedVectorSI::length);
    _fnMap.insert("min",&GeneratedVectorSI::min);
    _fnMap.insert("max",&GeneratedVectorSI::max);
    _fnMap.insert("mean",&GeneratedVectorSI::mean);
    _fnMap.insert("descriptionTip",&GeneratedVectorSI::descriptionTip);
    _fnMap.insert("store",&GeneratedVectorSI::store);
}

QString GeneratedVectorSI::doCommand(QString command_in) {

  if (!_vector) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  GeneratedVectorInterfaceMemberFn fn=_fnMap.value(command,&GeneratedVectorSI::noSuchFn);

  if(fn!=&GeneratedVectorSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doNamedObjectCommand(command_in, _vector);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool GeneratedVectorSI::isValid() {
  return _generatedvector.isPtrValid();
}

ScriptInterface* GeneratedVectorSI::newVector(ObjectStore *store) {
  GeneratedVectorPtr vector;
  vector = store->createObject<GeneratedVector>();
  return new GeneratedVectorSI(vector);
}

QByteArray GeneratedVectorSI::endEditUpdate() {
  _generatedvector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_generatedvector->Name()).toLatin1();
}

/*****************************/
/* generated vector commands */
/*****************************/

QString GeneratedVectorSI::change(QString& command) {
  QStringList vars = getArgs(command);

  _generatedvector->writeLock();

  _generatedvector->changeRange(
                 vars.at(0).toDouble(), // start
                 vars.at(1).toDouble(), // end
                 vars.at(2).toInt() // number of points
                 );
  _generatedvector->unlock();

  return "Done";
}


/******************************************************/
/* Editable Vectors                                   */
/******************************************************/
EditableVectorSI::EditableVectorSI(EditableVectorPtr it) {
  if (it) {
    _editablevector = it;
    _vector = it;
  } else {
    _editablevector = 0;
    _vector = 0;
  }


  _fnMap.insert("load",&EditableVectorSI::load);
  _fnMap.insert("store",&EditableVectorSI::store);
  _fnMap.insert("setValue",&EditableVectorSI::setValue);
  _fnMap.insert("resize",&EditableVectorSI::resize);
  _fnMap.insert("zero",&EditableVectorSI::zero);

  _fnMap.insert("value",&EditableVectorSI::value);
  _fnMap.insert("length",&EditableVectorSI::length);
  _fnMap.insert("min",&EditableVectorSI::min);
  _fnMap.insert("max",&EditableVectorSI::max);
  _fnMap.insert("mean",&EditableVectorSI::mean);
  _fnMap.insert("descriptionTip",&EditableVectorSI::descriptionTip);

}

QString EditableVectorSI::doCommand(QString command_in) {

  if (!_vector) {
    return "invalid";
  }

  QString command = command_in.left(command_in.indexOf('('));

  EditableVectorInterfaceMemberFn fn=_fnMap.value(command,&EditableVectorSI::noSuchFn);

  if(fn!=&EditableVectorSI::noSuchFn) {
    return CALL_MEMBER_FN(*this,fn)(command_in);
  }

  QString v=doNamedObjectCommand(command_in, _vector);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool EditableVectorSI::isValid() {
  return _editablevector.isPtrValid();
}

ScriptInterface* EditableVectorSI::newVector(ObjectStore *store) {
  EditableVectorPtr vector;
  vector = store->createObject<EditableVector>();
  return new EditableVectorSI(vector);
}

QByteArray EditableVectorSI::endEditUpdate() {
  _editablevector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+_editablevector->Name()).toLatin1();
}

QString EditableVectorSI::load(QString & command) {
  QString arg = getArg(command);

  QFile tmpfile(arg);
  tmpfile.open(QIODevice::ReadOnly);
  _editablevector->loadFromTmpFile(tmpfile);
  tmpfile.close();

  return "Done";
}

QString EditableVectorSI::setValue(QString & command) {
  QStringList vars = getArgs(command);

  _editablevector->setValue(vars.at(0).toInt(),
                            vars.at(1).toDouble());

  return "Done";
}

QString EditableVectorSI::resize(QString & command) {
  QStringList vars = getArgs(command);

  _editablevector->resize(vars.at(0).toInt(), false);

  return "Done";
}

QString EditableVectorSI::zero(QString &) {
  _editablevector->zero();

  return "Done";
}

}
