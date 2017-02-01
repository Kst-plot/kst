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

  _fnMap.insert("setProperty",&PluginSI::setProperty);

}


QString PluginSI::setProperty(QString& command) {
  QStringList vars = getArgs(command);

  QString key = vars.at(0);
  QString val = vars.at(1);

  if (_plugin) {
    _plugin->setProperty(key, val);
  }

  return "Done";
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


    QString v=doObjectCommand(command_in, _plugin);
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


    QString v=doObjectCommand(command_in, _equation);
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

  _fnMap.insert("change",&SpectrumSI::change);
  _fnMap.insert("sampleRate",&SpectrumSI::sampleRate);

  _fnMap.insert("interleavedAverage",&SpectrumSI::interleavedAverage);
  _fnMap.insert("fftLength",&SpectrumSI::fftLength);
  _fnMap.insert("apodize",&SpectrumSI::apodize);
  _fnMap.insert("removeMean",&SpectrumSI::removeMean);
  _fnMap.insert("vectorUnits",&SpectrumSI::vectorUnints);
  _fnMap.insert("rateUnits",&SpectrumSI::rateUnits);
  _fnMap.insert("apodizeFunctionIndex",&SpectrumSI::apodizeFunctionIndex);
  _fnMap.insert("gaussianSigma",&SpectrumSI::gaussianSigma);
  _fnMap.insert("outputTypeIndex",&SpectrumSI::outputTypeIndex);

  _fnMap.insert("setInputVector",&SpectrumSI::setInputVector);
  _fnMap.insert("outputVector",&SpectrumSI::outputVector);

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


    QString v=doObjectCommand(command_in, _psd);
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
  if (_psd) {
    QStringList vars = getArgs(command);

    QString vec_name = vars.at(0);
    VectorPtr vector = kst_cast<Vector>(_dataObject->store()->retrieveObject(vec_name));


    _psd->change(vector,
                 vars.at(1).toDouble(),            // sample_rate
                 (vars.at(2).toLower() == "true"), // interleaved_average,
                 vars.at(3).toInt(),               // fft_length,
                 (vars.at(4).toLower() == "true"), // apodize,
                 (vars.at(5).toLower() == "true"), // remove_mean,
                 vars.at(6),                       // vector unints
                 vars.at(7),                       // rate units
                 ApodizeFunction(vars.at(8).toInt()), // apodizeFunction,
                 vars.at(9).toDouble(),            // sigma,
                 PSDType(vars.at(10).toInt())     // output type
                 );
    
    
    return "done";
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::sampleRate(QString&) {
  if (_psd) {
    return QString::number(_psd->frequency());
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::interleavedAverage(QString &) {
  if (_psd) {
    if (_psd->average()) {
      return "True";
    } else {
      return "False";
    }
  } else {
    return "Invalid";
  }
}


QString SpectrumSI::fftLength(QString &) {
  if (_psd) {
    return QString::number(_psd->frequency());
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::apodize(QString &) {
  if (_psd) {
    if (_psd->apodize()) {
      return "True";
    } else {
      return "False";
    }
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::removeMean(QString &) {
  if (_psd) {
    if (_psd->removeMean()) {
      return "True";
    } else {
      return "False";
    }
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::vectorUnints(QString &) {
  if (_psd) {
    return _psd->vectorUnits();
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::rateUnits(QString &) {
  if (_psd) {
    return _psd->rateUnits();
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::apodizeFunctionIndex(QString &) {
  if (_psd) {
    return QString::number(int(_psd->apodizeFxn()));
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::gaussianSigma(QString &) {
  if (_psd) {
    return QString::number(_psd->gaussianSigma());
  } else {
    return "Invalid";
  }
}

QString SpectrumSI::outputTypeIndex(QString &) {
 if (_psd) {
   return QString::number(int(_psd->output()));
 } else {
   return "Invalid";
 }
}

/***************************/
/* HistogramSI             */
/***************************/
HistogramSI::HistogramSI(HistogramPtr histogram) {
  if (histogram) {
    _histogram = histogram;
    _dataObject = histogram;
  } else {
    _histogram = 0;
    _dataObject = 0;
  }

  _fnMap.insert("change",&HistogramSI::change);

  _fnMap.insert("xMin",&HistogramSI::xMin);
  _fnMap.insert("xMax",&HistogramSI::xMax);
  _fnMap.insert("nBins",&HistogramSI::nBins);
  _fnMap.insert("normalizationType",&HistogramSI::normalizationType);
  _fnMap.insert("autoBin",&HistogramSI::autoBin);

  _fnMap.insert("setInputVector",&HistogramSI::setInputVector);
  _fnMap.insert("outputVector",&HistogramSI::outputVector);
}

bool HistogramSI::isValid() {
  return _histogram;
}

QByteArray HistogramSI::endEditUpdate() {
  if (_histogram) {
    _histogram->registerChange();
    UpdateManager::self()->doUpdates(true);
    UpdateServer::self()->requestUpdateSignal();

    return ("Finished editing "%_histogram->Name()).toLatin1();
  } else {
    return ("Finished editing invalid histogram");
  }
}

QString HistogramSI::doCommand(QString command_in) {
  if (isValid()) {

    QString command = command_in.left(command_in.indexOf('('));

    HistogramInterfaceMemberFn fn=_fnMap.value(command,&HistogramSI::noSuchFn);

    if(fn!=&HistogramSI::noSuchFn) {
      return CALL_MEMBER_FN(*this,fn)(command_in);
    }

    QString v=doObjectCommand(command_in, _histogram);
    if (!v.isEmpty()) {
      return v;
    }

    return "No such command";
  } else {
    return "Invalid";
  }
}

ScriptInterface* HistogramSI::newHistogram(ObjectStore *store) {
  HistogramPtr histogram = store->createObject<Histogram>();

  return new HistogramSI(histogram);
}

QString HistogramSI::change(QString& command) {
  if (_histogram) {
    QStringList vars = getArgs(command);

    QString vec_name = vars.at(0);
    VectorPtr vector = kst_cast<Vector>(_dataObject->store()->retrieveObject(vec_name));


    _histogram->change(vector,
                       vars.at(1).toDouble(),            // xmin
                       vars.at(2).toDouble(),            // xmax
                       vars.at(3).toInt(),               // nbins
                       Histogram::NormalizationType(vars.at(4).toInt()), // normalization type
                       (vars.at(5).toLower() == "true") // real time autobin
                       );

    return "done";
  } else {
    return "Invalid";
  }
}

QString HistogramSI::xMin(QString &) {
  if (_histogram) {
    return QString::number(double(_histogram->xMin()));
  } else {
    return "Invalid";
  }
}

QString HistogramSI::xMax(QString &) {
  if (_histogram) {
    return QString::number(double(_histogram->xMax()));
  } else {
    return "Invalid";
  }
}

QString HistogramSI::nBins(QString &) {
  if (_histogram) {
    return QString::number(int(_histogram->numberOfBins()));
  } else {
    return "Invalid";
  }
}

QString HistogramSI::normalizationType(QString &) {
  if (_histogram) {
    return QString::number(int(_histogram->normalizationType()));
  } else {
    return "Invalid";
  }
}

QString HistogramSI::autoBin(QString &) {
  if (_histogram) {
    if (_histogram->realTimeAutoBin()) {
      return "True";
    } else {
      return "False";
    }
  } else {
    return "Invalid";
  }
}

}
