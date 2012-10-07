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


#include "cumulativeaverage.h"
#include "objectstore.h"
#include "ui_cumulativeaverageconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& VECTOR_OUT = "Avg(Y)";

class ConfigCumulativeAveragePlugin : public Kst::DataObjectConfigWidget, public Ui_CumulativeAverageConfig {
  public:
    ConfigCumulativeAveragePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_CumulativeAverageConfig() {
      setupUi(this);
    }

    ~ConfigCumulativeAveragePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (CumulativeAverageSource* source = static_cast<CumulativeAverageSource*>(dataObject)) {
        setSelectedVector(source->vector());
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
        _cfg->beginGroup("Cumulative Average DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Cumulative Average DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


CumulativeAverageSource::CumulativeAverageSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


CumulativeAverageSource::~CumulativeAverageSource() {
}


QString CumulativeAverageSource::_automaticDescriptiveName() const {
  if (vector()) {
    return QString(vector()->descriptiveName() + " Cumulative Average");
  } else {
    return QString("Cumulative Average");
  }
}

QString CumulativeAverageSource::descriptionTip() const {
  QString tip;

  tip = i18n("Cumulative Average: %1\n").arg(Name());

  tip += i18n("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void CumulativeAverageSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigCumulativeAveragePlugin* config = static_cast<ConfigCumulativeAveragePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void CumulativeAverageSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


// TODO
bool CumulativeAverageSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }


  /* Memory allocation */
  outputVector->resize(inputVector->length(), true);

  outputVector->value()[0] = inputVector->value()[0]; // i = 1

  for (int i = 1; i < inputVector->length(); ++i) {
    outputVector->value()[i] = (inputVector->value()[i] + (i * outputVector->value()[i-1])) / (i+1);
  }

  return true;
}


Kst::VectorPtr CumulativeAverageSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


QStringList CumulativeAverageSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList CumulativeAverageSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList CumulativeAverageSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList CumulativeAverageSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList CumulativeAverageSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList CumulativeAverageSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void CumulativeAverageSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString CumulativeAveragePlugin::pluginName() const { return "Cumulative Average"; }
QString CumulativeAveragePlugin::pluginDescription() const { return "Computes the cumulative average of the input vector."; }


Kst::DataObject *CumulativeAveragePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigCumulativeAveragePlugin* config = static_cast<ConfigCumulativeAveragePlugin*>(configWidget)) {

    CumulativeAverageSource* object = store->createObject<CumulativeAverageSource>();

    if (setupInputsOutputs) {
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


Kst::DataObjectConfigWidget *CumulativeAveragePlugin::configWidget(QSettings *settingsObject) const {
  ConfigCumulativeAveragePlugin *widget = new ConfigCumulativeAveragePlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, CumulativeAveragePlugin)
#endif

// vim: ts=2 sw=2 et
