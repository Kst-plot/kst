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


#include "cspline.h"
#include "objectstore.h"
#include "ui_csplineconfig.h"

#include <gsl/gsl_spline.h>
#include "../interpolations.h"

static const QString& VECTOR_IN_X = "Vector X In";
static const QString& VECTOR_IN_Y = "Vector Y In";
static const QString& VECTOR_IN_X1 = "Vector X' In";
static const QString& VECTOR_OUT = "Y Interpolated";

class ConfigWidgetCubicSplinePlugin : public Kst::DataObjectConfigWidget, public Ui_CubicSplineConfig {
  public:
    ConfigWidgetCubicSplinePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_CubicSplineConfig() {
      setupUi(this);
    }

    ~ConfigWidgetCubicSplinePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vectorX->setObjectStore(store); 
      _vectorY->setObjectStore(store); 
      _vectorX1->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vectorX, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorY, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
        connect(_vectorX1, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVectorX() { return _vectorX->selectedVector(); };
    void setSelectedVectorX(Kst::VectorPtr vector) { return _vectorX->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorY() { return _vectorY->selectedVector(); };
    void setSelectedVectorY(Kst::VectorPtr vector) { return _vectorY->setSelectedVector(vector); };

    Kst::VectorPtr selectedVectorX1() { return _vectorX1->selectedVector(); };
    void setSelectedVectorX1(Kst::VectorPtr vector) { return _vectorX1->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (CubicSplineSource* source = static_cast<CubicSplineSource*>(dataObject)) {
        setSelectedVectorX(source->vectorX());
        setSelectedVectorY(source->vectorY());
        setSelectedVectorX1(source->vectorX1());
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
        _cfg->beginGroup("Interpolation Cubic Spline DataObject Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector X1", _vectorX1->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Interpolation Cubic Spline DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector X").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVectorX(vector);
        }
        vectorName = _cfg->value("Input Vector Y").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vector2 = static_cast<Kst::Vector*>(object);
        if (vector2) {
          setSelectedVectorY(vector2);
        }
        vectorName = _cfg->value("Input Vector X'").toString();
        object = _store->retrieveObject(vectorName);
        Kst::Vector* vectorX1 = static_cast<Kst::Vector*>(object);
        if (vectorX1) {
          setSelectedVectorX1(vectorX1);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


CubicSplineSource::CubicSplineSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


CubicSplineSource::~CubicSplineSource() {
}


QString CubicSplineSource::_automaticDescriptiveName() const {
  return QString("Interpolation Cubic Spline Plugin Object");
}


void CubicSplineSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetCubicSplinePlugin* config = static_cast<ConfigWidgetCubicSplinePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_X1, config->selectedVectorX1());
  }
}


void CubicSplineSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool CubicSplineSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorX1 = _inputVectors[VECTOR_IN_X1];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  return interpolate( inputVectorX, inputVectorY, inputVectorX1, outputVector, gsl_interp_cspline);
}


Kst::VectorPtr CubicSplineSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr CubicSplineSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr CubicSplineSource::vectorX1() const {
  return _inputVectors[VECTOR_IN_X1];
}


QStringList CubicSplineSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_X1;
  return vectors;
}


QStringList CubicSplineSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList CubicSplineSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList CubicSplineSource::outputVectorList() const {
  return QStringList(VECTOR_OUT);
}


QStringList CubicSplineSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList CubicSplineSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void CubicSplineSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString CubicSplinePlugin::pluginName() const { return "Interpolation Cubic Spline Spline"; }
QString CubicSplinePlugin::pluginDescription() const { return "Generates a spline interpolation for a set of data."; }


Kst::DataObject *CubicSplinePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetCubicSplinePlugin* config = static_cast<ConfigWidgetCubicSplinePlugin*>(configWidget)) {

    CubicSplineSource* object = store->createObject<CubicSplineSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_X1, config->selectedVectorX1());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *CubicSplinePlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetCubicSplinePlugin *widget = new ConfigWidgetCubicSplinePlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ConvolvePlugin, CubicSplinePlugin)

// vim: ts=2 sw=2 et
