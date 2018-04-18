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


#include "fitgaussian_weighted.h"
#include "objectstore.h"
#include "ui_fitgaussian_weightedconfig.h"

#define NUM_PARAMS 4
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear_weighted.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_IN_WEIGHTS = "Weights Vector";
static const QString& SCALAR_IN_OFFSET = "Offset";

static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitGaussianWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitGaussian_WeightedConfig {
  public:
    ConfigWidgetFitGaussianWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitGaussian_WeightedConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFitGaussianWeightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _vectorWeights->setObjectStore(store);

      _scalarOffset->setObjectStore(store);
      _forceOffset->setChecked(false);
      _scalarOffset->setEnabled(false);

    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorWeights, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarOffset, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        _scalarOffset->setDefaultValue(0.0);
        connect(_forceOffset, SIGNAL(toggled(bool)), dialog, SIGNAL(modified()));
        connect(_forceOffset, SIGNAL(toggled(bool)), _scalarOffset, SLOT(setEnabled(bool)));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVectorX(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      setSelectedVectorY(vector);
    }

    Kst::ScalarPtr scalarOffset() {return _scalarOffset->selectedScalar(); };
    void setScalarOffset(Kst::ScalarPtr scalar) {_scalarOffset->setSelectedScalar(scalar);};

    void setVectorsLocked(bool locked = true) {
      _vectorX->setEnabled(!locked);
      _vectorY->setEnabled(!locked);
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorWeights() { return _vectorWeights->selectedVector(); };
    void setSelectedVectorWeights(Kst::VectorPtr vector) { return _vectorWeights->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitGaussianWeightedSource* source = static_cast<FitGaussianWeightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorWeights(source->vectorWeights());
        _forceOffset->setChecked(source->_forceOffset);
        setScalarOffset(source->scalarOffset());
      }
    }

    virtual bool configurePropertiesFromXml(Kst::ObjectStore *store, QXmlStreamAttributes& attrs) {

      bool validTag = true;

      setObjectStore(store);

      QStringRef av;
      bool force_offset = false;
      av = attrs.value("ForceOffset");
      if (!av.isNull()) {
        force_offset = QVariant(av.toString()).toBool();
      }
      _forceOffset->setChecked(force_offset);
//      if (force_offset) {
//        av = attrs.value("Offset");
//        if (!av.isNull()) {
//          QString name = av.toString();
//          Kst::ObjectPtr object = store->retrieveObject(name);
//          Kst::ScalarPtr scalar = Kst::kst_cast<Kst::Scalar>(object);
//          setScalarOffset(scalar);
//        }
//      }

      return validTag;
    }

  public slots:
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Fit Gaussian Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->setValue("Force Offset", _forceOffset->isChecked());
        if (_forceOffset->isChecked()) {
          _cfg->setValue("Offset", _scalarOffset->selectedScalar()->Name());
        }
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Gaussian Weighted Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorx = static_cast<Kst::Vector*>(object);
        if (vectorx) {
          setSelectedVectorX(vectorx);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectory = static_cast<Kst::Vector*>(object);
        if (vectory) {
          setSelectedVectorX(vectory);
        }
        vectorName = _cfg->value("Input Vector Weights").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorweights = static_cast<Kst::Vector*>(object);
        if (vectorweights) {
          setSelectedVectorX(vectorweights);
        }
        bool force_offset = _cfg->value("Force Offset").toBool();
        _forceOffset->setChecked(force_offset);
        if (force_offset) {
          QString scalarName = _cfg->value("Offset").toString();
          object = _store->retrieveObject(scalarName);
          Kst::Scalar* scalar = static_cast<Kst::Scalar*>(object);
          if (scalar) {
            setScalarOffset(scalar);
          }
        }

        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitGaussianWeightedSource::FitGaussianWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
  _forceOffset = false;
}


FitGaussianWeightedSource::~FitGaussianWeightedSource() {
}


QString FitGaussianWeightedSource::_automaticDescriptiveName() const {
  return tr("%1 Weighted Gaussian").arg(vectorY()->descriptiveName());
}


void FitGaussianWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitGaussianWeightedPlugin* config = static_cast<ConfigWidgetFitGaussianWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
    setInputScalar(SCALAR_IN_OFFSET, config->scalarOffset());
    _forceOffset = config->_forceOffset->isChecked();
  }
}


void FitGaussianWeightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");

  int i=0;
  for (QString paramName = parameterName(i); !paramName.isEmpty(); paramName = parameterName(++i)) {
    setOutputScalar(paramName, "");
  }
}

void function_initial_estimate( const double X[], const double Y[], int npts, double P[] ) {
  double min_y = 1E300;
  double max_y = -1E300;
  double min_x = 1E300;
  double max_x = -1E300;
  double mean_y = 0.0;
  double x_at_min_y=0;
  double x_at_max_y=0;

  double A, C, D;

  // find peak, valley, and mean
  for (int i = 0; i<npts; i++) {
    if (min_y > Y[i]) {
      min_y = Y[i];
      x_at_min_y = X[i];
    }
    if (max_y < Y[i]) {
      max_y = Y[i];
      x_at_max_y = X[i];
    }
    mean_y += Y[i];

    if (min_x > X[i]) {
      min_x = X[i];
    }
    if (max_x < X[i]) {
      max_x = X[i];
    }
  }
  if (npts>0) {
    mean_y /= double(npts);
  }

  // Heuristic for finding the sign of the : less time is spent in the peak than
  // in background if the range covers more than ~+- 2 sigma.
  // It will fail if you are
  // really zoomed into the gaussian.  Not sure what happens then :-(
  if (max_y - mean_y > mean_y - min_y) { // positive going gaussian
    A = max_y - min_y;
    D = min_y;
    C = x_at_max_y;
  } else { // negative going gaussian
    A = min_y - mean_y;
    D = max_y;
    C = x_at_min_y;
  }
  // guess that the width of the gaussian is around 1/10 of the x range (?)

  P[0] = A;
  P[1] = (max_x - min_x)*0.1;
  P[2] = C;
  P[3] = D;

}


double function_calculate(double x, double P[]) {
  double A = P[0];
  double B = 0.5/(P[1]*P[1]);
  double C = P[2];
  double D = offset_;

  if (n_params == 4) {
    D = P[3];
  }
  x -= C;

  return A*exp(-B*x*x) + D;
}

void function_derivative( double x, double P[], double dFdP[] ) {
  double A = P[0];
  double s = P[1];
  double B = 0.5/(s*s);
  double C = P[2];
  //double D = P[3];

  x -= C;

  double E = exp(-B*x*x);

  dFdP[0] = E;
  dFdP[1] = A*x*x*E/(s*s*s);
  dFdP[2] = 2*A*B*x*E;
  dFdP[3] = 1.0;

}


bool FitGaussianWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];
  Kst::ScalarPtr inputScalarOffset = _inputScalars[SCALAR_IN_OFFSET];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  if (_forceOffset) {
    if (inputScalarOffset) {
      offset_ = inputScalarOffset->value();
    } else {
      offset_ = 0;
    }
    n_params = 3;
  } else {
    n_params = 4;
  }


  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = tr("Gaussian Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = tr("Gaussian Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);


  bool bReturn = false;

  bReturn = kstfit_nonlinear_weighted( inputVectorX, inputVectorY, inputVectorWeights,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );
  return bReturn;
}


Kst::VectorPtr FitGaussianWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitGaussianWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitGaussianWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}

Kst::ScalarPtr FitGaussianWeightedSource::scalarOffset() const {
  return _inputScalars[SCALAR_IN_OFFSET];
}

QStringList FitGaussianWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitGaussianWeightedSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_OFFSET);
  return scalars;
}


QStringList FitGaussianWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitGaussianWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitGaussianWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitGaussianWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitGaussianWeightedSource::saveProperties(QXmlStreamWriter &s) {
  QString force_offset;
  force_offset.setNum(_forceOffset);
  s.writeAttribute("ForceOffset", force_offset);
}


QString FitGaussianWeightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
  case 0:
    parameter = "Amplitude";
    break;
  case 1:
    parameter = "\\sigma";
    break;
  case 2:
    parameter = "x_o";
    break;
  case 3:
    parameter = "Offset";
    break;
  default:
    parameter = "";
    break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitGaussianWeightedPlugin::pluginName() const { return tr("Gaussian Weighted Fit"); }
QString FitGaussianWeightedPlugin::pluginDescription() const { return tr("Generates a gaussian weighted fit for a set of data."); }


Kst::DataObject *FitGaussianWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitGaussianWeightedPlugin* config = static_cast<ConfigWidgetFitGaussianWeightedPlugin*>(configWidget)) {

    FitGaussianWeightedSource* object = store->createObject<FitGaussianWeightedSource>();
    object->_forceOffset = config->_forceOffset->isChecked();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
      object->setInputScalar(SCALAR_IN_OFFSET, config->scalarOffset());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitGaussianWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitGaussianWeightedPlugin *widget = new ConfigWidgetFitGaussianWeightedPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_FitGaussianWeightedPlugin, FitGaussianWeightedPlugin)
#endif

// vim: ts=2 sw=2 et
