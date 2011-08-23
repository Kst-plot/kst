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

/* 'boxcar decimation filter': not very useful. */

#include "bin.h"
#include "objectstore.h"
#include "ui_binconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scalar In";
static const QString& VECTOR_OUT = "Bins";

class ConfigWidgetBinPlugin : public Kst::DataObjectConfigWidget, public Ui_BinConfig {
  public:
    ConfigWidgetBinPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_BinConfig() {
      setupUi(this);
    }

    ~ConfigWidgetBinPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
      _scalarBin->setObjectStore(store);
      _scalarBin->setDefaultValue(10);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarBin, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalar() { return _scalarBin->selectedScalar(); };
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarBin->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (BinSource* source = static_cast<BinSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->binScalar());
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
        _cfg->beginGroup("Bin DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Bin Scalar", _scalarBin->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Bin DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Bin Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* binScalar = static_cast<Kst::Scalar*>(object);
        if (binScalar) {
          setSelectedScalar(binScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


BinSource::BinSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


BinSource::~BinSource() {
}


QString BinSource::_automaticDescriptiveName() const {
  return QString("Bin Plugin Object");
}


void BinSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetBinPlugin* config = static_cast<ConfigWidgetBinPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
  }
}


void BinSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool BinSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  //Make sure there is at least 1 element in the input vector
  if (inputVector->length() < 1) {
    _errorString = "Error:  Input Vector invalid size";
    return false;
  }
  //Make sure the bin size is at least 1
  if (inputScalar->value() < 1) {
    _errorString = "Error:  No Bins";
    return false;
  }

  // allocate the lengths
  outputVector->resize(int(inputVector->length() / inputScalar->value()), false);

  //now bin the data
  for (int i = 0; i < outputVector->length(); i++)
  {
      outputVector->value()[i] = 0;
      //add up the elements for this bin
      for (int j = 0; j < inputScalar->value(); j++)
      {
          outputVector->value()[i] += inputVector->value()[int(i*inputScalar->value()+j)];
      }
      //find the mean
      outputVector->value()[i] /= inputScalar->value();
  }
  return true;
}


Kst::VectorPtr BinSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr BinSource::binScalar() const {
  return _inputScalars[SCALAR_IN];
}


QStringList BinSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList BinSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList BinSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList BinSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList BinSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList BinSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void BinSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString BinPlugin::pluginName() const { return "Bin"; }
QString BinPlugin::pluginDescription() const { return "Bins data into the given size bins.  Each bin contains the mean of the elements belonging to the bin."; }


Kst::DataObject *BinPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetBinPlugin* config = static_cast<ConfigWidgetBinPlugin*>(configWidget)) {

    BinSource* object = store->createObject<BinSource>();

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


Kst::DataObjectConfigWidget *BinPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetBinPlugin *widget = new ConfigWidgetBinPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, BinPlugin)

// vim: ts=2 sw=2 et
