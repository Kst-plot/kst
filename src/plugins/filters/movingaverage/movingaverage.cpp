/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2019 Jonathan Liu                                     *
 *                   net147@gmail.com                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "movingaverage.h"
#include "objectstore.h"
#include "ui_movingaverageconfig.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_IN = "Samples Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigMovingAveragePlugin : public Kst::DataObjectConfigWidget, public Ui_MovingAverageConfig {
  public:
    ConfigMovingAveragePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_MovingAverageConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigMovingAveragePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) {
      _store = store;
      _vector->setObjectStore(store);
      _scalarSamples->setObjectStore(store);
      _weighted->setChecked(false);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarSamples, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_weighted, SIGNAL(toggled(bool)), dialog, SIGNAL(modified()));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVector(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      setSelectedVector(vector);
    }

    void setVectorsLocked(bool locked = true) {
      _vector->setEnabled(!locked);
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); }
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); }

    Kst::ScalarPtr selectedScalar() { return _scalarSamples->selectedScalar(); }
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarSamples->setSelectedScalar(scalar); }

    bool weighted() const { return _weighted->isChecked(); }
    void setWeighted(bool value) { _weighted->setChecked(value); }

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (MovingAverageSource* source = static_cast<MovingAverageSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarSamples());
        setWeighted(source->weighted());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

      QStringView av = attrs.value("Weighted");
      if (!av.isNull()) {
        setWeighted(QVariant(av.toString()).toBool());
      } else {
        setWeighted(false);
      }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Moving Average DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _scalarSamples->selectedScalar()->Name());
        _cfg->setValue("Weighted", weighted());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Moving Average DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar").toString();
        _scalarSamples->setSelectedScalar(scalarName);
        setWeighted(_cfg->value("Weighted", false).toBool());

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


MovingAverageSource::MovingAverageSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


MovingAverageSource::~MovingAverageSource() {
}


QString MovingAverageSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Moving Average").arg(vector()->descriptiveName());
  } else {
    return tr("Moving Average");
  }
}

QString MovingAverageSource::descriptionTip() const {
  QString tip;

  tip = tr("Moving Average: %1\n").arg(Name());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void MovingAverageSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigMovingAveragePlugin* config = static_cast<ConfigMovingAveragePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
    _weighted = config->weighted();
  }
}


void MovingAverageSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool MovingAverageSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }


  /* Memory allocation */
  outputVector->resize(inputVector->length(), true);

  double const *v_in = inputVector->noNanValue();
  double *v_out = outputVector->raw_V_ptr();
  int s_in = int(inputScalar->value());

  if (_weighted) {
    for (int i = 0; i < inputVector->length(); ++i) {
      double average = 0;
      int samples = qMin(i + 1, s_in);
      double weightingDivisor = samples * (samples + 1) / 2;
      int firstIndex = i - samples + 1;

      for (int j = firstIndex; j <= i; ++j)
        average += v_in[j] * (j - firstIndex + 1) / weightingDivisor;

      v_out[i] = average;
    }
  } else {
    for (int i = 0; i < inputVector->length(); ++i) {
      double sum = 0;
      int samples = qMin(i + 1, s_in);

      for (int j = i - samples + 1; j <= i; ++j)
        sum += v_in[j];

      v_out[i] = sum / samples;
    }
  }

  return true;
}


Kst::VectorPtr MovingAverageSource::vector() const {
  return _inputVectors[VECTOR_IN];
}

Kst::ScalarPtr MovingAverageSource::scalarSamples() const {
  return _inputScalars[SCALAR_IN];
}


QStringList MovingAverageSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList MovingAverageSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList MovingAverageSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList MovingAverageSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList MovingAverageSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList MovingAverageSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}

void MovingAverageSource::setProperty(const QString &key, const QString &val) {
    qDebug() << "setProperty" << key << val;
  if (key == "Weighted") {
    if (val.toLower() == "true") {
      setWeighted(true);
    } else if (val.toLower() == "false") {
      setWeighted(false);
    }
  }
}

void MovingAverageSource::saveProperties(QXmlStreamWriter &s) {
  s.writeAttribute("Weighted", QString::number(weighted()));
}


QString MovingAveragePlugin::pluginName() const { return tr("Moving Average"); }
QString MovingAveragePlugin::pluginDescription() const { return tr("Computes the moving average of the input vector."); }


Kst::DataObject *MovingAveragePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigMovingAveragePlugin* config = static_cast<ConfigMovingAveragePlugin*>(configWidget)) {

    MovingAverageSource* object = store->createObject<MovingAverageSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
      object->setupOutputs();
      object->setInputVector(VECTOR_IN, config->selectedVector());
    }

    object->setWeighted(config->weighted());
    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *MovingAveragePlugin::configWidget(QSettings *settingsObject) const {
  ConfigMovingAveragePlugin *widget = new ConfigMovingAveragePlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
