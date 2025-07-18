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


#include "crossspectrum.h"
#include "objectstore.h"
#include "ui_crossspectrumconfig.h"

static const QString& VECTOR_IN_ONE = "Vector In One";
static const QString& VECTOR_IN_TWO = "Vector In Two";
static const QString& SCALAR_IN_FFT = "Scalar In FFT";
static const QString& SCALAR_IN_RATE = "Scalar In Sample Rate";
static const QString& VECTOR_OUT_FREQ = "Frequency";
static const QString& VECTOR_OUT_IMAG = "Imaginary";
static const QString& VECTOR_OUT_REAL = "Real";

class ConfigCrossSpectrumPlugin : public Kst::DataObjectConfigWidget, public Ui_CrossSpectrumConfig {
  public:
    ConfigCrossSpectrumPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_CrossSpectrumConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigCrossSpectrumPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorOne->setObjectStore(store);
      _vectorTwo->setObjectStore(store);
      _scalarFFT->setObjectStore(store);
      _scalarRate->setObjectStore(store);
      _scalarFFT->setDefaultValue(10);
      _scalarRate->setDefaultValue(100);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorOne, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorTwo, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarFFT, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarRate, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorOne() { return _vectorOne->selectedVector(); };
    void setSelectedVectorOne(Kst::VectorPtr vector) { return _vectorOne->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorTwo() { return _vectorTwo->selectedVector(); };
    void setSelectedVectorTwo(Kst::VectorPtr vector) { return _vectorTwo->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarFFT() { return _scalarFFT->selectedScalar(); };
    void setSelectedScalarFFT(Kst::ScalarPtr scalar) { return _scalarFFT->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarRate() { return _scalarRate->selectedScalar(); };
    void setSelectedScalarRate(Kst::ScalarPtr scalar) { return _scalarRate->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (CrossSpectrumSource* source = static_cast<CrossSpectrumSource*>(dataObject)) {
        setSelectedVectorOne(source->vectorOne());
        setSelectedVectorTwo(source->vectorTwo());
        setSelectedScalarFFT(source->scalarFFT());
        setSelectedScalarRate(source->scalarRate());
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
        _cfg->beginGroup("Cross Spectrum DataObject Plugin");
        _cfg->setValue("Input Vector One", _vectorOne->selectedVector()->Name());
        _cfg->setValue("Input Vector Two", _vectorTwo->selectedVector()->Name());
        _cfg->setValue("Input Scalar FFT", _scalarFFT->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Sample Rate", _scalarRate->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Cross Spectrum DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector One").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorOne = static_cast<Kst::Vector*>(object);
        if (vectorOne) {
          setSelectedVectorOne(vectorOne);
        }
        vectorName = _cfg->value("Input Vector Two").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorTwo = static_cast<Kst::Vector*>(object);
        if (vectorTwo) {
          setSelectedVectorTwo(vectorTwo);
        }
        QString scalarName = _cfg->value("Input Scalar FFT").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarFFT = static_cast<Kst::Scalar*>(object);
        if (scalarFFT) {
          setSelectedScalarFFT(scalarFFT);
        }
        scalarName = _cfg->value("Input Scalar Sample Rate").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarRate = static_cast<Kst::Scalar*>(object);
        if (scalarRate) {
          setSelectedScalarRate(scalarRate);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


CrossSpectrumSource::CrossSpectrumSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


CrossSpectrumSource::~CrossSpectrumSource() {
}


QString CrossSpectrumSource::_automaticDescriptiveName() const {
  return tr("Cross Spectrum");
}


void CrossSpectrumSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigCrossSpectrumPlugin* config = static_cast<ConfigCrossSpectrumPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_ONE, config->selectedVectorOne());
    setInputVector(VECTOR_IN_TWO, config->selectedVectorTwo());
    setInputScalar(SCALAR_IN_FFT, config->selectedScalarFFT());
    setInputScalar(SCALAR_IN_RATE, config->selectedScalarRate());
  }
}


void CrossSpectrumSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_FREQ, "");
  setOutputVector(VECTOR_OUT_IMAG, "");
  setOutputVector(VECTOR_OUT_REAL, "");
}

#define KSTPSDMAXLEN 27
extern "C" void rdft(int n, int isgn, double *a); //fast fourier transform...

bool CrossSpectrumSource::algorithm() {
  Kst::VectorPtr iv1 = _inputVectors[VECTOR_IN_ONE];
  Kst::VectorPtr iv2 = _inputVectors[VECTOR_IN_TWO];

  double fft_len_exponent = _inputScalars[SCALAR_IN_FFT]->value();
  double SR = _inputScalars[SCALAR_IN_RATE]->value();

  int xps_len;
  
  double df;
  int i;
  int v_len;

  /* parse fft length */
  if ( fft_len_exponent > KSTPSDMAXLEN ) {
    fft_len_exponent = KSTPSDMAXLEN;
  }
  if ( fft_len_exponent < 2.0 )  {
    fft_len_exponent = 2.0;
  }
  xps_len = int (pow(2.0,  fft_len_exponent-1) + 0.1);

  /* input vector lengths - use the shorter one. */
  v_len = ( ( iv1->length() < iv2->length() ) ? iv1->length() : iv2->length() );

  while ( xps_len > v_len ) {
    xps_len/=2;
  }

  // allocate the lengths
  _outputVectors[VECTOR_OUT_REAL]->resize( xps_len, false );
  _outputVectors[VECTOR_OUT_IMAG]->resize( xps_len, false );
  _outputVectors[VECTOR_OUT_FREQ]->resize( xps_len, false );

  double *xspec_real = _outputVectors[VECTOR_OUT_REAL]->raw_V_ptr();
  double *xspec_imag = _outputVectors[VECTOR_OUT_IMAG]->raw_V_ptr();
  double *f = _outputVectors[VECTOR_OUT_FREQ]->raw_V_ptr();


  /* Fill the frequency and zero the xps */
  df = SR/( 2.0*double( xps_len-1 ) );
  for ( i=0; i<xps_len; ++i ) {
    f[i] = double( i ) * df;
    xspec_real[i] = 0.0;
    xspec_imag[i] = 0.0;
  }

  _psdCalculator.calculatePowerSpectrum(iv1->noNanValue(), v_len,
                                        xspec_real, xps_len,
                                        true,
                                        true, fft_len_exponent,
                                        true, WindowOriginal, 0,
                                        PSDPowerSpectrum, SR,
                                        iv2->noNanValue(), v_len, xspec_imag);


  Kst::LabelInfo label_info;

  label_info.quantity = tr("Cross Spectrum (real)");
  label_info.units.clear();
  label_info.name.clear();
  _outputVectors[VECTOR_OUT_REAL]->setLabelInfo(label_info);

  label_info.quantity = tr("Cross Spectrum (imaginary)");
  _outputVectors[VECTOR_OUT_IMAG]->setLabelInfo(label_info);

  label_info.quantity = tr("Frequency");
  _outputVectors[VECTOR_OUT_FREQ]->setLabelInfo(label_info);

  return true;
}


Kst::VectorPtr CrossSpectrumSource::vectorOne() const {
  return _inputVectors[VECTOR_IN_ONE];
}


Kst::VectorPtr CrossSpectrumSource::vectorTwo() const {
  return _inputVectors[VECTOR_IN_TWO];
}


Kst::ScalarPtr CrossSpectrumSource::scalarFFT() const {
  return _inputScalars[SCALAR_IN_FFT];
}


Kst::ScalarPtr CrossSpectrumSource::scalarRate() const {
  return _inputScalars[SCALAR_IN_RATE];
}


QStringList CrossSpectrumSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_ONE);
  vectors += VECTOR_IN_TWO;
  return vectors;
}


QStringList CrossSpectrumSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_FFT);
  scalars += SCALAR_IN_RATE;
  return scalars;
}


QStringList CrossSpectrumSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList CrossSpectrumSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_FREQ);
  vectors += VECTOR_OUT_IMAG;
  vectors += VECTOR_OUT_REAL;
  return vectors;
}


QStringList CrossSpectrumSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList CrossSpectrumSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void CrossSpectrumSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString CrossSpectrumPlugin::pluginName() const { return tr("Cross Spectrum"); }
QString CrossSpectrumPlugin::pluginDescription() const { return tr("Generates the cross power spectrum of one vector with another."); }


Kst::DataObject *CrossSpectrumPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigCrossSpectrumPlugin* config = static_cast<ConfigCrossSpectrumPlugin*>(configWidget)) {

    CrossSpectrumSource* object = store->createObject<CrossSpectrumSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN_FFT, config->selectedScalarFFT());
      object->setInputScalar(SCALAR_IN_RATE, config->selectedScalarRate());
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


Kst::DataObjectConfigWidget *CrossSpectrumPlugin::configWidget(QSettings *settingsObject) const {
  ConfigCrossSpectrumPlugin *widget = new ConfigCrossSpectrumPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
