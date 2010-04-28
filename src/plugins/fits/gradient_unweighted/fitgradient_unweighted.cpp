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


#include "fitgradient_unweighted.h"
#include "objectstore.h"
#include "ui_fitgradient_unweightedconfig.h"

#include <gsl/gsl_fit.h>
#include "../common.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& VECTOR_OUT_Y_LO = "Lo Vector";
static const QString& VECTOR_OUT_Y_HI = "Hi Vector";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitGradientUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitGradient_UnweightedConfig {
  public:
    ConfigWidgetFitGradientUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitGradient_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitGradientUnweightedPlugin() {}

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
      if (FitGradientUnweightedSource* source = static_cast<FitGradientUnweightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Gradient Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Gradient Plugin");
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


FitGradientUnweightedSource::FitGradientUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitGradientUnweightedSource::~FitGradientUnweightedSource() {
}


QString FitGradientUnweightedSource::_automaticDescriptiveName() const {
    return vectorY()->descriptiveName() + i18n(" Unweighted Gradient");
}


void FitGradientUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitGradientUnweightedPlugin* config = static_cast<ConfigWidgetFitGradientUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitGradientUnweightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputVector(VECTOR_OUT_Y_LO, "");
  setOutputVector(VECTOR_OUT_Y_HI, "");
  setOutputScalar(SCALAR_OUT, "");
}


bool FitGradientUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];

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
  double cov00 = 0.0;
  double dSumSq = 0.0;
  double y;
  double yErr;

  if( precursor( inputVectorX, inputVectorY, 0, &iLength, false, true, 2, pInputs, outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters, outputVectorYCovariance, outputVectorYLo, outputVectorYHi ) ) {

    if( !gsl_fit_mul( pInputs[XVALUES], 1, pInputs[YVALUES], 1, iLength, &c0, &cov00, &dSumSq ) ) {
      for( i=0; i<iLength; i++ ) {
        gsl_fit_mul_est( pInputs[XVALUES][i], c0, cov00, &y, &yErr );

        outputVectorYFitted->value()[i] = y;
        outputVectorYResiduals->value()[i] = pInputs[YVALUES][i] - y;
        outputVectorYLo->value()[i] = y - yErr;
        outputVectorYHi->value()[i] = y + yErr;
      }

      outputVectorYParameters->value()[0] = c0;
      outputVectorYCovariance->value()[0] = cov00;

      outputScalar->setValue(dSumSq / ( (double)iLength - 2.0 ));

      bReturn = true;
    } 
  }

  postcursor( false, pInputs );

  return bReturn;
}


Kst::VectorPtr FitGradientUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitGradientUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitGradientUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitGradientUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitGradientUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitGradientUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_LO;
  vectors += VECTOR_OUT_Y_HI;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitGradientUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitGradientUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitGradientUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitGradientUnweightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Gradient";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitGradientUnweightedPlugin::pluginName() const { return "Gradient Fit"; }
QString FitGradientUnweightedPlugin::pluginDescription() const { return "Generates a gradient fit for a set of data."; }


Kst::DataObject *FitGradientUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitGradientUnweightedPlugin* config = static_cast<ConfigWidgetFitGradientUnweightedPlugin*>(configWidget)) {

    FitGradientUnweightedSource* object = store->createObject<FitGradientUnweightedSource>();

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


Kst::DataObjectConfigWidget *FitGradientUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitGradientUnweightedPlugin *widget = new ConfigWidgetFitGradientUnweightedPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitGradientUnweightedPlugin, FitGradientUnweightedPlugin)

// vim: ts=2 sw=2 et
