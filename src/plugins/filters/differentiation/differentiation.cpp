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


#include "differentiation.h"
#include "objectstore.h"
#include "ui_differentiationconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scale Scalar";
static const QString& VECTOR_OUT = "dY/dX";

class ConfigDifferentiationPlugin : public Kst::DataObjectConfigWidget, public Ui_DifferentiationConfig {
  public:
    ConfigDifferentiationPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_DifferentiationConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigDifferentiationPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
      _scalarStep->setObjectStore(store);
      _scalarStep->setDefaultValue(1.0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarStep, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalar() { return _scalarStep->selectedScalar(); };
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarStep->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (DifferentiationSource* source = static_cast<DifferentiationSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarStep());
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
        _cfg->beginGroup("Differentiation DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _scalarStep->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Differentiation DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar").toString();
        _scalarStep->setSelectedScalar(scalarName);

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


DifferentiationSource::DifferentiationSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


DifferentiationSource::~DifferentiationSource() {
}


QString DifferentiationSource::_automaticDescriptiveName() const {
    if (vector()) {
      return tr("%1 Derivative").arg(vector()->descriptiveName());
    } else {
      return tr("Derivative");
    }
}

QString DifferentiationSource::descriptionTip() const {
  QString tip;

  tip = tr("Derivative: %1\n  dX: %2\n").arg(Name()).arg(scalarStep()->value());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void DifferentiationSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigDifferentiationPlugin* config = static_cast<ConfigDifferentiationPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void DifferentiationSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool DifferentiationSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  if (inputScalar->value() == 0) {
    _errorString = tr("Error:  Input Scalar Step must be not be 0.");
    return false;
  }

  /* Memory allocation */
  outputVector->resize(inputVector->length(), true);

  int i = 0;
  for (; i < inputVector->length()-1; i++) {
      outputVector->raw_V_ptr()[i] = (inputVector->value()[i+1] - inputVector->value()[i]) / inputScalar->value();
  }

  // Repeat the last point to keep the vector length, even though it does not bring much additional info
  outputVector->raw_V_ptr()[i] = (inputVector->value()[i] - inputVector->value()[i-1]) / inputScalar->value();
  return true;
}


Kst::VectorPtr DifferentiationSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr DifferentiationSource::scalarStep() const {
  return _inputScalars[SCALAR_IN];
}


QStringList DifferentiationSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList DifferentiationSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList DifferentiationSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList DifferentiationSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList DifferentiationSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList DifferentiationSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void DifferentiationSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString DifferentiationPlugin::pluginName() const { return tr("Fixed Step Differentiation"); }
QString DifferentiationPlugin::pluginDescription() const { return tr("Computes the discrete derivative of an input vector"); }


Kst::DataObject *DifferentiationPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigDifferentiationPlugin* config = static_cast<ConfigDifferentiationPlugin*>(configWidget)) {

    DifferentiationSource* object = store->createObject<DifferentiationSource>();

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


Kst::DataObjectConfigWidget *DifferentiationPlugin::configWidget(QSettings *settingsObject) const {
  ConfigDifferentiationPlugin *widget = new ConfigDifferentiationPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
