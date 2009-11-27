/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "fitlorentzian_unweighted.h"
#include "objectstore.h"
#include "ui_fitlorentzian_unweightedconfig.h"

#define NUM_PARAMS 3
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Y Fitted Vector";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals Vector";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance Vector";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitLorentzianUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitLorentzian_UnweightedConfig {
  public:
    ConfigWidgetFitLorentzianUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitLorentzian_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitLorentzianUnweightedPlugin() {}

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
      if (FitLorentzianUnweightedSource* source = static_cast<FitLorentzianUnweightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Lorentzian Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Lorentzian Plugin");
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


FitLorentzianUnweightedSource::FitLorentzianUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitLorentzianUnweightedSource::~FitLorentzianUnweightedSource() {
}


QString FitLorentzianUnweightedSource::_automaticDescriptiveName() const {
  return QString("Fit Lorentzian Plugin");
}


void FitLorentzianUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitLorentzianUnweightedPlugin* config = static_cast<ConfigWidgetFitLorentzianUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitLorentzianUnweightedSource::setupOutputs() {
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
  double dHW    = pdParameters[1];
  double dScale = pdParameters[2];
  double dY;

  dY  = ( dScale / M_PI ) * ( dHW / 2.0 );
  dY /= ( ( dX - dMean ) * ( dX - dMean ) )+( ( dHW / 2.0 ) * ( dHW / 2.0 ) );

  return dY;
}


void function_derivative( double dX, double* pdParameters, double* pdDerivatives ) {
  double dMean  = pdParameters[0];
  double dHW    = pdParameters[1];
  double dScale = pdParameters[2];
  double dDenom;  
  double ddMean;
  double ddHW;
  double ddScale;

  dDenom  = ( ( dX - dMean ) * ( dX - dMean ) ) + ( ( dHW / 2.0 ) * ( dHW / 2.0 ) );
  ddMean  = ( dScale / M_PI ) * dHW * ( dMean - dX ) / ( dDenom * dDenom );
  ddHW    = ( dScale / ( 2.0 * M_PI ) ) / ( dDenom * dDenom );
  ddHW   *= dDenom - ( dHW * dHW / 2.0 );
  ddScale = ( 1.0 / M_PI ) * ( dHW / 2.0 ) / dDenom;

  pdDerivatives[0] = ddMean;
  pdDerivatives[1] = ddHW;
  pdDerivatives[2] = ddScale;
}


bool FitLorentzianUnweightedSource::algorithm() {
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


Kst::VectorPtr FitLorentzianUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitLorentzianUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitLorentzianUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitLorentzianUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitLorentzianUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitLorentzianUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitLorentzianUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitLorentzianUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitLorentzianUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitLorentzianUnweightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Mean";
      break;
    case 1:
      parameter = "Half-width";
      break;
    case 2:
      parameter = "Scale";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitLorentzianUnweightedPlugin::pluginName() const { return "Lorentzian Fit"; }
QString FitLorentzianUnweightedPlugin::pluginDescription() const { return "Generates a lorentzian fit for a set of data."; }


Kst::DataObject *FitLorentzianUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitLorentzianUnweightedPlugin* config = static_cast<ConfigWidgetFitLorentzianUnweightedPlugin*>(configWidget)) {

    FitLorentzianUnweightedSource* object = store->createObject<FitLorentzianUnweightedSource>();

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


Kst::DataObjectConfigWidget *FitLorentzianUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitLorentzianUnweightedPlugin *widget = new ConfigWidgetFitLorentzianUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitLorentzianUnweightedPlugin, FitLorentzianUnweightedPlugin)

// vim: ts=2 sw=2 et
