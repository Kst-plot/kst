/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "fitsinusoid_unweighted.h"
#include "objectstore.h"
#include "ui_fitsinusoid_unweightedconfig.h"

#include <gsl/gsl_fit.h>
// #include "../common.h"
#include "../linear.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_IN_PERIOD = "Period Scalar";
static const QString& SCALAR_IN_HARMONICS = "Harmonics Scalar";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitSinusoidUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitSinusoid_UnweightedConfig {
  public:
    ConfigWidgetFitSinusoidUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitSinusoid_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitSinusoidUnweightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _scalarHarmonics->setObjectStore(store);
      _scalarPeriod->setObjectStore(store);
      _scalarHarmonics->setDefaultValue(0);
      _scalarPeriod->setDefaultValue(1);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarHarmonics, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarPeriod, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVectorX(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      setSelectedVectorY(vector);
    }

    void setVectorsLocked(bool locked = true) {
      _vectorX->setEnabled(!locked);
      _vectorY->setEnabled(!locked);
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarHarmonics() { return _scalarHarmonics->selectedScalar(); };
    void setSelectedScalarHarmonics(Kst::ScalarPtr scalar) { return _scalarHarmonics->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarPeriod() { return _scalarPeriod->selectedScalar(); };
    void setSelectedScalarPeriod(Kst::ScalarPtr scalar) { return _scalarPeriod->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitSinusoidUnweightedSource* source = static_cast<FitSinusoidUnweightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedScalarHarmonics(source->scalarHarmonics());
        setSelectedScalarPeriod(source->scalarPeriod());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

//       QStringRef av;
//       av = attrs.value("value");
//       if (!av.isNull()) {
//         _configValue = QVariant(av.toString()).toBool();
//       }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Fit Sinusoid Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Scalar Harmonics", _scalarHarmonics->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Period", _scalarPeriod->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Sinusoid Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorx = static_cast<Kst::Vector*>(object);
        if (vectorx) {
          setSelectedVectorX(vectorx);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectory = static_cast<Kst::Vector*>(object);
        if (vectory) {
          setSelectedVectorX(vectory);
        }
        QString scalarName = _cfg->value("Harmonics Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* harmonicsScalar = static_cast<Kst::Scalar*>(object);
        if (harmonicsScalar) {
          setSelectedScalarHarmonics(harmonicsScalar);
        }
        scalarName = _cfg->value("Period Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* periodScalar = static_cast<Kst::Scalar*>(object);
        if (periodScalar) {
          setSelectedScalarPeriod(periodScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitSinusoidUnweightedSource::FitSinusoidUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitSinusoidUnweightedSource::~FitSinusoidUnweightedSource() {
}


QString FitSinusoidUnweightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Sinusoid");
}


void FitSinusoidUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitSinusoidUnweightedPlugin* config = static_cast<ConfigWidgetFitSinusoidUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputScalar(SCALAR_IN_HARMONICS, config->selectedScalarHarmonics());
    setInputScalar(SCALAR_IN_PERIOD, config->selectedScalarPeriod());
  }
}


void FitSinusoidUnweightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}


double g_dPeriod;
double calculate_matrix_entry( double dX, int iPos ) {
  double dY;

  if( iPos == 0 ) {
    dY = 1.0;
  } else if( iPos % 2 == 1 ) {
    dY =  cos( (double)( ( iPos + 1 ) / 2 ) * 2.0 * M_PI * dX / g_dPeriod );
  } else {
    dY = -sin( (double)( ( iPos + 0 ) / 2 ) * 2.0 * M_PI * dX / g_dPeriod );
  }

  return dY;
}


bool FitSinusoidUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::ScalarPtr inputScalarHarmonics = _inputScalars[SCALAR_IN_HARMONICS];
  Kst::ScalarPtr inputScalarPeriod = _inputScalars[SCALAR_IN_PERIOD];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = i18n("Sinusoidal Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = i18n("Sinusoidal Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);

  bool bReturn = false;

  int iNumParams;

  iNumParams = 3 + ( 2 * (int)floor( inputScalarHarmonics->value() ) );
  if( iNumParams >= 3 ) {
    g_dPeriod = inputScalarPeriod->value();
    if( g_dPeriod > 0.0 )
    {
      bReturn = kstfit_linear_unweighted( inputVectorX, inputVectorY,
                                        outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                                        outputVectorYCovariance, outputScalar, iNumParams );
    }
  }

  return bReturn;
}


Kst::VectorPtr FitSinusoidUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitSinusoidUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::ScalarPtr FitSinusoidUnweightedSource::scalarHarmonics() const {
  return _inputScalars[SCALAR_IN_HARMONICS];
}


Kst::ScalarPtr FitSinusoidUnweightedSource::scalarPeriod() const {
  return _inputScalars[SCALAR_IN_PERIOD];
}


QStringList FitSinusoidUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitSinusoidUnweightedSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_HARMONICS);
  scalars += SCALAR_IN_PERIOD;
  return scalars;
}


QStringList FitSinusoidUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitSinusoidUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitSinusoidUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitSinusoidUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitSinusoidUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitSinusoidUnweightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Mean";
      break;
    case 1:
      parameter = "cos(%1 2PI x/P)";
      parameter.arg((index + 1 ) / 2);
      break;
    case 2:
      parameter = "-sin(%1 2PI x/P)";
      parameter.arg((index + 0 ) / 2);
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitSinusoidUnweightedPlugin::pluginName() const { return "Sinusoid Fit"; }
QString FitSinusoidUnweightedPlugin::pluginDescription() const { return "Generates a sinusoid fit for a set of data."; }


Kst::DataObject *FitSinusoidUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitSinusoidUnweightedPlugin* config = static_cast<ConfigWidgetFitSinusoidUnweightedPlugin*>(configWidget)) {

    Kst::ScalarPtr harmonics;
    Kst::ScalarPtr period;

    if (setupInputsOutputs) {
      harmonics = config->selectedScalarHarmonics();
      period = config->selectedScalarPeriod();
    }

    FitSinusoidUnweightedSource* object = store->createObject<FitSinusoidUnweightedSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputScalar(SCALAR_IN_HARMONICS, harmonics);
      object->setInputScalar(SCALAR_IN_PERIOD, period);
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitSinusoidUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitSinusoidUnweightedPlugin *widget = new ConfigWidgetFitSinusoidUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitSinusoidUnweightedPlugin, FitSinusoidUnweightedPlugin)

// vim: ts=2 sw=2 et
