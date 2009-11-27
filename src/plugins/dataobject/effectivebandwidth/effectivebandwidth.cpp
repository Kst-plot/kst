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


#include "effectivebandwidth.h"
#include "objectstore.h"
#include "ui_effectivebandwidthconfig.h"

static const QString& VECTOR_IN_X = "Vector In X";
static const QString& VECTOR_IN_Y = "Vector In Y";
static const QString& SCALAR_IN_MIN = "Min. White Noise Freq.";
static const QString& SCALAR_IN_FREQ = "Sampling Frequency (Hz)";
static const QString& SCALAR_IN_K = "K";

static const QString& SCALAR_OUT_LIMIT = "White Noise Limit";
static const QString& SCALAR_OUT_SIGMA = "White Noise Sigma";
static const QString& SCALAR_OUT_BANDWIDTH = "Effective Bandwidth";

class ConfigEffectiveBandwidthPlugin : public Kst::DataObjectConfigWidget, public Ui_EffectiveBandwidthConfig {
  public:
    ConfigEffectiveBandwidthPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_EffectiveBandwidthConfig() {
      setupUi(this);
    }

    ~ConfigEffectiveBandwidthPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _scalarMin->setObjectStore(store);
      _scalarFreq->setObjectStore(store);
      _scalarK->setObjectStore(store);
      _scalarMin->setDefaultValue(0);
      _scalarFreq->setDefaultValue(0);
      _scalarK->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMin, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarFreq, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarK, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarMin() { return _scalarMin->selectedScalar(); };
    void setSelectedScalarMin(Kst::ScalarPtr scalar) { return _scalarMin->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarFreq() { return _scalarFreq->selectedScalar(); };
    void setSelectedScalarFreq(Kst::ScalarPtr scalar) { return _scalarFreq->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarK() { return _scalarK->selectedScalar(); };
    void setSelectedScalarK(Kst::ScalarPtr scalar) { return _scalarK->setSelectedScalar(scalar); };


    virtual void setupFromObject(Kst::Object* dataObject) {
      if (EffectiveBandwidthSource* source = static_cast<EffectiveBandwidthSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedScalarMin(source->scalarMin());
        setSelectedScalarFreq(source->scalarFreq());
        setSelectedScalarK(source->scalarK());
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
        _cfg->beginGroup("Effective Bandwidth DataObject Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Scalar Min. White Noise Freq.", _scalarMin->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Sampling Frequency (Hz)", _scalarFreq->selectedScalar()->Name());
        _cfg->setValue("Input Scalar K", _scalarK->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Effective Bandwidth DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorX = static_cast<Kst::Vector*>(object);
        if (vectorX) {
          setSelectedVectorX(vectorX);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorY = static_cast<Kst::Vector*>(object);
        if (vectorY) {
          setSelectedVectorY(vectorY);
        }
        QString scalarName = _cfg->value("Input Scalar Min. White Noise Freq.").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarMin = static_cast<Kst::Scalar*>(object);
        if (scalarMin) {
          setSelectedScalarMin(scalarMin);
        }
        scalarName = _cfg->value("Input Scalar Sampling Frequency (Hz)").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarFreq = static_cast<Kst::Scalar*>(object);
        if (scalarFreq) {
          setSelectedScalarFreq(scalarFreq);
        }
        scalarName = _cfg->value("Input Scalar K").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarK = static_cast<Kst::Scalar*>(object);
        if (scalarK) {
          setSelectedScalarK(scalarK);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


EffectiveBandwidthSource::EffectiveBandwidthSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


EffectiveBandwidthSource::~EffectiveBandwidthSource() {
}


QString EffectiveBandwidthSource::_automaticDescriptiveName() const {
  return QString("Effective Bandwidth Plugin Object");
}


void EffectiveBandwidthSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigEffectiveBandwidthPlugin* config = static_cast<ConfigEffectiveBandwidthPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputScalar(SCALAR_IN_MIN, config->selectedScalarMin());
    setInputScalar(SCALAR_IN_FREQ, config->selectedScalarFreq());
    setInputScalar(SCALAR_IN_K, config->selectedScalarK());
  }
}


void EffectiveBandwidthSource::setupOutputs() {
  setOutputScalar(SCALAR_OUT_LIMIT, "");
  setOutputScalar(SCALAR_OUT_SIGMA, "");
  setOutputScalar(SCALAR_OUT_BANDWIDTH, "");
}


bool EffectiveBandwidthSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::ScalarPtr inputScalarMin = _inputScalars[SCALAR_IN_MIN];
  Kst::ScalarPtr inputScalarFreq = _inputScalars[SCALAR_IN_FREQ];
  Kst::ScalarPtr inputScalarK = _inputScalars[SCALAR_IN_K];

  Kst::ScalarPtr outputScalarLimit = _outputScalars[SCALAR_OUT_LIMIT];
  Kst::ScalarPtr outputScalarSigma = _outputScalars[SCALAR_OUT_SIGMA];
  Kst::ScalarPtr outputScalarBandwidth = _outputScalars[SCALAR_OUT_BANDWIDTH];

  //Make sure there is at least 1 element in the input vector
  if (inputVectorX->length() < 1) {
    _errorString = "Error:  Input Vector X invalid size";
    return false;
  }

  // Make sure the input vectors match.
  if (inputVectorX->length() != inputVectorY->length()) {
    _errorString = "Error:  Input Vector X and Input Vector Y are not the same length";
    return false;
  }

  double minWhiteNoiseFreq, samplingFrequency, radiometerConstantK;

  minWhiteNoiseFreq = inputScalarMin->value();
  samplingFrequency = inputScalarFreq->value();
  radiometerConstantK = inputScalarK->value();

  int minWhiteNoiseIndex;

  //fast calculation of index for minWhiteNoiseFreq
  int i_bot, i_top;
  i_bot = 0;
  i_top = inputVectorX->length() - 1;

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
  if ( !(minWhiteNoiseIndex>0) || !(minWhiteNoiseIndex<(inputVectorX->length()-1)) ) {
    _errorString = "Error:  Calculated Indices invalid";
    return false;
  }

  // calculate white noise limit
  double sumY, sumY2;
  sumY = sumY2 = 0;

  int i;
  double yi;

  for (i = minWhiteNoiseIndex; i < inputVectorX->length(); i++) {
    yi = inputVectorY->value()[i];
    sumY    +=  yi;
    sumY2   +=  pow(yi,2);
  }

  double ybar, ysigma;
  ybar = sumY/(inputVectorX->length() - minWhiteNoiseIndex);
  ysigma = sqrt((sumY2 - 2*ybar*sumY + pow(ybar,2)*(inputVectorX->length() - minWhiteNoiseIndex))/(inputVectorX->length() - minWhiteNoiseIndex));
  // end calculate white noise limit

  double effectiveBandwidth = 2*samplingFrequency*pow(radiometerConstantK*inputVectorY->value()[0]/ysigma,2);

  // output fit data
  outputScalarLimit->setValue(ybar);
  outputScalarSigma->setValue(ysigma);
  outputScalarBandwidth->setValue(effectiveBandwidth);

  return true;
}


Kst::VectorPtr EffectiveBandwidthSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr EffectiveBandwidthSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::ScalarPtr EffectiveBandwidthSource::scalarMin() const {
  return _inputScalars[SCALAR_IN_MIN];
}


Kst::ScalarPtr EffectiveBandwidthSource::scalarFreq() const {
  return _inputScalars[SCALAR_IN_FREQ];
}


Kst::ScalarPtr EffectiveBandwidthSource::scalarK() const {
  return _inputScalars[SCALAR_IN_K];
}


QStringList EffectiveBandwidthSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList EffectiveBandwidthSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_MIN);
  scalars += SCALAR_IN_FREQ;
  scalars += SCALAR_IN_K;
  return scalars;
}


QStringList EffectiveBandwidthSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList EffectiveBandwidthSource::outputVectorList() const {
  return QStringList( /*VECTOR_OUT*/ );
}


QStringList EffectiveBandwidthSource::outputScalarList() const {
  QStringList scalars(SCALAR_OUT_LIMIT);
  scalars += SCALAR_OUT_SIGMA;
  scalars += SCALAR_OUT_BANDWIDTH;
  return scalars;
}


QStringList EffectiveBandwidthSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void EffectiveBandwidthSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString EffectiveBandwidthPlugin::pluginName() const { return "Effective Bandwidth"; }
QString EffectiveBandwidthPlugin::pluginDescription() const { return "Calculates effective bandwidth from an amplitude spectrum."; }


Kst::DataObject *EffectiveBandwidthPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigEffectiveBandwidthPlugin* config = static_cast<ConfigEffectiveBandwidthPlugin*>(configWidget)) {

    EffectiveBandwidthSource* object = store->createObject<EffectiveBandwidthSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputScalar(SCALAR_IN_MIN, config->selectedScalarMin());
      object->setInputScalar(SCALAR_IN_FREQ, config->selectedScalarFreq());
      object->setInputScalar(SCALAR_IN_K, config->selectedScalarK());
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


Kst::DataObjectConfigWidget *EffectiveBandwidthPlugin::configWidget(QSettings *settingsObject) const {
  ConfigEffectiveBandwidthPlugin *widget = new ConfigEffectiveBandwidthPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, EffectiveBandwidthPlugin)

// vim: ts=2 sw=2 et
