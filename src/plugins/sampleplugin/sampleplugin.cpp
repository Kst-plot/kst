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

/**********************
ConfigWidgetSamplePlugin - This class defines the config widget that will be added to the 
BasicPluginDialog container for configuring the plugin.  By default the BasicPluginDialog has 
no controls and thus all desired inputs/outputs/options should included in the .ui.

DataObjectConfigWidget is defined in dataobject.h
***********************/
class ConfigWidgetSamplePlugin : public Kst::DataObjectConfigWidget, public Ui_SamplePluginConfig {
  public:
    ConfigWidgetSamplePlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_SamplePluginConfig() {
      setupUi(this);
    }

    ~ConfigWidgetSamplePlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      // _store must be set here.  Any additional controls requiring the objectstore should also
      // have it set at this time.
      _store = store; 
      _vector->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        // In order to for Apply button logic to function.  All controls change state must be linked to 
        // the dialog's modified signal.
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    // All information in the config widget must be publically accessible so that the plugin can pull 
    // the required information and use it construct the object.
    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    // This function is required and is used when editing of the object is triggered.  It must be able
    // to configure the widget with the settings currently in use by the object.
    virtual void setupFromObject(Kst::Object* dataObject) {
      if (SamplePluginSource* source = static_cast<SamplePluginSource*>(dataObject)) {
        setSelectedVector(source->vector());
      }
    }

    // This function is used when loading a .kst file from disk.  The plugins are expected to store any 
    // required parameters as attributes of the plugin tag.  The name and type are handled by BasicPluginFactory
    // as are all inputs and outputs.  This should be used for any non-generic attributes associated with the plugin.
    // If false is returned, the creation of the object will not occur.
    // NOTE:  Saving of the object is done in the objects saveProperties function.  Values must match between them.
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
    // Dialog Defaults control - Save
    // Uses the provided QSettings Object to save the plugin default details as a new group.
    // Called everytime a new dialog request is made.  Does not affect editing.
    virtual void save() {
      if (_cfg) {
        _cfg->beginGroup("Sample DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    // Dialog Defaults control - Load
    // Uses the provided QSettings Object to load the plugin default details.
    // Called everytime a new dialog request is completed.  Does not get called on edit.
    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Sample DataObject Plugin");
        QString vectorName = _cfg->value("Input Vector").toString();
        Kst::Object* object = _store->retrieveObject(vectorName);
        Kst::Vector* vector = static_cast<Kst::Vector*>(object);
        if (vector) {
          setSelectedVector(vector);
        }
        _cfg->endGroup();
      }
    }

  private:
    Kst::ObjectStore *_store;

};


/**********************
SamplePluginSource - This class defines the main DataObject which derives from BasicPlugin (a DataObject).
The key functions that this class must provide is the ability to create/modify the object, setup outputs, and
be able to process the data (algorithm function).

***********************/
SamplePluginSource::SamplePluginSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


SamplePluginSource::~SamplePluginSource() {
}


QString SamplePluginSource::_automaticDescriptiveName() const {
  return QString("Test Plugin Object");
}


// Change is used to configure the DataObject and will be run immediately after initial creation as well as 
// when modifications have occured.
void SamplePluginSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetSamplePlugin* config = static_cast<ConfigWidgetSamplePlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void SamplePluginSource::setupOutputs() {
  setOutputVector(VECTOR_OUT, "");
}


// The algorithm function must handle all the data calculations for the plugin.  This function is run during 
// each update of the input objects which includes after creation of the object.  Only output objects that have 
// have setOutputVector/setOutputScalar/setOutputString should be updated as others will not appear outside of
// the plugin.
bool SamplePluginSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr outputVector = _outputVectors[VECTOR_OUT];

  outputVector->resize(inputVector->length(), false);

  // Sample only.  Copy all values from input vector to output vector.
  for (int i = 0; i < inputVector->length(); i++) {
    outputVector->value()[i] = inputVector->value(i);
  }

  // Return true to continue update.  Returning false indicates that the update did not result in the output
  // objects changing and will not trigger updates of objects using them.
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


// This function allows for support of saving / loading this object from a .kst file.  The function should only 
// write attributes as any other advancement of the StreamWriter will result in a malformed Xml document.  All 
// values required to recreate the should be saved here.  All Input/Output Objects are saved by BasicPlugin and do 
// not need to be stored here.  This should be used for any non-generic attributes associated with the plugin.
// NOTE:  Loading of the object is done in the configWidgets configurePropertiesFromXml function.  Values must match between them.
void SamplePluginSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


// Name used to identify the plugin.  Used when loading the plugin.
QString SamplePlugin::pluginName() const { return "Sample DataObject Plugin"; }


/**********************
SamplePlugin - This class defines the plugin interface to the DataObject defined by the plugin.
The primary requirements of this class are to provide the necessary connections to create the object
which includes providing access to the configuration widget.

***********************/

// The create function is responsible for creating the DataObject.  It is provided the ObjectStore as well as a
// fully configured configWidget to setup the object from.  
// setupInputOutputs is a flag to mark whether the object should automatically setup the inputs and outputs.  New
// creations will always configure them.  When loading from a .kst file they will be done manually.
Kst::DataObject *SamplePlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetSamplePlugin* config = static_cast<ConfigWidgetSamplePlugin*>(configWidget)) {

    // Create a generic object.
    SamplePluginSource* object = store->createObject<SamplePluginSource>();

    // Setup the object values based on the config widget.
    // object->setValue(config->value());

    // If the inputs/outputs should be configured, setup them up from the configWidget.
    if (setupInputsOutputs) {
      object->setInputVector(VECTOR_IN, config->selectedVector());
      object->setupOutputs();
    }

    // Set the pluginName.
    object->setPluginName(pluginName());

    // Trigger an update of the object.  This is required to trigger the algorithm call and creation of the output.
    // If inputs/outputs was not triggered, this will also be run a second time after all inputs/outputs have been 
    // configured.
    object->writeLock();
    object->update();
    object->unlock();

    return object;
  }
  return 0;
}


// Request for this plugins configuration widget.  
Kst::DataObjectConfigWidget *SamplePlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetSamplePlugin *widget = new ConfigWidgetSamplePlugin(settingsObject);
  return widget;
}

Q_EXPORT_PLUGIN2(kstplugin_sampleplugin, SamplePlugin)

// vim: ts=2 sw=2 et
