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


#include "fitexponential_weighted.h"
#include "objectstore.h"
#include "ui_fitexponential_weightedconfig.h"

#define NUM_PARAMS 4
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear_weighted.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_IN_WEIGHTS = "Weights Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitExponentialWeightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitExponential_WeightedConfig {
  public:
    ConfigWidgetFitExponentialWeightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitExponential_WeightedConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetFitExponentialWeightedPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _vectorWeights->setObjectStore(store);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorWeights, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    void setVectorX(Kst::VectorPtr vector) {
      setSelectedVectorX(vector);
    }

    void setVectorY(Kst::VectorPtr vector) {
      setSelectedVectorY(vector);
    }

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
      if (FitExponentialWeightedSource* source = static_cast<FitExponentialWeightedSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorWeights(source->vectorWeights());
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
        _cfg->beginGroup("Fit Exponential Weighted Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector Weights", _vectorWeights->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Exponential Weighted Plugin");
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
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitExponentialWeightedSource::FitExponentialWeightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitExponentialWeightedSource::~FitExponentialWeightedSource() {
}


QString FitExponentialWeightedSource::_automaticDescriptiveName() const {
  return tr("%1 Weighted Exponential").arg(vectorY()->descriptiveName());
}


void FitExponentialWeightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitExponentialWeightedPlugin* config = static_cast<ConfigWidgetFitExponentialWeightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
  }
}


void FitExponentialWeightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}

double _X0 = 0; // this use of a global is a hack to inject the first sample into the function.

void swapDouble(double *A, double *B) {
  double C;

  C = *A;
  *A = *B;
  *B = C;
}


void function_initial_estimate( const double x[], const double y[], int npts, double P0[] ) {
  Q_UNUSED( x )
  Q_UNUSED( y )
  Q_UNUSED( npts )

  _X0 = x[0];

  // determine the signs of the terms.
  // get the average of the first 5%, last 5% and middle 5% of points
  int n_ave = npts/20;
  if (n_ave < 1) n_ave = 1;
  if (n_ave > 100) n_ave = 100;

  double y0 = 0;
  double x0 = 0;
  double x1 = 0;
  double y1 = 0;
  double x2 = 0;
  double y2 = 0;
  int d1 = npts/2 - n_ave/2;
  int d2 = npts-n_ave;

  if ((d1 + n_ave > npts) || (d2 + n_ave > npts)) { // bail if not enough points.
    P0[0] =  1.0;
    P0[1] =  0.0;
    P0[2] =  0.0;
    return;
  }

  for (int i=0; i<n_ave; i++) {
    x0+=x[i];
    y0+=y[i];
    x1+=x[i+d1];
    y1+=y[i+d1];
    x2+=x[i+d2];
    y2+=y[i+d2];
  }
  x0 /= (double)n_ave;
  y0 /= (double)n_ave;
  x1 /= (double)n_ave;
  y1 /= (double)n_ave;
  x2 /= (double)n_ave;
  y2 /= (double)n_ave;

  // Make sure x0, x1, x2 are monotonic.
  if (x2 > x0) {
    if (x1 > x2) {
      swapDouble(&x1, &x2);
      swapDouble(&y1, &y2);
    }
    if (x1 < x0) {
      swapDouble(&x1, &x0);
      swapDouble(&y1, &y0);
    }
  } else {
    if (x1 < x2) {
      swapDouble(&x1, &x2);
      swapDouble(&y1, &y2);
    }
    if (x1 > x0) {
      swapDouble(&x1, &x0);
      swapDouble(&y1, &y0);
    }
  }

  if ((x1 == x0) || (x2 == x0) || (x1 == x2)) { // bail if no x range
    P0[0] =  1.0;
    P0[1] =  0.0;
    P0[2] =  0.0;
    return;
  }

  P0[0] =  fabs(y2 - y0)/M_E;
  P0[1] =  M_E/fabs(x2 - x0);
  P0[2] =  y0;

  double m = (y2 - y0)/(x2 - x0);
  if (m > 0) { // rising
    if ((x1-x0)*m + y0 > y1) { // neg curvature +A, +B
      //P0[0] *= -1;
      //P0[1] *= -1.0;
    } else {   // -A, -B
      P0[0] *= -1;
      P0[1] *= -1.0;
    }
  } else { // falling
    if ((x1-x0)*m + y0 > y1) { // Curving down +A, -B
      //P0[0] *= -1;
      P0[1] *= -1.0;
    } else { // -A, +B
      P0[0] *= -1;
      //P0[1] *= -1.0;
    }
  }

  fflush(stdout);
}

// It would be nice to fit exp(B*(x-_X0)) + C, and not have A,
// but A is required to at least hold the sign.  We we just fix
// _X0 to the first X value in the data set, and fit for A.
double function_calculate( double x, double* P ) {
  double A = P[0];
  double B = P[1];
  double C = P[2];
  double y;

  y  = ( A*exp( B*(x - _X0) ) ) + C;

  return y;
}


void function_derivative( double x, double* P, double* dFdP ) {
  double A  = P[0];
  double B = P[1];
  double exp_BxXo;

  // dFdA = exp(b*(x-_X0)
  // dFdB = A*(x-_X0)*exp(b*(x-_X0))

  exp_BxXo = exp( B * (x-_X0) );

  dFdP[0]   = exp_BxXo;
  dFdP[1]   = (x-_X0) * A * exp_BxXo;
  dFdP[2]   = 1.0;

}




bool FitExponentialWeightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorWeights = _inputVectors[VECTOR_IN_WEIGHTS];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];

  Kst::LabelInfo label_info = inputVectorY->labelInfo();

  _X0 = inputVectorX->noNanValue()[0];
  n_params = 3;

  label_info.name = tr("A\\Cdotexp((x-x_o)/\\tau) + C fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = tr("Exponential Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);


  bool bReturn = false;

  bReturn = kstfit_nonlinear_weighted( inputVectorX, inputVectorY, inputVectorWeights,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );

  outputVectorYParameters->raw_V_ptr()[1] = 1.0/outputVectorYParameters->raw_V_ptr()[1];
  outputVectorYParameters->raw_V_ptr()[3] = _X0;

  return bReturn;
}


Kst::VectorPtr FitExponentialWeightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitExponentialWeightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr FitExponentialWeightedSource::vectorWeights() const {
  return _inputVectors[VECTOR_IN_WEIGHTS];
}


QStringList FitExponentialWeightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_WEIGHTS;
  return vectors;
}


QStringList FitExponentialWeightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitExponentialWeightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitExponentialWeightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitExponentialWeightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitExponentialWeightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitExponentialWeightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitExponentialWeightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "A";
      break;
    case 1:
      parameter = "\\tau";
      break;
    case 2:
      parameter = "C";
      break;
    case 3:
      parameter = "X_o";
      break;
    default:
      parameter = "";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitExponentialWeightedPlugin::pluginName() const { return tr("Exponential Weighted Fit"); }
QString FitExponentialWeightedPlugin::pluginDescription() const { return tr("Generates an exponential weighted fit for a set of data."); }


Kst::DataObject *FitExponentialWeightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitExponentialWeightedPlugin* config = static_cast<ConfigWidgetFitExponentialWeightedPlugin*>(configWidget)) {

    FitExponentialWeightedSource* object = store->createObject<FitExponentialWeightedSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_WEIGHTS, config->selectedVectorWeights());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitExponentialWeightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitExponentialWeightedPlugin *widget = new ConfigWidgetFitExponentialWeightedPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
