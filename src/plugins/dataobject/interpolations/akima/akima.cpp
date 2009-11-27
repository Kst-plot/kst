/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "akima.h"
#include "objectstore.h"
#include "ui_akimaconfig.h"

#include <gsl/gsl_spline.h>
#include "../interpolations.h"

static const QString& VECTOR_IN_X = "Vector X In";
static const QString& VECTOR_IN_Y = "Vector Y In";
static const QString& VECTOR_IN_X1 = "Vector X' In";
static const QString& VECTOR_OUT = "Y Interpolated";

class ConfigWidgetAkimaPlugin : public Kst::DataObjectConfigWidget, public Ui_AkimaConfig {
  public:
    ConfigWidgetAkimaPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_AkimaConfig() {
      setupUi(this);
    }

    ~ConfigWidgetAkimaPlugin() {}

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
      if (AkimaSource* source = static_cast<AkimaSource*>(dataObject)) {
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
        _cfg->beginGroup("Interpolation Akima DataObject Plugin");
        _cfg->setValue("Input Vector X", _vectorX->selectedVector()->Name());
        _cfg->setValue("Input Vector Y", _vectorY->selectedVector()->Name());
        _cfg->setValue("Input Vector X1", _vectorX1->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Interpolation Akima DataObject Plugin");
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


AkimaSource::AkimaSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


AkimaSource::~AkimaSource() {
}


QString AkimaSource::_automaticDescriptiveName() const {
  return QString("Interpolation Akima Plugin Object");
}


void AkimaSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetAkimaPlugin* config = static_cast<ConfigWidgetAkimaPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN_X, config->selectedVectorX());
    setInputVector(VECTOR_IN_Y, config->selectedVectorY());
    setInputVector(VECTOR_IN_X1, config->selectedVectorX1());
  }
}


void AkimaSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool AkimaSource::algorithm() {
  Kst::VectorPtr inputVectorX = _inputVectors[VECTOR_IN_X];
  Kst::VectorPtr inputVectorY = _inputVectors[VECTOR_IN_Y];
  Kst::VectorPtr inputVectorX1 = _inputVectors[VECTOR_IN_X1];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  return interpolate( inputVectorX, inputVectorY, inputVectorX1, outputVector, gsl_interp_akima);
}


Kst::VectorPtr AkimaSource::vectorX() const {
  return _inputVectors[VECTOR_IN_X];
}


Kst::VectorPtr AkimaSource::vectorY() const {
  return _inputVectors[VECTOR_IN_Y];
}


Kst::VectorPtr AkimaSource::vectorX1() const {
  return _inputVectors[VECTOR_IN_X1];
}


QStringList AkimaSource::inputVectorList() const {
  QStringList vectors(VECTOR_IN_X);
  vectors += VECTOR_IN_Y;
  vectors += VECTOR_IN_X1;
  return vectors;
}


QStringList AkimaSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList AkimaSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList AkimaSource::outputVectorList() const {
  return QStringList(VECTOR_OUT);
}


QStringList AkimaSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList AkimaSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void AkimaSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString AkimaPlugin::pluginName() const { return "Interpolation Akima Spline"; }
QString AkimaPlugin::pluginDescription() const { return "Generates a non-rounded Akima interpolation for a set of data."; }


Kst::DataObject *AkimaPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetAkimaPlugin* config = static_cast<ConfigWidgetAkimaPlugin*>(configWidget)) {

    AkimaSource* object = store->createObject<AkimaSource>();

    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN_X, config->selectedVectorX());
      object->setInputVector(VECTOR_IN_Y, config->selectedVectorY());
      object->setInputVector(VECTOR_IN_X1, config->selectedVectorX1());
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


Kst::DataObjectConfigWidget *AkimaPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetAkimaPlugin *widget = new ConfigWidgetAkimaPlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_ConvolvePlugin, AkimaPlugin)

// vim: ts=2 sw=2 et
