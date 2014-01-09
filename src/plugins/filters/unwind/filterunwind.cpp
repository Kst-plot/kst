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


#include "filterunwind.h"
#include "objectstore.h"
#include "ui_filterunwindconfig.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_MAXIMUM_IN = "Maximum Scalar";
static const QString& SCALAR_MINIMUM_IN = "Minimum Scalar";
static const QString& SCALAR_STEP_IN = "Step Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigWidgetFilterUnwindPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterUnwindConfig {
  public:
    ConfigWidgetFilterUnwindPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterUnwindConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFilterUnwindPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarMinumum->setObjectStore(store);
      _scalarMaximum->setObjectStore(store);
      _scalarStep->setObjectStore(store);
      _scalarMinumum->setDefaultValue(0.0);
      _scalarMaximum->setDefaultValue(360.0);
      _scalarStep->setDefaultValue(50.0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMinumum, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMaximum, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::ScalarPtr selectedMinimumScalar() { return _scalarMinumum->selectedScalar(); };
    void setSelectedMinimumScalar(Kst::ScalarPtr scalar) { return _scalarMinumum->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedMaximumScalar() { return _scalarMaximum->selectedScalar(); };
    void setSelectedMaximumScalar(Kst::ScalarPtr scalar) { return _scalarMaximum->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedStepScalar() { return _scalarStep->selectedScalar(); };
    void setSelectedStepScalar(Kst::ScalarPtr scalar) { return _scalarStep->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterUnwindSource* source = static_cast<FilterUnwindSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedMaximumScalar(source->maximumScalar());
        setSelectedMinimumScalar(source->minimumScalar());
        setSelectedStepScalar(source->stepScalar());
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
        _cfg->beginGroup("Filter Unwind Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Minimum Scalar", _scalarMinumum->selectedScalar()->Name());
        _cfg->setValue("Maximum Scalar", _scalarMaximum->selectedScalar()->Name());
        _cfg->setValue("Step Scalar", _scalarStep->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Unwind Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Maximum Scalar").toString();
        _scalarMaximum->setSelectedScalar(scalarName);

        scalarName = _cfg->value("Minimum Scalar").toString();
        _scalarMinumum->setSelectedScalar(scalarName);

        scalarName = _cfg->value("Step Scalar").toString();
        _scalarStep->setSelectedScalar(scalarName);

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterUnwindSource::FilterUnwindSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterUnwindSource::~FilterUnwindSource() {
}


QString FilterUnwindSource::_automaticDescriptiveName() const {
  if (vector()) {
    return QString(vector()->descriptiveName() + " Unwind");
  } else {
    return QString("Unwind");
  }
}


QString FilterUnwindSource::descriptionTip() const {
  QString tip;

  tip = tr("Unwind Filter: %1\n  %2 - %3\n  Step: %4 %").arg(Name()).arg(minimumScalar()->value()).arg(maximumScalar()->value()).arg(stepScalar()->value());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}

void FilterUnwindSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFilterUnwindPlugin* config = static_cast<ConfigWidgetFilterUnwindPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_MAXIMUM_IN, config->selectedMaximumScalar());
    setInputScalar(SCALAR_MINIMUM_IN, config->selectedMinimumScalar());
    setInputScalar(SCALAR_STEP_IN, config->selectedStepScalar());
  }
}


void FilterUnwindSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool FilterUnwindSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr minimumScalar = _inputScalars[SCALAR_MINIMUM_IN];
  Kst::ScalarPtr maximumScalar = _inputScalars[SCALAR_MAXIMUM_IN];
  Kst::ScalarPtr stepScalar = _inputScalars[SCALAR_STEP_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }

  int N = inputVector->length();
  double max = maximumScalar->value();
  double min = minimumScalar->value();
  double step = stepScalar->value();
  double last_x_in;
  double range;
  double x;
  double wind = 0;
  int i;

  if (max<min) {
    qSwap(max, min);
  }

  range = max - min;
  if ( (N < 1) || (max==min) || (step <=0) || (step>=100) ) {
    return false;
  }

  outputVector->resize(N, false);
  step *= (max-min)/100.0;


  last_x_in = inputVector->value(0);
  outputVector->value()[0] = last_x_in;

  for (i=1; i<N; i++) {
    x = inputVector->value(i);
    if ((x>max) || (x<min)) { // invalid/spike... ignore.
      x = last_x_in;
    }
    if (x-last_x_in > step) {
      wind -= range;
    } else if (last_x_in - x > step) {
      wind += range;
    }
    outputVector->value()[i] = x + wind;
    last_x_in = x;
  }

  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = tr("Unwind %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return true;
}


Kst::VectorPtr FilterUnwindSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterUnwindSource::maximumScalar() const {
  return _inputScalars[SCALAR_MAXIMUM_IN];
}


Kst::ScalarPtr FilterUnwindSource::minimumScalar() const {
  return _inputScalars[SCALAR_MINIMUM_IN];
}


Kst::ScalarPtr FilterUnwindSource::stepScalar() const {
  return _inputScalars[SCALAR_STEP_IN];
}


QStringList FilterUnwindSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterUnwindSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_MAXIMUM_IN );
  inputScalars += SCALAR_MINIMUM_IN;
  inputScalars += SCALAR_STEP_IN;
  return inputScalars;
}


QStringList FilterUnwindSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterUnwindSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterUnwindSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterUnwindSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterUnwindSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FilterUnwindPlugin::pluginName() const { return "Unwind Filter"; }
QString FilterUnwindPlugin::pluginDescription() const { return "Unwinds data that have wrapped."; }


Kst::DataObject *FilterUnwindPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFilterUnwindPlugin* config = static_cast<ConfigWidgetFilterUnwindPlugin*>(configWidget)) {

    FilterUnwindSource* object = store->createObject<FilterUnwindSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_MAXIMUM_IN, config->selectedMaximumScalar());
      object->setInputScalar(SCALAR_MINIMUM_IN, config->selectedMinimumScalar());
      object->setInputScalar(SCALAR_STEP_IN, config->selectedStepScalar());
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


Kst::DataObjectConfigWidget *FilterUnwindPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFilterUnwindPlugin *widget = new ConfigWidgetFilterUnwindPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_FilterUnwindPlugin, FilterUnwindPlugin)
#endif

// vim: ts=2 sw=2 et
