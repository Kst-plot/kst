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


#include "fitlorentzian_unweighted.h"
#include "objectstore.h"
#include "ui_fitlorentzian_unweightedconfig.h"

#define NUM_PARAMS 4
#define MAX_NUM_ITERATIONS 500

#include <gsl/gsl_fit.h>
#include "../non_linear.h"

static const QString& VECTOR_IN_X = "X Vector";
static const QString& VECTOR_IN_Y = "Y Vector";
static const QString& VECTOR_OUT_Y_FITTED = "Fit";
static const QString& VECTOR_OUT_Y_RESIDUALS = "Residuals";
static const QString& VECTOR_OUT_Y_PARAMETERS = "Parameters Vector";
static const QString& VECTOR_OUT_Y_COVARIANCE = "Covariance";
static const QString& SCALAR_OUT = "chi^2/nu";

class ConfigWidgetFitLorentzianUnweightedPlugin : public Kst::DataObjectConfigWidget, public Ui_FitLorentzian_UnweightedConfig {
  public:
    ConfigWidgetFitLorentzianUnweightedPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_FitLorentzian_UnweightedConfig() {
      setupUi(this);
    }

    ~ConfigWidgetFitLorentzianUnweightedPlugin() {}

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

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (FitLorentzianUnweightedSource* source = static_cast<FitLorentzianUnweightedSource*>(dataObject)) {
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
        _cfg->beginGroup("Fit Lorentzian Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Fit Lorentzian Plugin");
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
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


FitLorentzianUnweightedSource::FitLorentzianUnweightedSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


FitLorentzianUnweightedSource::~FitLorentzianUnweightedSource() {
}


QString FitLorentzianUnweightedSource::_automaticDescriptiveName() const {
  return vectorY()->descriptiveName() + i18n(" Lorentzian");
}


void FitLorentzianUnweightedSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetFitLorentzianUnweightedPlugin* config = static_cast<ConfigWidgetFitLorentzianUnweightedPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
  }
}


void FitLorentzianUnweightedSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_Y_FITTED, "");
  setOutputVector(VECTOR_OUT_Y_RESIDUALS, "");
  setOutputVector(VECTOR_OUT_Y_PARAMETERS, "");
  setOutputVector(VECTOR_OUT_Y_COVARIANCE, "");
  setOutputScalar(SCALAR_OUT, "");
}


void function_initial_estimate(const double X[], const double Y[], int npts, double P[]) {
  double min_y = 1E300;
  double max_y = -1E300;
  double min_x = 1E300;
  double max_x = -1E300;
  double mean_y = 0.0;
  double x_at_min_y;
  double x_at_max_y;

  double A, B, D;

  // find peak, vally, and mean
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

  // Heuristic for finding the sign : less time is spent in the peak than
  // in background if the range covers more than ~+- 2 sigma.
  // It will fail if you are
  // really zoomed in.  Not sure what happens then :-(
  if (max_y - mean_y > mean_y - min_y) { // positive going gaussian
    A = max_y - min_y;
    D = min_y;
    B = x_at_max_y;
  } else { // negative going gaussian
    A = min_y - mean_y;
    D = max_y;
    B = x_at_min_y;
  }

  P[0] = A; // amplitude
  P[1] = B; // x0
  P[2] = (max_x - min_x)*0.1; // Half Width: guess 1/10 the fit range
  P[3] = D; // offset
}


double function_calculate(double x, double P[]) {
  double A = P[0]; // amplitude
  double B = P[1]; // x0
  double C = P[2]; // Half Width
  double D = P[3]; // offset

  x -= B;

  double Y = A/(1.0 + x*x/(C*C)) + D;

  return Y;

}

void function_derivative(double x, double P[], double dFdP[]) {

  double A = P[0]; // amplitude
  double B = P[1]; // x0
  double C = P[2]; // Half Width

  double C2 = C*C;
  x -= B;
  double x2 = x*x;
  double m = (x2 + C2);


  dFdP[0] = 1.0/(1.0 + x2/C2); // dF/dA
  dFdP[1] = 2.0*A*C2*x/(m*m);
  dFdP[2] = 2.0*A*C*x2/(m*m);
  dFdP[3] = 1.0;
}

bool FitLorentzianUnweightedSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];

  Kst::VectorPtr outputVectorYFitted = _outputVectors[VECTOR_OUT_Y_FITTED];
  Kst::VectorPtr outputVectorYResiduals = _outputVectors[VECTOR_OUT_Y_RESIDUALS];
  Kst::VectorPtr outputVectorYParameters = _outputVectors[VECTOR_OUT_Y_PARAMETERS];
  Kst::VectorPtr outputVectorYCovariance = _outputVectors[VECTOR_OUT_Y_COVARIANCE];
  Kst::ScalarPtr outputScalar = _outputScalars[SCALAR_OUT];


  Kst::LabelInfo label_info = inputVectorY->labelInfo();
  label_info.name = i18n("Lorentzian Fit to %1").arg(label_info.name);
  outputVectorYFitted->setLabelInfo(label_info);

  label_info.name = i18n("Lorentzian Fit Residuals");
  outputVectorYResiduals->setLabelInfo(label_info);

  bool bReturn = false;

  bReturn = kstfit_nonlinear( inputVectorX, inputVectorY,
                              outputVectorYFitted, outputVectorYResiduals, outputVectorYParameters,
                              outputVectorYCovariance, outputScalar );
  return bReturn;
}


Kst::VectorPtr FitLorentzianUnweightedSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr FitLorentzianUnweightedSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


QStringList FitLorentzianUnweightedSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList FitLorentzianUnweightedSource::inputScalarList() const {
  return QStringList();
}


QStringList FitLorentzianUnweightedSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList FitLorentzianUnweightedSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_Y_FITTED);
  vectors += VECTOR_OUT_Y_RESIDUALS;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  vectors += VECTOR_OUT_Y_COVARIANCE;
  vectors += VECTOR_OUT_Y_PARAMETERS;
  return vectors;
}


QStringList FitLorentzianUnweightedSource::outputScalarList() const {
  return QStringList( SCALAR_OUT );
}


QStringList FitLorentzianUnweightedSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void FitLorentzianUnweightedSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString FitLorentzianUnweightedSource::parameterName(int index) const {
  QString parameter;
  switch (index) {
    case 0:
      parameter = "Amplitide";
      break;
    case 1:
      parameter = "x_o";
      break;
    case 2:
      parameter = "Half Width";
      break;
    case 3:
      parameter = "Offset";
      break;
  }

  return parameter;
}


// Name used to identify the plugin.  Used when loading the plugin.
QString FitLorentzianUnweightedPlugin::pluginName() const { return "Lorentzian Fit"; }
QString FitLorentzianUnweightedPlugin::pluginDescription() const { return "Generates a lorentzian fit for a set of data."; }


Kst::DataObject *FitLorentzianUnweightedPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetFitLorentzianUnweightedPlugin* config = static_cast<ConfigWidgetFitLorentzianUnweightedPlugin*>(configWidget)) {

    FitLorentzianUnweightedSource* object = store->createObject<FitLorentzianUnweightedSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *FitLorentzianUnweightedPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetFitLorentzianUnweightedPlugin *widget = new ConfigWidgetFitLorentzianUnweightedPlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_FitLorentzianUnweightedPlugin, FitLorentzianUnweightedPlugin)
#endif

// vim: ts=2 sw=2 et
