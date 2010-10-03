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


#include "fitpolynomial_weighted.h"
#include "objectstore.h"
#include "ui_fitpolynomial_weightedconfig.h"

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
static const QString& SCALAR_IN = "Order Scalar";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitPolynomialWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitPolynomial_WeightedConfig {
  public:
    ConfigWidgetFitPolynomialWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitPolynomial_WeightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitPolynomialWeightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _vectorWeights->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarOrder->setDefaultValue(2);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorWeights, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarOrder, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedScalarOrder() { return _scalarOrder->selectedScalar(); };
    void setSelectedScalarOrder(Kst::ScalarPtr scalar) { return _scalarOrder->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitPolynomialWeightedSource* source = static_cast<FitPolynomialWeightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorWeights(source->vectorWeights());
        setSelectedScalarOrder(source->scalarOrder());
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
        _cfg->beginGroup("Fit Polynomial Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->setValue("Input Scalar Order", _scalarOrder->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Polynomial Weighted Plugin");
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
        QString scalarName = _cfg->value("Order Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* orderScalar = static_cast<Kst::Scalar*>(object);
        if (orderScalar) {
          setSelectedScalarOrder(orderScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitPolynomialWeightedSource::FitPolynomialWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitPolynomialWeightedSource::~FitPolynomialWeightedSource() {
}


QString FitPolynomialWeightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Weighted Polynomial");
}


void FitPolynomialWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitPolynomialWeightedPlugin* config = static_cast<ConfigWidgetFitPolynomialWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
    setInputScalar(SCALAR_IN, config->selectedScalarOrder());
  }
}


void FitPolynomialWeightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}


double calculate_matrix_entry( double dX, int iPos ) {
  double dY;

  dY = pow( dX, (double)iPos );

  return dY;
}


bool FitPolynomialWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];
  Kst::ScalarPtr inputScalarOrder = _inputScalars[SCALAR_IN];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  bool bReturn = false;

  int iNumParams;

  iNumParams = 1 + (int)floor( inputScalarOrder->value() );
  if( iNumParams > 0 ) {
    bReturn = kstfit_linear_weighted( inputVectorX, inputVectorY, inputVectorWeights,
                                        outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                                        outputVectorYCovariance, outputScalar, iNumParams );
  }

  return bReturn;
}


Kst::VectorPtr FitPolynomialWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitPolynomialWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitPolynomialWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}


Kst::ScalarPtr FitPolynomialWeightedSource::scalarOrder() const {
  return _inputScalars[SCALAR_IN];
}


QStringList FitPolynomialWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitPolynomialWeightedSource::inputScalarList() const {
  return QStringList(SCALAR_IN);
}


QStringList FitPolynomialWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitPolynomialWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitPolynomialWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitPolynomialWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitPolynomialWeightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitPolynomialWeightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "x^%1";
      parameter.arg(index);
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitPolynomialWeightedPlugin::pluginName() const { return "Polynomial Weighted Fit"; }
QString FitPolynomialWeightedPlugin::pluginDescription() const { return "Generates a polynomial weighted fit for a set of data."; }


Kst::DataObject *FitPolynomialWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitPolynomialWeightedPlugin* config = static_cast<ConfigWidgetFitPolynomialWeightedPlugin*>(configWidget)) {

    FitPolynomialWeightedSource* object = store->createObject<FitPolynomialWeightedSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
      object->setInputScalar(SCALAR_IN, config->selectedScalarOrder());
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


Kst::DataObjectConfigWidget *FitPolynomialWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitPolynomialWeightedPlugin *widget = new ConfigWidgetFitPolynomialWeightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitPolynomialWeightedPlugin, FitPolynomialWeightedPlugin)

// vim: ts=2 sw=2 et
