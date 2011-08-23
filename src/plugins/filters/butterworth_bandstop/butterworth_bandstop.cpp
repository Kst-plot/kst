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


#include "butterworth_bandstop.h"
#include "objectstore.h"
#include "ui_filterbutterworthbandstopconfig.h"

#include "../filters.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_ORDER_IN = "Order Scalar";
static const QString& SCALAR_RATE_IN = "Central Frequency / Sample Rate Scalar";
static const QString& SCALAR_BANDWIDTH_IN = "Band width Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigFilterButterworthBandStopPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterButterworthBandStopConfig {
  public:
    ConfigFilterButterworthBandStopPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterButterworthBandStopConfig() {
      setupUi(this);
    }

    ~ConfigFilterButterworthBandStopPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarRate->setObjectStore(store);
      _scalarBandwidth->setObjectStore(store);
      _scalarOrder->setDefaultValue(4);
      _scalarRate->setDefaultValue(.2);
      _scalarBandwidth->setDefaultValue(.02);
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
      setSelectedVector(vector);
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
      if (FilterButterworthBandStopSource* source = static_cast<FilterButterworthBandStopSource*>(dataObject)) {
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
        _cfg->beginGroup("Filter Band Stop Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Order Scalar", _scalarOrder->selectedScalar()->Name());
        _cfg->setValue("Central Frequency / Sample Rate Scalar", _scalarRate->selectedScalar()->Name());
        _cfg->setValue("Band width Scalar", _scalarBandwidth->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Band Stop Plugin");
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


FilterButterworthBandStopSource::FilterButterworthBandStopSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterButterworthBandStopSource::~FilterButterworthBandStopSource() {
}


QString FilterButterworthBandStopSource::_automaticDescriptiveName() const {
  return QString(vector()->descriptiveName() + " Band Stop");
}


void FilterButterworthBandStopSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigFilterButterworthBandStopPlugin* config = static_cast<ConfigFilterButterworthBandStopPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
    setInputScalar(SCALAR_RATE_IN, config->selectedRateScalar());
    setInputScalar(SCALAR_BANDWIDTH_IN, config->selectedBandwidthScalar());
  }
}


void FilterButterworthBandStopSource::setupOutputs() {
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
    dValue = 1.0 - dValue;
  } else {
    dValue = 0.0;
  }

  return dValue;
}


bool FilterButterworthBandStopSource::algorithm() {

  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr orderScalar = _inputScalars[SCALAR_ORDER_IN];
  Kst::ScalarPtr rateScalar = _inputScalars[SCALAR_RATE_IN];
  Kst::ScalarPtr bandwidthScalar = _inputScalars[SCALAR_BANDWIDTH_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }


  Kst::ScalarList scalars;
  scalars.insert(0, orderScalar);
  scalars.insert(1, rateScalar);
  scalars.insert(2, bandwidthScalar);

  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = i18n("Filtered %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return kst_pass_filter( inputVector, scalars, outputVector);
}


Kst::VectorPtr FilterButterworthBandStopSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterButterworthBandStopSource::orderScalar() const {
  return _inputScalars[SCALAR_ORDER_IN];
}


Kst::ScalarPtr FilterButterworthBandStopSource::rateScalar() const {
  return _inputScalars[SCALAR_RATE_IN];
}


Kst::ScalarPtr FilterButterworthBandStopSource::bandwidthScalar() const {
  return _inputScalars[SCALAR_BANDWIDTH_IN];
}


QStringList FilterButterworthBandStopSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterButterworthBandStopSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_ORDER_IN );
  inputScalars += SCALAR_RATE_IN;
  inputScalars += SCALAR_BANDWIDTH_IN;
  return inputScalars;
}


QStringList FilterButterworthBandStopSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterButterworthBandStopSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterButterworthBandStopSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterButterworthBandStopSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterButterworthBandStopSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString ButterworthBandStopPlugin::pluginName() const { return "Band Stop Filter"; }
QString ButterworthBandStopPlugin::pluginDescription() const { return "Filters a vector with a zero phase band stop filter with a butterworth amplitude response."; }


Kst::DataObject *ButterworthBandStopPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigFilterButterworthBandStopPlugin* config = static_cast<ConfigFilterButterworthBandStopPlugin*>(configWidget)) {

    FilterButterworthBandStopSource* object = store->createObject<FilterButterworthBandStopSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
      object->setInputScalar(SCALAR_RATE_IN, config->selectedRateScalar());
      object->setInputScalar(SCALAR_BANDWIDTH_IN, config->selectedBandwidthScalar());
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


Kst::DataObjectConfigWidget *ButterworthBandStopPlugin::configWidget(QSettings *settingsObject) const {
  ConfigFilterButterworthBandStopPlugin *widget = new ConfigFilterButterworthBandStopPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ButterworthBandStopPlugin, ButterworthBandStopPlugin)

// vim: ts=2 sw=2 et
