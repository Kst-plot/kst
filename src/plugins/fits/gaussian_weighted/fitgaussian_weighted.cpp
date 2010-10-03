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


#include "fitgaussian_weighted.h"
#include "objectstore.h"
#include "ui_fitgaussian_weightedconfig.h"

#define NUM_PARAMS 3
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear_weighted.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_IN_WEIGHTS = "Weights Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitGaussianWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitGaussian_WeightedConfig {
  public:
    ConfigWidgetFitGaussianWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitGaussian_WeightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitGaussianWeightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _vectorWeights->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorWeights, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitGaussianWeightedSource* source = static_cast<FitGaussianWeightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorWeights(source->vectorWeights());
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
        _cfg->beginGroup("Fit Gaussian Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Gaussian Weighted Plugin");
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
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitGaussianWeightedSource::FitGaussianWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitGaussianWeightedSource::~FitGaussianWeightedSource() {
}


QString FitGaussianWeightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Weighted Gaussian");
}


void FitGaussianWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitGaussianWeightedPlugin* config = static_cast<ConfigWidgetFitGaussianWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
  }
}


void FitGaussianWeightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}


void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates ) {
  double dMin;
  double dMax;

  gsl_stats_minmax( &dMin, &dMax, pdX, 1, iLength );

  pdParameterEstimates[0] = gsl_stats_mean( pdX, 1, iLength );
  pdParameterEstimates[1] = ( dMax - dMin ) / 2.0;
  pdParameterEstimates[2] = gsl_stats_max( pdY, 1, iLength );
}


double function_calculate( double dX, double* pdParameters ) {
  double dMean  = pdParameters[0];
  double dSD    = pdParameters[1];
  double dScale = pdParameters[2];
  double dY;

  dY  = ( dScale / ( dSD * M_SQRT2 * M_SQRTPI ) );
  dY *= exp( -( ( dX - dMean ) * ( dX - dMean ) ) / ( 2.0 * dSD * dSD ) );

  return dY;
}


void function_derivative( double dX, double* pdParameters, double* pdDerivatives ) {
  double dMean  = pdParameters[0];
  double dSD    = pdParameters[1];
  double dScale = pdParameters[2];
  double dExp;  
  double ddMean;
  double ddSD;
  double ddScale;

  dExp    = exp( -( ( dX - dMean ) * ( dX - dMean ) ) / ( 2.0 * dSD * dSD ) );
  ddMean  = ( dX - dMean ) * dScale * dExp / ( dSD * dSD * dSD * M_SQRT2 * M_SQRTPI );
  ddSD    = dScale * dExp / ( dSD * dSD * M_SQRT2 * M_SQRTPI );
  ddSD   *= ( ( dX - dMean ) * ( dX - dMean ) / ( dSD * dSD ) ) - 1.0;
  ddScale = dExp / ( dSD * M_SQRT2 * M_SQRTPI );

  pdDerivatives[0] = ddMean;
  pdDerivatives[1] = ddSD;
  pdDerivatives[2] = ddScale;
}


bool FitGaussianWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  bool bReturn = false;

  bReturn = kstfit_nonlinear_weighted( inputVectorX, inputVectorY, inputVectorWeights,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );
  return bReturn;
}


Kst::VectorPtr FitGaussianWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitGaussianWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitGaussianWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}


QStringList FitGaussianWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitGaussianWeightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitGaussianWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitGaussianWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitGaussianWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitGaussianWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitGaussianWeightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitGaussianWeightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Mean";
      break;
    case 1:
      parameter = "SD";
      break;
    case 2:
      parameter = "Scale";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitGaussianWeightedPlugin::pluginName() const { return "Gaussian Weighted Fit"; }
QString FitGaussianWeightedPlugin::pluginDescription() const { return "Generates a gaussian weighted fit for a set of data."; }


Kst::DataObject *FitGaussianWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitGaussianWeightedPlugin* config = static_cast<ConfigWidgetFitGaussianWeightedPlugin*>(configWidget)) {

    FitGaussianWeightedSource* object = store->createObject<FitGaussianWeightedSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
      object->setupOutputs();
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitGaussianWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitGaussianWeightedPlugin *widget = new ConfigWidgetFitGaussianWeightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitGaussianWeightedPlugin, FitGaussianWeightedPlugin)

// vim: ts=2 sw=2 et
