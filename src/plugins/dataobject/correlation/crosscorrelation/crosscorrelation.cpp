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


#include "crosscorrelation.h"
#include "objectstore.h"
#include "ui_crosscorrelationconfig.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>


static const QString& VECTOR_IN_ONE = "Vector One In";
static const QString& VECTOR_IN_TWO = "Vector Two In";
static const QString& VECTOR_OUT_STEP = "Offset";
static const QString& VECTOR_OUT_CORRELATED = "C";

class ConfigCrossCorrelationPlugin : public Kst::DataObjectConfigWidget, public Ui_CrossCorrelationConfig {
  public:
    ConfigCrossCorrelationPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_CrossCorrelationConfig() {
      setupUi(this);
    }

    ~ConfigCrossCorrelationPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorOne->setObjectStore(store); 
      _vectorTwo->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorOne, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorTwo, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorOne() { return _vectorOne->selectedVector(); };
    void setSelectedVectorOne(Kst::VectorPtr vector) { return _vectorOne->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorTwo() { return _vectorTwo->selectedVector(); };
    void setSelectedVectorTwo(Kst::VectorPtr vector) { return _vectorTwo->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (CrossCorrelationSource* source = static_cast<CrossCorrelationSource*>(dataObject)) {
        setSelectedVectorOne(source->vectorOne());
        setSelectedVectorTwo(source->vectorTwo());
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
        _cfg->beginGroup("Cross Correlation DataObject Plugin");
        _cfg->setValue("Input Vector One", _vectorOne->selectedVector()->Name());
        _cfg->setValue("Input Vector Two", _vectorTwo->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Cross Correlation DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector One").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVectorOne(vector);
        }
        vectorName = _cfg->value("Input Vector Two").toString();
        Kst::Object* object2 = _store->retrieveObject(vectorName);
        Kst::Vector* vector2 = static_cast<Kst::Vector*>(object2);
        if (vector2) {
          setSelectedVectorTwo(vector2);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


CrossCorrelationSource::CrossCorrelationSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


CrossCorrelationSource::~CrossCorrelationSource() {
}


QString CrossCorrelationSource::_automaticDescriptiveName() const {
  return QString("Cross Correlation");
}


void CrossCorrelationSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigCrossCorrelationPlugin* config = static_cast<ConfigCrossCorrelationPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_ONE, config->selectedVectorOne());
    setInputVector(VECTOR_IN_TWO, config->selectedVectorTwo());
  }
}


void CrossCorrelationSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_STEP, "");
  setOutputVector(VECTOR_OUT_CORRELATED, "");
}


bool CrossCorrelationSource::algorithm() {
  Kst::VectorPtr inputVectorOne = _inputVectors[VECTOR_IN_ONE];
  Kst::VectorPtr inputVectorTwo = _inputVectors[VECTOR_IN_TWO];
  Kst::VectorPtr outputVectorStep = _outputVectors[VECTOR_OUT_STEP];
  Kst::VectorPtr outputVectorCorrelated = _outputVectors[VECTOR_OUT_CORRELATED];

  if (inputVectorOne->length() <= 0 || inputVectorTwo->length() <= 0 || inputVectorOne->length() != inputVectorTwo->length()) {
    _errorString = "Error:  Input Vectors - invalid size";
    return false;
  }

  double* pdArrayOne;
  double* pdArrayTwo;
  double* pdResult[2];
  double  dReal;
  double  dImag;

  int iLength;
  int iLengthNew;

  bool bReturn = false;

  //
  // zero-pad the array...
  //
  iLength  = inputVectorOne->length();
  iLength *= 2;

  outputVectorStep->resize(inputVectorOne->length(), false);
  outputVectorCorrelated->resize(inputVectorTwo->length(), false);

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
  pdArrayTwo = new double[iLength];
  if (pdArrayOne != NULL && pdArrayTwo != NULL) {
    //
    // zero-pad the two arrays...
    //
    memset( pdArrayOne, 0, iLength * sizeof( double ) );
    memcpy( pdArrayOne, inputVectorOne->value(), inputVectorOne->length() * sizeof( double ) );

    memset( pdArrayTwo, 0, iLength * sizeof( double ) );
    memcpy( pdArrayTwo, inputVectorTwo->value(), inputVectorTwo->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdArrayOne, 1, iLength ) == 0) {
      if (gsl_fft_real_radix2_transform( pdArrayTwo, 1, iLength ) == 0) {
        //
        // multiply one FFT by the complex conjugate of the other...
        //
        for (int i=0; i<iLength/2; i++) {
          if (i==0 || i==(iLength/2)-1) {
            pdArrayOne[i] = pdArrayOne[i] * pdArrayTwo[i];
          } else {
            dReal = pdArrayOne[i] * pdArrayTwo[i] + pdArrayOne[iLength-i] * pdArrayTwo[iLength-i];
            dImag = pdArrayOne[i] * pdArrayTwo[iLength-i] - pdArrayOne[iLength-i] * pdArrayTwo[i];

            pdArrayOne[i] = dReal;
            pdArrayOne[iLength-i] = dImag;
          }
        }

        //
        // do the inverse FFT...
        //
        if (gsl_fft_halfcomplex_radix2_inverse( pdArrayOne, 1, iLength ) == 0) {
          pdResult[0] = outputVectorStep->value();
          pdResult[1] = outputVectorCorrelated->value();

          if (pdResult[0] != NULL && pdResult[1] != NULL) {
            for (int i = 0; i < inputVectorOne->length(); ++i) {
              outputVectorStep->value()[i] = pdResult[0][i];
            }
            for (int i = 0; i < inputVectorTwo->length(); ++i) {
              outputVectorCorrelated->value()[i] = pdResult[1][i];
            }

            for (int i = 0; i < inputVectorOne->length(); i++) {
                outputVectorStep->value()[i] = (double)( i - ( inputVectorOne->length() / 2 ) );
            }

            memcpy( &(outputVectorCorrelated->value()[inputVectorOne->length() / 2]),
                    &(pdArrayOne[0]),
                    ( ( inputVectorOne->length() + 1 ) / 2 ) * sizeof( double ) );

            memcpy( &(outputVectorCorrelated->value()[0]),
                    &(pdArrayOne[iLength - (inputVectorOne->length() / 2)]),
                    ( inputVectorOne->length() / 2 ) * sizeof( double ) );

            bReturn = true;
          }
        }
      }
    }
  }
  delete[] pdArrayOne;
  delete[] pdArrayTwo;

  return bReturn;
}


Kst::VectorPtr CrossCorrelationSource::vectorOne() const {
  return _inputVectors[VECTOR_IN_ONE];
}


Kst::VectorPtr CrossCorrelationSource::vectorTwo() const {
  return _inputVectors[VECTOR_IN_TWO];
}


QStringList CrossCorrelationSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_ONE);
  vectors += VECTOR_IN_TWO;
  return vectors;
}


QStringList CrossCorrelationSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList CrossCorrelationSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList CrossCorrelationSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_CORRELATED);
  vectors += VECTOR_OUT_STEP;
  return vectors;
}


QStringList CrossCorrelationSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList CrossCorrelationSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void CrossCorrelationSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString CrossCorrelationPlugin::pluginName() const { return "Cross Correlation"; }
QString CrossCorrelationPlugin::pluginDescription() const { return "Generates the correlation of one vector with another."; }


Kst::DataObject *CrossCorrelationPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigCrossCorrelationPlugin* config = static_cast<ConfigCrossCorrelationPlugin*>(configWidget)) {

    CrossCorrelationSource* object = store->createObject<CrossCorrelationSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_ONE, config->selectedVectorOne());
      object->setInputVector(VECTOR_IN_TWO, config->selectedVectorTwo());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *CrossCorrelationPlugin::configWidget(QSettings *settingsObject) const {
  ConfigCrossCorrelationPlugin *widget = new ConfigCrossCorrelationPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_ConvolvePlugin, CrossCorrelationPlugin)
#endif

// vim: ts=2 sw=2 et
