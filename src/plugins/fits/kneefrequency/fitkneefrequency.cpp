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


#include "fitkneefrequency.h"
#include "objectstore.h"
#include "ui_fitkneefrequencyconfig.h"

#include <gsl/gsl_fit.h>
#include "../common.h"

#define KNEEFREQ_NUMPARAMETERS 5

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& SCALAR_IN_MAX = "Max 1/f^a Freq Scalar";
static const QString& SCALAR_IN_MIN = "Min. White Noise Freq Scalar";
static const QString& SCALAR_IN_WHITENOISE = "White Noise C Scalar";
static const QString& VECTOR_OUT_Y_FITTED = "Y Fitted Vector";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals Vector";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";

class ConfigWidgetFitKneeFrequencyPlugin : public Kst::DataObjectConfigWidget, public Ui_FitKneeFrequencyConfig {
  public:
    ConfigWidgetFitKneeFrequencyPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitKneeFrequencyConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitKneeFrequencyPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _scalarMax->setObjectStore(store);
      _scalarMin->setObjectStore(store);
      _scalarWhiteNoise->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMax, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMin, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarWhiteNoise, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedScalarMax() { return _scalarMax->selectedScalar(); };
    void setSelectedScalarMax(Kst::ScalarPtr scalar) { return _scalarMax->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarMin() { return _scalarMin->selectedScalar(); };
    void setSelectedScalarMin(Kst::ScalarPtr scalar) { return _scalarMin->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarWhiteNoise() { return _scalarWhiteNoise->selectedScalar(); };
    void setSelectedScalarWhiteNoise(Kst::ScalarPtr scalar) { return _scalarWhiteNoise->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitKneeFrequencySource* source = static_cast<FitKneeFrequencySource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedScalarMax(source->scalarMax());
        setSelectedScalarMin(source->scalarMin());
        setSelectedScalarWhiteNoise(source->scalarWhiteNoise());
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
        _cfg->beginGroup("Fit Knee Frequency Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Scalar Max 1/f^a Freq", _scalarMax->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Min. White Noise Freq", _scalarMin->selectedScalar()->Name());
        _cfg->setValue("Input Scalar White Noise C", _scalarWhiteNoise->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Knee Frequency Plugin");
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
        QString scalarName = _cfg->value("Input Scalar Max 1/f^a Freq").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* maxScalar = static_cast<Kst::Scalar*>(object);
        if (maxScalar) {
          setSelectedScalarMax(maxScalar);
        }
        scalarName = _cfg->value("Input Scalar Min. White Noise Freq").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* minScalar = static_cast<Kst::Scalar*>(object);
        if (minScalar) {
          setSelectedScalarMin(minScalar);
        }
        scalarName = _cfg->value("Input Scalar White Noise C").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* whiteNoiseScalar = static_cast<Kst::Scalar*>(object);
        if (whiteNoiseScalar) {
          setSelectedScalarWhiteNoise(whiteNoiseScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitKneeFrequencySource::FitKneeFrequencySource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitKneeFrequencySource::~FitKneeFrequencySource() {
}


QString FitKneeFrequencySource::_automaticDescriptiveName() const {
  return QString("Fit Knee Frequency Plugin");
}


void FitKneeFrequencySource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitKneeFrequencyPlugin* config = static_cast<ConfigWidgetFitKneeFrequencyPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputScalar(SCALAR_IN_MAX, config->selectedScalarMax());
    setInputScalar(SCALAR_IN_MIN, config->selectedScalarMin());
    setInputScalar(SCALAR_IN_WHITENOISE, config->selectedScalarWhiteNoise());
  }
}


void FitKneeFrequencySource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
}


bool FitKneeFrequencySource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::ScalarPtr inputScalarMax = _inputScalars[SCALAR_IN_MAX];
  Kst::ScalarPtr inputScalarMin = _inputScalars[SCALAR_IN_MIN];
  Kst::ScalarPtr inputScalarWhiteNoise = _inputScalars[SCALAR_IN_WHITENOISE];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];

  if (inputVectorX->length() != inputVectorY->length())  {
    _errorString = "Error:  Input Vector Sizes do not match";
    return false;
  }
  if (inputVectorX->length() < 1)  {
    _errorString = "Error:  Input Vector X invalid";
    return false;
  }

  int inArraysLength = inputVectorX->length();

  outputVectorYFitted->resize(inArraysLength);
  outputVectorYResiduals->resize(inArraysLength);
  outputVectorYParameters->resize(KNEEFREQ_NUMPARAMETERS);

  double xi, yi;
  int i;
  double maxOneOverFFreq, minWhiteNoiseFreq, whiteNoiseC;

  maxOneOverFFreq = inputScalarMax->value();
  minWhiteNoiseFreq = inputScalarMin->value();
  whiteNoiseC = inputScalarWhiteNoise->value();

  int maxOneOverFIndex, minWhiteNoiseIndex;

  //fast calculation of index for maxOneOverFFreq
  int i_bot = 0;
  int i_top = inArraysLength - 1;

  while (i_bot + 1 < i_top) {
    int i0 = (i_top + i_bot)/2;
    if (maxOneOverFFreq < inputVectorX->value()[i0]) {
      i_top = i0;
    } else {
      i_bot = i0;
    }
  }
  maxOneOverFIndex = i_top; //top because we use i_bot+1.

  //fast calculation of index for minWhiteNoiseFreq
  i_bot = 0;
  i_top = inArraysLength - 1;

  while (i_bot + 1 < i_top) {
    int i0 = (i_top + i_bot)/2;
    if (minWhiteNoiseFreq < inputVectorX->value()[i0]) {
      i_top = i0;
    } else {
      i_bot = i0;
    }
  }
  minWhiteNoiseIndex = i_top;

  //verify calculated indices.
  if ( !(maxOneOverFIndex>0) || !(minWhiteNoiseIndex>=maxOneOverFIndex) || !(minWhiteNoiseIndex<(inArraysLength-1)) ) {
    _errorString = "Error:  Input Frequencies are Invalid\n";
    return false;
  }

  // calculate white noise limit
  double sumY, sumY2;
  sumY = sumY2 = 0;

  for (i = minWhiteNoiseIndex; i < inArraysLength; i++) {
    yi = inputVectorY->value()[i];
    sumY    +=  yi;
    sumY2   +=  pow(yi,2);
  }

  double ybar, ysigma;
  ybar = sumY/(inArraysLength - minWhiteNoiseIndex);
  ysigma = sqrt((sumY2 - 2*ybar*sumY + pow(ybar,2)*(inArraysLength - minWhiteNoiseIndex))/(inArraysLength - minWhiteNoiseIndex));
  // end calculate white noise limit

  // fit 1/f noise
  double sumLnXLnY, sumLnX, sumLnY, sumLnX2;
  sumLnXLnY = sumLnX = sumLnY = sumLnX2 = 0;

  for (i = 0; i < maxOneOverFIndex; i++) {
    xi = inputVectorX->value()[i];
    yi = inputVectorY->value()[i];

    if (!(xi>0) || !((yi-ybar)>0)) {
      _errorString = "Error:  Input Data Invalid.\n";
      return false;
    }

    sumLnXLnY += log(xi)*log(yi-ybar); //-ybar to isolate 1/f noise.
    sumLnX    += log(xi);
    sumLnY    += log(yi-ybar);
    sumLnX2   += pow(log(xi),2);
  }

  double a, b;
  a = (maxOneOverFIndex*sumLnXLnY - sumLnX*sumLnY)/(maxOneOverFIndex*sumLnX2 - pow(sumLnX,2));
  b = exp((sumLnY - a*sumLnX)/maxOneOverFIndex);
  // end fit 1/f noise

  double knee_freq = pow(ybar*whiteNoiseC/b,1.0/a); // calculate knee frequency.

  // output fit data
  for (i = 0; i < maxOneOverFIndex; i++) {
      outputVectorYFitted->value()[i] = b * pow(inputVectorX->value()[i],a) + ybar;
      outputVectorYResiduals->value()[i] = inputVectorY->value()[i] - outputVectorYFitted->value()[i];
  }

  for (i = maxOneOverFIndex; i < minWhiteNoiseIndex; i++) { // zeros for unfitted region.
      outputVectorYFitted->value()[i] = 0;
      outputVectorYResiduals->value()[i] = 0;
  }

  for (i = minWhiteNoiseIndex; i < inArraysLength; i++) {
      outputVectorYFitted->value()[i] = ybar;
      outputVectorYResiduals->value()[i] = outputVectorYFitted->value()[i] - ybar;
  }

  outputVectorYParameters->value()[0] = ybar;
  outputVectorYParameters->value()[1] = ysigma;
  outputVectorYParameters->value()[2] = b;
  outputVectorYParameters->value()[3] = -a;
  outputVectorYParameters->value()[4] = knee_freq;

  return true;
}


Kst::VectorPtr FitKneeFrequencySource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitKneeFrequencySource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::ScalarPtr FitKneeFrequencySource::scalarMax() const {
  return _inputScalars[SCALAR_IN_MAX];
}


Kst::ScalarPtr FitKneeFrequencySource::scalarMin() const {
  return _inputScalars[SCALAR_IN_MIN];
}


Kst::ScalarPtr FitKneeFrequencySource::scalarWhiteNoise() const {
  return _inputScalars[SCALAR_IN_WHITENOISE];
}


QStringList FitKneeFrequencySource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitKneeFrequencySource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_MAX);
  scalars += SCALAR_IN_MIN;
  scalars += SCALAR_IN_WHITENOISE;
  return scalars;
}


QStringList FitKneeFrequencySource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitKneeFrequencySource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitKneeFrequencySource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT */);
}


QStringList FitKneeFrequencySource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitKneeFrequencySource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitKneeFrequencySource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "White Noise Limit";
      break;
    case 1:
      parameter = "White Noise Sigma";
      break;
    case 2:
      parameter = "1/f^a Amplitude";
      break;
    case 3:
      parameter = "1/f^a Power Law a";
      break;
    case 4:
      parameter = "Knee Frequency";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitKneeFrequencyPlugin::pluginName() const { return "Knee Frequency Fit"; }
QString FitKneeFrequencyPlugin::pluginDescription() const { return "Generates a knee frequency fit for a set of data."; }


Kst::DataObject *FitKneeFrequencyPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitKneeFrequencyPlugin* config = static_cast<ConfigWidgetFitKneeFrequencyPlugin*>(configWidget)) {

    FitKneeFrequencySource* object = store->createObject<FitKneeFrequencySource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputScalar(SCALAR_IN_MAX, config->selectedScalarMax());
      object->setInputScalar(SCALAR_IN_MIN, config->selectedScalarMin());
      object->setInputScalar(SCALAR_IN_WHITENOISE, config->selectedScalarWhiteNoise());
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


Kst::DataObjectConfigWidget *FitKneeFrequencyPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitKneeFrequencyPlugin *widget = new ConfigWidgetFitKneeFrequencyPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FitKneeFrequencyPlugin, FitKneeFrequencyPlugin)

// vim: ts=2 sw=2 et
