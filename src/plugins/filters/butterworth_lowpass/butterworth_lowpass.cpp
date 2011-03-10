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


#include "butterworth_lowpass.h"
#include "objectstore.h"
#include "ui_filterbutterworthlowpassconfig.h"
#include "scalar.h"

#include "../filters.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_ORDER_IN = "Order Scalar";
static const QString& SCALAR_CUTOFF_IN = "Cutoff / Spacing Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigFilterButterworthLowPassPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterButterworthLowPassConfig {
  public:
    ConfigFilterButterworthLowPassPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterButterworthLowPassConfig() {
      setupUi(this);
    }

    ~ConfigFilterButterworthLowPassPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarCutoff->setObjectStore(store);
      _scalarOrder->setDefaultValue(4);
      _scalarCutoff->setDefaultValue(0.1);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarOrder, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarCutoff, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedOrderScalar() { return _scalarOrder->selectedScalar(); };
    void setSelectedOrderScalar(Kst::ScalarPtr scalar) { return _scalarOrder->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedCutoffScalar() { return _scalarCutoff->selectedScalar(); };
    void setSelectedCutoffScalar(Kst::ScalarPtr scalar) { return _scalarCutoff->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterButterworthLowPassSource* source = static_cast<FilterButterworthLowPassSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedOrderScalar(source->orderScalar());
        setSelectedCutoffScalar(source->cutoffScalar());
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
        _cfg->beginGroup("Filter Low Pass Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Order Scalar", _scalarOrder->selectedScalar()->Name());
        _cfg->setValue("Cutoff / Spacing Scalar", _scalarCutoff->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Low Pass Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Order Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* orderScalar = static_cast<Kst::Scalar*>(object);
        if (orderScalar) {
          setSelectedOrderScalar(orderScalar);
        }
        scalarName = _cfg->value("Cutoff / Spacing Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* cutoffScalar = static_cast<Kst::Scalar*>(object);
        if (cutoffScalar) {
          setSelectedCutoffScalar(cutoffScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterButterworthLowPassSource::FilterButterworthLowPassSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterButterworthLowPassSource::~FilterButterworthLowPassSource() {
}


QString FilterButterworthLowPassSource::_automaticDescriptiveName() const {
  return QString(vector()->descriptiveName() + " Low Pass");
}


void FilterButterworthLowPassSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigFilterButterworthLowPassPlugin* config = static_cast<ConfigFilterButterworthLowPassPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
    setInputScalar(SCALAR_CUTOFF_IN, config->selectedCutoffScalar());
  }
}


void FilterButterworthLowPassSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


double filter_calculate( double dFreqValue, Kst::ScalarList scalars ) {
  double dValue;

  dValue = 1.0 / ( 1.0 + pow( dFreqValue / scalars.at(1)->value(), 2.0 * scalars.at(0)->value() ) ); 

  return dValue;
}


bool FilterButterworthLowPassSource::algorithm() {

  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr orderScalar = _inputScalars[SCALAR_ORDER_IN];
  Kst::ScalarPtr cutoffScalar = _inputScalars[SCALAR_CUTOFF_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }

  Kst::ScalarList scalars;
  scalars.insert(0, orderScalar);
  scalars.insert(1, cutoffScalar);

  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = i18n("Filtered %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);
  return kst_pass_filter( inputVector, scalars, outputVector);
}


Kst::VectorPtr FilterButterworthLowPassSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterButterworthLowPassSource::cutoffScalar() const {
  return _inputScalars[SCALAR_CUTOFF_IN];
}


Kst::ScalarPtr FilterButterworthLowPassSource::orderScalar() const {
  return _inputScalars[SCALAR_ORDER_IN];
}


QStringList FilterButterworthLowPassSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterButterworthLowPassSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_CUTOFF_IN );
  inputScalars += SCALAR_ORDER_IN;
  return inputScalars;
}


QStringList FilterButterworthLowPassSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterButterworthLowPassSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterButterworthLowPassSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterButterworthLowPassSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterButterworthLowPassSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString ButterworthLowPassPlugin::pluginName() const { return "Low Pass Filter"; }
QString ButterworthLowPassPlugin::pluginDescription() const { return "Filters a vector with a zero phase low pass filter with a butterworth amplitude response."; }


Kst::DataObject *ButterworthLowPassPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigFilterButterworthLowPassPlugin* config = static_cast<ConfigFilterButterworthLowPassPlugin*>(configWidget)) {

    FilterButterworthLowPassSource* object = store->createObject<FilterButterworthLowPassSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputScalar(SCALAR_CUTOFF_IN, config->selectedCutoffScalar());
      object->setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
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


Kst::DataObjectConfigWidget *ButterworthLowPassPlugin::configWidget(QSettings *settingsObject) const {
  ConfigFilterButterworthLowPassPlugin *widget = new ConfigFilterButterworthLowPassPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ButterworthLowPassPlugin, ButterworthLowPassPlugin)

// vim: ts=2 sw=2 et
