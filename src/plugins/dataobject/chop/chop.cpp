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


 /* FIXME: rename to something more clear */
 /* differences even samples from odd samples.  Used for LFI, whose 
    Dicky-switch produces even samples looking at a cold load, and
    odd samples looking at the sky */


#include "chop.h"
#include "objectstore.h"
#include "ui_chopconfig.h"

static const QString& VECTOR_IN = "Vector In";
static const QString& VECTOR_OUT_ODD = "Odd Vector";
static const QString& VECTOR_OUT_EVEN = "Even Vector";
static const QString& VECTOR_OUT_DIFFERENCE = "Difference Vector";
static const QString& VECTOR_OUT_INDEX = "Index Vector";

class ConfigWidgetChopPlugin : public Kst::DataObjectConfigWidget, public Ui_ChopConfig {
  public:
    ConfigWidgetChopPlugin(QSettings* cfg) : DataObjectConfigWidget(cfg), Ui_ChopConfig() {
      _store = 0;
      setupUi(this);
    }

    ~ConfigWidgetChopPlugin() {}

    void setObjectStore(Kst::ObjectStore* store) { 
      _store = store; 
      _vector->setObjectStore(store); 
    }

    void setupSlots(QWidget* dialog) {
      if (dialog) {
        connect(_vector, SIGNAL(selectionChanged(QString)), dialog, SIGNAL(modified()));
      }
    }

    Kst::VectorPtr selectedVector() { return _vector->selectedVector(); };
    void setSelectedVector(Kst::VectorPtr vector) { return _vector->setSelectedVector(vector); };

    virtual void setupFromObject(Kst::Object* dataObject) {
      if (ChopSource* source = static_cast<ChopSource*>(dataObject)) {
        setSelectedVector(source->vector());
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
        _cfg->beginGroup("Chop DataObject Plugin");
        _cfg->setValue("Input Vector", _vector->selectedVector()->Name());
        _cfg->endGroup();
      }
    }

    virtual void load() {
      if (_cfg && _store) {
        _cfg->beginGroup("Chop DataObject Plugin");
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


ChopSource::ChopSource(Kst::ObjectStore *store)
: Kst::BasicPlugin(store) {
}


ChopSource::~ChopSource() {
}


QString ChopSource::_automaticDescriptiveName() const {
  return tr("Chop Plugin Object");
}


void ChopSource::change(Kst::DataObjectConfigWidget *configWidget) {
  if (ConfigWidgetChopPlugin* config = static_cast<ConfigWidgetChopPlugin*>(configWidget)) {
    setInputVector(VECTOR_IN, config->selectedVector());
  }
}


void ChopSource::setupOutputs() {
  setOutputVector(VECTOR_OUT_ODD, "");
  setOutputVector(VECTOR_OUT_EVEN, "");
  setOutputVector(VECTOR_OUT_DIFFERENCE, "");
  setOutputVector(VECTOR_OUT_INDEX, "");
}


bool ChopSource::algorithm() {
  Kst::VectorPtr inputVector = _inputVectors[VECTOR_IN];
  Kst::VectorPtr outputVectorOdd = _outputVectors[VECTOR_OUT_ODD];
  Kst::VectorPtr outputVectorEven = _outputVectors[VECTOR_OUT_EVEN];
  Kst::VectorPtr outputVectorDifference = _outputVectors[VECTOR_OUT_DIFFERENCE];
  Kst::VectorPtr outputVectorIndex = _outputVectors[VECTOR_OUT_INDEX];

  //Make sure there is at least 1 element in the input vector
  if (inputVector->length() < 1) {
    _errorString = tr("Error:  Input Vector invalid size");
    return false;
  }

  int iLength = inputVector->length();
  int iLengthNew = (int)ceil(iLength / 2.0);

  outputVectorOdd->resize(iLengthNew, false);
  outputVectorEven->resize(iLengthNew, false);
  outputVectorDifference->resize(iLengthNew, false);
  outputVectorIndex->resize(iLengthNew, false);

  for (int i = 0; i < iLength; i+=2) {
    outputVectorOdd->raw_V_ptr()[i/2] = inputVector->raw_V_ptr()[i];
    outputVectorEven->raw_V_ptr()[i/2] = inputVector->raw_V_ptr()[i+1];
    outputVectorDifference->raw_V_ptr()[i/2] = inputVector->raw_V_ptr()[i] - inputVector->raw_V_ptr()[i+1];
    outputVectorIndex->raw_V_ptr()[i/2] = i/2;
  }

  return true;
}


Kst::VectorPtr ChopSource::vector() const {
  return _inputVectors[VECTOR_IN];
}


QStringList ChopSource::inputVectorList() const {
  return QStringList( VECTOR_IN );
}


QStringList ChopSource::inputScalarList() const {
  return QStringList( /*SCALAR_IN*/ );
}


QStringList ChopSource::inputStringList() const {
  return QStringList( /*STRING_IN*/ );
}


QStringList ChopSource::outputVectorList() const {
  QStringList vectorList(VECTOR_OUT_ODD);
  vectorList += VECTOR_OUT_EVEN;
  vectorList += VECTOR_OUT_DIFFERENCE;
  vectorList += VECTOR_OUT_INDEX;
  return vectorList;
}


QStringList ChopSource::outputScalarList() const {
  return QStringList( /*SCALAR_OUT*/ );
}


QStringList ChopSource::outputStringList() const {
  return QStringList( /*STRING_OUT*/ );
}


void ChopSource::saveProperties(QXmlStreamWriter &s) {
  Q_UNUSED(s);
//   s.writeAttribute("value", _configValue);
}


QString ChopPlugin::pluginName() const { return tr("Chop"); }
QString ChopPlugin::pluginDescription() const { return tr("Chops a given data set into odd, even, difference, and Index data sets."); }


Kst::DataObject *ChopPlugin::create(Kst::ObjectStore *store, Kst::DataObjectConfigWidget *configWidget, bool setupInputsOutputs) const {

  if (ConfigWidgetChopPlugin* config = static_cast<ConfigWidgetChopPlugin*>(configWidget)) {

    ChopSource* object = store->createObject<ChopSource>();

    if (setupInputsOutputs) {
      object->setupOutputs();
      object->setInputVector(VECTOR_IN, config->selectedVector());
    }

    object->setPluginName(pluginName());

    object->writeLock();
    object->registerChange();
    object->unlock();

    return object;
  }
  return 0;
}


Kst::DataObjectConfigWidget *ChopPlugin::configWidget(QSettings *settingsObject) const {
  ConfigWidgetChopPlugin *widget = new ConfigWidgetChopPlugin(settingsObject);
  return widget;
}

// vim: ts=2 sw=2 et
