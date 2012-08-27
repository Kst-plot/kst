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

/* FIXME: figure out what this plugin is supposed to do, and comment
   accordingly. */

#include "phase.h"
#include "objectstore.h"
#include "ui_phaseconfig.h"

static const QString& VECTOR_IN_TIME = "Vector In Time";
static const QString& VECTOR_IN_DATA = "Vector In Data";
static const QString& SCALAR_IN_PERIOD = "Period";
static const QString& SCALAR_IN_ZEROPHASE = "Zero Phase";

static const QString& VECTOR_OUT_PHASE = "Phase";
static const QString& VECTOR_OUT_DATA = "Data Out";

class ConfigPhasePlugin : public Kst::DataObjectConfigWidget, public Ui_PhaseConfig {
  public:
    ConfigPhasePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_PhaseConfig() {
      setupUi(this);
    }

    ~ConfigPhasePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorTime->setObjectStore(store);
      _vectorData->setObjectStore(store);
      _scalarPeriod->setObjectStore(store);
      _scalarZeroPhase->setObjectStore(store);
      _scalarPeriod->setDefaultValue(0);
      _scalarZeroPhase->setDefaultValue(0);
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorTime, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorData, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarPeriod, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_scalarZeroPhase, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorTime() { return _vectorTime->selectedVector(); };
    void setSelectedVectorTime(Kst::VectorPtr vector) { return _vectorTime->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorData() { return _vectorData->selectedVector(); };
    void setSelectedVectorData(Kst::VectorPtr vector) { return _vectorData->setSelectedVector(vector); };

    Kst::ScalarPtr selectedScalarPeriod() { return _scalarPeriod->selectedScalar(); };
    void setSelectedScalarPeriod(Kst::ScalarPtr scalar) { return _scalarPeriod->setSelectedScalar(scalar); };

    Kst::ScalarPtr selectedScalarZeroPhase() { return _scalarZeroPhase->selectedScalar(); };
    void setSelectedScalarZeroPhase(Kst::ScalarPtr scalar) { return _scalarZeroPhase->setSelectedScalar(scalar); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (PhaseSource* source = static_cast<PhaseSource*>(dataObject)) {
        setSelectedVectorTime(source->vectorTime());
        setSelectedVectorData(source->vectorData());
        setSelectedScalarPeriod(source->scalarPeriod());
        setSelectedScalarZeroPhase(source->scalarZeroPhase());
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
        _cfg->beginGroup("Phase DataObject Plugin");
        _cfg->setValue("Input Vector Time", _vectorTime->selectedVector()->Name());
        _cfg->setValue("Input Vector Data", _vectorData->selectedVector()->Name());
        _cfg->setValue("Input Scalar Period", _scalarPeriod->selectedScalar()->Name());
        _cfg->setValue("Input Scalar Zero Phase", _scalarZeroPhase->selectedScalar()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Phase DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector Time").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorTime = static_cast<Kst::Vector*>(object);
        if (vectorTime) {
          setSelectedVectorTime(vectorTime);
        }
        vectorName = _cfg->value("Input Vector Data").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorData = static_cast<Kst::Vector*>(object);
        if (vectorData) {
          setSelectedVectorData(vectorData);
        }
        QString scalarName = _cfg->value("Input Scalar Period").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarPeriod = static_cast<Kst::Scalar*>(object);
        if (scalarPeriod) {
          setSelectedScalarPeriod(scalarPeriod);
        }
        scalarName = _cfg->value("Input Scalar Zero Phase").toString();
        object = _store->retrieveObject(scalarName);
        Kst::Scalar* scalarZeroPhase = static_cast<Kst::Scalar*>(object);
        if (scalarZeroPhase) {
          setSelectedScalarZeroPhase(scalarZeroPhase);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


PhaseSource::PhaseSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


PhaseSource::~PhaseSource() {
}


QString PhaseSource::_automaticDescriptiveName() const {
  return QString("Phase Plugin Object");
}


void PhaseSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigPhasePlugin* config = static_cast<ConfigPhasePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_TIME, config->selectedVectorTime());
    setInputVector(VECTOR_IN_DATA, config->selectedVectorData());
    setInputScalar(SCALAR_IN_PERIOD, config->selectedScalarPeriod());
    setInputScalar(SCALAR_IN_ZEROPHASE, config->selectedScalarZeroPhase());
  }
}


void PhaseSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_PHASE, "");
  setOutputVector(VECTOR_OUT_DATA, "");
}


bool PhaseSource::algorithm() {
  Kst::VectorPtr inputVectorTime = _inputVectors[VECTOR_IN_TIME];
  Kst::VectorPtr inputVectorData = _inputVectors[VECTOR_IN_DATA];
  Kst::ScalarPtr inputScalarPeriod = _inputScalars[SCALAR_IN_PERIOD];
  Kst::ScalarPtr inputScalarZeroPhase = _inputScalars[SCALAR_IN_ZEROPHASE];

  Kst::VectorPtr outputVectorPhase = _outputVectors[VECTOR_OUT_PHASE];
  Kst::VectorPtr outputVectorDataOut = _outputVectors[VECTOR_OUT_DATA];

  double* pResult[2];
  double  dPhasePeriod = inputScalarPeriod->value();
  double dPhaseZero = inputScalarZeroPhase->value();
  int iLength;

  bool bReturn = false;

  if (dPhasePeriod <= 0.0) {
    _errorString = "Error:  Input Scalar Phase must be greater than zero.";
    return false;
  }

  if (inputVectorTime->length() != inputVectorData->length()) {
    _errorString = "Error:  Input Vector lengths do not match.";
    return false;
  }

  iLength = inputVectorTime->length();

  outputVectorPhase->resize(iLength, true);
  pResult[0] = outputVectorPhase->value();

  outputVectorDataOut->resize(iLength, true);
  pResult[1] = outputVectorDataOut->value();

  if (pResult[0] != NULL && pResult[1] != NULL) {
    for (int i = 0; i < outputVectorPhase->length(); ++i) {
      outputVectorPhase->value()[i] = pResult[0][i];
    }
    for (int i = 0; i < outputVectorDataOut->length(); ++i) {
      outputVectorDataOut->value()[i] = pResult[1][i];
    }

    /*
    determine the outputVectorPhase...
    */
    for (int i=0; i<iLength; i++) {
      outputVectorPhase->value()[i] = fmod( ( inputVectorTime->value()[i] - dPhaseZero ) / dPhasePeriod, 1.0 );
    }

    /*
    sort by outputVectorPhase...
    */
    memcpy( outputVectorDataOut->value(), inputVectorData->value(), iLength * sizeof( double ) );
    double* sort[2];
    sort[0] = outputVectorPhase->value();
    sort[1] = outputVectorDataOut->value();
    quicksort( sort, 0, iLength-1 );

    bReturn = true;
  }

  return bReturn;
}


void PhaseSource::swap(double* pData[], int iOne, int iTwo) {
  double dTemp;

  for (int i=0; i<2; i++) {
    dTemp = pData[i][iOne];
    pData[i][iOne] = pData[i][iTwo];
    pData[i][iTwo] = dTemp;
  }
}


void PhaseSource::quicksort( double* pData[], int iLeft, int iRight) {
  double dVal = pData[0][iRight];
  int i = iLeft - 1;
  int j = iRight;

  if (iRight <= iLeft) {
    return;
  }

  while (1) {
    while (pData[0][++i] < dVal) {}

    while (dVal < pData[0][--j]) {
      if (j == iLeft) {
        break;
      }
    }
    if (i >= j) {
      break;
    }
    swap( pData, i, j );
  }
  swap( pData, i, iRight );
  quicksort( pData, iLeft, i-1 );
  quicksort( pData, i+1, iRight );
}


Kst::VectorPtr PhaseSource::vectorTime() const {
  return _inputVectors[VECTOR_IN_TIME];
}


Kst::VectorPtr PhaseSource::vectorData() const {
  return _inputVectors[VECTOR_IN_DATA];
}


Kst::ScalarPtr PhaseSource::scalarPeriod() const {
  return _inputScalars[SCALAR_IN_PERIOD];
}


Kst::ScalarPtr PhaseSource::scalarZeroPhase() const {
  return _inputScalars[SCALAR_IN_ZEROPHASE];
}


QStringList PhaseSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_TIME);
  vectors += VECTOR_IN_DATA;
  return vectors;
}


QStringList PhaseSource::inputScalarList() const {
  QStringList scalars(SCALAR_IN_PERIOD);
  scalars += SCALAR_IN_ZEROPHASE;
  return scalars;
}


QStringList PhaseSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList PhaseSource::outputVectorList() const {
  QStringList vectors(VECTOR_OUT_PHASE);
  vectors += VECTOR_OUT_DATA;
  return vectors;
}


QStringList PhaseSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList PhaseSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void PhaseSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString PhasePlugin::pluginName() const { return "Phase"; }
QString PhasePlugin::pluginDescription() const { return "Phases a given data set to the specified period and zero outputVectorPhase."; }


Kst::DataObject *PhasePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigPhasePlugin* config = static_cast<ConfigPhasePlugin*>(configWidget)) {

    PhaseSource* object = store->createObject<PhaseSource>();

    if (setupInputsOutputs) {
      object->setInputScalar(SCALAR_IN_PERIOD, config->selectedScalarPeriod());
      object->setInputScalar(SCALAR_IN_ZEROPHASE, config->selectedScalarZeroPhase());
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_TIME, config->selectedVectorTime());
      object->setInputVector(VECTOR_IN_DATA, config->selectedVectorData());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *PhasePlugin::configWidget(QSettings *settingsObject) const {
  ConfigPhasePlugin *widget = new ConfigPhasePlugin(settingsObject);
  return widget;
}

#ifndef QT5
Q_EXPORT_PLUGIN2(kstplugin_BinPlugin, PhasePlugin)
#endif

// vim: ts=2 sw=2 et
