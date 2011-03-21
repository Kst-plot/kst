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


#include "fitpolynomial_unweighted.h"
#include "objectstore.h"
#include "ui_fitpolynomial_unweightedconfig.h"

#include <gsl/gsl_fit.h>
// #include "../common.h"
#include "../linear.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_IN = "Order Scalar";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitPolynomialUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitPolynomial_UnweightedConfig {
  public:
    ConfigWidgetFitPolynomialUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitPolynomial_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitPolynomialUnweightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarOrder->setDefaultValue(2);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedScalarOrder() { return _scalarOrder->selectedScalar(); };
    void setSelectedScalarOrder(Kst::ScalarPtr scalar) { return _scalarOrder->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitPolynomialUnweightedSource* source = static_cast<FitPolynomialUnweightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
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
        _cfg->beginGroup("Fit Polynomial Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Scalar Order", _scalarOrder->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Polynomial Plugin");
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
        QString scalarName = _cfg->value("Input Order Scalar").toString();
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


FitPolynomialUnweightedSource::FitPolynomialUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitPolynomialUnweightedSource::~FitPolynomialUnweightedSource() {
}


QString FitPolynomialUnweightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Polynomial");
}


void FitPolynomialUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitPolynomialUnweightedPlugin* config = static_cast<ConfigWidgetFitPolynomialUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputScalar(SCALAR_IN, config->selectedScalarOrder());
  }
}


void FitPolynomialUnweightedSource::setupOutputs() {
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


bool FitPolynomialUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::ScalarPtr inputScalarOrder = _inputScalars[SCALAR_IN];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = i18n("Polynomial Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = i18n("Polynomial Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);

  bool bReturn = false;

  int iNumParams;

  iNumParams = 1 + (int)floor( inputScalarOrder->value() );
  if( iNumParams > 0 ) {
    bReturn = kstfit_linear_unweighted( inputVectorX, inputVectorY,
                                        outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                                        outputVectorYCovariance, outputScalar, iNumParams );
  }

  return bReturn;
}


// TODO merge with FitPolynomialWeightedSource
QString FitPolynomialUnweightedSource::parameterVectorToString() const {

  QString str = Name();

  if (hasParameterVector()) {
    Kst::VectorPtr vectorParam = _outputVectors["Parameters Vector"];
    for (int i = 0; i < vectorParam->length(); i++) {
      QString paramName = parameterName(i);
      if (!paramName.isEmpty()) {
        if (_outputScalars.contains(paramName)) {
          QString name = _outputScalars[paramName]->Name();
          double value = _outputScalars[paramName]->value();
          QString sign;
          if (value >= 0) {
            sign = " +";
          } else {
            sign = " "; // Just for the space, the "-" is already in the number
          }
          if (i == 0) {
            str += QString("\n[%1]").arg(name);
          } else if (i == 1) { // Special case x^1 to x
            str += QString("%1[%2]x").arg(sign).arg(name);
          } else {
            str += QString("%1[%2]%3").arg(sign).arg(name).arg(paramName);
          }
        }
      }
    }
  }

  return str;
}


Kst::VectorPtr FitPolynomialUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitPolynomialUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::ScalarPtr FitPolynomialUnweightedSource::scalarOrder() const {
  return _inputScalars[SCALAR_IN];
}


QStringList FitPolynomialUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitPolynomialUnweightedSource::inputScalarList() const {
  return QStringList(SCALAR_IN);
}


QStringList FitPolynomialUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitPolynomialUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitPolynomialUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitPolynomialUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitPolynomialUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitPolynomialUnweightedSource::parameterName(int index) const {
  return QString("x^%1").arg(index);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitPolynomialUnweightedPlugin::pluginName() const { return "Polynomial Fit"; }
QString FitPolynomialUnweightedPlugin::pluginDescription() const { return "Generates a polynomial fit for a set of data."; }


Kst::DataObject *FitPolynomialUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitPolynomialUnweightedPlugin* config = static_cast<ConfigWidgetFitPolynomialUnweightedPlugin*>(configWidget)) {

    FitPolynomialUnweightedSource* object = store->createObject<FitPolynomialUnweightedSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
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


Kst::DataObjectConfigWidget *FitPolynomialUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitPolynomialUnweightedPlugin *widget = new ConfigWidgetFitPolynomialUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitPolynomialUnweightedPlugin, FitPolynomialUnweightedPlugin)

// vim: ts=2 sw=2 et
