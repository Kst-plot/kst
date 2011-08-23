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

/* bin Y values into N bins from X min to X max, according to their X values.
   This is like a 1D map, where X specifices the pixel the data go into. */

#include "syncbin.h"
#include "objectstore.h"
#include "ui_syncbinconfig.h"

// macros to find the top, bottom, and middle of a bin
#define BINMID(x) XMin+(XMax-XMin)*(double(x)+0.5)/double(nbins)

#define BIN( x ) int(double(nbins)*(x-XMin)/(XMax-XMin))

static const QString& VECTOR_IN_X = "Vector In X";
static const QString& VECTOR_IN_Y = "Vector In Y";
static const QString& SCALAR_IN_BINS = "Number of Bins";
static const QString& SCALAR_IN_XMIN = "X Min";
static const QString& SCALAR_IN_XMAX = "X Max";

static const QString& VECTOR_OUT_X_OUT = "X out";
static const QString& VECTOR_OUT_Y_OUT = "Y out";
static const QString& VECTOR_OUT_Y_ERROR = "Y error";
static const QString& VECTOR_OUT_N = "N";

class ConfigSyncBinPlugin : public Kst::DataObjectConfigWidget, public Ui_SyncBinConfig {
  public:
    ConfigSyncBinPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_SyncBinConfig() {
      setupUi(this);
    }

    ~ConfigSyncBinPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store);
      _vectorY->setObjectStore(store);
      _scalarBins->setObjectStore(store);
      _scalarXMin->setObjectStore(store);
      _scalarXMax->setObjectStore(store);
      _scalarBins->setDefaultValue(0);
      _scalarXMin->setDefaultValue(0);
      _scalarXMax->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarBins, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarXMin, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarXMax, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarBins() { return _scalarBins->selectedScalar(); };
    void setSelectedScalarBins(Kst::ScalarPtr scalar) { return _scalarBins->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarXMin() { return _scalarXMin->selectedScalar(); };
    void setSelectedScalarXMin(Kst::ScalarPtr scalar) { return _scalarXMin->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarXMax() { return _scalarXMax->selectedScalar(); };
    void setSelectedScalarXMax(Kst::ScalarPtr scalar) { return _scalarXMax->setSelectedScalar(scalar); };


    virtual void setupFromObject(Kst::Object* dataObject) {
      if (SyncBinSource* source = static_cast<SyncBinSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedScalarBins(source->scalarBins());
        setSelectedScalarXMin(source->scalarXMin());
        setSelectedScalarXMax(source->scalarXMax());
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
        _cfg->beginGroup("SyncBin DataObject Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Scalar Number of Bins", _scalarBins->selectedScalar()->Name());
        _cfg->setValue("Input Scalar X Min", _scalarXMin->selectedScalar()->Name());
        _cfg->setValue("Input Scalar X Max", _scalarXMax->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("SyncBin DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorX = static_cast<Kst::Vector*>(object);
        if (vectorX) {
          setSelectedVectorX(vectorX);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorY = static_cast<Kst::Vector*>(object);
        if (vectorY) {
          setSelectedVectorY(vectorY);
        }
        QString scalarName = _cfg->value("Input Scalar Number of Bins").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarBins = static_cast<Kst::Scalar*>(object);
        if (scalarBins) {
          setSelectedScalarBins(scalarBins);
        }
        scalarName = _cfg->value("Input Scalar X Min").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarXMin = static_cast<Kst::Scalar*>(object);
        if (scalarXMin) {
          setSelectedScalarXMin(scalarXMin);
        }
        scalarName = _cfg->value("Input Scalar X Max").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarXMax = static_cast<Kst::Scalar*>(object);
        if (scalarXMax) {
          setSelectedScalarXMax(scalarXMax);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


SyncBinSource::SyncBinSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


SyncBinSource::~SyncBinSource() {
}


QString SyncBinSource::_automaticDescriptiveName() const {
  return QString("SyncBin Plugin Object");
}


void SyncBinSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigSyncBinPlugin* config = static_cast<ConfigSyncBinPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputScalar(SCALAR_IN_BINS, config->selectedScalarBins());
    setInputScalar(SCALAR_IN_XMIN, config->selectedScalarXMin());
    setInputScalar(SCALAR_IN_XMAX, config->selectedScalarXMax());
  }
}


void SyncBinSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_X_OUT, "");
  setOutputVector(VECTOR_OUT_Y_OUT, "");
  setOutputVector(VECTOR_OUT_Y_ERROR, "");
  setOutputVector(VECTOR_OUT_N, "");
}


bool SyncBinSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::ScalarPtr inputScalarBins = _inputScalars[SCALAR_IN_BINS];
  Kst::ScalarPtr inputScalarXMin = _inputScalars[SCALAR_IN_XMIN];
  Kst::ScalarPtr inputScalarXMax = _inputScalars[SCALAR_IN_XMAX];

  Kst::VectorPtr outputVectorX = _outputVectors[VECTOR_OUT_X_OUT];
  Kst::VectorPtr outputVectorY = _outputVectors[VECTOR_OUT_Y_OUT];
  Kst::VectorPtr outputVectorYError = _outputVectors[VECTOR_OUT_Y_ERROR];
  Kst::VectorPtr outputVectorN = _outputVectors[VECTOR_OUT_N];

  int nbins = int( inputScalarBins->value() );
  int n_in;
  double XMin = inputScalarXMin->value();
  double XMax = inputScalarXMax->value();
  double *Xout, *Yout, *Yerr, *N;

  //Make sure there is at least 1 element in the input vector
  if (inputVectorX->length() < 1) {
    _errorString = "Error:  Input Vector X invalid size";
    return false;
  }
  if (inputVectorX->length() != inputVectorY->length()) {
    _errorString = "Error:  Input Vector lengths do not match";
    return false;
  }

  if (nbins < 2) {
    _errorString = "Error:  Bins must be at least 2";
    return false;
  }

  //resize the output arrays
  outputVectorX->resize(nbins, true);
  outputVectorY->resize(nbins, true);
  outputVectorYError->resize(nbins, true);
  outputVectorN->resize(nbins, true);

  // convenience definitions
  n_in = int( inputVectorX->length() );
  const double *Xin = inputVectorX->value();
  const double *Yin = inputVectorY->value();
  Xout = outputVectorX->value();
  Yout = outputVectorY->value();
  Yerr = outputVectorYError->value();
  N    = outputVectorN->value();

  // set/check XMax and XMin
  if ( XMax <= XMin ) { // autobin
    XMax = XMin = Xin[0];
    for (int i=1; i<n_in; i++ ) {
      if ( XMax>Xin[i] ) XMax = Xin[i];
      if ( XMin<Xin[i] ) XMin = Xin[i];
    }
    // make sure end points are included.
    double d = (XMax - XMin)/double(nbins*100.0);
    XMax+=d;
    XMin-=d;
  }

  if ( XMax == XMin ) { // don't want divide by zero...
    XMax +=1;
    XMin -=1;
  }

  // Fill Xout and zero Yout and Yerr
  for ( int i=0; i<nbins; i++ ) {
    Xout[i] = BINMID( i );
    Yout[i] = Yerr[i] = 0.0;
    N[i] = 0.0;
  }

  //bin the data
  int bin, last_bin=-1;
  int last_N=0;
  double last_sY=0;

  for ( int i=0; i<n_in; i++ ) {
    bin = BIN( Xin[i] );
    if (bin == last_bin) {
      last_sY += Yin[i];
      last_N++;
    } else { // new bin
      if (last_N>0) {
        last_sY/=last_N;
        if ( (last_bin>=0) && (last_bin<nbins) ) {
          Yout[last_bin]+=last_sY;
          Yerr[last_bin]+=last_sY*last_sY;
          N[last_bin]++;
        }
      }
      last_sY = Yin[i];
      last_N = 1;
      last_bin = bin;
    }
  }
  if (last_N>0) {
    last_sY/=last_N;
    if ( (last_bin>=0) && (last_bin<nbins) ) {
      Yout[last_bin]+=last_sY;
      Yerr[last_bin]+=last_sY*last_sY;
      N[last_bin]++;
    }
  }

  // normalize the inputScalarBins
  for ( int i = 0; i<nbins; i++ ) {
    if ( N[i]>0 ) {
      Yerr[i] = sqrt( Yerr[i] - Yout[i]*Yout[i]/N[i] )/N[i];
      Yout[i]/=N[i];
    }
  }

  return true;
}


Kst::VectorPtr SyncBinSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr SyncBinSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::ScalarPtr SyncBinSource::scalarBins() const {
  return _inputScalars[SCALAR_IN_BINS];
}


Kst::ScalarPtr SyncBinSource::scalarXMin() const {
  return _inputScalars[SCALAR_IN_XMIN];
}


Kst::ScalarPtr SyncBinSource::scalarXMax() const {
  return _inputScalars[SCALAR_IN_XMAX];
}


QStringList SyncBinSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  return vectors;
}


QStringList SyncBinSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_BINS);
  scalars += SCALAR_IN_XMIN;
  scalars += SCALAR_IN_XMAX;
  return scalars;
}


QStringList SyncBinSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList SyncBinSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_X_OUT);
  vectors += VECTOR_OUT_Y_OUT;
  vectors += VECTOR_OUT_Y_ERROR;
  vectors += VECTOR_OUT_N;
  return vectors;
}


QStringList SyncBinSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList SyncBinSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void SyncBinSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString SyncBinPlugin::pluginName() const { return "SyncBin"; }
QString SyncBinPlugin::pluginDescription() const { return "Synchronously coadd vector Y into inputScalarBins defined by vector X.  Like a 1D map."; }


Kst::DataObject *SyncBinPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigSyncBinPlugin* config = static_cast<ConfigSyncBinPlugin*>(configWidget)) {

    SyncBinSource* object = store->createObject<SyncBinSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN_BINS, config->selectedScalarBins());
      object->setInputScalar(SCALAR_IN_XMIN, config->selectedScalarXMin());
      object->setInputScalar(SCALAR_IN_XMAX, config->selectedScalarXMax());
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


Kst::DataObjectConfigWidget *SyncBinPlugin::configWidget(QSettings *settingsObject) const {
  ConfigSyncBinPlugin *widget = new ConfigSyncBinPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, SyncBinPlugin)

// vim: ts=2 sw=2 et
