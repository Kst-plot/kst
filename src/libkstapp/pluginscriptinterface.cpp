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

#include "pluginscriptinterface.h"

#include <QStringBuilder>

namespace Kst {

PluginSI::PluginSI(BasicPluginPtr plugin) {
  if (plugin) {
    _plugin = plugin;
  } else {
    _plugin = 0;
  }
}

bool PluginSI::isValid() {
  return _plugin;
}

QByteArray PluginSI::endEditUpdate() {
  if (_plugin) {
    return ("Finished editing "%_plugin->Name()).toLatin1();
  } else {
    return ("Finished editing invalid plugin");
  }
}

QString PluginSI::doCommand(QString x) {

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
    }

    return "Done";
  } else {
    return "Invalid";
  }

}

ScriptInterface* newPlugin(ObjectStore *store, QByteArray pluginName) {
  BasicPluginPtr plugin;

}


}
