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

PluginSI::PluginSI(BasicPluginPtr plugin, ObjectStore *store) {
  if (plugin && store) {
    _plugin = plugin;
    _store = store;
  } else {
    _plugin = 0;
    _store = 0;
  }
}

QByteArrayList PluginSI::commands() {

  QByteArrayList ba;
  ba<< "setInputVector()" << "setInputScalar()" << "setInputMatrix()" <<
      "inputVector()" << "inputScalar()" << "inputMatrix()";

  return ba;
}

bool PluginSI::isValid() {
  return _plugin;
}

QByteArray PluginSI::getHandle() {
  if (_plugin) {
    return ("Finished editing "%_plugin->Name()).toLatin1();
  } else {
    return ("Finished editing invalid plugin");
  }
}

QString PluginSI::doCommand(QString x) {

  if (isValid()) {
    QStringList params;
    if (x.startsWith("setInputVector(")) {
      x.remove("setInputVector(");
      x.remove(x.lastIndexOf(")"),1);
      params = x.split(',');
      if (params.size()==2) {
        VectorPtr V = kst_cast<Vector>(_store->retrieveObject(params[1]));
        if (V) {
          _plugin->setInputVector(params[0], V);
        }
      }
    } else if (x.startsWith("setInputScalar(")) {
      x.remove("setInputScalar(");
      x.remove(x.lastIndexOf(")"),1);
      params = x.split(',');
      if (params.size()==2) {
        ScalarPtr S = kst_cast<Scalar>(_store->retrieveObject(params[1]));
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


}
