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


#include "fitexponential_unweighted.h"
#include "objectstore.h"
#include "ui_fitexponential_unweightedconfig.h"

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

class ConfigWidgetFitExponentialUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitExponential_UnweightedConfig {
  public:
    ConfigWidgetFitExponentialUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitExponential_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitExponentialUnweightedPlugin() {}

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
      if (FitExponentialUnweightedSource* source = static_cast<FitExponentialUnweightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Exponential Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Exponential Plugin");
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


FitExponentialUnweightedSource::FitExponentialUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitExponentialUnweightedSource::~FitExponentialUnweightedSource() {
}


QString FitExponentialUnweightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Unweighted Exponential");
}


void FitExponentialUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitExponentialUnweightedPlugin* config = static_cast<ConfigWidgetFitExponentialUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitExponentialUnweightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}


void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates ) {  
  Q_UNUSED( pdX )
  Q_UNUSED( pdY )
  Q_UNUSED( iLength )

  pdParameterEstimates[0] =  1.0;
  pdParameterEstimates[1] =  0.0;
  pdParameterEstimates[2] =  0.0;
}


double function_calculate( double dX, double* pdParameters ) {
  double dScale  = pdParameters[0];
  double dLambda = pdParameters[1];
  double dOffset = pdParameters[2];
  double dY;

  dY  = ( dScale * exp( -dLambda * dX ) ) + dOffset;

  return dY;
}


void function_derivative( double dX, double* pdParameters, double* pdDerivatives ) {
  double dScale  = pdParameters[0];
  double dLambda = pdParameters[1];
  double dExp;  
  double ddScale;
  double ddLambda;
  double ddOffset;

  dExp     = exp( -dLambda * dX );
  ddScale  = dExp;
  ddLambda = -dX * dScale * dExp;
  ddOffset = 1.0;

  pdDerivatives[0] = ddScale;
  pdDerivatives[1] = ddLambda;
  pdDerivatives[2] = ddOffset;
}


bool FitExponentialUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  bool bReturn = false;

  bReturn = kstfit_nonlinear( inputVectorX, inputVectorY,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );
  return bReturn;
}


Kst::VectorPtr FitExponentialUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitExponentialUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitExponentialUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitExponentialUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitExponentialUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitExponentialUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitExponentialUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitExponentialUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitExponentialUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitExponentialUnweightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Scale";
      break;
    case 1:
      parameter = "Lambda";
      break;
    case 2:
      parameter = "Offset";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitExponentialUnweightedPlugin::pluginName() const { return "Exponential Fit"; }
QString FitExponentialUnweightedPlugin::pluginDescription() const { return "Generates a exponential fit for a set of data."; }


Kst::DataObject *FitExponentialUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitExponentialUnweightedPlugin* config = static_cast<ConfigWidgetFitExponentialUnweightedPlugin*>(configWidget)) {

    FitExponentialUnweightedSource* object = store->createObject<FitExponentialUnweightedSource>();

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


Kst::DataObjectConfigWidget *FitExponentialUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitExponentialUnweightedPlugin *widget = new ConfigWidgetFitExponentialUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitExponentialUnweightedPlugin, FitExponentialUnweightedPlugin)

// vim: ts=2 sw=2 et
