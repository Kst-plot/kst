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

#include "stringscriptinterface.h"

#include "objectstore.h"
#include "datasourcepluginmanager.h"
#include "updatemanager.h"
#include "updateserver.h"

#include <QStringBuilder>

namespace Kst {

StringGenSI::StringGenSI(StringPtr it) {
    str=it;
}

QString StringGenSI::doCommand(QString x) {

  QString v=doObjectCommand(x, str);
  if (!v.isEmpty()) {
    return v;
  }

  if (x.startsWith(QLatin1String("setValue("))) {
    str->writeLock();
    str->setValue(x.remove("setValue(").remove(')'));
    str->unlock();
    return "Done";
  } else if (x.startsWith(QLatin1String("value()"))) {
    return str->value();
  }
  return "No such command";
}

bool StringGenSI::isValid() {
  return str.isPtrValid();
}

ScriptInterface* StringGenSI::newString(ObjectStore *store) {
  StringPtr string;
  string = store->createObject<String>();
  string->setOrphan(true);
  string->setEditable(true);
  return new StringGenSI(string);
}

QByteArray StringGenSI::endEditUpdate() {
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+str->Name()).toLatin1();
}

/////////////////////////////////////////////////////////////////////////////////////

StringDataSI::StringDataSI(DataStringPtr it) {
    str=it;
}

QString StringDataSI::doCommand(QString x) {

  QString v=doObjectCommand(x, str);
  if (!v.isEmpty()) {
    return v;
  }

  if(x.startsWith(QLatin1String("change("))) {
    x.remove("change(").remove(')');
    QStringList p = x.split(',');
    DataSourcePtr ds = DataSourcePluginManager::findOrLoadSource(
                         str->store(), p.at(0));
    str->writeLock();
    str->change(ds,p.at(1));
    str->unlock();
    return "Done";
  } else if (x.startsWith(QLatin1String("value()"))) {
    return str->value();
  }
  return "No such command";
}

bool StringDataSI::isValid() {
    return str.isPtrValid();
}

ScriptInterface* StringDataSI::newString(ObjectStore *store) {
  DataStringPtr string;
  string = store->createObject<DataString>();

  return new StringDataSI(string);
}

QByteArray StringDataSI::endEditUpdate() {
  UpdateManager::self()->doUpdates(true);
  UpdateServer::self()->requestUpdateSignal();
  return ("Finished editing "+str->Name()).toLatin1();
}

}
