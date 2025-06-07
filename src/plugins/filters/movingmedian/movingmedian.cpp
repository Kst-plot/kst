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


#include "movingmedian.h"
#include "objectstore.h"
#include "ui_movingmedianconfig.h"
#include <algorithm>

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_IN = "Samples Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigMovingMedianPlugin : public Kst::DataObjectConfigWidget, public Ui_MovingMedianConfig {
  public:
    ConfigMovingMedianPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_MovingMedianConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigMovingMedianPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) {
      _store = store;
      _vector->setObjectStore(store);
      _scalarSamples->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarSamples, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (MovingMedianSource* source = static_cast<MovingMedianSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarSamples());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

//       QStringView av;
//       av = attrs.value("value");
//       if (!av.isNull()) {
//         _configValue = QVariant(av.toString()).toBool();
//       }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Moving Median DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _scalarSamples->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Moving Median DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar").toString();
        _scalarSamples->setSelectedScalar(scalarName);

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


MovingMedianSource::MovingMedianSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


MovingMedianSource::~MovingMedianSource() {
}


QString MovingMedianSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Moving Median").arg(vector()->descriptiveName());
  } else {
    return tr("Moving Median");
  }
}

QString MovingMedianSource::descriptionTip() const {
  QString tip;

  tip = tr("Moving Median: %1\n").arg(Name());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void MovingMedianSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigMovingMedianPlugin* config = static_cast<ConfigMovingMedianPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void MovingMedianSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


// TODO
bool MovingMedianSource::algorithm() {
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
  static QVector<double> buffer;

  buffer.reserve(s_in);

  for (int i = 0; i < inputVector->length(); ++i) {
    int divisor = qMin(i + 1, s_in);

    buffer.clear();

    for (int j = i - divisor + 1; j <= i; ++j)
      buffer.append(v_in[j]);

    std::stable_sort(buffer.begin(), buffer.end());

    int middleIndex = buffer.size() / 2;

    if (buffer.size() % 2 == 0)
      v_out[i] = (buffer.at(middleIndex - 1) + buffer.at(middleIndex)) / 2;
    else
      v_out[i] = buffer.at(middleIndex);
  }

  return true;
}


Kst::VectorPtr MovingMedianSource::vector() const {
  return _inputVectors[VECTOR_IN];
}

Kst::ScalarPtr MovingMedianSource::scalarSamples() const {
  return _inputScalars[SCALAR_IN];
}


QStringList MovingMedianSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList MovingMedianSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList MovingMedianSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList MovingMedianSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList MovingMedianSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList MovingMedianSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void MovingMedianSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString MovingMedianPlugin::pluginName() const { return tr("Moving Median"); }
QString MovingMedianPlugin::pluginDescription() const { return tr("Computes the moving median of the input vector."); }


Kst::DataObject *MovingMedianPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigMovingMedianPlugin* config = static_cast<ConfigMovingMedianPlugin*>(configWidget)) {

    MovingMedianSource* object = store->createObject<MovingMedianSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
      object->setupOutputs();
      object->setInputVector(VECTOR_IN, config->selectedVector());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *MovingMedianPlugin::configWidget(QSettings *settingsObject) const {
  ConfigMovingMedianPlugin *widget = new ConfigMovingMedianPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
