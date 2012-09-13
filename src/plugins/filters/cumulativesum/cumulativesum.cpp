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


#include "cumulativesum.h"
#include "objectstore.h"
#include "ui_cumulativesumconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scale Scalar";
static const QString& VECTOR_OUT = "sum(Y)dX";

class ConfigCumulativeSumPlugin : public Kst::DataObjectConfigWidget, public Ui_CumulativeSumConfig {
  public:
    ConfigCumulativeSumPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_CumulativeSumConfig() {
      setupUi(this);
    }

    ~ConfigCumulativeSumPlugin() {}

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
      if (CumulativeSumSource* source = static_cast<CumulativeSumSource*>(dataObject)) {
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
        _cfg->beginGroup("Cumulative Sum DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _scalarStep->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Cumulative Sum DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarStep = static_cast<Kst::Scalar*>(object);
        if (scalarStep) {
          setSelectedScalar(scalarStep);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


CumulativeSumSource::CumulativeSumSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


CumulativeSumSource::~CumulativeSumSource() {
}


QString CumulativeSumSource::_automaticDescriptiveName() const {
  if (vector()) {
    return QString(vector()->descriptiveName() + " Integral");
  } else {
    return QString("Integral");
  }
}

QString CumulativeSumSource::descriptionTip() const {
  QString tip;

  tip = i18n("Integral: %1\n  dX: %2\n").arg(Name()).arg(scalarStep()->value());

  tip += i18n("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void CumulativeSumSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigCumulativeSumPlugin* config = static_cast<ConfigCumulativeSumPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void CumulativeSumSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool CumulativeSumSource::algorithm() {
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

  outputVector->value()[0] = inputVector->value()[0];

  for (int i = 0; i < inputVector->length()-1; i++) {
    outputVector->value()[i+1] = inputVector->value()[i]*inputScalar->value() + outputVector->value()[i];
  }

  return true;
}


Kst::VectorPtr CumulativeSumSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr CumulativeSumSource::scalarStep() const {
  return _inputScalars[SCALAR_IN];
}


QStringList CumulativeSumSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList CumulativeSumSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList CumulativeSumSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList CumulativeSumSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList CumulativeSumSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList CumulativeSumSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void CumulativeSumSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString CumulativeSumPlugin::pluginName() const { return "Cumulative Sum"; }
QString CumulativeSumPlugin::pluginDescription() const { return "Computes the cumulative sum (integral) of the input vector."; }


Kst::DataObject *CumulativeSumPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigCumulativeSumPlugin* config = static_cast<ConfigCumulativeSumPlugin*>(configWidget)) {

    CumulativeSumSource* object = store->createObject<CumulativeSumSource>();

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


Kst::DataObjectConfigWidget *CumulativeSumPlugin::configWidget(QSettings *settingsObject) const {
  ConfigCumulativeSumPlugin *widget = new ConfigCumulativeSumPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, CumulativeSumPlugin)
#endif

// vim: ts=2 sw=2 et
