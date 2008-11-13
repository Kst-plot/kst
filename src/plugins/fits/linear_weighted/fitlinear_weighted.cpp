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


#include "fitlinear_weighted.h"
#include "objectstore.h"
#include "ui_fitlinear_weightedconfig.h"

#include <gsl/gsl_fit.h>
#include "../common.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_IN_WEIGHTS = "Weights Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Y Fitted Vector";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals Vector";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance Vector";
static const QString& VECTOR_OUT_Y_LO = "Lo Vector";
static const QString& VECTOR_OUT_Y_HI = "Hi Vector";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitLinearWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitLinear_WeightedConfig {
  public:
    ConfigWidgetFitLinearWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitLinear_WeightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitLinearWeightedPlugin() {}

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
      if (FitLinearWeightedSource* source = static_cast<FitLinearWeightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Linear Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Linear Weighted Plugin");
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


FitLinearWeightedSource::FitLinearWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitLinearWeightedSource::~FitLinearWeightedSource() {
}


QString FitLinearWeightedSource::_automaticDescriptiveName() const {
  return QString("Fit Linear Weighted Plugin");
}


void FitLinearWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitLinearWeightedPlugin* config = static_cast<ConfigWidgetFitLinearWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
  }
}


void FitLinearWeightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputVector(VECTOR_OUT_Y_LO, "");
  setOutputVector(VECTOR_OUT_Y_HI, "");
  setOutputScalar(SCALAR_OUT, "");
}


bool FitLinearWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::VectorPtr outputVectorYLo = _outputVectors[VECTOR_OUT_Y_LO];
  Kst::VectorPtr outputVectorYHi = _outputVectors[VECTOR_OUT_Y_HI];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  int i = 0;
  int iLength;
  bool bReturn = false;
  double* pInputs[3];
  double c0 = 0.0;
  double c1 = 0.0;
  double cov00 = 0.0;
  double cov01 = 0.0;
  double cov11 = 0.0;
  double dSumSq = 0.0;
  double y;
  double yErr;

  if( precursor( inputVectorX, inputVectorY, inputVectorWeights, &iLength, true, true, 2, pInputs, outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters, outputVectorYCovariance, outputVectorYLo, outputVectorYHi ) ) {

    if( !gsl_fit_wlinear( pInputs[XVALUES], 1, pInputs[WEIGHTS], 1, pInputs[YVALUES], 1, iLength, &c0, &c1, &cov00, &cov01, &cov11, &dSumSq ) ) {
      for( i=0; i<iLength; i++ ) {
        gsl_fit_linear_est( pInputs[XVALUES][i], c0, c1, cov00, cov01, cov11, &y, &yErr );
        outputVectorYFitted->value()[i] = y;
        outputVectorYResiduals->value()[i] = pInputs[YVALUES][i] - y;
        outputVectorYLo->value()[i] = y - yErr;
        outputVectorYHi->value()[i] = y + yErr;
      }

      outputVectorYParameters->value()[0] = c0;
      outputVectorYParameters->value()[1] = c1;
      outputVectorYCovariance->value()[0] = cov00;
      outputVectorYCovariance->value()[1] = cov01;
      outputVectorYCovariance->value()[2] = cov11;

      outputScalar->setValue(dSumSq / ( (double)iLength - 2.0 ));
      bReturn = true;
    }
  }

  postcursor( true, pInputs );

  return bReturn;
}


Kst::VectorPtr FitLinearWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitLinearWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitLinearWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}


QStringList FitLinearWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitLinearWeightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitLinearWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitLinearWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_LO;
  vectors += VECTOR_OUT_Y_HI;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitLinearWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitLinearWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitLinearWeightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitLinearWeightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Intercept";
      break;
    case 1:
      parameter = "Gradient";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitLinearWeightedPlugin::pluginName() const { return "Linear Weighted Fit"; }
QString FitLinearWeightedPlugin::pluginDescription() const { return "Generates a linear weighted fit for a set of data."; }


Kst::DataObject *FitLinearWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitLinearWeightedPlugin* config = static_cast<ConfigWidgetFitLinearWeightedPlugin*>(configWidget)) {

    FitLinearWeightedSource* object = store->createObject<FitLinearWeightedSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
      object->setupOutputs();
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->update();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitLinearWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitLinearWeightedPlugin *widget = new ConfigWidgetFitLinearWeightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitLinearWeightedPlugin, FitLinearWeightedPlugin)

// vim: ts=2 sw=2 et
