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


#include "shift.h"
#include "objectstore.h"
#include "ui_shiftconfig.h"
#include "math_kst.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Shift value (# points, negative allowed)";
static const QString& VECTOR_OUT = "Shifted Vector";

class ConfigWidgetShiftPlugin : public Kst::DataObjectConfigWidget, public Ui_ShiftConfig {
  public:
    ConfigWidgetShiftPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_ShiftConfig() {
      setupUi(this);
    }

    ~ConfigWidgetShiftPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
      _scalarShift->setObjectStore(store);
      _scalarShift->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarShift, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalar() { return _scalarShift->selectedScalar(); };
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarShift->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (ShiftSource* source = static_cast<ShiftSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalar());
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
        _cfg->beginGroup("Shift DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar Shift", _scalarShift->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Shift DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar Shift").toString();
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


ShiftSource::ShiftSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


ShiftSource::~ShiftSource() {
}


QString ShiftSource::_automaticDescriptiveName() const {
  return QString("Shift Plugin Object");
}


void ShiftSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetShiftPlugin* config = static_cast<ConfigWidgetShiftPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void ShiftSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool ShiftSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  int delay = 0;

  /* Memory allocation */
  outputVector->resize(inputVector->length(), false);

  delay = (int)inputScalar->value();
  /* Protect against invalid inputs */
  if (delay > inputVector->length()) {
    delay = inputVector->length();
  } else if (delay < -inputVector->length()) {
    delay = -inputVector->length();
  }

  /* First case: positive shift (forwards/right shift)*/
  if (delay >= 0) {
    /* Pad beginning with zeros */
    for (int i = 0; i < delay; i++) {
      outputVector->value()[i] = NAN;
    }
    /* Then, copy values with the right offset */
    for (int i = delay; i < inputVector->length(); i++) {
      outputVector->value()[i] = inputVector->value()[i-delay];
    }
  }

  /* Second case: negative shift (backwards/left shift)*/
  else if (delay < 0) {
    delay = -delay; /* Easier to visualize :-) */
    /* Copy values with the right offset */
    for (int i = 0; i < inputVector->length()-delay; i++) {
      outputVector->value()[i] = inputVector->value()[i+delay];
    }
    /* Pad end with zeros */
    for (int i = inputVector->length()-delay; i < inputVector->length(); i++) {
      outputVector->value()[i] = NAN;
    }
  }
  return true;
}


Kst::VectorPtr ShiftSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr ShiftSource::scalar() const {
  return _inputScalars[SCALAR_IN];
}


QStringList ShiftSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList ShiftSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList ShiftSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList ShiftSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList ShiftSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList ShiftSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void ShiftSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString ShiftPlugin::pluginName() const { return "Shift"; }
QString ShiftPlugin::pluginDescription() const { return "Shifts (and truncates) a vector."; }


Kst::DataObject *ShiftPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetShiftPlugin* config = static_cast<ConfigWidgetShiftPlugin*>(configWidget)) {

    ShiftSource* object = store->createObject<ShiftSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
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


Kst::DataObjectConfigWidget *ShiftPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetShiftPlugin *widget = new ConfigWidgetShiftPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, ShiftPlugin)

// vim: ts=2 sw=2 et
