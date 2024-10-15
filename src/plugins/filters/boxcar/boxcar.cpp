/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2024 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "boxcar.h"
#include "objectstore.h"
#include "ui_boxcarconfig.h"

static const QString& VECTOR_IN = "Y Vector";
static const QString& SCALAR_IN = "Samples Scalar";
static const QString& VECTOR_OUT = "Y";

class ConfigBoxcarPlugin : public Kst::DataObjectConfigWidget, public Ui_BoxcarConfig {


    public:
    ConfigBoxcarPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_BoxcarConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigBoxcarPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) {
      _store = store;
      _vector->setObjectStore(store);
      _scalarSamples->setObjectStore(store);
      // _stagesSpin->setValue(3);
      // _sampleRateSpin->setValue(5.0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarSamples, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_stagesSpin, SIGNAL(valueChanged(int)), dialog, SIGNAL(modified()));
        connect(_sampleRateSpin, SIGNAL(valueChanged(double)), dialog, SIGNAL(modified()));
        connect(_scalarSamples, SIGNAL(selectionChanged(QString)), this, SLOT(updateLabels()));
        connect(_stagesSpin, SIGNAL(valueChanged(int)), this, SLOT(updateLabels()));
        connect(_sampleRateSpin, SIGNAL(valueChanged(double)), this, SLOT(updateLabels()));
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

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); }
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); }

    Kst::ScalarPtr selectedScalar() { return _scalarSamples->selectedScalar(); }
    void setSelectedScalar(Kst::ScalarPtr scalar) { return _scalarSamples->setSelectedScalar(scalar); }

    int stagesSpin() const {return _stagesSpin->value();}
    void setStagesSpin(int stage) {_stagesSpin->setValue(stage);}
    double sampleRateSpin() const {return _sampleRateSpin->value();}
    void setSampleRateSpin(double sr) {_sampleRateSpin->setValue(sr);}

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (BoxcarSource* source = static_cast<BoxcarSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedScalar(source->scalarSamples());
        setStagesSpin(source->stages());
        setSampleRateSpin(source->sampleRate());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);

      bool validTag = true;

      QStringRef av = attrs.value("Stages");
      if (!av.isNull()) {
        setStagesSpin(QVariant(av.toString()).toInt());
      } else {
        setStagesSpin(1);
      }

      av = attrs.value("SampleRate");
      if (!av.isNull()) {
        setSampleRateSpin(QVariant(av.toString()).toDouble());
      } else {
        setSampleRateSpin(1.0);
      }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Moving Average DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _scalarSamples->selectedScalar()->Name());
        _cfg->setValue("Stages", stagesSpin());
        _cfg->setValue("SampleRate", sampleRateSpin());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Boxcar DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        QString scalarName = _cfg->value("Input Scalar").toString();
        _scalarSamples->setSelectedScalar(scalarName);
        setStagesSpin(_cfg->value("Stages", 1).toInt());
        setSampleRateSpin(_cfg->value("SampleRate", 1.0).toDouble());

        _cfg->endGroup();
      }
    }

    virtual void updateLabels() {
      double sr = sampleRateSpin();
      double box_len = 11.0; //selectedScalar()->value();
      if (selectedScalar()) {
        int i_box_len = selectedScalar()->value();
        i_box_len = i_box_len/2;
        box_len = i_box_len*2+1;
        //selectedScalar()->setValue(box_len);
      }
      double stages = stagesSpin();

      double cutoff;
      double f;
      double p;

      // Cutoff relations emperically determined.
      // an anylytic is possible, but... not worth it?
      switch((int)box_len) {
      case 3:
        if (stages <5.1) {
          f = 0.2094;
          p = -0.67;
          cutoff = f/powf(2.0,p) * powf(stages+1.0, p);
        } else {
          f = 0.0860584165582;
          p = -0.49;
          cutoff = f/powf(8.0,p) * powf(stages+1.0, p);
        }
      break;
      case 5:
        if (stages <5.1) {
          f = 0.122474516267;
          p = -0.32;
          cutoff = f/powf(2.0,p) * powf(stages+1.0, p);
        } else {
          f = 0.07535;
          p = -0.45;
          cutoff = f/powf(8.0,p) * powf(stages+1.0, p);
        }
      break;
      case 7:
        if (stages <5.1) {
          f = 0.0868583287554;
          p = -0.35;
          cutoff = f/powf(2.0,p) * powf(stages+1.0, p);
        } else {
          f = 0.052;
          p = -0.45;
          cutoff = f/powf(8.0,p) * powf(stages+1.0, p);
        }
      break;
      default:
        f = 0.58/box_len;
        p=-0.427 + 0.07*(9/stages - 1);
        cutoff = f/powf(2.0,p) * powf(stages+1.0, p);
        break;
      }

      cutoff *= sr;

      QString str = QString("%1 Hz").arg(cutoff, 0, 'g', 2);
      _cutoffLabel->setText(str);
    }

  private:
    Kst::ObjectStore *_store;

};


BoxcarSource::BoxcarSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


BoxcarSource::~BoxcarSource() {
}


QString BoxcarSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Boxcar Low Pass").arg(vector()->descriptiveName());
  } else {
    return tr("Boxcar Low Pass");
  }
}

QString BoxcarSource::descriptionTip() const {
  QString tip;

  tip = tr("Boxcar Lowpass: %1\n").arg(Name());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void BoxcarSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigBoxcarPlugin* config = static_cast<ConfigBoxcarPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
    setInputScalar(SCALAR_IN, config->selectedScalar());
    setStages(config->stagesSpin());
    setSampleRate(config->sampleRateSpin());
  }
}


void BoxcarSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}

void BoxcarSource::SingleStageBoxcar(double *v_out, const double *v_in, int vec_len, int box_len) {
  // the box len must be shorter than the length of the vector.
  box_len = qMin(box_len, vec_len);
  // In order to maintain 0 phase shift,. box_len must be odd.
  if (box_len%2 == 0) {
    box_len -= 1;
  }
  // box_len must be at least sample long.
  box_len = qMax(box_len, 1);

  int i,j,n;
  // vec_len = 12
  // box_len = 5
  // 0            1            2            3            4            5            6            7            8            9            a          b
  // 0           012         01234        12345        23456        34567        45678        56789        6789a        789ab         9ab         b

  // vec_len = 10
  // box_len = 9
  // 0            1            2            3            4            5            6            7            8            9
  // 0           012         01234       0123456     012345678    123456789     3456789       56789         789           9

  // vec_len = 11
  // box_len = 11
  // 0            1            2            3            4            5            6            7            8            9            a
  // 0           012         01234       0123456     012345678   0123456789a   23456789a     456789a       6789a         89a           a

  // Underfiltered head
  v_out[0] = v_in[0];
  n=1;
  i=1;
  while (n<box_len) {
    v_out[i] = v_out[i-1] + v_in[n] + v_in[n+1];
    v_out[i-1] /= (double)n;
    n+=2;
    ++i;
  }

  // main part
  int b2 = box_len/2;
  while (i<vec_len - b2) {
    v_out[i] = v_out[i-1] - v_in[i-b2-1] + v_in[i+b2];
    v_out[i-1] /= (double)box_len;
    ++i;
  }

  j = i-b2-1;

  // underfiltered tail
  n = box_len;
  while (i<vec_len) {
    v_out[i] = v_out[i-1] - v_in[j] - v_in[j+1];
    v_out[i-1] /= (double)n;
    n-=2;
    j+=2;
    ++i;
  }

}


bool BoxcarSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::ScalarPtr inputScalar = _inputScalars[SCALAR_IN];
  Kst::VectorPtr outputVector;
  // maintain kst file compatibility if the output vector name is changed.
  if (_outputVectors.contains(VECTOR_OUT)) {
    outputVector = _outputVectors[VECTOR_OUT];
  } else {
    outputVector = _outputVectors.values().at(0);
  }
  int vec_len = inputVector->length();


  /* Memory allocation */
  outputVector->resize(vec_len, true);

  double const *v_in = inputVector->noNanValue();
  double *v_out = outputVector->raw_V_ptr();
  int box_len = int(inputScalar->value());
  double scaled_boxlen = box_len;

  SingleStageBoxcar(v_out, v_in, vec_len, box_len);

  if (stages() == 1) {
    SingleStageBoxcar(v_out, v_in, vec_len, box_len); // s0
  } else {
    // Use a multi stage filter.
    // Scale the box length each stage
    // to fill in the ripples in the stop band.
    double A = pow(0.5,1.0/(double)(stages()+1)); // geometric factor for scaling filter length.

    double *v_scratch = new double[vec_len];
    SingleStageBoxcar(v_scratch, v_in, vec_len, box_len); // s0
    scaled_boxlen *= A;
    box_len = qMax(3, (int)scaled_boxlen);
    int stage;
    for (stage = 1; stage < stages()-1; stage+=2) {
      SingleStageBoxcar(v_out, v_scratch, vec_len, box_len); // s1
      scaled_boxlen *= A;
      box_len = qMax(3, (int)scaled_boxlen);
      SingleStageBoxcar(v_scratch, v_out, vec_len, box_len); // s2
      scaled_boxlen *= A;
      box_len = qMax(3, (int)scaled_boxlen);
    }
    if (stage<stages()) {
      SingleStageBoxcar(v_out, v_scratch, vec_len, box_len); // s3
    } else {
      for (int i=0; i<vec_len; i++) {
        v_out[i] = v_scratch[i];
      }
    }

    delete[] v_scratch;
  }
  return true;
}


Kst::VectorPtr BoxcarSource::vector() const {
  return _inputVectors[VECTOR_IN];
}

Kst::ScalarPtr BoxcarSource::scalarSamples() const {
  return _inputScalars[SCALAR_IN];
}


QStringList BoxcarSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList BoxcarSource::inputScalarList() const {
  return QStringList( SCALAR_IN );
}


QStringList BoxcarSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList BoxcarSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList BoxcarSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList BoxcarSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}

void BoxcarSource::setProperty(const QString &key, const QString &val) {
    if (key == "Stages") {
      setStages(val.toDouble());
    }

    if (key == "SampleRate") {
      setSampleRate(val.toDouble());
    }
}

void BoxcarSource::saveProperties(QXmlStreamWriter &s) {
   s.writeAttribute("Stages", QString::number(stages()));
   s.writeAttribute("SampleRate", QString::number(sampleRate()));
}


QString BoxcarPlugin::pluginName() const { return tr("Boxcar Lowpass Filter"); }
QString BoxcarPlugin::pluginDescription() const { return tr("Computes the boxcar lowpass of the input vector."); }


Kst::DataObject *BoxcarPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigBoxcarPlugin* config = static_cast<ConfigBoxcarPlugin*>(configWidget)) {

    BoxcarSource* object = store->createObject<BoxcarSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN, config->selectedScalar());
      object->setupOutputs();
      object->setInputVector(VECTOR_IN, config->selectedVector());
    }

    object->setStages(config->stagesSpin());
    object->setSampleRate(config->sampleRateSpin());
    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *BoxcarPlugin::configWidget(QSettings *settingsObject) const {
  ConfigBoxcarPlugin *widget = new ConfigBoxcarPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, BoxcarPlugin)
#endif

// vim: ts=2 sw=2 et
