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

#include "butterworth_bandpass.h"
#include "objectstore.h"
#include "ui_filterbutterworthbandpassconfig.h"

#include "../filters.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_ORDER_IN = "Order Scalar";
static const QString& SCALAR_RATE_IN = "Central Frequency / Sample Rate Scalar";
static const QString& SCALAR_BANDWIDTH_IN = "Band width Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigFilterButterworthBandPassPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterButterworthBandPassConfig {
  public:
    ConfigFilterButterworthBandPassPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterButterworthBandPassConfig() {
      _store = 0;

      setupUi(this);
      _scalarRate->setIsFOverSR(true);
      _scalarBandwidth->setIsFOverSR(true);
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
        connect(_scalarRate, SIGNAL(SRChanged(QString)), _scalarBandwidth, SLOT(setSR(QString)));
        connect(_scalarBandwidth, SIGNAL(SRChanged(QString)), _scalarRate, SLOT(setSR(QString)));
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
        _cfg->beginGroup("Filter Band Pass Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Order Scalar", _scalarOrder->selectedScalar()->descriptiveName());
        _cfg->setValue("Central Frequency / Sample Rate Scalar", _scalarRate->selectedScalar()->descriptiveName());
        _cfg->setValue("Band width Scalar", _scalarBandwidth->selectedScalar()->descriptiveName());
        _cfg->setValue("Sample Rate", _scalarRate->SR());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Band Pass Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }

        QString scalarName = _cfg->value("Order Scalar").toString();
        _scalarOrder->setSelectedScalar(scalarName);

        _scalarRate->setSR(_cfg->value("Sample Rate", 1.0).toString());
        _scalarBandwidth->setSR(_cfg->value("Sample Rate", 1.0).toString());

        scalarName = _cfg->value("Central Frequency / Sample Rate Scalar").toString();
        _scalarRate->setSelectedScalar(scalarName);

        scalarName = _cfg->value("Band width Scalar").toString();
        _scalarBandwidth->setSelectedScalar(scalarName);

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
  return tr("%1 Band Pass", "arg1 is a vector").arg(vector()->descriptiveName());
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

int min_pad(Kst::ScalarList scalars) {
  double hp = scalars.at(1)->value() - 0.5*scalars.at(2)->value();

  if (hp >0) {
    return int (1.0/hp);
  } else {
    return 0.0;
  }
}


double filter_calculate( double dFreqValue, Kst::ScalarList scalars ) {
  double dValue;

  int order2 = int(2.0 * scalars.at(0)->value());
  double lowpass  = scalars.at(1)->value() + 0.5*scalars.at(2)->value();
  double highpass = scalars.at(1)->value() - 0.5*scalars.at(2)->value();

  //if( dFreqValue > 0.0 ) {
  dValue = 1.0 / ( 1.0 + gsl_pow_int( dFreqValue / lowpass, order2) );
  dValue *= 1.0 / ( 1.0 + gsl_pow_int( highpass / dFreqValue, order2) );
  //} else {
  //  dValue = 0.0;
  //}

  return dValue;
}


bool FilterButterworthBandPassSource::algorithm() {

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
  label_info.name = tr("Filtered %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

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
QString ButterworthBandPassPlugin::pluginName() const { return tr("Band Pass Filter"); }
QString ButterworthBandPassPlugin::pluginDescription() const { return tr("Filters a vector with a zero phase band pass filter with a butterworth amplitude response."); }


Kst::DataObject *ButterworthBandPassPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigFilterButterworthBandPassPlugin* config = static_cast<ConfigFilterButterworthBandPassPlugin*>(configWidget)) {

    FilterButterworthBandPassSource* object = store->createObject<FilterButterworthBandPassSource>();

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


Kst::DataObjectConfigWidget *ButterworthBandPassPlugin::configWidget(QSettings *settingsObject) const {

  ConfigFilterButterworthBandPassPlugin *widget = new ConfigFilterButterworthBandPassPlugin(settingsObject);

  return widget;
}

// vim: ts=2 sw=2 et
