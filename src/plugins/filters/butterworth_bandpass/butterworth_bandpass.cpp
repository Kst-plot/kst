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


#include "butterworth_bandpass.h"
#include "objectstore.h"
#include "ui_filterbutterworthbandpassconfig.h"

#include "../filters.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_ORDER_IN = "Order Scalar";
static const QString& SCALAR_RATE_IN = "Central Frequency / Sample Rate Scalar";
static const QString& SCALAR_BANDWIDTH_IN = "Band width Scalar";
static const QString& VECTOR_OUT = "Filtered";

class ConfigFilterButterworthBandPassPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterButterworthBandPassConfig {
  public:
    ConfigFilterButterworthBandPassPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterButterworthBandPassConfig() {
      setupUi(this);
    }

    ~ConfigFilterButterworthBandPassPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarRate->setObjectStore(store);
      _scalarBandwidth->setObjectStore(store);
      _scalarOrder->setDefaultValue(4);
      _scalarRate->setDefaultValue(0.052);
      _scalarBandwidth->setDefaultValue(0.096);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarOrder, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarRate, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarBandwidth, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVector(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      Q_UNUSED(vector);
    }

    void setVectorsLocked(bool locked = true) {
      _vector->setEnabled(!locked);
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedOrderScalar() { return _scalarOrder->selectedScalar(); };
    void setSelectedOrderScalar(Kst::ScalarPtr scalar) { return _scalarOrder->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedRateScalar() { return _scalarRate->selectedScalar(); };
    void setSelectedRateScalar(Kst::ScalarPtr scalar) { return _scalarRate->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedBandwidthScalar() { return _scalarBandwidth->selectedScalar(); };
    void setSelectedBandwidthScalar(Kst::ScalarPtr scalar) { return _scalarBandwidth->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterButterworthBandPassSource* source = static_cast<FilterButterworthBandPassSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedOrderScalar(source->orderScalar());
        setSelectedRateScalar(source->rateScalar());
        setSelectedBandwidthScalar(source->bandwidthScalar());
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
        _cfg->beginGroup("Filter Butterworth Band Pass Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Order Scalar", _scalarOrder->selectedScalar()->Name());
        _cfg->setValue("Central Frequency / Sample Rate Scalar", _scalarRate->selectedScalar()->Name());
        _cfg->setValue("Band width Scalar", _scalarBandwidth->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Butterworth Band Pass Plugin");
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
        scalarName = _cfg->value("Central Frequency / Sample Rate Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* rateScalar = static_cast<Kst::Scalar*>(object);
        if (rateScalar) {
          setSelectedRateScalar(rateScalar);
        }
        scalarName = _cfg->value("Band width Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* bandwidthScalar = static_cast<Kst::Scalar*>(object);
        if (bandwidthScalar) {
          setSelectedBandwidthScalar(bandwidthScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterButterworthBandPassSource::FilterButterworthBandPassSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterButterworthBandPassSource::~FilterButterworthBandPassSource() {
}


QString FilterButterworthBandPassSource::_automaticDescriptiveName() const {
  return QString("Butterworth Band Pass Filtered Object");
}


void FilterButterworthBandPassSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigFilterButterworthBandPassPlugin* config = static_cast<ConfigFilterButterworthBandPassPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
    setInputScalar(SCALAR_RATE_IN, config->selectedRateScalar());
    setInputScalar(SCALAR_BANDWIDTH_IN, config->selectedBandwidthScalar());
  }
}


void FilterButterworthBandPassSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


double filter_calculate( double dFreqValue, Kst::ScalarList scalars ) {
  double dValue;

  int order2 = int(2.0 * scalars.at(0)->value());
  double lowpass  = scalars.at(1)->value() + 0.5*scalars.at(2)->value();
  double highpass = scalars.at(1)->value() - 0.5*scalars.at(2)->value();

  if( dFreqValue > 0.0 ) {
    dValue = 1.0 / ( 1.0 + gsl_pow_int( dFreqValue / lowpass, order2) );
    dValue *= 1.0 / ( 1.0 + gsl_pow_int( highpass / dFreqValue, order2) );
  } else {
    dValue = 0.0;
  }

  return dValue;
}


bool FilterButterworthBandPassSource::algorithm() {

  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr orderScalar = _inputScalars[SCALAR_ORDER_IN];
  Kst::ScalarPtr rateScalar = _inputScalars[SCALAR_RATE_IN];
  Kst::ScalarPtr bandwidthScalar = _inputScalars[SCALAR_BANDWIDTH_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  Kst::ScalarList scalars;
  scalars.insert(0, orderScalar);
  scalars.insert(1, rateScalar);
  scalars.insert(2, bandwidthScalar);

  return kst_pass_filter( inputVector, scalars, outputVector);
}


Kst::VectorPtr FilterButterworthBandPassSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterButterworthBandPassSource::orderScalar() const {
  return _inputScalars[SCALAR_ORDER_IN];
}


Kst::ScalarPtr FilterButterworthBandPassSource::rateScalar() const {
  return _inputScalars[SCALAR_RATE_IN];
}


Kst::ScalarPtr FilterButterworthBandPassSource::bandwidthScalar() const {
  return _inputScalars[SCALAR_BANDWIDTH_IN];
}


QStringList FilterButterworthBandPassSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterButterworthBandPassSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_ORDER_IN );
  inputScalars += SCALAR_RATE_IN;
  inputScalars += SCALAR_BANDWIDTH_IN;
  return inputScalars;
}


QStringList FilterButterworthBandPassSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterButterworthBandPassSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterButterworthBandPassSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterButterworthBandPassSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterButterworthBandPassSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString ButterworthBandPassPlugin::pluginName() const { return "Butterworth Band Pass Filter"; }
QString ButterworthBandPassPlugin::pluginDescription() const { return "Filters a set of data with a zero phase Butterworth band pass filter."; }


Kst::DataObject *ButterworthBandPassPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigFilterButterworthBandPassPlugin* config = static_cast<ConfigFilterButterworthBandPassPlugin*>(configWidget)) {

    FilterButterworthBandPassSource* object = store->createObject<FilterButterworthBandPassSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
      object->setInputScalar(SCALAR_RATE_IN, config->selectedRateScalar());
      object->setInputScalar(SCALAR_BANDWIDTH_IN, config->selectedBandwidthScalar());
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


Kst::DataObjectConfigWidget *ButterworthBandPassPlugin::configWidget(QSettings *settingsObject) const {
  ConfigFilterButterworthBandPassPlugin *widget = new ConfigFilterButterworthBandPassPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ButterworthBandPassPlugin, ButterworthBandPassPlugin)

// vim: ts=2 sw=2 et
