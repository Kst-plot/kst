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

QString doVectorScriptCommand(QString command,Vector *vector) {

  QString v=ScriptInterface::doNamedObjectCommand(command, vector);
  if (!v.isEmpty()) {
    return v;
  }

  if (command.startsWith("value(")) {
    command.remove("value(").chop(1);
    return QString::number(vector->value(command.toInt()));
  } else if (command.startsWith("length(")) {
    return QString::number(vector->length());
  } else if (command.startsWith("min(")) {
    return QString::number(vector->min());
  } else if (command.startsWith("max(")) {
    return QString::number(vector->max());
  } else if (command.startsWith("mean(")) {
    return QString::number(vector->mean());
  } else if (command.startsWith("descriptionTip(")) {
    return vector->descriptionTip();
  }

  return QString();
}


/******************************************************/
/* Data Vectors                                       */
/******************************************************/
VectorDataSI::VectorDataSI(DataVectorPtr it) {
    vector=it;
}

QString VectorDataSI::doCommand(QString command) {

  QString v=doVectorScriptCommand(command, vector);
  if (!v.isEmpty()) {
    return v;
  }

  if (command.startsWith(QLatin1String("change("))) {
    command.remove("change(").remove(')');
    QStringList p = command.split(',');
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                         vector->store(), p.at(0));
    vector->writeLock();
    vector->change(ds,
                   p.at(1),         // field
                   p.at(2).toInt(), // f0
                   p.at(3).toInt(), // n
                   p.at(4).toInt(), // skip
                   p.at(4).toInt() > 0, // do skip
                   p.at(5) == "True" // do average
                   );
    vector->unlock();
    return "Done";
  } else if (command.startsWith("field(")) {
    return vector->field();
  } else if (command.startsWith("filename(")) {
    return vector->filename();
  } else if (command.startsWith("start(")) {
    return QString::number(vector->startFrame());
  } else if (command.startsWith("NFrames(")) {
    return QString::number(vector->numFrames());
  } else if (command.startsWith("skip(")) {
    return QString::number(vector->skip());
  } else if (command.startsWith("boxcarFirst(")) {
    return vector->doAve()?"True":"False";
  }

  return "No such command";
}

bool VectorDataSI::isValid() {
  return vector.isPtrValid();
}

ScriptInterface* VectorDataSI::newVector(ObjectStore *store) {
  DataVectorPtr vector;
  vector = store->createObject<DataVector>();
  return new VectorDataSI(vector);
}

QByteArray VectorDataSI::endEditUpdate() {
  vector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+vector->Name()).toLatin1();
}

/******************************************************/
/* Generated  Vectors                                 */
/******************************************************/
VectorGenSI::VectorGenSI(GeneratedVectorPtr it) {
    vector=it;
}

QString VectorGenSI::doCommand(QString command) {

  QString v=doVectorScriptCommand(command, vector);
  if (!v.isEmpty()) {
    return v;
  }

  if (command.startsWith(QLatin1String("change("))) {
    command.remove("change(").remove(')');
    QStringList p = command.split(',');

    vector->writeLock();

    vector->changeRange(
                   p.at(0).toDouble(), // start
                   p.at(1).toDouble(), // end
                   p.at(2).toInt() // number of points
                   );
    vector->unlock();
    return "Done";
  }

  return "No such command";
}

bool VectorGenSI::isValid() {
  return vector.isPtrValid();
}

ScriptInterface* VectorGenSI::newVector(ObjectStore *store) {
  GeneratedVectorPtr vector;
  vector = store->createObject<GeneratedVector>();
  return new VectorGenSI(vector);
}

QByteArray VectorGenSI::endEditUpdate() {
  vector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+vector->Name()).toLatin1();
}

/******************************************************/
/* Plain (base) Vectors                               */
/******************************************************/
VectorSI::VectorSI(VectorPtr it) {
    vector=it;
}

QString VectorSI::doCommand(QString command) {

  QString v=doVectorScriptCommand(command, vector);
  if (!v.isEmpty()) {
    return v;
  }

  return "No such command";
}

bool VectorSI::isValid() {
  return vector.isPtrValid();
}

ScriptInterface* VectorSI::newVector(ObjectStore *) {
  return 0L;
}

QByteArray VectorSI::endEditUpdate() {
  vector->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+vector->Name()).toLatin1();
}


}
