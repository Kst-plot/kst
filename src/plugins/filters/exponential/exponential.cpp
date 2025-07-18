/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2019 William Ledda                                    *
 *                   villy80@hotmail.it                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "exponential.h"
#include "objectstore.h"
#include "ui_exponentialconfig.h"

#include <iostream>

static const QString& VECTOR_IN   = "Vector In";
static const QString& EXP_IN      = "Alpha exponent";
static const QString& VECTOR_OUT  = "Avg(Y)";

class ConfigExponentialPlugin : public Kst::DataObjectConfigWidget, public Ui_ExponentialConfig {
  public:
    ConfigExponentialPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_ExponentialConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigExponentialPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store);
      _exp->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_exp, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
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

    Kst::VectorPtr selectedVector() {
        return _vector->selectedVector();
    }

    void setSelectedVector(Kst::VectorPtr vector) {
        return _vector->setSelectedVector(vector);
    }

    Kst::ScalarPtr selectedScalar() {
        return _exp->selectedScalar();
    }

    void setSelectedAlphaExp(Kst::ScalarPtr scalar) {
        return _exp->setSelectedScalar(scalar);
    }

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (ExponentialSource* source = static_cast<ExponentialSource*>(dataObject)) {
        setSelectedVector(source->vector());
        setSelectedAlphaExp(source->alphaExp());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {
      Q_UNUSED(store);
      Q_UNUSED(attrs);
      return true;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Exponential Filter DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->setValue("Input Scalar", _exp->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Exponential Filter DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }

        QString scalarName = _cfg->value("Input Scalar").toString();
        _exp->setSelectedScalar(scalarName);
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


ExponentialSource::ExponentialSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


ExponentialSource::~ExponentialSource() {
}


QString ExponentialSource::_automaticDescriptiveName() const {
  if (vector()) {
    return tr("%1 Exponential Filter").arg(vector()->descriptiveName());
  } else {
    return tr("Exponential Filter");
  }
}

QString ExponentialSource::descriptionTip() const {
  QString tip;

  tip = tr("Exponential Filter: %1\n").arg(Name());

  tip += tr("\nInput: %1").arg(vector()->descriptionTip());
  return tip;
}


void ExponentialSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigExponentialPlugin* config = static_cast<ConfigExponentialPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN,   config->selectedVector());
    setInputScalar(EXP_IN, config->selectedScalar());
  }
}


void ExponentialSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}

// TODO
bool ExponentialSource::algorithm() {
    Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
    Kst::ScalarPtr inputScalar = _inputScalars[EXP_IN];
    Kst::VectorPtr outputVector;
    // maintain kst file compatibility if the output vector name is changed.
    if (_outputVectors.contains(VECTOR_OUT)) {
      outputVector = _outputVectors[VECTOR_OUT];
    } else {
      outputVector = _outputVectors.values().at(0);
    }


    /* Memory allocation */
    outputVector->resize(inputVector->length(), true);

    if(inputVector->length() > 0) {
        double const *v_in = inputVector->noNanValue();
        double *v_out = outputVector->raw_V_ptr();
        const double ALPHA = inputScalar->value();
        //!> Since (1 - ALPHA) is constant as well, there is no need to compute at each cycle
        const double ALPHA_HIST = 1 - ALPHA;
        int len = inputVector->length();

        v_out[0] = v_in[0];

        for (int t = 1; t < len; t++) {
            //exp filter = alpha * x(t) + (1 - alpha) * x(t - 1)
            v_out[t] = ALPHA * v_in[t] + ALPHA_HIST * v_out[t - 1];
        }

       Kst::LabelInfo label_info = inputVector->labelInfo();
       label_info.name = tr("%1 exp %2").arg(label_info.name).arg(ALPHA);
       outputVector->setLabelInfo(label_info);
    }

    return true;
}


Kst::VectorPtr ExponentialSource::vector() const {
  return _inputVectors[VECTOR_IN];
}

Kst::ScalarPtr ExponentialSource::alphaExp() const
{
    return _inputScalars[EXP_IN];
}


QStringList ExponentialSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}

QStringList ExponentialSource::inputScalarList() const {
  return QStringList( EXP_IN );
}

QStringList ExponentialSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}

QStringList ExponentialSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}

QStringList ExponentialSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList ExponentialSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void ExponentialSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString ExponentialPlugin::pluginName() const { return tr("Exponential filter"); }
QString ExponentialPlugin::pluginDescription() const { return tr("Computes the exponential of an input vector."); }


Kst::DataObject *ExponentialPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigExponentialPlugin* config = static_cast<ConfigExponentialPlugin*>(configWidget)) {

    ExponentialSource* object = store->createObject<ExponentialSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(EXP_IN, config->selectedScalar());
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


Kst::DataObjectConfigWidget *ExponentialPlugin::configWidget(QSettings *settingsObject) const {
  ConfigExponentialPlugin *widget = new ConfigExponentialPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
