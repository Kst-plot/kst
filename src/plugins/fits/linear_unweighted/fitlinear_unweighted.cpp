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


#include "fitlinear_unweighted.h"
#include "objectstore.h"
#include "ui_fitlinear_unweightedconfig.h"

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

class ConfigWidgetFitLinearUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitLinear_UnweightedConfig {
  public:
    ConfigWidgetFitLinearUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitLinear_UnweightedConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFitLinearUnweightedPlugin() {}

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
      if (FitLinearUnweightedSource* source = static_cast<FitLinearUnweightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Linear Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Linear Plugin");
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


FitLinearUnweightedSource::FitLinearUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitLinearUnweightedSource::~FitLinearUnweightedSource() {
}


QString FitLinearUnweightedSource::_automaticDescriptiveName() const {
  return tr("%1 Unweighted Linear").arg(vectorY()->descriptiveName());
}


void FitLinearUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitLinearUnweightedPlugin* config = static_cast<ConfigWidgetFitLinearUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitLinearUnweightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputVector(VECTOR_OUT_Y_LO, "");
  setOutputVector(VECTOR_OUT_Y_HI, "");
  setOutputScalar(SCALAR_OUT, "");
}


bool FitLinearUnweightedSource::algorithm() {
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
  int iLength = 0;
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

  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = tr("Linear Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = tr("Linear Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);

  label_info.name = tr("Linear Fit Lower Limit");
  outputVectorYLo->setLabelInfo(label_info);

  label_info.name = tr("Linear Fit Upper Limit");
  outputVectorYHi->setLabelInfo(label_info);

  if( precursor( inputVectorX, inputVectorY, 0, &iLength, false, true, 2, pInputs, outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters, outputVectorYCovariance, outputVectorYLo, outputVectorYHi ) ) {

    if( !gsl_fit_linear( pInputs[XVALUES], 1, pInputs[YVALUES], 1, iLength, &c0, &c1, &cov00, &cov01, &cov11, &dSumSq ) ) {
      for( i=0; i<iLength; ++i ) {
        gsl_fit_linear_est( pInputs[XVALUES][i], c0, c1, cov00, cov01, cov11, &y, &yErr );
        outputVectorYFitted->raw_V_ptr()[i] = y;
        outputVectorYResiduals->raw_V_ptr()[i] = pInputs[YVALUES][i] - y;
        outputVectorYLo->raw_V_ptr()[i] = y - yErr;
        outputVectorYHi->raw_V_ptr()[i] = y + yErr;
      }

      outputVectorYParameters->raw_V_ptr()[0] = c0;
      outputVectorYParameters->raw_V_ptr()[1] = c1;
      outputVectorYCovariance->raw_V_ptr()[0] = cov00;
      outputVectorYCovariance->raw_V_ptr()[1] = cov01;
      outputVectorYCovariance->raw_V_ptr()[2] = cov11;

      outputScalar->setValue(dSumSq / ( (double)iLength - 2.0 ));
      bReturn = true;
    }
  }

   postcursor( false, pInputs );

  return bReturn;
}


Kst::VectorPtr FitLinearUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitLinearUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitLinearUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitLinearUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitLinearUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitLinearUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_LO;
  vectors += VECTOR_OUT_Y_HI;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitLinearUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitLinearUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitLinearUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitLinearUnweightedSource::parameterName(int index) const {
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
QString FitLinearUnweightedPlugin::pluginName() const { return tr("Linear Fit"); }
QString FitLinearUnweightedPlugin::pluginDescription() const { return tr("Generates a linear fit for a set of data."); }


Kst::DataObject *FitLinearUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitLinearUnweightedPlugin* config = static_cast<ConfigWidgetFitLinearUnweightedPlugin*>(configWidget)) {

    FitLinearUnweightedSource* object = store->createObject<FitLinearUnweightedSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitLinearUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitLinearUnweightedPlugin *widget = new ConfigWidgetFitLinearUnweightedPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
