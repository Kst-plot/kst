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

#include "objectstore.h"

#include <QStringBuilder>

namespace Kst {


/***************************/
/* dataobject commands     */
/***************************/
QString DataObjectSI::setInputVector(QString& command) {
  QStringList vars = getArgs(command);

  QString key = vars.at(0);
  QString vec_name = vars.at(1);

  VectorPtr v = kst_cast<Vector>(_dataObject->store()->retrieveObject(vec_name));
  if (v) {
    _dataObject->setInputVector(key, v);
    return "Done";
  } else {
    return QString("Vector %1 not found").arg(vec_name);
  }
}


QString DataObjectSI::setInputScalar(QString& command) {
  QStringList vars = getArgs(command);

  QString key = vars.at(0);
  QString x_name = vars.at(1);

  ScalarPtr x = kst_cast<Scalar>(_dataObject->store()->retrieveObject(x_name));
  if (x) {
    _dataObject->setInputScalar(key, x);
    return "Done";
  } else {
    return QString("Scalar %1 not found").arg(x_name);
  }
}


QString DataObjectSI::outputVector(QString& command) {
  QString key = getArg(command);

  VectorPtr vout = _dataObject->outputVector(key);
  if (vout) {
    return vout->shortName();
  } else {
    return "Invalid";
  }

}


QString DataObjectSI::outputScalar(QString& command) {
  QString key = getArg(command);

  ScalarPtr xout = _dataObject->outputScalar(key);
  if (xout) {
    return xout->shortName();
  } else {
    return "Invalid";
  }

}

/***************************/
/* PluginSI                */
/***************************/
PluginSI::PluginSI(BasicPluginPtr plugin) {
  if (plugin) {
    _plugin = plugin;
    _dataObject = plugin;
  } else {
    _plugin = 0;
    _dataObject = 0;
  }

  _fnMap.insert("setInputVector",&PluginSI::setInputVector);
  _fnMap.insert("setInputScalar",&PluginSI::setInputScalar);
  _fnMap.insert("outputVector",&PluginSI::outputVector);
  _fnMap.insert("outputScalar",&PluginSI::outputScalar);

}

bool PluginSI::isValid() {
  return _plugin;
}

QByteArray PluginSI::endEditUpdate() {
  if (_plugin) {
    _plugin->registerChange();
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();

    return ("Finished editing "%_plugin->Name()).toLatin1();
  } else {
    return ("Finished editing invalid data object");
  }
}

QString PluginSI::doCommand(QString command_in) {

  if (isValid()) {

    QString command = command_in.left(command_in.indexOf('('));

    PluginInterfaceMemberFn fn=_fnMap.value(command,&PluginSI::noSuchFn);

    if(fn!=&PluginSI::noSuchFn) {
      return CALL_MEMBER_FN(*this,fn)(command_in);
    }


    QString v=doNamedObjectCommand(command_in, _plugin);
    if (!v.isEmpty()) {
      return v;
    }

    return "No such command";
  } else {
    return "Invalid";
  }

}

ScriptInterface* PluginSI::newPlugin(ObjectStore *store, QByteArray pluginName) {
  DataObjectConfigWidget* configWidget = DataObject::pluginWidget(pluginName);

  if (configWidget) {
      BasicPluginPtr plugin = kst_cast<BasicPlugin>(DataObject::createPlugin(pluginName, store, configWidget));
      return new PluginSI(kst_cast<BasicPlugin>(plugin));
  }

  return 0L;
}

/***************************/
/* EquationSI              */
/***************************/
EquationSI::EquationSI(EquationPtr equation) {
  if (equation) {
    _equation = equation;
    _dataObject = equation;
  } else {
    _equation = 0;
    _dataObject = 0;
  }

  _fnMap.insert("setEquation",&EquationSI::setEquation);
  _fnMap.insert("equation",&EquationSI::equation);

  _fnMap.insert("setInputVector",&EquationSI::setInputVector);
  _fnMap.insert("setInputScalar",&EquationSI::setInputScalar);
  _fnMap.insert("outputVector",&EquationSI::outputVector);
  _fnMap.insert("outputScalar",&EquationSI::outputScalar);

}

bool EquationSI::isValid() {
  return _equation;
}

QByteArray EquationSI::endEditUpdate() {
  if (_equation) {
    _equation->registerChange();
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();

    return ("Finished editing "%_equation->Name()).toLatin1();
  } else {
    return ("Finished editing invalid equation");
  }
}

QString EquationSI::doCommand(QString command_in) {
  if (isValid()) {

    QString command = command_in.left(command_in.indexOf('('));

    EquationInterfaceMemberFn fn=_fnMap.value(command,&EquationSI::noSuchFn);

    if(fn!=&EquationSI::noSuchFn) {
      return CALL_MEMBER_FN(*this,fn)(command_in);
    }


    QString v=doNamedObjectCommand(command_in, _equation);
    if (!v.isEmpty()) {
      return v;
    }

    return "No such command";
  } else {
    return "Invalid";
  }

}

ScriptInterface* EquationSI::newEquation(ObjectStore *store) {
  EquationPtr equation = store->createObject<Equation>();

  return new EquationSI(equation);
}


QString EquationSI::equation(QString&) {
  if (_equation) {
    return _equation->equation();
  } else {
    return "Invalid";
  }
}


QString EquationSI::setEquation(QString& command) {
  if (_equation) {
    QString eq = getArg(command);

    _equation->setEquation(eq);
    return "done";
  } else {
    return "Invalid";
  }
}


/***************************/
/* SpectrumSI              */
/***************************/
SpectrumSI::SpectrumSI(PSDPtr psd) {
  if (psd) {
    _psd = psd;
    _dataObject = psd;
  } else {
    _psd = 0;
    _dataObject = 0;
  }

  _fnMap.insert("setInputVector",&SpectrumSI::setInputVector);
  _fnMap.insert("setInputScalar",&SpectrumSI::setInputScalar);
  _fnMap.insert("outputVector",&SpectrumSI::outputVector);
  _fnMap.insert("outputScalar",&SpectrumSI::outputScalar);

}

bool SpectrumSI::isValid() {
  return _psd;
}

QByteArray SpectrumSI::endEditUpdate() {
  if (_psd) {
    _psd->registerChange();
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();

    return ("Finished editing "%_psd->Name()).toLatin1();
  } else {
    return ("Finished editing invalid spectrum");
  }
}

QString SpectrumSI::doCommand(QString command_in) {
  if (isValid()) {

    QString command = command_in.left(command_in.indexOf('('));

    SpectrumInterfaceMemberFn fn=_fnMap.value(command,&SpectrumSI::noSuchFn);

    if(fn!=&SpectrumSI::noSuchFn) {
      return CALL_MEMBER_FN(*this,fn)(command_in);
    }


    QString v=doNamedObjectCommand(command_in, _psd);
    if (!v.isEmpty()) {
      return v;
    }

    return "No such command";
  } else {
    return "Invalid";
  }
}

ScriptInterface* SpectrumSI::newSpectrum(ObjectStore *store) {
  PSDPtr psd = store->createObject<PSD>();

  return new SpectrumSI(psd);
}


QString SpectrumSI::change(QString& command) {
  /*
  if (_psd) {
    QStringList vars = getArgs(command);
    
    _psd->change(vector,
                 sampleRate, 
                 interleavedAverage,
                 FFTLength,
                 apodize,
                 removeMean,
                 vectorUnits,
                 rateUnits,
                 apodizeFunction,
                 sigma,
                 output,
                 interpolateOverHoles);
    
    
    return "done";
  } else {
    return "Invalid";
  }
  */
  return "Invalid";
}


}
