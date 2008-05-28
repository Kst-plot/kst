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


#include "sampleplugin.h"
#include "objectstore.h"
#include "ui_samplepluginconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& SCALAR_IN = "Scalar In";
static const QString& STRING_IN = "String In";
static const QString& VECTOR_OUT = "Vector Out";
static const QString& SCALAR_OUT = "Scalar Out";
static const QString& STRING_OUT = "String Out";


class ConfigWidgetSamplePlugin : public Kst::DataObjectConfigWidget, public Ui_SamplePluginConfig {
  public:
    ConfigWidgetSamplePlugin() : DataObjectConfigWidget(), Ui_SamplePluginConfig() {
      setupUi(this);
    }

    ~ConfigWidgetSamplePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { _vector->setObjectStore(store); }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (SamplePluginSource* source = static_cast<SamplePluginSource*>(dataObject)) {
        setSelectedVector(source->vector());
      }
    }
};


SamplePluginSource::SamplePluginSource(Kst::ObjectStore *store, const Kst::ObjectTag& tag)
: Kst::BasicPlugin(store, tag) {
}


SamplePluginSource::~SamplePluginSource() {
}


QString SamplePluginSource::_automaticDescriptiveName() const {
  return QString("Test Plugin Object");
}


void SamplePluginSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetSamplePlugin* config = static_cast<ConfigWidgetSamplePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void SamplePluginSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


bool SamplePluginSource::algorithm() {
   //Do nothing
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  outputVector->resize(inputVector->length(), false);

  for (int i = 0; i < inputVector->length(); i++) {
    outputVector->value()[i] = inputVector->value(i);
  }
  return true;
}


Kst::VectorPtr SamplePluginSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


QStringList SamplePluginSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList SamplePluginSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList SamplePluginSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList SamplePluginSource::outputVectorList() const {
  return QStringList( VECTOR_OUT );
}


QStringList SamplePluginSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList SamplePluginSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


QString SamplePlugin::pluginName() const { return "Sample DataObject Plugin"; }

Kst::DataObject *SamplePlugin::create(Kst::ObjectStore *store,
                               Kst::ObjectTag &tag, Kst::DataObjectConfigWidget *configWidget) const {

  if (ConfigWidgetSamplePlugin* config = static_cast<ConfigWidgetSamplePlugin*>(configWidget)) {

    SamplePluginSource* object = store->createObject<SamplePluginSource>(tag);

    object->setInputVector(VECTOR_IN, config->selectedVector());
    object->setupOutputs();

    object->setPluginName(pluginName());

    object->writeLock();
    object->update();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *SamplePlugin::configWidget() const {
  return new ConfigWidgetSamplePlugin();
}

Q_EXPORT_PLUGIN2(kstplugin_sampleplugin, SamplePlugin)

// vim: ts=2 sw=2 et
