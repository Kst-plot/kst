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


#include "fitgaussian_unweighted.h"
#include "objectstore.h"
#include "ui_fitgaussian_unweightedconfig.h"

#define NUM_PARAMS 3
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitGaussianUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitGaussian_UnweightedConfig {
  public:
    ConfigWidgetFitGaussianUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitGaussian_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitGaussianUnweightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitGaussianUnweightedSource* source = static_cast<FitGaussianUnweightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
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
        _cfg->beginGroup("Fit Gaussian Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Gaussian Plugin");
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
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitGaussianUnweightedSource::FitGaussianUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitGaussianUnweightedSource::~FitGaussianUnweightedSource() {
}


QString FitGaussianUnweightedSource::_automaticDescriptiveName() const {
    return vectorY()->descriptiveName() + i18n(" Unweighted Gaussian");
}


void FitGaussianUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitGaussianUnweightedPlugin* config = static_cast<ConfigWidgetFitGaussianUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitGaussianUnweightedSource::setupOutputs() {
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


bool FitGaussianUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];


  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = i18n("Gaussian Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = i18n("Gaussian Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);

  bool bReturn = false;

  bReturn = kstfit_nonlinear( inputVectorX, inputVectorY,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );
  return bReturn;
}


Kst::VectorPtr FitGaussianUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitGaussianUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitGaussianUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitGaussianUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitGaussianUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitGaussianUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitGaussianUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitGaussianUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitGaussianUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitGaussianUnweightedSource::parameterName(int index) const {
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
QString FitGaussianUnweightedPlugin::pluginName() const { return "Gaussian Fit"; }
QString FitGaussianUnweightedPlugin::pluginDescription() const { return "Generates a gaussian fit for a set of data."; }


Kst::DataObject *FitGaussianUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitGaussianUnweightedPlugin* config = static_cast<ConfigWidgetFitGaussianUnweightedPlugin*>(configWidget)) {

    FitGaussianUnweightedSource* object = store->createObject<FitGaussianUnweightedSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
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


Kst::DataObjectConfigWidget *FitGaussianUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitGaussianUnweightedPlugin *widget = new ConfigWidgetFitGaussianUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitGaussianUnweightedPlugin, FitGaussianUnweightedPlugin)

// vim: ts=2 sw=2 et
