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

#include "scalarscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QStringBuilder>


namespace Kst {

/******************************************************/
/* Generated Scalars                                  */
/******************************************************/
ScalarGenSI::ScalarGenSI(ScalarPtr it) {
    scalar=it;
}

QString ScalarGenSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, scalar);
  if (!v.isEmpty()) {
    return v;
  }

  if (x.startsWith(QLatin1String("setValue("))) {
    scalar->writeLock();
    scalar->setValue(x.remove("setValue(").remove(')').toDouble());
    scalar->unlock();
    return "Done";
  } else if (x.startsWith(QLatin1String("value()"))) {
    return QString::number(scalar->value());
  }
  return "No such command";
}

bool ScalarGenSI::isValid() {
  return scalar.isPtrValid();
}

ScriptInterface* ScalarGenSI::newScalar(ObjectStore *store) {
  ScalarPtr scalar;
  scalar = store->createObject<Scalar>();
  scalar->setOrphan(true);
  scalar->setEditable(true);
  return new ScalarGenSI(scalar);
}

QByteArray ScalarGenSI::endEditUpdate() {
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+scalar->Name()).toLatin1();
}

/******************************************************/
/* Data Scalars                                       */
/******************************************************/
ScalarDataSI::ScalarDataSI(DataScalarPtr it) {
    scalar=it;
}

QString ScalarDataSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, scalar);
  if (!v.isEmpty()) {
    return v;
  }

  if (x.startsWith(QLatin1String("change("))) {
    x.remove("change(").remove(')');
    QStringList p = x.split(',');
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                         scalar->store(), p.at(0));
    scalar->writeLock();
    scalar->change(ds,p.at(1));
    scalar->unlock();
    return "Done";
  } else if (x.startsWith(QLatin1String("file()"))) {
    return scalar->filename();
  } else if (x.startsWith(QLatin1String("field()"))) {
    return scalar->field();
  } else if (x.startsWith(QLatin1String("value()"))) {
    return QString::number(scalar->value());
  }
  return "No such command";
}

bool ScalarDataSI::isValid() {
  return scalar.isPtrValid();
}

ScriptInterface* ScalarDataSI::newScalar(ObjectStore *store) {
  DataScalarPtr scalar;
  scalar = store->createObject<DataScalar>();
  return new ScalarDataSI(scalar);
}

QByteArray ScalarDataSI::endEditUpdate() {
  scalar->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+scalar->Name()).toLatin1();
}

/******************************************************/
/* Vector Scalars                                       */
/******************************************************/
ScalarVectorSI::ScalarVectorSI(VScalarPtr it) {
    scalar=it;
}

QString ScalarVectorSI::doCommand(QString x) {

  QString v=doNamedObjectCommand(x, scalar);
  if (!v.isEmpty()) {
    return v;
  }

  if (x.startsWith(QLatin1String("change("))) {
    x.remove("change(").remove(')');
    QStringList p = x.split(',');
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                         scalar->store(), p.at(0));
    scalar->writeLock();
    scalar->change(ds,p.at(1), p.at(2).toInt());
    scalar->unlock();
    return "Done";
  } else if (x.startsWith(QLatin1String("value()"))) {
    return QString::number(scalar->value());
  } else if (x.startsWith(QLatin1String("file()"))) {
    return scalar->filename();
  } else if (x.startsWith(QLatin1String("field()"))) {
    return scalar->field();
  } else if (x.startsWith(QLatin1String("frame()"))) {
    return QString::number(scalar->F0());
  }
  return "No such command";
}

bool ScalarVectorSI::isValid() {
  return scalar.isPtrValid();
}

ScriptInterface* ScalarVectorSI::newScalar(ObjectStore *store) {
  VScalarPtr scalar;
  scalar = store->createObject<VScalar>();
  return new ScalarVectorSI(scalar);
}

QByteArray ScalarVectorSI::endEditUpdate() {
  scalar->registerChange();
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+scalar->Name()).toLatin1();
}

}
