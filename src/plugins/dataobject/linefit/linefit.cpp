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


#include "linefit.h"
#include "objectstore.h"
#include "ui_linefitconfig.h"

#include "math_kst.h"

static const QString& VECTOR_IN_X = "Vector In X";
static const QString& VECTOR_IN_Y = "Vector In Y";
static const QString& VECTOR_OUT_X = "X Interpolated";
static const QString& VECTOR_OUT_Y = "Y Interpolated";

static const QString& SCALAR_OUT_A = "a";
static const QString& SCALAR_OUT_B = "b";
static const QString& SCALAR_OUT_CHI2 = "chi^2";


class ConfigWidgetLineFitPlugin : public Kst::DataObjectConfigWidget, public Ui_LineFitConfig {
  public:
    ConfigWidgetLineFitPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_LineFitConfig() {
      setupUi(this);
    }

    ~ConfigWidgetLineFitPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store); 
      _vectorY->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (LineFitSource* source = static_cast<LineFitSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
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
        _cfg->beginGroup("Line Fit DataObject Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Line Fit DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVectorX(vector);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectory = static_cast<Kst::Vector*>(object);
        if (vectory) {
          setSelectedVectorY(vectory);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


LineFitSource::LineFitSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


LineFitSource::~LineFitSource() {
}


QString LineFitSource::_automaticDescriptiveName() const {
  return QString("Line Fit Plugin Object");
}


void LineFitSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetLineFitPlugin* config = static_cast<ConfigWidgetLineFitPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void LineFitSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_X, "");
  setOutputVector(VECTOR_OUT_Y, "");
  setOutputScalar(SCALAR_OUT_A, "");
  setOutputScalar(SCALAR_OUT_B, "");
  setOutputScalar(SCALAR_OUT_CHI2, "");
}


bool LineFitSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr outputVectorX = _outputVectors[VECTOR_OUT_X];
  Kst::VectorPtr outputVectorY = _outputVectors[VECTOR_OUT_Y];

  Kst::ScalarPtr outputScalarA = _outputScalars[SCALAR_OUT_A];
  Kst::ScalarPtr outputScalarB = _outputScalars[SCALAR_OUT_B];
  Kst::ScalarPtr outputScalarChi = _outputScalars[SCALAR_OUT_CHI2];

  int i = 0;
  double a = 0.0, b = 0.0, sx = 0.0, sy = 0.0, sxoss = 0.0, st2 = 0.0, chi2 = 0.0;
  double xScale;

  if (inputVectorY->length() < 1) {
    _errorString = "Error: Input Vector Y Length invalid";
    return false;
  }

  if (inputVectorX->length() < 1) {
    _errorString = "Error: Input Vector X Length invalid";
    return false;
  }

  outputVectorX->resize( 2, false );
  outputVectorY->resize( 2, false );

  if (inputVectorY->length() == 1) {
    outputVectorX->value()[0] = inputVectorX->value()[0];
    outputVectorX->value()[1] = inputVectorX->value()[inputVectorX->length() - 1];
    outputVectorY->value()[0] = inputVectorY->value()[0];
    outputVectorY->value()[1] = inputVectorY->value()[0];
    outputScalarA->setValue( inputVectorY->value()[0] );
    outputScalarB->setValue( 0 );
    outputScalarChi->setValue( chi2 );
    return true;
  }

  xScale = inputVectorX->length()/inputVectorY->length();

  for (i = 0; i < inputVectorY->length(); i++) {
    double z = xScale*i;
    long int idx = long(Kst::roundDouble(z));
    double skew = z - floor(z); /* [0..1] */
    long int idx2 = idx + 1;
    sy += inputVectorY->value()[i];
    while (idx2 >= inputVectorY->length()) {
      idx2--;
    }
    sx += inputVectorX->value()[idx] + (inputVectorX->value()[idx2] - inputVectorX->value()[idx])*skew;
  }

  sxoss = sx / inputVectorX->length();

  for (i = 0; i < inputVectorX->length(); i++) {
    double t = inputVectorX->value()[i] - sxoss;
    st2 += t * t;
    b += t * inputVectorY->value()[i];
  }

  b /= st2;
  a = (sy - sx*b)/inputVectorX->length();

  /* could put goodness of fit, etc, in here */

  outputVectorX->value()[0] = inputVectorX->value()[0];
  outputVectorX->value()[1] = inputVectorX->value()[inputVectorX->length()-1];
  outputVectorY->value()[0] = a+b*outputVectorX->value()[0];
  outputVectorY->value()[1] = a+b*outputVectorX->value()[1];

  for (i = 0; i < inputVectorX->length(); i++) {
    double z = xScale*i;
    long int idx = long(Kst::roundDouble(z));
    double skew = z - floor(z); /* [0..1] */
    long int idx2 = idx + 1;
    double newX;
    double ci;
    while (idx2 >= inputVectorX->length()) {
      idx2--;
    }
    newX = inputVectorX->value()[idx] + (inputVectorX->value()[idx2] - inputVectorX->value()[idx])*skew;
    ci = inputVectorY->value()[i] - a - b*newX;
    chi2 += ci*ci;
  }

  outputScalarA->setValue( a );
  outputScalarB->setValue( b );
  outputScalarChi->setValue( chi2 );
  return true;
}


Kst::VectorPtr LineFitSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr LineFitSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList LineFitSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList LineFitSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList LineFitSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList LineFitSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_X);
  vectors += VECTOR_OUT_Y;
  return vectors;
}


QStringList LineFitSource::outputScalarList() const {
  QStringList scalars(SCALAR_OUT_A);
  scalars += SCALAR_OUT_B;
  scalars += SCALAR_OUT_CHI2;
  return scalars;
}


QStringList LineFitSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void LineFitSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString LineFitPlugin::pluginName() const { return "Line Fit"; }
QString LineFitPlugin::pluginDescription() const { return "Generates a line of best fit for a set of data."; }


Kst::DataObject *LineFitPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetLineFitPlugin* config = static_cast<ConfigWidgetLineFitPlugin*>(configWidget)) {

    LineFitSource* object = store->createObject<LineFitSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
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


Kst::DataObjectConfigWidget *LineFitPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetLineFitPlugin *widget = new ConfigWidgetLineFitPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ConvolvePlugin, LineFitPlugin)

// vim: ts=2 sw=2 et
