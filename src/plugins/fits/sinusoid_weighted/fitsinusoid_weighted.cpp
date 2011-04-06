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


#include "fitsinusoid_weighted.h"
#include "objectstore.h"
#include "ui_fitsinusoid_weightedconfig.h"

#include <gsl/gsl_fit.h>
// #include "../common.h"
#include "../linear_weighted.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_IN_WEIGHTS = "Weights Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_IN_PERIOD = "Period Scalar";
static const QString& SCALAR_IN_HARMONICS = "Harmonics Scalar";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitSinusoidWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitSinusoid_WeightedConfig {
  public:
    ConfigWidgetFitSinusoidWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitSinusoid_WeightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitSinusoidWeightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _vectorWeights->setObjectStore(store);
      _scalarHarmonics->setObjectStore(store);
      _scalarPeriod->setObjectStore(store);
      _scalarHarmonics->setDefaultValue(0);
      _scalarPeriod->setDefaultValue(1);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorWeights, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::VectorPtr selectedVectorWeights() { return _vectorWeights->selectedVector(); };
    void setSelectedVectorWeights(Kst::VectorPtr vector) { return _vectorWeights->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarHarmonics() { return _scalarHarmonics->selectedScalar(); };
    void setSelectedScalarHarmonics(Kst::ScalarPtr scalar) { return _scalarHarmonics->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarPeriod() { return _scalarPeriod->selectedScalar(); };
    void setSelectedScalarPeriod(Kst::ScalarPtr scalar) { return _scalarPeriod->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitSinusoidWeightedSource* source = static_cast<FitSinusoidWeightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorWeights(source->vectorWeights());
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
        _cfg->beginGroup("Fit Sinusoid Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->setValue("Input Scalar Harmonics", _scalarHarmonics->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Period", _scalarPeriod->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Sinusoid Weighted Plugin");
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
        vectorName = _cfg->value("Input Vector Weights").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorweights = static_cast<Kst::Vector*>(object);
        if (vectorweights) {
          setSelectedVectorX(vectorweights);
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


FitSinusoidWeightedSource::FitSinusoidWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitSinusoidWeightedSource::~FitSinusoidWeightedSource() {
}


QString FitSinusoidWeightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Weighted Sinusoid");
}


void FitSinusoidWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitSinusoidWeightedPlugin* config = static_cast<ConfigWidgetFitSinusoidWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
    setInputScalar(SCALAR_IN_HARMONICS, config->selectedScalarHarmonics());
    setInputScalar(SCALAR_IN_PERIOD, config->selectedScalarPeriod());
  }
}


void FitSinusoidWeightedSource::setupOutputs() {
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


bool FitSinusoidWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];
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
      bReturn = kstfit_linear_weighted( inputVectorX, inputVectorY, inputVectorWeights,
                                        outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                                        outputVectorYCovariance, outputScalar, iNumParams );
    }
  }

  return bReturn;
}


Kst::VectorPtr FitSinusoidWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitSinusoidWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitSinusoidWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}


Kst::ScalarPtr FitSinusoidWeightedSource::scalarHarmonics() const {
  return _inputScalars[SCALAR_IN_HARMONICS];
}


Kst::ScalarPtr FitSinusoidWeightedSource::scalarPeriod() const {
  return _inputScalars[SCALAR_IN_PERIOD];
}


QStringList FitSinusoidWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitSinusoidWeightedSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_HARMONICS);
  scalars += SCALAR_IN_PERIOD;
  return scalars;
}


QStringList FitSinusoidWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitSinusoidWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitSinusoidWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitSinusoidWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitSinusoidWeightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitSinusoidWeightedSource::parameterName(int index) const {
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
QString FitSinusoidWeightedPlugin::pluginName() const { return "Sinusoid Weighted Fit"; }
QString FitSinusoidWeightedPlugin::pluginDescription() const { return "Generates a sinusoid weighted fit for a set of data."; }


Kst::DataObject *FitSinusoidWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitSinusoidWeightedPlugin* config = static_cast<ConfigWidgetFitSinusoidWeightedPlugin*>(configWidget)) {

    Kst::ScalarPtr harmonics;
    Kst::ScalarPtr period;

    // access/create input scalars before creating plugin
    // in order to preserve continuous scalar shortnames
    if (setupInputsOutputs) {
      harmonics = config->selectedScalarHarmonics();
      period = config->selectedScalarPeriod();
    }

    FitSinusoidWeightedSource* object = store->createObject<FitSinusoidWeightedSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
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


Kst::DataObjectConfigWidget *FitSinusoidWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitSinusoidWeightedPlugin *widget = new ConfigWidgetFitSinusoidWeightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitSinusoidWeightedPlugin, FitSinusoidWeightedPlugin)

// vim: ts=2 sw=2 et
