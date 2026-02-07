/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2013 The Kst Team                                     *
 *                   kst@kde.org                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "activitylevel.h"
#include "objectstore.h"
#include "ui_activitylevel.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN_SAMPLING = "Sampling";
static const QString& SCALAR_IN_WINDOWWIDTH = "Window Width";
static const QString& SCALAR_IN_THRESHOLD = "Threshold";
static const QString& VECTOR_OUT_ACTIVITY = "Activity";
static const QString& VECTOR_OUT_REVERSALS = "Nb Reversals";
static const QString& VECTOR_OUT_STDDEV = "Sliding Standard Deviation";
static const QString& VECTOR_OUT_DENOISED = "Denoised Input";

class ConfigWidgetActivityLevelPlugin : public Kst::DataObjectConfigWidget, public Ui_ActivityLevelConfig {
  public:
    ConfigWidgetActivityLevelPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_ActivityLevelConfig() {
      setupUi(this);
    }

    ~ConfigWidgetActivityLevelPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _windowWidth->setObjectStore(store);
      _windowWidth->setDefaultValue(3);
      _samplingTime->setObjectStore(store);
      _samplingTime->setDefaultValue(0.025);
      _noiseThreshold->setObjectStore(store);
      _noiseThreshold->setDefaultValue(0.2);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_samplingTime, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_windowWidth, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_noiseThreshold, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    Kst::ScalarPtr selectedSamplingTime() { return _samplingTime->selectedScalar(); };
    void setSelectedSamplingTime(Kst::ScalarPtr scalar) { return _samplingTime->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedWindowWidth() { return _windowWidth->selectedScalar(); };
    void setSelectedWindowWidth(Kst::ScalarPtr scalar) { return _windowWidth->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedNoiseThreshold() { return _noiseThreshold->selectedScalar(); };
    void setSelectedNoiseThreshold(Kst::ScalarPtr scalar) { return _noiseThreshold->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (ActivityLevelSource* source = static_cast<ActivityLevelSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedSamplingTime(source->samplingTime());
        setSelectedWindowWidth(source->windowWidth());
        setSelectedNoiseThreshold(source->noiseThreshold());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

//       QStringView av;
//       av = attrs.value("value");
//       if (!av.isNull()) {
//         _configValue = QVariant(av.toString()).toBool();
//       }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Activity Level DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar Sampling Time", _samplingTime->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Window Width", _windowWidth->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Noise Threshold", _noiseThreshold->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Activity Level DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        // Sampling Time
        QString scalarName = _cfg->value("Input Scalar Sampling Time").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalar = static_cast<Kst::Scalar*>(object);
        if (scalar) {
          setSelectedSamplingTime(scalar);
        }
        // Window Width
        scalarName = _cfg->value("Input Scalar Window Width").toString();
        object = _store->retrieveObject(scalarName);
        scalar = static_cast<Kst::Scalar*>(object);
        if (scalar) {
          setSelectedWindowWidth(scalar);
        }
        // Noise Threshold
        scalarName = _cfg->value("Input Scalar Noise Threshold").toString();
        object = _store->retrieveObject(scalarName);
        scalar = static_cast<Kst::Scalar*>(object);
        if (scalar) {
          setSelectedNoiseThreshold(scalar);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


ActivityLevelSource::ActivityLevelSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


ActivityLevelSource::~ActivityLevelSource() {
}


QString ActivityLevelSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Activity Level").arg(vector()->descriptiveName());
  } else {
    return tr("Activity Level");
  }
}


QString ActivityLevelSource::descriptionTip() const {
  QString tip;
  tip = tr("Activity Level: %1\n  Sampling Time: %2 (s)\n  Window width: %3 (s)\n  Noise Threshold: %4 \n").
                              arg(Name()).arg(samplingTime()->value()).arg(windowWidth()->value()).arg(noiseThreshold()->value());
  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void ActivityLevelSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetActivityLevelPlugin* config = static_cast<ConfigWidgetActivityLevelPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN_SAMPLING, config->selectedSamplingTime());
    setInputScalar(SCALAR_IN_WINDOWWIDTH, config->selectedWindowWidth());
    setInputScalar(SCALAR_IN_THRESHOLD, config->selectedNoiseThreshold());
  }
}


void ActivityLevelSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_ACTIVITY, "");
  setOutputVector(VECTOR_OUT_REVERSALS, "");
  setOutputVector(VECTOR_OUT_STDDEV, "");
  setOutputVector(VECTOR_OUT_DENOISED, "");
}


bool ActivityLevelSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr samplingTime = _inputScalars[SCALAR_IN_SAMPLING];
  Kst::ScalarPtr windowWidth = _inputScalars[SCALAR_IN_WINDOWWIDTH];
  Kst::ScalarPtr noiseThreshold = _inputScalars[SCALAR_IN_THRESHOLD];
  Kst::VectorPtr outputVectorActivity = _outputVectors[VECTOR_OUT_ACTIVITY];
  Kst::VectorPtr outputVectorReversals = _outputVectors[VECTOR_OUT_REVERSALS];
  Kst::VectorPtr outputVectorStdDeviation = _outputVectors[VECTOR_OUT_STDDEV];
  Kst::VectorPtr outputVectorDenoised = _outputVectors[VECTOR_OUT_DENOISED];

  int i, length;
  // Check for consistent values
  if (windowWidth->value() < samplingTime->value() || samplingTime->value() == 0.0) {
    return false;
  }

  int iSamplesForWindow = (int) (windowWidth->value() / samplingTime->value());
  double dStandardDeviation = 0.0, dTotal = 0.0, dVariance = 0.0, dSquaredTotal = 0.0;
  int iTrendPrevious = 0, iTrend = 0;
  double dNbReversals = 0.0; // Compute as a double since we output a vector of doubles anyway
  length = inputVector->length();
  /* The metric is computed over a couple of seconds, let us compute the corresponding number of samples */
  if (iSamplesForWindow > length) {
    _errorString = tr("Error: Input vector too short.");
    return false;
  }
  if (iSamplesForWindow < 2) {
    _errorString = tr("Error: the window must be broader.");
    return false;
  }

  /* Array allocations */
  outputVectorActivity->resize(length, true);
  outputVectorReversals->resize(length, true);
  outputVectorStdDeviation->resize(length, true);
  outputVectorDenoised->resize(length, true);

//  /* Requantize to avoid noise creating many unwanted sign changes */
//  if (noiseThreshold->value() > 0.0) {
//    for (i = 0; i < length; ++i) {
//      outputVectorDenoised->raw_V_ptr()[i] = (double) rint( inputVector->value()[i] / noiseThreshold->value() ) * noiseThreshold->value();
//    }
//  }
  /* Recompute input data, taking direction changes only when they exceed a given threshold */
  if (noiseThreshold->value() > 0.0) {
    iTrendPrevious = (inputVector->value()[1]-inputVector->value()[0] > 0) ? 1 : -1;
    outputVectorDenoised->raw_V_ptr()[0] = inputVector->value()[0];
    bool bFreeze = false;
    for (i = 1; i < length; ++i) {
      // Update current trend
      if (inputVector->value()[i] == inputVector->value()[i-1]) {
        iTrend = 0;
      } else {
        iTrend = ( (inputVector->value()[i]-inputVector->value()[i-1]) > 0) ? 1 : -1;
      }
      // Check what to do with the value
      if ( iTrendPrevious * iTrend >= 0 && !bFreeze) {
        outputVectorDenoised->raw_V_ptr()[i] = inputVector->value()[i];
        iTrendPrevious = iTrend;
      } else { // Change of direction: check whether the delta is significant, otherwise freeze the value
        if ( qAbs(inputVector->value()[i] - outputVectorDenoised->raw_V_ptr()[i-1]) >= noiseThreshold->value() ) { // Delta is significant: keep value
          outputVectorDenoised->raw_V_ptr()[i] = inputVector->value()[i];
          bFreeze = false;
          iTrendPrevious = iTrend;
        } else {
          outputVectorDenoised->raw_V_ptr()[i] = outputVectorDenoised->raw_V_ptr()[i-1];
          bFreeze = true;
        }
      }
    }
  }

  /* Compute initial values for first windowWidth seconds */
  dTotal = outputVectorDenoised->raw_V_ptr()[0] + outputVectorDenoised->raw_V_ptr()[1];
  dSquaredTotal += outputVectorDenoised->raw_V_ptr()[0] * outputVectorDenoised->raw_V_ptr()[0] + outputVectorDenoised->raw_V_ptr()[1] * outputVectorDenoised->raw_V_ptr()[1];
  outputVectorReversals->raw_V_ptr()[1] = outputVectorReversals->raw_V_ptr()[0] = 0.0;
  outputVectorStdDeviation->raw_V_ptr()[1] = outputVectorStdDeviation->raw_V_ptr()[0] = 0.0;
  outputVectorActivity->raw_V_ptr()[1] = outputVectorActivity->raw_V_ptr()[0] = 0.0;
  for (i = 2; i < iSamplesForWindow; ++i) {
    /* Update previous sign if needed */
    if (outputVectorDenoised->raw_V_ptr()[i-1] != outputVectorDenoised->raw_V_ptr()[i-2]) {
      iTrendPrevious = ( (outputVectorDenoised->raw_V_ptr()[i-1] - outputVectorDenoised->raw_V_ptr()[i-2]) > 0 ) ? 1 : -1;
    }
    /* Compute current sign */
    if (outputVectorDenoised->raw_V_ptr()[i] != outputVectorDenoised->raw_V_ptr()[i-1]) {
      iTrend = ( (outputVectorDenoised->raw_V_ptr()[i] - outputVectorDenoised->raw_V_ptr()[i-1]) > 0 ) ? 1 : -1;
    } else {
      iTrend = 0;
    }
    /* Check for reversal */
    if ( iTrend * iTrendPrevious < 0 ) {
      dNbReversals += 1.0;
    }
    dTotal += outputVectorDenoised->raw_V_ptr()[i];
    dSquaredTotal += outputVectorDenoised->raw_V_ptr()[i] * outputVectorDenoised->raw_V_ptr()[i];
    /* Store zeros as long as we do not have enough values */
    outputVectorReversals->raw_V_ptr()[i] = 0.0;
    outputVectorStdDeviation->raw_V_ptr()[i] = 0.0;
    outputVectorActivity->raw_V_ptr()[i] = 0.0;
  }
  dVariance  = 1.0 / ( (double)iSamplesForWindow - 1.0 );
  dVariance *= dSquaredTotal - ( dTotal * dTotal / (double)iSamplesForWindow );
  if( dVariance > 0.0 ) { // The computation method can have numerical artefacts leading to negative values here!
    dStandardDeviation = sqrt( dVariance );
  } else {
    dVariance = 0.0;
    dStandardDeviation = 0.0;
  }
  /* Now, we can actually store the first useful value (exactly the right number of samples processed) */
  outputVectorReversals->raw_V_ptr()[i] = dNbReversals;
  outputVectorStdDeviation->raw_V_ptr()[i] = dStandardDeviation;
  outputVectorActivity->raw_V_ptr()[i] = dNbReversals * dStandardDeviation;

  /* Finally, update continuously for each new value for the rest of values */
  double outgoingValue, outgoingValuePrev, outgoingValueNext, incomingValue, incomingValuePrev, incomingValueNext;
  for (i = iSamplesForWindow; i < length; ++i) {
    dTotal += outputVectorDenoised->raw_V_ptr()[i] - outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow];
    dSquaredTotal += outputVectorDenoised->raw_V_ptr()[i] * outputVectorDenoised->raw_V_ptr()[i] - outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow] * outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow];
    dVariance  = 1.0 / ( (double)iSamplesForWindow - 1.0 );
    dVariance *= dSquaredTotal - ( dTotal * dTotal / (double)iSamplesForWindow );
    if( dVariance > 0.0 ) {
      dStandardDeviation = sqrt( dVariance );
    } else {
      dVariance = 0.0;
      dStandardDeviation = 0.0;
    }
    /* Update the number of reversals, by removing 1 if the outgoing data point was a reversal and adding 1 if the incoming point is one */
    outgoingValue = outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow];
    outgoingValuePrev = outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow-1];
    outgoingValueNext = outputVectorDenoised->raw_V_ptr()[i-iSamplesForWindow+1];
    incomingValue = outputVectorDenoised->raw_V_ptr()[i];
    incomingValuePrev = outputVectorDenoised->raw_V_ptr()[i-1];
    if (i == length-1) { // Protect against accessing past the boundary of the vector
      incomingValueNext = outputVectorDenoised->raw_V_ptr()[i];
    } else {
      incomingValueNext = outputVectorDenoised->raw_V_ptr()[i+1];
    }
    if ( (outgoingValue-outgoingValuePrev)*(outgoingValueNext-outgoingValue) < 0) {
      dNbReversals = qMax(dNbReversals - 1.0, double(0.0)); // Avoid getting negative values, which can happen
    }
    if ( (incomingValue-incomingValuePrev)*(incomingValueNext-incomingValue) < 0) {
      dNbReversals += 1.0;
    }

    /* Store values */
    outputVectorReversals->raw_V_ptr()[i] = dNbReversals;
    outputVectorStdDeviation->raw_V_ptr()[i] = dStandardDeviation;
    outputVectorActivity->raw_V_ptr()[i] = dNbReversals * dStandardDeviation;
  }
  return true;
}


Kst::VectorPtr ActivityLevelSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


Kst::ScalarPtr ActivityLevelSource::samplingTime() const {
  return _inputScalars[SCALAR_IN_SAMPLING];
}


Kst::ScalarPtr ActivityLevelSource::windowWidth() const {
  return _inputScalars[SCALAR_IN_WINDOWWIDTH];
}


Kst::ScalarPtr ActivityLevelSource::noiseThreshold() const {
  return _inputScalars[SCALAR_IN_THRESHOLD];
}


QStringList ActivityLevelSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList ActivityLevelSource::inputScalarList() const {
  QStringList scalars( SCALAR_IN_SAMPLING );
  scalars += SCALAR_IN_WINDOWWIDTH;
  scalars += SCALAR_IN_THRESHOLD;
  return scalars;
}


QStringList ActivityLevelSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList ActivityLevelSource::outputVectorList() const {
  QStringList vectors( VECTOR_OUT_ACTIVITY );
  vectors += VECTOR_OUT_REVERSALS;
  vectors += VECTOR_OUT_STDDEV;
  vectors += VECTOR_OUT_DENOISED;
  return vectors;
}


QStringList ActivityLevelSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList ActivityLevelSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void ActivityLevelSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString ActivityLevelPlugin::pluginName() const { return tr("Activity Level"); }
QString ActivityLevelPlugin::pluginDescription() const { return tr("Computes the activity level of a signal as the product of standard deviation and number of reversals over a sliding window."); }


Kst::DataObject *ActivityLevelPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetActivityLevelPlugin* config = static_cast<ConfigWidgetActivityLevelPlugin*>(configWidget)) {

    ActivityLevelSource* object = store->createObject<ActivityLevelSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN_SAMPLING, config->selectedSamplingTime());
      object->setInputScalar(SCALAR_IN_WINDOWWIDTH, config->selectedWindowWidth());
      object->setInputScalar(SCALAR_IN_THRESHOLD, config->selectedNoiseThreshold());
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


Kst::DataObjectConfigWidget *ActivityLevelPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetActivityLevelPlugin *widget = new ConfigWidgetActivityLevelPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
