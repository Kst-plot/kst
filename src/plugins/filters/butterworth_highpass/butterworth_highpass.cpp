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


#include "butterworth_highpass.h"
#include "objectstore.h"
#include "ui_filterbutterworthhighpassconfig.h"

#include "../filters.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_ORDER_IN = "Order Scalar";
static const QString& SCALAR_CUTOFF_IN = "Cutoff / Spacing Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigFilterButterworthHighPassPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterButterworthHighPassConfig {
  public:
    ConfigFilterButterworthHighPassPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterButterworthHighPassConfig() {
      _store = 0;
      setupUi(this);

      _scalarCutoff->setIsFOverSR(true);
    }

    ~ConfigFilterButterworthHighPassPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarOrder->setObjectStore(store);
      _scalarCutoff->setObjectStore(store);
      _scalarOrder->setDefaultValue(4);
      _scalarCutoff->setDefaultValue(0.02);
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
      if (FilterButterworthHighPassSource* source = static_cast<FilterButterworthHighPassSource*>(dataObject)) {
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
        _cfg->beginGroup("Filter High Pass Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Order Scalar", _scalarOrder->selectedScalar()->Name());
        _cfg->setValue("Cutoff / Spacing Scalar", _scalarCutoff->selectedScalar()->Name());
        _cfg->setValue("Sample Rate", _scalarCutoff->SR());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter High Pass Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Order Scalar").toString();
        _scalarOrder->setSelectedScalar(scalarName);

        // set the SR before the cutoff/SampleRate so frequency is updated correctly.
        _scalarCutoff->setSR(_cfg->value("Sample Rate", "1.0").toString());

        scalarName = _cfg->value("Cutoff / Spacing Scalar").toString();
        _scalarCutoff->setSelectedScalar(scalarName);

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterButterworthHighPassSource::FilterButterworthHighPassSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterButterworthHighPassSource::~FilterButterworthHighPassSource() {
}


QString FilterButterworthHighPassSource::_automaticDescriptiveName() const {
  return tr("%1 High Pass").arg(vector()->descriptiveName());
}


void FilterButterworthHighPassSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigFilterButterworthHighPassPlugin* config = static_cast<ConfigFilterButterworthHighPassPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
    setInputScalar(SCALAR_CUTOFF_IN, config->selectedCutoffScalar());
  }
}


void FilterButterworthHighPassSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


int min_pad(Kst::ScalarList scalars) {
   double hp = scalars.at(1)->value();
   if (hp >0) {
     return int (1.0/hp);
   } else {
     return 0.0;
   }
}

double filter_calculate( double f, Kst::ScalarList scalars ) {
  double gain;

  if (f > 0.0) {
    gain = 1.0 / ( 1.0 + pow( scalars.at(1)->value() / f, 2.0 * scalars.at(0)->value() ) );
  } else {
    gain = 0.0;
  }

  return gain;
}


bool FilterButterworthHighPassSource::algorithm() {

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
  label_info.name = tr("Filtered %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return kst_pass_filter( inputVector, scalars, outputVector);
}


Kst::VectorPtr FilterButterworthHighPassSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterButterworthHighPassSource::cutoffScalar() const {
  return _inputScalars[SCALAR_CUTOFF_IN];
}


Kst::ScalarPtr FilterButterworthHighPassSource::orderScalar() const {
  return _inputScalars[SCALAR_ORDER_IN];
}


QStringList FilterButterworthHighPassSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterButterworthHighPassSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_CUTOFF_IN );
  inputScalars += SCALAR_ORDER_IN;
  return inputScalars;
}


QStringList FilterButterworthHighPassSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterButterworthHighPassSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterButterworthHighPassSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterButterworthHighPassSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterButterworthHighPassSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString ButterworthHighPassPlugin::pluginName() const { return tr("High Pass Filter"); }
QString ButterworthHighPassPlugin::pluginDescription() const { return tr("Filters a vector with a zero phase high pass filter with a butterworth amplitude response."); }


Kst::DataObject *ButterworthHighPassPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigFilterButterworthHighPassPlugin* config = static_cast<ConfigFilterButterworthHighPassPlugin*>(configWidget)) {

    FilterButterworthHighPassSource* object = store->createObject<FilterButterworthHighPassSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_CUTOFF_IN, config->selectedCutoffScalar());
      object->setInputScalar(SCALAR_ORDER_IN, config->selectedOrderScalar());
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


Kst::DataObjectConfigWidget *ButterworthHighPassPlugin::configWidget(QSettings *settingsObject) const {
  ConfigFilterButterworthHighPassPlugin *widget = new ConfigFilterButterworthHighPassPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
