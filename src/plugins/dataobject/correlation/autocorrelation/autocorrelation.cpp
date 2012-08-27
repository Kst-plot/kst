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


#include "autocorrelation.h"
#include "objectstore.h"
#include "ui_autocorrelationconfig.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

static const QString& VECTOR_IN = "Vector In";
static const QString& VECTOR_OUT_AUTO = "Auto-Correlated";
static const QString& VECTOR_OUT_STEP = "Step Value";

class ConfigAutoCorrelationPlugin : public Kst::DataObjectConfigWidget, public Ui_AutoCorrelationConfig {
  public:
    ConfigAutoCorrelationPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_AutoCorrelationConfig() {
      setupUi(this);
    }

    ~ConfigAutoCorrelationPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (AutoCorrelationSource* source = static_cast<AutoCorrelationSource*>(dataObject)) {
        setSelectedVector(source->vector());
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
        _cfg->beginGroup("Auto Correlation DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Auto Correlation DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


AutoCorrelationSource::AutoCorrelationSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


AutoCorrelationSource::~AutoCorrelationSource() {
}


QString AutoCorrelationSource::_automaticDescriptiveName() const {
  return QString("Auto Correlation Plugin Object");
}


void AutoCorrelationSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigAutoCorrelationPlugin* config = static_cast<ConfigAutoCorrelationPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void AutoCorrelationSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_AUTO, "");
  setOutputVector(VECTOR_OUT_STEP, "");
}


bool AutoCorrelationSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr outputVectorAuto = _outputVectors[VECTOR_OUT_AUTO];
  Kst::VectorPtr outputVectorStep = _outputVectors[VECTOR_OUT_STEP];

  //Make sure there is at least 1 element in the input vector
  if (inputVector->length() < 1) {
    _errorString = "Error:  Input Vector invalid size";
    return false;
  }

  double* pdArrayOne;
  double* pdResult;
  double* pdCorrelate;
  double  dReal;
  double  dImag;

  int iLength;
  int iLengthNew;

  bool bReturn = false;

  //
  // zero-pad the array...
  //
  iLength  = inputVector->length();
  iLength *= 2;

  outputVectorAuto->resize(inputVector->length(), false);
  outputVectorStep->resize(inputVector->length(), false);

  //
  // round iLength up to the nearest power of two...
  //
  iLengthNew = 64;
  while( iLengthNew < iLength && iLengthNew > 0) {
    iLengthNew *= 2;
  }
  iLength = iLengthNew;

  if (iLength <= 0) {
    _errorString = "Error:  Invalid Input length calculated";
    return false;
  }

  pdArrayOne = new double[iLength];
  if (pdArrayOne != NULL) {
    //
    // zero-pad the two arrays...
    //
    memset( pdArrayOne, 0, iLength * sizeof( double ) );
    memcpy( pdArrayOne, inputVector->value(), inputVector->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdArrayOne, 1, iLength ) == 0) {
      //
      // multiply the FFT by its complex conjugate...
      //
      for (int i=0; i<iLength/2; i++) {
        if (i==0 || i==(iLength/2)-1) {
          pdArrayOne[i] *= pdArrayOne[i];
        } else {
          dReal = pdArrayOne[i] * pdArrayOne[i] + pdArrayOne[iLength-i] * pdArrayOne[iLength-i];
          dImag = pdArrayOne[i] * pdArrayOne[iLength-i] - pdArrayOne[iLength-i] * pdArrayOne[i];

          pdArrayOne[i] = dReal;
          pdArrayOne[iLength-i] = dImag;
        }
      }

      //
      // do the inverse FFT...
      //
      if (gsl_fft_halfcomplex_radix2_inverse( pdArrayOne, 1, iLength ) == 0) {
        pdResult = outputVectorStep->value();
        pdCorrelate = outputVectorAuto->value();

        if (pdResult != NULL && pdCorrelate != NULL) {
          for (int i = 0; i < inputVector->length(); ++i) {
            outputVectorStep->value()[i] = pdResult[i];
          }
          for (int i = 0; i < inputVector->length(); ++i) {
            outputVectorAuto->value()[i] = pdCorrelate[i];
          }

          for (int i = 0; i < inputVector->length(); i++) {
              outputVectorStep->value()[i] = (double)( i - ( inputVector->length() / 2 ) );
          }

          memcpy( &(outputVectorAuto->value()[inputVector->length() / 2]),
                  &(pdArrayOne[0]),
                  ( ( inputVector->length() + 1 ) / 2 ) * sizeof( double ) );

          memcpy( &(outputVectorAuto->value()[0]),
                  &(pdArrayOne[iLength - (inputVector->length() / 2)]),
                  ( inputVector->length() / 2 ) * sizeof( double ) );

          bReturn = true;
        }
      }
    }
  }
  delete[] pdArrayOne;

  return bReturn;
}


Kst::VectorPtr AutoCorrelationSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


QStringList AutoCorrelationSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList AutoCorrelationSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList AutoCorrelationSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList AutoCorrelationSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_AUTO);
  vectors += VECTOR_OUT_STEP;
  return vectors;
}


QStringList AutoCorrelationSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList AutoCorrelationSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void AutoCorrelationSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString AutoCorrelationPlugin::pluginName() const { return "Auto Correlation"; }
QString AutoCorrelationPlugin::pluginDescription() const { return "Generates the auto-correlation of a vector."; }


Kst::DataObject *AutoCorrelationPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigAutoCorrelationPlugin* config = static_cast<ConfigAutoCorrelationPlugin*>(configWidget)) {

    AutoCorrelationSource* object = store->createObject<AutoCorrelationSource>();

    if (setupInputsOutputs) {
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


Kst::DataObjectConfigWidget *AutoCorrelationPlugin::configWidget(QSettings *settingsObject) const {
  ConfigAutoCorrelationPlugin *widget = new ConfigAutoCorrelationPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, AutoCorrelationPlugin)
#endif

// vim: ts=2 sw=2 et
