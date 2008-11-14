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


#include "noiseaddition.h"
#include "objectstore.h"
#include "ui_noiseadditionconfig.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scalar In";
static const QString& VECTOR_OUT = "Vector Out";

class ConfigNoiseAdditionPlugin : public Kst::DataObjectConfigWidget, public Ui_NoiseAdditionConfig {
  public:
    ConfigNoiseAdditionPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_NoiseAdditionConfig() {
      setupUi(this);
    }

    ~ConfigNoiseAdditionPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarSigma->setObjectStore(store);
      _scalarSigma->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarSigma, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalar() { return _scalarSigma->selectedScalar(); };
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarSigma->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (NoiseAdditionSource* source = static_cast<NoiseAdditionSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarSigma());
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
        _cfg->beginGroup("Noise Addition DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar Sigma", _scalarSigma->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Noise Addition DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar Sigma").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalar = static_cast<Kst::Scalar*>(object);
        if (scalar) {
          setSelectedScalar(scalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


NoiseAdditionSource::NoiseAdditionSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


NoiseAdditionSource::~NoiseAdditionSource() {
}


QString NoiseAdditionSource::_automaticDescriptiveName() const {
  return QString("Noise Addition Plugin Object");
}


void NoiseAdditionSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigNoiseAdditionPlugin* config = static_cast<ConfigNoiseAdditionPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void NoiseAdditionSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool NoiseAdditionSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  //Make sure there is at least 1 element in the input vector
  if (inputVector->length() < 1) {
    _errorString = "Error:  Input Vector invalid size";
    return false;
  }

  const gsl_rng_type* pGeneratorType;
  gsl_rng* pRandomNumberGenerator;
  double* pResult[1];
  int iRetVal = false;
  int iLength = inputVector->length();

  outputVector->resize(iLength, false);
  pResult[0] = outputVector->value();

  pGeneratorType = gsl_rng_default;
  pRandomNumberGenerator = gsl_rng_alloc( pGeneratorType );
  if (pRandomNumberGenerator != NULL) {
    if (pResult[0] != NULL) {
      for (int i=0; i<iLength; i++) {
        outputVector->value()[i] = inputVector->value()[i] + gsl_ran_gaussian( pRandomNumberGenerator, inputScalar->value() );
      }

      iRetVal = true;
    }
    gsl_rng_free( pRandomNumberGenerator );
  }

  return true;
}


Kst::VectorPtr NoiseAdditionSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr NoiseAdditionSource::scalarSigma() const {
  return _inputScalars[SCALAR_IN];
}


QStringList NoiseAdditionSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList NoiseAdditionSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList NoiseAdditionSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList NoiseAdditionSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList NoiseAdditionSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList NoiseAdditionSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void NoiseAdditionSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString NoiseAdditionPlugin::pluginName() const { return "Noise Addition"; }
QString NoiseAdditionPlugin::pluginDescription() const { return "Adds Gaussian noise to a set of data, of a specified standard deviation."; }


Kst::DataObject *NoiseAdditionPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigNoiseAdditionPlugin* config = static_cast<ConfigNoiseAdditionPlugin*>(configWidget)) {

    NoiseAdditionSource* object = store->createObject<NoiseAdditionSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
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


Kst::DataObjectConfigWidget *NoiseAdditionPlugin::configWidget(QSettings *settingsObject) const {
  ConfigNoiseAdditionPlugin *widget = new ConfigNoiseAdditionPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, NoiseAdditionPlugin)

// vim: ts=2 sw=2 et
