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


#include "convolve.h"
#include "objectstore.h"
#include "ui_convolveconfig.h"

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>


static const QString& VECTOR_IN_ONE = "Vector One In";
static const QString& VECTOR_IN_TWO = "Vector Two In";
static const QString& VECTOR_OUT = "Vector Out";

class ConfigWidgetConvolvePlugin : public Kst::DataObjectConfigWidget, public Ui_ConvolveConfig {
  public:
    ConfigWidgetConvolvePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_ConvolveConfig() {
      setupUi(this);
    }

    ~ConfigWidgetConvolvePlugin() {}

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
      if (ConvolveSource* source = static_cast<ConvolveSource*>(dataObject)) {
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
        _cfg->beginGroup("Convolve DataObject Plugin");
        _cfg->setValue("Input Vector One", _vectorOne->selectedVector()->Name());
        _cfg->setValue("Input Vector Two", _vectorTwo->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Convolve DataObject Plugin");
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


ConvolveSource::ConvolveSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


ConvolveSource::~ConvolveSource() {
}


QString ConvolveSource::_automaticDescriptiveName() const {
  return QString("Convolve Plugin Object");
}


void ConvolveSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetConvolvePlugin* config = static_cast<ConfigWidgetConvolvePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_ONE, config->selectedVectorOne());
    setInputVector(VECTOR_IN_TWO, config->selectedVectorTwo());
  }
}


void ConvolveSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool ConvolveSource::algorithm() {
  Kst::VectorPtr inputVectorOne = _inputVectors[VECTOR_IN_ONE];
  Kst::VectorPtr inputVectorTwo = _inputVectors[VECTOR_IN_TWO];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  if (inputVectorOne->length() <= 0 && inputVectorTwo->length() <= 0) {
    _errorString = "Error:  Input Vectors - invalid size";
    return false;
  }

  double* pdResponse;
  double* pdConvolve;
  double* pdResult;
  double  dReal;
  double  dImag;

  Kst::VectorPtr response;
  Kst::VectorPtr convolve;

  int iLength;
  int iLengthNew;

  bool bReturn = false;
  int iResponseMidpoint;

  // determine which is the response function:
  //  i.e. which is shorter...
  if (inputVectorOne->length() < inputVectorTwo->length()) {
    response = inputVectorOne;
    convolve = inputVectorTwo;
  } else {
    response = inputVectorTwo;
    convolve = inputVectorOne;
  }

  outputVector->resize(convolve->length(), false);

  iResponseMidpoint = response->length() / 2;
  iLength = convolve->length() + iResponseMidpoint;

  // round iLength up to the nearest factor of two...
  iLengthNew = 64;
  while (iLengthNew < iLength && iLengthNew > 0) {
    iLengthNew *= 2;
  }
  iLength = iLengthNew;

  if (iLength <= 0) {
    _errorString = "Error:  Invalid Input length calculated";
    return false;
  }

  pdResponse = new double[iLength];
  pdConvolve = new double[iLength];
  if (pdResponse != NULL && pdConvolve != NULL) {
    //
    // sort the response function into wrap-around order...
    //
    memset( pdResponse, 0, iLength * sizeof( double ) );

    for (int i = 0; i < iResponseMidpoint; i++) {
      pdResponse[i]                           = response->value()[iResponseMidpoint+i];
      pdResponse[iLength-iResponseMidpoint+i] = response->value()[i];
    }

    //
    // handle the case where the response function has an odd number of points...
    //
    if (iResponseMidpoint % 2 == 1) {
      pdResponse[iResponseMidpoint]           = response->value()[response->length()-1];
    }

    //
    // zero-pad the convolve array...
    //
    memset( pdConvolve, 0, iLength * sizeof( double ) );
    memcpy( pdConvolve, convolve->value(), convolve->length() * sizeof( double ) );

    //
    // calculate the FFTs of the two functions...
    //
    if (gsl_fft_real_radix2_transform( pdResponse, 1, iLength ) == 0) {
      if (gsl_fft_real_radix2_transform( pdConvolve, 1, iLength ) == 0) {
        //
        // multiply the FFTs together...
        //
        for (int i=0; i < iLength/2; i++) {
          if (i==0 || i==(iLength/2)-1) {
            pdResponse[i] = pdResponse[i] * pdConvolve[i];
          } else {
            dReal = pdResponse[i] * pdConvolve[i] - pdResponse[iLength-i] * pdConvolve[iLength-i];
            dImag = pdResponse[i] * pdConvolve[iLength-i] + pdResponse[iLength-i] * pdConvolve[i];

            pdResponse[i]         = dReal;
            pdResponse[iLength-i] = dImag;
          }
        }

        //
        // do the inverse FFT...
        //
        if (gsl_fft_halfcomplex_radix2_inverse( pdResponse, 1, iLength) == 0) {
          pdResult = outputVector->value();

          if (pdResult != NULL) {
            for (int i = 0; i < convolve->length(); ++i) {
              outputVector->value()[i] = pdResult[i];
            }

            memcpy( pdResult, pdResponse, convolve->length() * sizeof( double ) );

            bReturn = true;
          }
        }
      }
    }
  }
  delete[] pdResponse;
  delete[] pdConvolve;

  return bReturn;
}


Kst::VectorPtr ConvolveSource::vectorOne() const {
  return _inputVectors[VECTOR_IN_ONE];
}


Kst::VectorPtr ConvolveSource::vectorTwo() const {
  return _inputVectors[VECTOR_IN_TWO];
}


QStringList ConvolveSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_ONE);
  vectors += VECTOR_IN_TWO;
  return vectors;
}


QStringList ConvolveSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList ConvolveSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList ConvolveSource::outputVectorList() const {
  return QStringList(VECTOR_OUT);
}


QStringList ConvolveSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList ConvolveSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void ConvolveSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString ConvolvePlugin::pluginName() const { return "Convolve"; }
QString ConvolvePlugin::pluginDescription() const { return "Generates the convolution of one vector with another."; }


Kst::DataObject *ConvolvePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetConvolvePlugin* config = static_cast<ConfigWidgetConvolvePlugin*>(configWidget)) {

    ConvolveSource* object = store->createObject<ConvolveSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_ONE, config->selectedVectorOne());
      object->setInputVector(VECTOR_IN_TWO, config->selectedVectorTwo());
      object->setupOutputs();
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *ConvolvePlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetConvolvePlugin *widget = new ConfigWidgetConvolvePlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ConvolvePlugin, ConvolvePlugin)

// vim: ts=2 sw=2 et
