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
  return QString("Cross Spectrum Plugin Object");
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
  Kst::VectorPtr inputVectorOne = _inputVectors[VECTOR_IN_ONE];
  Kst::VectorPtr inputVectorTwo = _inputVectors[VECTOR_IN_TWO];
  Kst::ScalarPtr inputScalarFFT = _inputScalars[SCALAR_IN_FFT];
  Kst::ScalarPtr inputScalarRate = _inputScalars[SCALAR_IN_RATE];
  Kst::VectorPtr outputVectorFrequency = _outputVectors[VECTOR_OUT_FREQ];
  Kst::VectorPtr outputVectorImaginary = _outputVectors[VECTOR_OUT_IMAG];
  Kst::VectorPtr outputVectorReal = _outputVectors[VECTOR_OUT_REAL];

  double SR = inputScalarRate->value(); // sample rate
  double df;
  int i,  xps_len;
  double *a,  *b;
  double mean_a,  mean_b;
  int dv0,  dv1,  v_len;
  int i_subset,  n_subsets;
  int i_samp,  copyLen;
  double norm_factor;

  /* parse fft length */
  xps_len = int( inputScalarRate->value() - 0.99);
  if ( xps_len > KSTPSDMAXLEN ) {
    xps_len = KSTPSDMAXLEN;
  }
  if ( xps_len<2 )  {
    xps_len = 2;
  }
  xps_len = int ( pow( 2.0,  xps_len ) );

  /* input vector lengths */
  v_len = ( ( inputVectorOne->length() < inputVectorTwo->length() ) ? inputVectorOne->length() : inputVectorTwo->length() );
  dv0 = v_len/inputVectorOne->length();
  dv1 = v_len/inputVectorTwo->length();

  while ( xps_len > v_len ) {
    xps_len/=2;
  }

  // allocate the lengths
  outputVectorReal->resize( xps_len, false );
  outputVectorImaginary->resize( xps_len, false );
  outputVectorFrequency->resize( xps_len, false );

  /* Fill the frequency and zero the xps */
  df = SR/( 2.0*double( xps_len-1 ) );
  for ( i=0; i<xps_len; i++ ) {
    outputVectorFrequency->value()[i] = double( i ) * df;
    outputVectorReal->value()[i] = 0.0;
    outputVectorImaginary->value()[i] = 0.0;
  }

  /* allocate input arrays */
  int ALen = xps_len * 2;
  a = new double[ALen];
  b = new double[ALen];

  /* do the fft's */
  n_subsets = v_len/xps_len + 1;

  for ( i_subset=0; i_subset<n_subsets; i_subset++ ) {
        /* copy each chunk into a[] and find mean */
    if (i_subset*xps_len + ALen <= v_len) {
      copyLen = ALen;
    } else {
      copyLen = v_len - i_subset*xps_len;
    }
    mean_b = mean_a = 0;
    for (i_samp = 0; i_samp < copyLen; i_samp++) {
      i = ( i_samp + i_subset*xps_len )/dv0;
      mean_a += (
        a[i_samp] = inputVectorOne->value()[i]
        );
      i = ( i_samp + i_subset*xps_len )/dv1;
      mean_b += (
        b[i_samp] = inputVectorTwo->value()[i]
        );
    }
    if (copyLen>1) {
      mean_a/=(double)copyLen;
      mean_b/=(double)copyLen;
    }

    /* Remove Mean and apodize */
    for (i_samp=0; i_samp<copyLen; i_samp++) {
      a[i_samp] -= mean_a;
      b[i_samp] -= mean_b;
    }

    for (;i_samp < ALen; i_samp++) {
      a[i_samp] = 0.0;
      b[i_samp] = 0.0;
    }

    /* fft */
    rdft(ALen, 1, a);
    rdft(ALen, 1, b);

    /* sum each bin into psd[] */
    outputVectorReal->value()[0] += ( a[0]*b[0] );
    outputVectorReal->value()[xps_len-1] += ( a[1]*b[1] );
    for (i_samp=1; i_samp<xps_len-1; i_samp++) {
      outputVectorReal->value()[i_samp]+= ( a[i_samp*2] * b[i_samp*2] -
                                   a[i_samp*2+1] * b[i_samp*2+1] );
      outputVectorImaginary->value()[i_samp]+= ( -a[i_samp*2] * b[i_samp*2+1] +
                                   a[i_samp*2+1] * b[i_samp*2] );
    }// (a+ci)(b+di)* = ab+cd +i(-ad + cb)
  }

  /* renormalize */
  norm_factor = 1.0/((double(SR)*double(xps_len))*double(n_subsets));
  for ( i=0; i<xps_len; i++ ) {
    outputVectorReal->value()[i]*=norm_factor;
    outputVectorImaginary->value()[i]*=norm_factor;
  }

  delete[] b;
  delete[] a;

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


QString CrossSpectrumPlugin::pluginName() const { return "Cross Spectrum DataObject Plugin"; }
QString CrossSpectrumPlugin::pluginDescription() const { return "Generates the cross power spectrum of one vector with another."; }


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

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, CrossSpectrumPlugin)

// vim: ts=2 sw=2 et
