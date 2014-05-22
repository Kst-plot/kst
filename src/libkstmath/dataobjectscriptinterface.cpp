/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012 Barth Netterfield                                *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dataobjectscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

DataObjectSI::DataObjectSI(DataObjectPtr plugin) {
  if (plugin) {
    _plugin = plugin;
  } else {
    _plugin = 0;
  }
}

bool DataObjectSI::isValid() {
  return _plugin;
}

QByteArray DataObjectSI::endEditUpdate() {
  if (_plugin) {
    _plugin->registerChange();
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();

    return ("Finished editing "%_plugin->Name()).toLatin1();
  } else {
    return ("Finished editing invalid plugin");
  }
}

QString DataObjectSI::doCommand(QString x) {

  if (isValid()) {

    QString v=doNamedObjectCommand(x, _plugin);
    if (!v.isEmpty()) {
      return v;
    }
    QStringList params;
    if (x.startsWith("setInputVector(")) {
      x.remove("setInputVector(");
      x.remove(x.lastIndexOf(")"),1);
      params = x.split(',');
      if (params.size()==2) {
        VectorPtr V = kst_cast<Vector>(_plugin->store()->retrieveObject(params[1]));
        if (V) {
          _plugin->setInputVector(params[0], V);
        }
      }
    } else if (x.startsWith("setInputScalar(")) {
      x.remove("setInputScalar(");
      x.remove(x.lastIndexOf(")"),1);
      params = x.split(',');
      if (params.size()==2) {
        ScalarPtr S = kst_cast<Scalar>(_plugin->store()->retrieveObject(params[1]));
        if (S) {
          _plugin->setInputScalar(params[0], S);
        }
      }
    } else if (x.startsWith("outputVector(")) {
      x.remove("outputVector(");
      x.remove(x.lastIndexOf(")"),1);
      VectorPtr vout = _plugin->outputVector(x);
      if (vout) {
        return vout->shortName();
      } else {
        return "Invalid";
      }
    } else if (x.startsWith("outputScalar(")) {
      x.remove("outputScalar(");
      x.remove(x.lastIndexOf(")"),1);
      ScalarPtr xout = _plugin->outputScalar(x);
      if (xout) {
        return xout->shortName();
      } else {
        return "Invalid";
      }
    }
    return "Done";
  } else {
    return "Invalid";
  }

}

ScriptInterface* DataObjectSI::newPlugin(ObjectStore *store, QByteArray pluginName) {
  DataObjectConfigWidget* configWidget = DataObject::pluginWidget(pluginName);

  if (configWidget) {
      BasicPluginPtr plugin = kst_cast<BasicPlugin>(DataObject::createPlugin(pluginName, store, configWidget));
      return new DataObjectSI(kst_cast<DataObject>(plugin));
  }

  return 0L;
}


}
