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


#include "filterwindow.h"
#include "objectstore.h"
#include "ui_filterwindowconfig.h"

/* For NAN */
#include "math_kst.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_MAXIMUM_IN = "Maximum Scalar";
static const QString& SCALAR_MINIMUM_IN = "Minimum Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigWidgetFilterWindowPlugin : public Kst::DataObjectConfigWidget, public Ui_FilterWindowConfig {
  public:
    ConfigWidgetFilterWindowPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FilterWindowConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFilterWindowPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _scalarMinumum->setObjectStore(store);
      _scalarMaximum->setObjectStore(store);
      _scalarMinumum->setDefaultValue(0.0);
      _scalarMaximum->setDefaultValue(1.0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMinumum, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarMaximum, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FilterWindowSource* source = static_cast<FilterWindowSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedMaximumScalar(source->maximumScalar());
        setSelectedMinimumScalar(source->minimumScalar());
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
        _cfg->beginGroup("Filter Window Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Minimum Scalar", _scalarMinumum->selectedScalar()->Name());
        _cfg->setValue("Maximum Scalar", _scalarMaximum->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Filter Window Plugin");
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

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FilterWindowSource::FilterWindowSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FilterWindowSource::~FilterWindowSource() {
}


QString FilterWindowSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Window").arg(vector()->descriptiveName());
  } else {
    return tr("Window");
  }
}


QString FilterWindowSource::descriptionTip() const {
  QString tip;

  tip = tr("Window Filter: %1\n  %2 - %3\n  ", "%2 - %3 specifies a numerical range").arg(Name()).arg(minimumScalar()->value()).arg(maximumScalar()->value());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}

void FilterWindowSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFilterWindowPlugin* config = static_cast<ConfigWidgetFilterWindowPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_MAXIMUM_IN, config->selectedMaximumScalar());
    setInputScalar(SCALAR_MINIMUM_IN, config->selectedMinimumScalar());
  }
}


void FilterWindowSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool FilterWindowSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr minimumScalar = _inputScalars[SCALAR_MINIMUM_IN];
  Kst::ScalarPtr maximumScalar = _inputScalars[SCALAR_MAXIMUM_IN];
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
  double x;
  int i;

  if (max<min) {
    qSwap(max, min);
  }

  if ( (N < 1) || (max==min) ) {
    return false;
  }

  outputVector->resize(N, false);


  for (i=0; i<N; ++i) {
    x = inputVector->value(i);
    if ((x>max) || (x<min)) { // out of range: Nanify
      outputVector->value()[i] = NAN;
    } else {
      outputVector->value()[i] = x;
    }
  }

  Kst::LabelInfo label_info = inputVector->labelInfo();
  label_info.name = tr("Window %1").arg(label_info.name);
  outputVector->setLabelInfo(label_info);

  return true;
}


Kst::VectorPtr FilterWindowSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr FilterWindowSource::maximumScalar() const {
  return _inputScalars[SCALAR_MAXIMUM_IN];
}


Kst::ScalarPtr FilterWindowSource::minimumScalar() const {
  return _inputScalars[SCALAR_MINIMUM_IN];
}


QStringList FilterWindowSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList FilterWindowSource::inputScalarList() const {
  QStringList inputScalars( SCALAR_MAXIMUM_IN );
  inputScalars += SCALAR_MINIMUM_IN;
  return inputScalars;
}


QStringList FilterWindowSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FilterWindowSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList FilterWindowSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList FilterWindowSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FilterWindowSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FilterWindowPlugin::pluginName() const { return tr("Window Filter"); }
QString FilterWindowPlugin::pluginDescription() const { return tr("Windows data that have wrapped."); }


Kst::DataObject *FilterWindowPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFilterWindowPlugin* config = static_cast<ConfigWidgetFilterWindowPlugin*>(configWidget)) {

    FilterWindowSource* object = store->createObject<FilterWindowSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_MAXIMUM_IN, config->selectedMaximumScalar());
      object->setInputScalar(SCALAR_MINIMUM_IN, config->selectedMinimumScalar());
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


Kst::DataObjectConfigWidget *FilterWindowPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFilterWindowPlugin *widget = new ConfigWidgetFilterWindowPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_FilterWindowPlugin, FilterWindowPlugin)
#endif

// vim: ts=2 sw=2 et
