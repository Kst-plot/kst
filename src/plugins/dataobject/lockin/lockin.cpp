/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                   sbenton@phsyics.utoronto.ca                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "lockin.h"
#include "objectstore.h"
#include "ui_lockinconfig.h"
#include "iirfilter.h"

#include <vector>
#include <complex>

using std::vector;
typedef std::complex<double> complex;

//TODO maybe use Qt vector types, and kst filters
//TODO allow reference waveforms other than square wave
//TODO these parameters, and filter cutoffs, and the guess period should be made commandable
const double f_samp = 4.0E6/384.0/104.0;    //sample frequency
const int DELAY = 0;



static const QString& SHORT_NAME = "Lock-In";
static const QString& PLUGIN_NAME = "Lock-In DataObject Plugin";
static const QString& PLUGIN_DESCRIPTION = "Lock-In amplifies Input using a Reference carrier wave";
static const QString& IN_INPUT_VECTOR = "Input Vector";
static const QString& IN_REF_VECTOR = "Reference Vector";
static const QString& OUT_REF_VECTOR_NORM = "Normalized Reference Vector";
static const QString& OUT_LOCKIN_RESULT = "Lock-In Result";

class ConfigLockInPlugin : public Kst::DataObjectConfigWidget, public Ui_LockInConfig {
  public:
    ConfigLockInPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_LockInConfig() {
      setupUi(this);
    }

    ~ConfigLockInPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _inputVector->setObjectStore(store);
      _refVector->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_inputVector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_refVector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedInputVector() { return _inputVector->selectedVector(); };
    void setSelectedInputVector(Kst::VectorPtr vector) { return _inputVector->setSelectedVector(vector); };

    Kst::VectorPtr selectedRefVector() { return _refVector->selectedVector(); };
    void setSelectedRefVector(Kst::VectorPtr vector) { return _refVector->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (LockInSource* source = static_cast<LockInSource*>(dataObject)) {
        setSelectedInputVector(source->inputVector());
        setSelectedRefVector(source->refVector());
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
        _cfg->beginGroup(PLUGIN_NAME);
        _cfg->setValue(IN_INPUT_VECTOR, _inputVector->selectedVector()->Name());
        _cfg->setValue(IN_REF_VECTOR, _refVector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup(PLUGIN_NAME);
        QString vectorName = _cfg->value(IN_INPUT_VECTOR).toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedInputVector(vector);
        }
        vectorName = _cfg->value(IN_REF_VECTOR).toString();
        Kst::Object* object2 = _store->retrieveObject(vectorName);
        Kst::Vector* vector2 = static_cast<Kst::Vector*>(object2);
        if (vector2) {
          setSelectedRefVector(vector2);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


LockInSource::LockInSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


LockInSource::~LockInSource() {
}


QString LockInSource::_automaticDescriptiveName() const {
  return PLUGIN_NAME;
}


void LockInSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigLockInPlugin* config = static_cast<ConfigLockInPlugin*>(configWidget)) {
    setInputVector(IN_INPUT_VECTOR, config->selectedInputVector());
    setInputVector(IN_REF_VECTOR, config->selectedRefVector());
  }
}


void LockInSource::setupOutputs() {
  setOutputVector(OUT_REF_VECTOR_NORM, "");
  setOutputVector(OUT_LOCKIN_RESULT, "");
}

/* REALLY BAD CODE
 * use two copies of this function, to have independent static variables
 */
static complex f_pll(double ref)
{
  static double period = 20.0;
  static double phase = 0.0;

  static double T = 0.0;

  static int ref_edge = 0;
  static int pll_edge = 0;

  static double ph = 0.0;
  static double dph = 0.0;

  static BesselHP1<double> fref(0.0125/f_samp);
  static BesselLP1<double> fperiod(0.15/f_samp);
  static BesselLP1<double> fphase(0.15/f_samp);

  double ref_ = fref(ref);
  ph += dph;
  T  += 1.0;
  double pll_ref = cos(ph);

  if(ref_edge == 0  && ref_ >= 0.0) {
    period = fperiod(T);
    ref_edge = 1;
    dph = (2.0*M_PI)/period;
    T =  0.0;
  } else if(ref_edge == 1 && ref_ < 0.0 && T > 1.0) {
    ref_edge = 0;
  }


  if(pll_edge == 0 && (pll_ref >= 0.0)) {
    phase = fphase(T*dph);
    pll_edge = 1;
  } else if(pll_edge == 1 && (pll_ref < 0.0)) {
    pll_edge = 0;
  }

  return complex( ((cos(ph+phase) > 0.0) ? 1 : -1),
         ((sin(ph+phase) > 0.0) ? 1 : -1) );
}

static complex b_pll(double ref)
{
  static double period = 20.0;
  static double phase = 0.0;

  static double T = 0.0;

  static int ref_edge = 0;
  static int pll_edge = 0;

  static double ph = 0.0;
  static double dph = 0.0;

  static BesselHP1<double> fref(0.0125/f_samp);
  static BesselLP1<double> fperiod(0.15/f_samp);
  static BesselLP1<double> fphase(0.15/f_samp);

  double ref_ = fref(ref);
  ph += dph;
  T  += 1.0;
  double pll_ref = cos(ph);

  if(ref_edge == 0  && ref_ >= 0.0) {
    period = fperiod(T);
    ref_edge = 1;
    dph = (2.0*M_PI)/period;
    T =  0.0;
  } else if(ref_edge == 1 && ref_ < 0.0 && T > 1.0) {
    ref_edge = 0;
  }


  if(pll_edge == 0 && (pll_ref >= 0.0)) {
    phase = fphase(T*dph);
    pll_edge = 1;
  } else if(pll_edge == 1 && (pll_ref < 0.0)) {
    pll_edge = 0;
  }

  return complex( ((cos(ph+phase) > 0.0) ? 1 : -1),
         ((sin(ph+phase) > 0.0) ? 1 : -1) );
}

bool LockInSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[IN_INPUT_VECTOR];
  Kst::VectorPtr refVector = _inputVectors[IN_REF_VECTOR];
  Kst::VectorPtr normRefVector = _outputVectors[OUT_REF_VECTOR_NORM];
  Kst::VectorPtr lockinResult = _outputVectors[OUT_LOCKIN_RESULT];

  if (inputVector->length() <= 0 || refVector->length() <= 0 || inputVector->length() != refVector->length()) {
    _errorString = "Error:  Input Vectors - invalid size";
    return false;
  }

  int iLength = inputVector->length();

  normRefVector->resize(iLength, false);
  lockinResult->resize(iLength, false);

  double* pdInput = inputVector->value();
  double* pdRef = refVector->value();
  double* pdNormRef = normRefVector->value();
  double* pdResult = lockinResult->value();

  /* to remove initial settling, evaluate the result separately in forward and backward direction */
  BesselHP1<double> f_filt_d(0.15/f_samp);
  BesselLP4<complex> f_filt_li(0.39/f_samp);
  BesselHP1<double> b_filt_d(0.15/f_samp);
  BesselLP4<complex> b_filt_li(0.39/f_samp);

  for (int i=0; i<iLength; i++) {
    complex f_pll_ref = f_pll(pdRef[i]);
    double f_result = abs(f_filt_li(f_filt_d(pdInput[i])*f_pll_ref));
    complex b_pll_ref = b_pll(pdRef[iLength-i-1]);
    double b_result = abs(b_filt_li(b_filt_d(pdInput[iLength-i-1])*b_pll_ref));
    //combine the forwards and backwards data into result
    if (i >= iLength/2) {
      if (i >= (iLength*3)/4) {
        //for first and last quarter of the domain, use backward and forward result
        pdResult[i] = f_result;
        pdResult[iLength-i-1] = b_result;
      } else {
        //for middle half, use weight that changes linearly between the extremes
        double weight = 2.0*double(i)/double(iLength) - 0.5;
        pdResult[i] = weight*f_result + (1.0-weight)*b_result;
        pdResult[iLength-i-1] = weight*b_result + (1.0-weight)*f_result;
      }
      //for the reference, just split the result at the middle
      pdNormRef[i] = f_pll_ref.real();
      pdNormRef[iLength-i-1] = b_pll_ref.real();
    }
  }

  return true;
}


Kst::VectorPtr LockInSource::inputVector() const {
  return _inputVectors[IN_INPUT_VECTOR];
}


Kst::VectorPtr LockInSource::refVector() const {
  return _inputVectors[IN_REF_VECTOR];
}


QStringList LockInSource::inputVectorList() const {
  QStringList vectors(IN_INPUT_VECTOR);
  vectors += IN_REF_VECTOR;
  return vectors;
}


QStringList LockInSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList LockInSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList LockInSource::outputVectorList() const {
  QStringList vectors(OUT_LOCKIN_RESULT);
  vectors += OUT_REF_VECTOR_NORM;
  return vectors;
}


QStringList LockInSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList LockInSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void LockInSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString LockInPlugin::pluginName() const { return SHORT_NAME; }
QString LockInPlugin::pluginDescription() const { return PLUGIN_DESCRIPTION; }


Kst::DataObject *LockInPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigLockInPlugin* config = static_cast<ConfigLockInPlugin*>(configWidget)) {

    LockInSource* object = store->createObject<LockInSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(IN_INPUT_VECTOR, config->selectedInputVector());
      object->setInputVector(IN_REF_VECTOR, config->selectedRefVector());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *LockInPlugin::configWidget(QSettings *settingsObject) const {
  ConfigLockInPlugin *widget = new ConfigLockInPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_LockInPlugin, LockInPlugin)

// vim: ts=2 sw=2 et
