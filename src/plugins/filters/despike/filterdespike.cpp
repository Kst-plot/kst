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


#include "filterdespike.h"
#include "objectstore.h"
#include "ui_filterdespikeconfig.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_NSIGMA_IN = "NSigma Scalar";
static const QString& SCALAR_SPACING_IN = "Spacing Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigWidgetFilterDespikePlugin : public Kst::DataObjectConfigWidget, public Ui_FilterDespikeConfig {
  public:
    ConfigWidgetFilterDespikePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterDespikeConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFilterDespikePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarNSigma->setObjectStore(store);
      _scalarSpacing->setObjectStore(store);
      _scalarNSigma->setDefaultValue(5.0);
      _scalarSpacing->setDefaultValue(1.0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarNSigma, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarSpacing, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedNSigmaScalar() { return _scalarNSigma->selectedScalar(); };
    void setSelectedNSigmaScalar(Kst::ScalarPtr scalar) { return _scalarNSigma->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedSpacingScalar() { return _scalarSpacing->selectedScalar(); };
    void setSelectedSpacingScalar(Kst::ScalarPtr scalar) { return _scalarSpacing->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterDespikeSource* source = static_cast<FilterDespikeSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedNSigmaScalar(source->nSigmaScalar());
        setSelectedSpacingScalar(source->spacingScalar());
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
        _cfg->beginGroup("Filter Despike Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Spacing Scalar", _scalarNSigma->selectedScalar()->Name());
        _cfg->setValue("NSigma Scalar", _scalarNSigma->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Despike Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Spacing Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* spacingScalar = static_cast<Kst::Scalar*>(object);
        if (spacingScalar) {
          setSelectedSpacingScalar(spacingScalar);
        }
        scalarName = _cfg->value("NSigma Scalar").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* nSigmaScalar = static_cast<Kst::Scalar*>(object);
        if (nSigmaScalar) {
          setSelectedNSigmaScalar(nSigmaScalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterDespikeSource::FilterDespikeSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterDespikeSource::~FilterDespikeSource() {
}


QString FilterDespikeSource::_automaticDescriptiveName() const {
  return QString(vector()->descriptiveName() + " Despike");
}


QString FilterDespikeSource::descriptionTip() const {
  QString tip;

  tip = i18n("Despike Filter: %1\n  Spacing: %2\n  NSigma: %3").arg(Name()).arg(spacingScalar()->value()).arg(nSigmaScalar()->value());

  tip += i18n("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}

void FilterDespikeSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFilterDespikePlugin* config = static_cast<ConfigWidgetFilterDespikePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_NSIGMA_IN, config->selectedNSigmaScalar());
    setInputScalar(SCALAR_SPACING_IN, config->selectedSpacingScalar());
  }
}


void FilterDespikeSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool FilterDespikeSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr nSigmaScalar = _inputScalars[SCALAR_NSIGMA_IN];
  Kst::ScalarPtr spacingScalar = _inputScalars[SCALAR_SPACING_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }

  int N = inputVector->length();
  double last_good;
  double mdev = 0;
  double cut = nSigmaScalar->value();
  int dx = int(spacingScalar->value());
  int i;
  int spike_start = -1;
  int border = dx*2;

  if ( (N < 1) || (cut <= 0) || (dx < 1) || (dx > N/2) ) {
    return false;
  }

  /* get mean deviation of 3 pt difference */
  for (i=dx; i<N-dx; i++) {
    mdev += fabs(inputVector->value(i)-(inputVector->value(i-dx) + inputVector->value(i+dx))*0.5);
  }
  mdev /= double(N);

  cut *= mdev;

  // resize the output array
  outputVector->resize(inputVector->length(), false);

  // for first dx points, do a 2 point difference
  last_good = inputVector->value(0);
  for (i=0; i<dx; i++) {
    if (fabs(inputVector->value(i) - inputVector->value(i+dx))>cut) {
      if (spike_start<0) { 
        spike_start = i-border;
        if (spike_start<0) {
          spike_start = 0;
        }
      }
    } else {
      if (spike_start>=0) {
        i += 4*border-1; 
        if (i>=N) {
          i=N-1;
        }
        for (int j=spike_start; j<=i; j++) {
          outputVector->value()[j] = last_good;
        }
        spike_start = -1;
      }
      last_good = outputVector->value()[i] = inputVector->value(i);
    }
  }
  // do a 3 point difference where it is possible
  for (i=dx; i<N-dx; i++) {
    if (fabs(inputVector->value(i) - (inputVector->value(i-dx) + inputVector->value(i+dx))*0.5)>cut) {
      if (spike_start<0) { 
        spike_start = i-border;
        if (spike_start<0) {
          spike_start = 0;
        }
      }
    } else {
      if (spike_start>=0) {
        i += 4*border-1; 
        if (i>=N) {
          i=N-1;
        }
        for (int j=spike_start; j<=i; j++) {
          outputVector->value()[j] = last_good;
        }
        spike_start = -1;
      } else {
        last_good = outputVector->value()[i] = inputVector->value(i);
      }
    }
  }
  // do a 2 point difference for last dx points
  for (i=N-dx-1; i<N; i++) {
    if (fabs(inputVector->value(i-dx) - inputVector->value(i))>cut) {
      if (spike_start<0) { 
        spike_start = i-border;
        if (spike_start<0) {
          spike_start = 0;
        }
      }
    } else {
      if (spike_start>=0) {
        i += 4*border-1;
        if (i>=N) {
          i=N-1;
        }
        for (int j=spike_start; j<=i; j++) {
          outputVector->value()[j] = last_good;
        }
        spike_start = -1;
      } else {
        last_good = outputVector->value()[i] = inputVector->value(i);
      }
    }
  }
  if (spike_start>=0) {
    for (int j=spike_start; j<N; j++) {
      outputVector->value()[j] = last_good;
    }
  }

  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = i18n("Despiked %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return true;
}


Kst::VectorPtr FilterDespikeSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterDespikeSource::spacingScalar() const {
  return _inputScalars[SCALAR_SPACING_IN];
}


Kst::ScalarPtr FilterDespikeSource::nSigmaScalar() const {
  return _inputScalars[SCALAR_NSIGMA_IN];
}


QStringList FilterDespikeSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterDespikeSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_NSIGMA_IN );
  inputScalars += SCALAR_SPACING_IN;
  return inputScalars;
}


QStringList FilterDespikeSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterDespikeSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterDespikeSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterDespikeSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterDespikeSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FilterDespikePlugin::pluginName() const { return "Despike Filter"; }
QString FilterDespikePlugin::pluginDescription() const { return "Finds and removes spikes using a 3 point difference."; }


Kst::DataObject *FilterDespikePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFilterDespikePlugin* config = static_cast<ConfigWidgetFilterDespikePlugin*>(configWidget)) {

    FilterDespikeSource* object = store->createObject<FilterDespikeSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_SPACING_IN, config->selectedSpacingScalar());
      object->setInputScalar(SCALAR_NSIGMA_IN, config->selectedNSigmaScalar());
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


Kst::DataObjectConfigWidget *FilterDespikePlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFilterDespikePlugin *widget = new ConfigWidgetFilterDespikePlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_FilterDespikePlugin, FilterDespikePlugin)

// vim: ts=2 sw=2 et
